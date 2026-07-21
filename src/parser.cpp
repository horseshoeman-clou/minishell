#include<sstream>
#include<vector>
#include<iostream>

#include "../include/parser.h"

enum class State {
NONE,
SINGLE,
DOUBLE
};


std::vector<std::string> tokenize(const std::string& input){

std::vector<std::string> tokens;
std::string token;

State state = State::NONE;
bool tokenStarted = false;

for(size_t i=0; i<input.size(); i++){
char c=input[i];

switch(state){

case State::NONE:
if(c == '"'){
state=State::DOUBLE;
tokenStarted = true;
}
else if(c=='\''){
state=State::SINGLE;
tokenStarted=true;
}
else if(std::isspace(c)){
if(tokenStarted){
tokens.push_back(token);
token.clear();
tokenStarted=false;
}
}
else{
token+=c;
tokenStarted=true;
}

break;

case State::DOUBLE:
if(c=='"'){
state=State::NONE;
}
else{
token+=c;
tokenStarted=true;
}
break;

case State::SINGLE:
if(c=='\''){
state=State::NONE;
}
else{
token+=c;
tokenStarted=true;
}
break;
}
}


if(tokenStarted){
tokens.push_back(token);
}

for(std::string& token : tokens){

if(!token.empty() && token[0] =='$'){
std::string variableName="";

for(size_t i=1;i<token.size();i++){
variableName+=token[i];
}

char* value = getenv(variableName.c_str());

if(value !=nullptr){

token = value;
}
else{
token ="";
}
}
}
return tokens;
}

