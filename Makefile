CXX=g++
CXXFLAGS=-g -std=c++11 -I./include
LDFLAGS=

SRC=$(notdir $(wildcard src/*.cc))
OBJ=$(patsubst %.cc,objs/%.o,$(SRC))
HEADERS=$(wildcard include/*.h)

all:prepare-dep libautumn unitest autumn

prepare-dep:
	@mkdir -p objs
	@mkdir -p lib

libautumn:libautumn.a
	@mv $^ lib

libautumn.a:$(OBJ)
	ar -r $@ $^ $(LDFLAGS)

objs/%.o:src/%.cc $(HEADERS)
	$(CXX) -o $@ -c $< $(CXXFLAGS)


unitest:
	$(MAKE) -C unitest

autumn:repl/autumn.cc libautumn.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L./lib -lautumn

clean:
	rm -rf lib objs
	$(MAKE) -C unitest clean

.PHONY:all
.PHONY:prepare-dep
.PHONY:autumn
.PHONY:unitest
.PHONY:clean
