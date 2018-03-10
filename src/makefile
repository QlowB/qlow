CXX := g++
CXXFLAGS := -g -Wall -std=c++17
LINKFLAGS :=

YACC := bison
YACCFLAGS := -d
LEX := flex
LEXFLAGS :=

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp */*.cpp)) \
    parser.o lexer.o
LIBRARIES := 
EXECUTABLE := qlow


all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	$(CXX) $^ $(LINKFLAGS) -o $@


%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.cpp: %.y
	$(YACC) -o $@ $< $(YACCFLAGS)

%.cpp: %.l parser.cpp
	$(LEX) -o $@ $< $(LEXFLAGS)



clean:
	rm -f $(EXECUTABLE) *.o parser.cpp parser.hpp lexer.cpp


