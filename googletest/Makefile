CXXFLAGS=-g -std=c++17 -Werror -I./include
LDFLAGS=

SRC=$(notdir $(wildcard src/*.cc))
OBJ=$(patsubst %.cc,objs/%.o,$(SRC))
HEADERS=$(wildcard include/*.h)

all:prepare-dep libgtest

prepare-dep:
	@mkdir -p objs
	@mkdir -p lib

libgtest:libgtest.a
	@mv $^ lib

libgtest.a:$(OBJ)
	ar -r $@ $^

objs/%.o:src/%.cc $(HEADERS)
	$(CXX) -o $@ -c $< $(CXXFLAGS)


clean:
	rm -rf lib objs

.PHONY:all
.PHONY:libgtest
.PHONY:prepare-dep
.PHONY:clean
