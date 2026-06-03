#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

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
