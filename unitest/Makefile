CXXFLAGS=-g -std=c++17 -Werror -I../googletest/include -I../include
LDFLAGS=-L../googletest/lib -L../lib -lgtest -lpthread -lautumn

DEPS=../lib/libautumn.a

all:prepare-dep test

prepare-dep:$(DEPS)

test:lexer_test parser_test
	@for bin in $^; do ./$$bin; done

lexer_test:lexer_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

parser_test:parser_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

%.o:%.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf *_test *.o

.PHONY:all
.PHONY:prepare-dep
.PHONY:test
.PHONY:clean
