#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<iostream>
#include<fcntl.h>
#include<filesystem>

#include "../include/executor.h"
#include "../include/shell_state.h"

int custom_cd(const std::vector<Token>& args){

if(args.size() ==1){
const char* home = getenv("HOME");

if(!home){
std::cerr<<"cd: HOME not set\n";
return 1;
}

if(chdir(home)!=0){
perror("cd");;
return 1;
}
return 0;
}

const std::string& path= args[1].value;

if(chdir(path.c_str())!=0){
perror("cd");
return 1;
}
setenv("PWD", path.c_str(), 1);
return 0;
}


int custom_pwd(){

std::error_code ec;
auto path = std::filesystem::current_path(ec);

if(ec){
std::cerr<<"pwd: "<<ec.message()<<'\n';
return 1;
}

std::cout<<path.string()<<'\n';
return 0;
}

struct Command{

std::vector<Token> args;
std::string inputFile;
std::string outputFile;
bool append = false;
};

std::vector<char*> buildArgs(const std::vector<Token>& command){

std::vector<char*> args;

for(const auto& token:command){
args.push_back(const_cast<char*>(token.value.data()));
}

args.push_back(nullptr);

return args;
}

std::vector<char*> buildArgs(const Command& command){

return buildArgs(command.args);
}


void executeCommand(std::vector<Token>& tokens){

if(tokens.empty()){
return;
}

if(tokens[0].value=="pwd"){
last_exit_status=custom_cd(tokens);
return;
}

if(tokens[0].value=="cd"){
last_exit_status=custom_cd(tokens);
return;
}

std::vector<char*> args=buildArgs(tokens);

pid_t pid=fork();

if(pid==0){
execvp(args[0],args.data());

perror("execvp failed");
exit(127);
}
else{
int status;
waitpid(pid, &status, 0);

if(WIFEXITED(status)){
last_exit_status = WEXITSTATUS(status);
}
else{
last_exit_status = 1;
}
}
}

void executePipeRedirection(const std::vector<Token>& tokens)
{
std::vector<Command> commands;
Command current;

for (size_t i = 0; i < tokens.size(); i++)
{
std::string token = tokens[i].value;

if (token == "|")
{
if (current.args.empty())
{
std::cout << "Syntax error near pipe\n";
return;
}

commands.push_back(current);
current = Command{};
}
else if (token == "<")
{
if (i + 1 >= tokens.size())
{
std::cout << "Syntax error near redirection\n";
return;
}

current.inputFile = tokens[++i].value;
}
else if (token == ">")
{
if (i + 1 >= tokens.size())
{
std::cout << "Syntax error near redirection\n";
return;
}

current.outputFile = tokens[++i].value;
}
else if (token == ">>")
{
if (i + 1 >= tokens.size())
{
std::cout << "Syntax error near redirection\n";
return;
}

current.outputFile = tokens[++i].value;
current.append = true;
}
else
{
current.args.push_back(Token(token));
}
}

if (current.args.empty())
{
std::cout << "Syntax error near pipe\n";
return;
}

commands.push_back(current);

int n = commands.size();

int pipeFd[n - 1][2];

for (int i = 0; i < n - 1; i++)
{
if (pipe(pipeFd[i]) == -1)
{
perror("pipe");
return;
}
}

for (int i = 0; i < n; i++)
{
pid_t pid = fork();

if (pid == -1)
{
perror("fork");
return;
}

if (pid == 0)
{
int fd;

if (!commands[i].inputFile.empty())
{
fd = open(commands[i].inputFile.c_str(), O_RDONLY);

if (fd == -1)
{
perror("open");
exit(1);
}

if (dup2(fd, STDIN_FILENO) == -1)
{
close(fd);
perror("dup2 stdin");
exit(1);
}

}
else if (i > 0)
{
if (dup2(pipeFd[i - 1][0], STDIN_FILENO) == -1)
{
perror("dup2 stdin");
exit(1);
}
}

if (!commands[i].outputFile.empty())
{
if (commands[i].append)
{
fd = open(commands[i].outputFile.c_str(),O_WRONLY | O_CREAT | O_APPEND,0644);
}
else
{
fd = open(commands[i].outputFile.c_str(),O_WRONLY | O_CREAT | O_TRUNC,0644);
}

if (fd == -1)
{
perror("open");
exit(1);
}

if (dup2(fd, STDOUT_FILENO) == -1)
{
close(fd);
perror("dup2 stdout");
exit(1);
}

}
else if (i < n - 1)
{
if (dup2(pipeFd[i][1], STDOUT_FILENO) == -1)
{
perror("dup2 stdout");
exit(1);
}
}


for (int j = 0; j < n - 1; j++)
{
close(pipeFd[j][0]);
close(pipeFd[j][1]);
}

if(commands[i].args.empty()){
exit(1);
}

if(commands[i].args[0].value == "pwd"){
last_exit_status = custom_pwd();
return;
}

if(commands[i].args[0].value == "cd"){
last_exit_status = custom_cd(commands[i].args);
return;
}

std::vector<char*> args = buildArgs(commands[i]);


execvp(args[0], args.data());
perror("execvp failed");
exit(127);
}
else{
int status;
waitpid(pid, &status, 0);
if(WIFEXITED(status)){
last_exit_status=WEXITSTATUS(status);
}
else{
last_exit_status = 1;
}
}
}

for (int i = 0; i < n - 1; i++)
{
close(pipeFd[i][0]);
close(pipeFd[i][1]);
}

for (int i = 0; i < n; i++)
{
wait(nullptr);
}
}
