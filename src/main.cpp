#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#include "../include/executor.h"
#include "../include/parser.h"


void testPipe(){

int pipefd[2];

if(pipe(pipefd)==-1){
perror("pipe failed");
return;
}

pid_t pid1=fork();
if(pid1==0){

close(pipefd[0]);

dup2(pipefd[1],STDOUT_FILENO);

close(pipefd[1]);

char* lsArgs[]={
(char*) "ls",
nullptr
};

execvp(lsArgs[0],lsArgs);

perror("execvp failed");

exit(1);
}

pid_t pid2=fork();

if(pid2==0){

close(pipefd[1]);

dup2(pipefd[0],STDIN_FILENO);

close(pipefd[0]);

char* grepArgs[]={
(char*)"grep",
(char*)"cpp",
nullptr
};

execvp(grepArgs[0],grepArgs);

exit(1);
}

close(pipefd[0]);
close(pipefd[1]);

wait(nullptr);
wait(nullptr);
}


int main(){


char* username=getenv("USER");

while(true){

std::cout<<"\033[1;31m"<<username<<"\033[0m"<<"\033[1;36m@minishell>\033[0m ";

std::string input;
std::getline(std::cin,input);

if(input=="exit"){
break;
}



std::vector<std::string> tokens=tokenizeWhitespace(input);

if(tokens.empty()){
continue;
}


if(tokens[0]=="cd"){

if(tokens.size()<2){

std::cout<<"Usage: cd <directory>\n";
continue;
}

if(chdir(tokens[1].c_str())!=0){
perror("cd failed");
}
continue;
}

bool hasPipe=false;

for(const auto& token : tokens){

if(token=="|"){
hasPipe=true;
break;
}
}

if(hasPipe){

testPipe();
continue;
}

executeCommand(tokens);
}

return 0;
}


