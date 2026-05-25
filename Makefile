CXX = g++
CXXFLAGS = -Wall -std=c++17

SRC = src/main.cpp src/shell.cpp src/parser.cpp src/executor.cpp
OUT = minishell

all:
	$(CXX)  $(CXXFLAGS) $(SRC) -o $(OUT)
