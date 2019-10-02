CXX=g++
CXXFLAGS=-g -std=c++11 -I../googletest/include -I../include 
LDFLAGS=-L../googletest/lib64 -L../lib -lgtest_main -lgtest -lpthread -lautumn

DEPS=../lib/libautumn.a

all:prepare-dep test

prepare-dep:$(DEPS)

test:lexer_test
	./$<

lexer_test:lexer_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

%.o:%.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

.PHONY:all
.PHONY:prepare-dep
.PHONY:test
