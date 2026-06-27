#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<iostream>
#include<fcntl.h>

#include "../include/executor.h"

void executeCommand(std::vector<std::string>& tokens){

if(tokens.empty()){
return;
}

std::vector<char*> args;

for(auto& token:tokens){

args.push_back(const_cast<char*>(token.c_str()));
}

args.push_back(nullptr);

pid_t pid=fork();

if(pid==0){

execvp(args[0],args.data());

perror("execvp failed");
exit(1);
}
else{
wait(nullptr);
}

}


void executePipe(const std::vector<std::string> tokens){

std::vector<std::string> left,right;

bool pipeFound=false;

for(const auto& token : tokens){

if(token == "|"){
pipeFound=true;
continue;
}

if(!pipeFound){
left.push_back(token);
}
else{
right.push_back(token);
}

}

if(left.empty() || right.empty()){

std::cout<<"Syntax error near pipe\n";
return;
}

std::vector<char*> leftArgs;

for(auto& token : left){

leftArgs.push_back(const_cast<char*>(token.c_str()));
}

leftArgs.push_back(nullptr);


std::vector<char*> rightArgs;

for(auto& token : right){

rightArgs.push_back(const_cast<char*>(token.c_str()));
}

rightArgs.push_back(nullptr);


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

execvp(leftArgs[0],leftArgs.data());

perror("execvp failed");

exit(1);
}

pid_t pid2=fork();

if(pid2==0){

close(pipefd[1]);

dup2(pipefd[0],STDIN_FILENO);

close(pipefd[0]);

execvp(rightArgs[0],rightArgs.data());

exit(1);
}

close(pipefd[0]);
close(pipefd[1]);

wait(nullptr);
wait(nullptr);
}


void executeRedirection(const std::vector<std::string> tokens){

std::vector<std::string> command;
std::string filename;
std::string redirectOp;

bool redirectFound=false;

for(auto token : tokens){

if(token=="<" || token==">" || token ==">>"){
redirectOp=token;
redirectFound=true;
continue;
}

if(!redirectFound){
command.push_back(token);
continue;
}

else{
filename=token;
continue;
}
}

if(command.empty() || filename.empty()){

std::cout<<"Syntax error near redirection\n";
return;
}

std::vector<char*> commandArgs;

for(const auto& token : command){

commandArgs.push_back(const_cast<char*>(token.c_str()));
}

commandArgs.push_back(nullptr);

pid_t pid=fork();

if(pid==-1){
perror("fork failed");
return;
}

if(pid==0){
int fd;

if(redirectOp==">"){
fd=open(filename.c_str(),O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd,STDOUT_FILENO);
close(fd);
}
else if(redirectOp==">>"){

fd=open(filename.c_str(),O_WRONLY | O_CREAT | O_APPEND, 0644);
dup2(fd,STDOUT_FILENO);
}
else{
fd=open(filename.c_str(),O_RDONLY);

if(fd==-1){
perror("open failed");
exit(1);
}
if(dup2(fd,STDIN_FILENO)==-1){
perror("dup2 failed");
exit(1);
}

close(fd);
}


execvp(commandArgs[0],commandArgs.data());
perror("execvp failed");
exit(1);
}
else{
wait(nullptr);
}
}

