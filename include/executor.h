#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<vector>
#include<string>
#include<fcntl.h>

#ifndef PARSER_H
#define PARSER_H

struct Token {
std::string value;
bool singleQuoted = false;

Token() = default;
explicit Token(const std::string& s) : value(s){}
};

#endif

void executeCommand(std::vector<Token>& tokens);

void executePipeRedirection(const std::vector<Token>& tokens);
