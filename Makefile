CXX = g++
CXXFLAGS = -Wall -std=c++17

SRC = src/main.cpp src/shell.cpp src/parser.cpp src/executor.cpp src/history.cpp
OBJS = $(SRC:.cpp=.o)
TARGET = minishell

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean



