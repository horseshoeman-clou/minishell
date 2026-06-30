#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<vector>
#include<string>
#include<fcntl.h>

void executeCommand(std::vector<std::string>& tokens);

void executePipeRedirection(const std::vector<std::string>& tokens);
