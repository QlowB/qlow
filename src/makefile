# building depends on debian packages: "g++" "llvm-dev" "bison" "flex" (and "make", duh)
# running depends on "llvm" if not statically linked

CXX := clang++

LLVMCONFIG := llvm-config-5.0

INCLUDEDIRS := -I $(shell $(LLVMCONFIG) --includedir)
CXXFLAGS := -std=c++17 $(INCLUDEDIRS) -w # -Wall -Wextra

ifdef STATIC
LDFLAGS := $(shell $(LLVMCONFIG) --link-static --ldflags --system-libs --libs all) -static -dead-strip -s
else
LDFLAGS := $(shell $(LLVMCONFIG) --ldflags --system-libs --libs all)
endif

YACC := bison
YACCFLAGS := -d
LEX := flex
LEXFLAGS := "-8"

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
	$(LEX) $(LEXFLAGS) -o $@ $<



.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECTS) parser.cpp parser.hpp lexer.cpp


