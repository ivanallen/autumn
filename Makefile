CXXFLAGS=-g -std=c++17 -Werror -I./include
LDFLAGS=

ifdef CODECOV
	CXXFLAGS += -coverage
endif

SRC=$(notdir $(wildcard src/*.cc))
OBJ=$(patsubst %.cc,objs/%.o,$(SRC))
HEADERS=$(wildcard include/*.h)

all:prepare-dep libautumn autumn googletest unitest

prepare-dep:
	@mkdir -p objs
	@mkdir -p lib

libautumn:./lib/libautumn.a

./lib/libautumn.a:$(OBJ)
	ar -r $@ $^

objs/%.o:src/%.cc $(HEADERS)
	$(CXX) -o $@ -c $< $(CXXFLAGS)

googletest:
	$(MAKE) -C googletest

unitest:
	$(MAKE) -C unitest

autumn:repl/autumn.cc ./lib/libautumn.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L./lib -lautumn -lreadline

clean:
	rm -rf lib objs *.gcov *.gcno *.gcda
	$(MAKE) -C googletest clean
	$(MAKE) -C unitest clean

.PHONY:all
.PHONY:prepare-dep
.PHONY:libautumn
.PHONY:googletest
.PHONY:unitest
.PHONY:clean
