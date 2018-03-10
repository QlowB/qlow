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



clear:
	rm -f *.o  */*.o parser/Parser.cpp parser/Lexer.cpp


