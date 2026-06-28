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

std::vector<std::string> currentCommand;
std::vector<std::vector<std::string>> commands;


for(const auto& token : tokens){

if(token != "|"){
currentCommand.push_back(token);
}
else{
if(currentCommand.empty()){
std::cout<<"Syntax error near pipe\n";
return;
}
commands.push_back(currentCommand);
currentCommand.clear();
continue;
}
}
if(currentCommand.empty()){
std::cout<<"Syntax error near pipe\n";
return;
}
commands.push_back(currentCommand);

int n=commands.size();

int pipeFd[n-1][2];

for(int i=0;i<n-1;i++){
if(pipe(pipeFd[i])==-1){
perror("pipe");
return;
}
}


for(int i=0;i<n;i++){

pid_t pid=fork();

if(pid==-1){
perror("fork");
return;
}

if(pid==0){

if(i>0){
if(dup2(pipeFd[i-1][0],STDIN_FILENO)==-1){
perror("dup2 stdin");
exit(1);
}
}

if(i<n-1){
if(dup2(pipeFd[i][1],STDOUT_FILENO)==-1){
perror("dup2 stdout");
exit(1);
}
}

for(int j=0;j<n-1;j++){

close(pipeFd[j][0]);
close(pipeFd[j][1]);
}

std::vector<char*> args;

for(auto& token: commands[i]){

args.push_back(const_cast<char*>(token.c_str()));
}

args.push_back(nullptr);

execvp(args[0],args.data());

perror("evecvp failed");

exit(1);
}
}

for(int i=0;i<n-1;i++){

close(pipeFd[i][0]);
close(pipeFd[i][1]);
}

for(int i=0;i<n;i++){
wait(nullptr);

}
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

