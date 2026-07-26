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


































































































































































