CXXFLAGS=-g -std=c++17 -Werror -fno-access-control -I../googletest/include -I../include
LDFLAGS=-L../googletest/lib -L../lib -lgtest -lpthread -lautumn

ifdef CODECOV
	CXXFLAGS += -coverage
	LDFLAGS += -lgcov
endif

DEPS=../lib/libautumn.a

all:prepare-dep test

prepare-dep:$(DEPS)

test:format_test lexer_test parser_test evaluator_test builtin_test
	@for bin in $^; do AUTUMN_COLOR_OFF=1 ./$$bin; done

format_test:format_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

lexer_test:lexer_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

parser_test:parser_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

evaluator_test:evaluator_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

builtin_test:builtin_test.o $(DEPS)
	$(CXX) -o $@ $< $(LDFLAGS)

%.o:%.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf *_test *.o *.gcov *.gcno *.gcda

.PHONY:all
.PHONY:prepare-dep
.PHONY:test
.PHONY:clean
