#include<vector>
#include<string>
#include<sstream>

#ifndef PARSER_H
#define PARSER_H

struct Token {
std::string value;
bool singleQuoted = false;

Token() = default;
explicit Token(const std:: string& s) : value(s){}
};

#endif

std::vector<Token> tokenize(const std::string& input);
