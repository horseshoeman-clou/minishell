#ifndef HISTORY_H
#define HISTORY_H

#include<string>
#include<vector>

class History {

std::vector<std::string> commands;

size_t index = SIZE_MAX;
std::string saved_line;

public:
void add(const std::string& cmd);

bool up(std::string& current);

bool down(std::string& current);

};

#endif

std::string readLine(History& history);

extern std::string SHELL_PROMPT;
