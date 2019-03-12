# building depends on debian packages: "g++" "llvm-dev" "bison" "flex" (and "make", duh)
# running depends on "llvm" if not statically linked

CXX := clang++

LLVMCONFIG := llvm-config

INCLUDEDIRS := -I$(shell $(LLVMCONFIG) --includedir) -I.. -Isem/ -Iast/ -I.
CXXFLAGS := -std=c++17 $(INCLUDEDIRS) -w # -Wall -Wextra

ifndef STATIC
LDFLAGS := $(shell $(LLVMCONFIG) --link-static --ldflags --system-libs --libs all) -static -dead-strip -s
else
LDFLAGS := $(shell $(LLVMCONFIG) --ldflags --system-libs --libs all)
endif

YACC := bison
YACCFLAGS := -d
LEX := flex
LEXFLAGS :=

#OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp */*.cpp))
OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp */*.cpp)) \
    ast/syntax.o ast/lexer.o
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

ast/syntax.o: ast/syntax.cpp ast/lexer.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) -fno-strict-aliasing

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.cpp: %.y
	$(YACC) -o $@ $< $(YACCFLAGS)

%.cpp: %.l ast/syntax.cpp
	$(LEX) $(LEXFLAGS) -o $@ $<



.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECTS) ast/syntax.cpp ast/syntax.hpp ast/lexer.cpp ast/lexer.h


