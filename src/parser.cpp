#include<sstream>
#include<vector>
#include<iostream>

#include "../include/parser.h"

enum class State {
NONE,
SINGLE,
DOUBLE
};

std::vector<Token> tokenize(const std::string& input){

std::vector<Token> tokens;
std::string token;

State state = State::NONE;
bool tokenStarted = false;
bool currentSingleQuote = false;

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

Token t;
t.value = token;
t.singleQuoted = currentSingleQuote;

tokens.push_back(t);
token.clear();
tokenStarted=false;
currentSingleQuote = false;
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
currentSingleQuote = true;
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
Token t;
t.value = token;
t.singleQuoted = currentSingleQuote;
tokens.push_back(t);
}

for(Token& token : tokens){

if(!token.singleQuoted && !token.value.empty() && token.value[0] =='$'){
std::string variableName="";

for(size_t i=1;i<token.value.size();i++){
variableName+=token.value[i];
}

char* value = getenv(variableName.c_str());

if(value !=nullptr){

token.value = value;
}
else{
token.value ="";
}
}
}
return tokens;
}

