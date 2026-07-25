#include<sstream>
#include<vector>
#include<iostream>
#include<cctype>
#include<cstdlib>

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
bool escaped = false;

for(size_t i=0; i<input.size(); i++){
char c=input[i];

if(escaped){
token+=c;
tokenStarted = true;
escaped = false;
continue;
}

if(state!=State::SINGLE && c == '\\'){
escaped = true;
continue;
}

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

if(c == '\\' && i+1<input.size()){

token+=input[i+1];
i++;
}
else if (c == '\''){
state = State::NONE;
}
else{
token+=c;
tokenStarted = true;
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

if(token.singleQuoted)
continue;

if(!token.value.empty()){
std::string expanded;

for(size_t i=0;i<token.value.size();i++){

if(token.value[i] == '$'){

std::string var;
i++;

while(i< token.value.size() && (std::isalnum(static_cast<unsigned char>(token.value[i])) || token.value[i] == '_')){

var+=token.value[i];
i++;
}
i--;


const char* value = std::getenv(var.c_str());

if(value){
expanded += value;
}
else{
expanded += '$';
expanded += var;
}
}

else{
expanded+=token.value[i];
}
}

token.value = expanded;

}
}

return tokens;
}

