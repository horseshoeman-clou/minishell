#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<vector>
#include<string>
#include<fcntl.h>

void executeCommand(std::vector<std::string>& tokens);

void executePipe(const std::vector<std::string> tokens);

void executeRedirection(const std::vector<std::string> tokens);
