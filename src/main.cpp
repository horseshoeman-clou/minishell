#include<iostream>
#include<string>
#include<sstream>
#include<vector>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#include "../include/executor.h"
#include "../include/parser.h"
#include "../include/history.h"
#include "../include/shell_state.h"

int last_exit_status = 0;


const char* username = std::getenv("USER")? std::getenv("USER") : "user";

const std::string RED = "\033[1;31m";
const std::string CYAN = "\033[1;36m";
const std::string RESET = "\033[0m";

std::string SHELL_PROMPT = RED + username + RESET + CYAN + "@minishell" + RESET + ": " ;


int main(){

History history;

while(true){

std::cout<<SHELL_PROMPT<<std::flush;

std::string line = readLine(history);

if(line=="exit"){
break;
}



std::vector<Token> tokens=tokenize(line);


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


