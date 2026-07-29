#include<sstream>
#include<vector>
#include<iostream>
#include<cctype>
#include<cstdlib>
#include<unistd.h>
#include<glob.h>

#include "../include/parser.h"
#include "../include/shell_state.h"

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

if(c == '\\' && i+1 < input.size()){
char next = input[i+1];

if(next == '"' || next == '\'' || next == '\\'){
token += next;
i++;
tokenStarted = true;
}
else{
token += c;
tokenStarted = true;
}
}

else if(c == '"'){
state=State::DOUBLE;
tokenStarted = true;
}
else if(c=='\''){
state=State::SINGLE;
tokenStarted=true;
currentSingleQuote = true;
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

if(c == '\\' && i+1<input.size()){
char next = input[i+1];

if(next == '"' || next == '\\' || next == '$' || next == '`'){

if( next == '$'){
token += '\\';

token += '$';
i++;
}
else{

token+=next;
i++;
}
}
else{
token+=c;
}
}
else if(c=='"'){
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

if(token.singleQuoted || token.value.empty())
continue;

std::string expanded;
for(size_t i=0;i<token.value.size();i++){

if(token.value[i] == '$' && i+1 <token.value.size()){
char next = token.value[i+1];

if(next == '?'){
expanded+=std::to_string(last_exit_status);
i++;
}
else if(next == '$'){
expanded += std::to_string(getpid());
i++;
}
else{
std::string var;
i++;
while(i<token.value.size() &&(std::isalnum(static_cast<unsigned char>(token.value[i])) || token.value[i] == '_')){

var += token.value[i];
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
}
else if(token.value[i] == '\\' && i+1 < token.value.size() && token.value[i+1] == '$'){
expanded+='$';
i++;
}
else{
expanded += token.value[i];
}
}
token.value = expanded;
}


std::vector<Token> expanded_tokens;

for(const Token& token : tokens){

if(token.singleQuoted || token.value.find_first_of("*?[") == std::string::npos){

expanded_tokens.push_back(token);
continue;
}

glob_t glob_result;

if(glob(token.value.c_str(), GLOB_TILDE, nullptr, &glob_result) == 0){

for(size_t i=0; i<glob_result.gl_pathc; i++){

Token t;
t.value = glob_result.gl_pathv[i];
t.singleQuoted = false;

expanded_tokens.push_back(t);
}
globfree(&glob_result);
}
else{

if(!token.value.empty()){
std::string expanded;

for(size_t i=0;i<token.value.size();i++){

if(token.value[i] == '$' && i+1<token.value.size()){

char next = token.value[i+1];

if(next=='?'){

expanded+=std::to_string(last_exit_status);
i++;
}

else if(next == '$'){
expanded+=std::to_string(getpid());
i++;
}

else{

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
}

else if(token.value[i] == '\\' && i+1<token.value.size() && token.value[i+1] == '$'){
expanded+= '$';
i++;
}
else{
expanded+=token.value[i];

}
}
Token t = token;
t.value = expanded;
expanded_tokens.push_back(t);
}
else{
expanded_tokens.push_back(token);
}
}
}
return expanded_tokens;
}

