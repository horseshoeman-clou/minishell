#include<termios.h>
#include<unistd.h>
#include<iostream>
#include<dirent.h>
#include<algorithm>
#include<sys/stat.h>

#include "../include/history.h"

struct RawMode {

termios original;
RawMode(){

tcgetattr(STDIN_FILENO, &original);

termios raw = original;
raw.c_lflag  &= ~(ICANON | ECHO);

tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

~RawMode(){

tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

};

void History::add(const std::string& cmd){

if(!cmd.empty()){
commands.push_back(cmd);
}

index=SIZE_MAX;
saved_line.clear();
}

bool History::up(std::string& current){

if(commands.empty()) return false;

if(index == SIZE_MAX){
saved_line = current;
index = commands.size();
}

if(index>0){
index--;
current = commands[index];
return true;
}

return false;
}

bool History::down(std::string& current){

if(index == SIZE_MAX) return false;

if(index < commands.size()-1){

index++;
current = commands[index];
return true;
}

index = SIZE_MAX;
current = saved_line;
return true;
}

std::pair<size_t, size_t> getCurrentWordBounds(const std::string& line, size_t cursor){

size_t start = line.rfind(' ', cursor-1);

if(start == std::string::npos)
start = 0;
else
start++;

size_t end = line.find(' ', cursor);

if(end == std::string::npos)
end = line.size();

return {start,end};
}

std::vector<std::string> getMatches(const std::string& prefix){

std::vector<std::string> matches;

size_t last_slash = prefix.find('/');
std::string dir = ".";
std::string file_prefix = prefix;

if(last_slash != std::string::npos){

dir = prefix.substr(0, last_slash);

file_prefix = prefix.substr(last_slash + 1);

if(dir.empty())
dir="/";
}

DIR* d = opendir(dir.c_str());
if(!d)
return matches;

struct dirent* entry;

while((entry = readdir(d)) != nullptr){

std::string name = entry-> d_name;

if(name == "." || name == "..")
continue;

if(file_prefix.empty() || (name.size() >= file_prefix.size() && name.compare(0, file_prefix.size(),file_prefix)==0)){

std::string full_path;

if(dir == "."){
full_path = name;
}
else if(dir == "/"){
full_path == "/" + name;
}
else{
full_path = dir + "/" + name;
}

matches.push_back(full_path);
}
}

closedir(d);

std::sort(matches.begin(), matches.end());
return matches;
}

const std::vector<std::string> BUILTINS = {"echo", "cd", "pwd", "exit", "false", "true"};

bool isDirectory(const std::string& path){

struct stat st;
return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

std::vector<std::string> getCommandMatches(const std::string& prefix){

std::vector<std::string> matches;

for(const auto& cmd : BUILTINS){

if(cmd.size()>=prefix.size() && cmd.compare(0, prefix.size(), prefix) == 0){
matches.push_back(cmd);
}
}

const char* path_env = std::getenv("PATH");

if(!path_env)
return matches;

std::string path_str = path_env;

size_t start = 0;

while(start<path_str.size()){
size_t end = path_str.find(":",start);

if(end == std::string::npos)
end = path_str.size();

std::string dir = path_str.substr(start, end-start);

DIR* d = opendir(dir.c_str());

if(d){

struct dirent* entry;

while((entry = readdir(d)) != nullptr){
std::string name = entry->d_name;

if(name.size() >= prefix.size() && name.compare(0, prefix.size(), prefix) == 0){

if(std::find(matches.begin(), matches.end(), name) == matches.end()) {

matches.push_back(name);
}
}
}
closedir(d);
}

start = end+1;
}

std::sort(matches.begin(), matches.end());
return matches;
}

std::string longestCommonPrefix(const std::vector<std::string>& matches){

if(matches.empty())
return "";

std::string prefix = matches[0];

for(size_t i=1;i<matches.size(); i++){

size_t len = 0;

while(len <prefix.size() && len < matches[i].size() && prefix[len] == matches[i][len]){
len++;
}

prefix.resize(len);
}

return prefix;
}

void printMatches(const std::vector<std::string>& matches){

std::cout<<'\n';
for(const auto& m : matches){

std::cout<<m<<" ";
}

std::cout<<'\n';
}


std::string readLine(History& history){

RawMode raw;
std::string line;
size_t cursor_pos = 0;

auto redraw = [&](const std::string& current_line, size_t pos){

std::cout<<"\033[2K\r"<<SHELL_PROMPT<<current_line<<std::flush;

if(pos<current_line.size()){
std::cout<<"\033["<<(current_line.size()-pos)<<"D"<<std::flush;
}
};

std::string last_tab_prefix = "";
std::string last_tab_line = "";

while(true){

char c;

if(read(STDIN_FILENO, &c, 1) != 1)
break;

if(c == '\x1b'){

char seq[2];

if(read(STDIN_FILENO, &seq[0],1) != 1)
continue;

if(read(STDIN_FILENO, &seq[1],1) != 1)
continue;

if(seq[0] == '['){

switch (seq[1]){

case 'A' :

if(history.up(line)){
cursor_pos = line.size();
redraw(line, cursor_pos);
}

break;

case 'B' :

if(history.down(line)){
cursor_pos = line.size();
redraw(line, cursor_pos);
}

break;

case 'C':

if(cursor_pos < line.size()){
cursor_pos++;
redraw(line, cursor_pos);
}

break;

case 'D':

if(cursor_pos>0){
cursor_pos--;
redraw(line,cursor_pos);
}

break;

}
}
continue;
}

if(c=='\t'){

auto [word_start, word_end] = getCurrentWordBounds(line, cursor_pos);

std::string prefix = line.substr(word_start, word_end-word_start);

bool is_first_word = (word_start == 0);

std::vector<std::string> matches;

if(is_first_word){
matches = getCommandMatches(prefix);
}
else{
 matches = getMatches(prefix);
}

if(matches.empty()){

}
else if(matches.size() == 1){

std::string completion = matches[0];

if(!is_first_word && isDirectory(completion)){
completion += '/';
}
else if(is_first_word){
completion += ' ';
}

line.replace(word_start, word_end-word_start, completion);

cursor_pos = word_start+ completion.size();

redraw(line, cursor_pos);
}
else{

std::string lcp = longestCommonPrefix(matches);

bool double_tab = (prefix == last_tab_prefix && line == last_tab_line);

if(double_tab || lcp.size() == prefix.size()){

printMatches(matches);

std::cout<<SHELL_PROMPT<<line<<std::flush;

if(cursor_pos < line.size()){

std::cout<<"\033["<<(line.size()-cursor_pos)<<"D"<<std::flush;
}
}

else if(lcp.size() > prefix.size()){
line.replace(word_start,word_end-word_start, lcp);

cursor_pos=word_start+lcp.size();
redraw(line, cursor_pos);
}
}

last_tab_prefix = prefix;
last_tab_line = line;
continue;
}


if(c=='\n' || c == '\r'){

std::cout<<'\n';
history.add(line);
return line;
}

if(c==127 || c  == '\b'){

if(cursor_pos>0){

line.erase(cursor_pos-1,1);
cursor_pos--;
redraw(line, cursor_pos);
}

continue;
}

if(c >= 32 && c <=127){

line.insert(cursor_pos, 1, c);
cursor_pos++;
redraw(line, cursor_pos);
}
}

return line;
}
