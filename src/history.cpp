#include<termios.h>
#include<unistd.h>
#include<iostream>

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

index=-1;
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

index = -1;
current = saved_line;
return true;
}

std::string readLine(History& history){

RawMode raw;
std::string line;

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
std::cout<<"\033[2K\r"<<SHELL_PROMPT <<line<<std::flush;
}

break;

case 'B' :

if(history.down(line)){
std::cout<<"\033[2K\r"<<SHELL_PROMPT<<line<<std::flush;
}
break;
}
}
continue;
}

if(c=='\n' || c == '\r'){

std::cout<<'\n';
history.add(line);
return line;
}

if(c==127 || c  == '\b'){

if(!line.empty()){
line.pop_back();
std::cout<<"\033[2K\r"<<SHELL_PROMPT<<line<<std::flush;
}

continue;
}

if(c >= 32 && c <=127){
line+=c;
std::cout<<c<<std::flush;
}
}

return line;
}
