CXXFLAGS=-g -std=c++17 -Werror -coverage -I./include
LDFLAGS=

SRC=$(notdir $(wildcard src/*.cc))
OBJ=$(patsubst %.cc,objs/%.o,$(SRC))
HEADERS=$(wildcard include/*.h)

all:prepare-dep libautumn googletest unitest autumn

prepare-dep:
	@mkdir -p objs
	@mkdir -p lib

libautumn:libautumn.a
	@mv $^ lib

libautumn.a:$(OBJ)
	ar -r $@ $^

objs/%.o:src/%.cc $(HEADERS)
	$(CXX) -o $@ -c $< $(CXXFLAGS)

googletest:
	$(MAKE) -C googletest

unitest:
	$(MAKE) -C unitest

autumn:repl/autumn.cc libautumn.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L./lib -lautumn -lreadline

clean:
	rm -rf lib objs *.gcov *.gcno *.gcda
	$(MAKE) -C googletest clean
	$(MAKE) -C unitest clean

.PHONY:all
.PHONY:prepare-dep
.PHONY:autumn
.PHONY:googletest
.PHONY:unitest
.PHONY:clean
