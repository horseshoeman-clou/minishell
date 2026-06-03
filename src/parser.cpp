#include<sstream>
#include<vector>

#include "../include/parser.h"

std::vector<std::string> tokenizeWhitespace(const std::string& input){

std::istringstream stream(input);
std::vector<std::string> tokens;
std::string token;

while(stream >> token){
tokens.push_back(token);
}

return tokens;
}

