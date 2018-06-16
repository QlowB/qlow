# building depends on debian packages: "g++" "llvm-dev" "bison" "flex" (and "make", duh)
# running depends on "llvm"

CXX := g++

INCLUDEDIRS := -I`llvm-config --includedir`
CXXFLAGS := -std=c++17 $(INCLUDEDIRS) # -Wall -Wextra

ifdef STATIC
LDFLAGS := `llvm-config --link-static --ldflags --system-libs --libs all` -static
else
LDFLAGS := `llvm-config --ldflags --system-libs --libs all`
endif

YACC := bison
YACCFLAGS := -d
LEX := flex
LEXFLAGS :=

#OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp */*.cpp))
OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp)) \
    parser.o lexer.o
LIBRARIES := 
EXECUTABLE := qlow

.PHONY: all
all: $(EXECUTABLE)

release: CXXFLAGS += -O3 -flto
release: LDFLAGS += -O3 -flto
release: all

debug: CXXFLAGS += -DDEBUGGING -g
debug: all

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $@

parser.o: parser.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) -fno-strict-aliasing

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.cpp: %.y
	$(YACC) -o $@ $< $(YACCFLAGS)

%.cpp: %.l parser.cpp
	$(LEX) -o $@ $< $(LEXFLAGS)



.PHONY: clean
clean:
	rm -f $(EXECUTABLE) *.o parser.cpp parser.hpp lexer.cpp


