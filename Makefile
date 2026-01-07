CXX = g++
CXXFLAGS = --std=c++17

SRC = lexer.cpp main.cpp nfa.cpp parser.cpp
OUT = test

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)

run:
	./test.exe