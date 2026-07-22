#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#include "../include/executor.h"
#include "../include/parser.h"

int main(){


char* username=getenv("USER");

while(true){

std::cout<<"\033[1;31m"<<username<<"\033[0m"<<"\033[1;36m@minishell>\033[0m ";

std::string input;
std::getline(std::cin,input);

if(input=="exit"){
break;
}



std::vector<Token> tokens=tokenize(input);

if(tokens.empty()){
continue;
}

bool hasPipe=false;
bool hasDirection=false;

for(const auto& token : tokens){

if(token.value=="|"){
hasPipe=true;
}

if(token.value==">" || token.value=="<" || token.value==">>"){
hasDirection=true;
}
}

if(hasPipe || hasDirection){
executePipeRedirection(tokens);
continue;
}
else{
executeCommand(tokens);
}
}

return 0;
}


