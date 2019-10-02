CXX=g++
CXXFLAGS=-g -std=c++11 -I./include
LDFLAGS=

SRC=$(notdir $(wildcard src/*.cc))
OBJ=$(patsubst %.cc,objs/%.o,$(SRC))

all:prepare-dep autumn unitest

prepare-dep:
	@mkdir -p objs
	@mkdir -p lib

autumn:libautumn.a
	@mv $^ lib

libautumn.a:$(OBJ)
	ar -r $@ $^ $(LDFLAGS)

objs/%.o:src/%.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)


unitest:
	$(MAKE) -C unitest

clean:
	rm -rf lib objs

.PHONY:all
.PHONY:prepare-dep
.PHONY:autumn
.PHONY:unitest
.PHONY:clean
