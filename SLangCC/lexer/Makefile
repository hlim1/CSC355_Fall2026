CXX = g++
LEX = flex
BISON = bison
CXXFLAGS ?= -std=c++11 -w

PHASE := "LEXER"

ifeq ($(PHASE),"LEXER")
	CPPFLAGS += -DPHASE_LEXER
else ifeq ($(PHASE),"PARSER")
	CPPFLAGS += -DPHASE_LEXER -DPHASE_PARSER
else ifeq ($(PHASE),"SEMANTIC")
	CPPFLAGS += -DPHASE_LEXER -DPHASE_PARSER -DPHASE_SEMANTIC
endif

TARGET = lexer
TEST_INPUT = tests/test1.sl
PARSER_GRAMMAR = ../parser/parser.y
PARSER_SOURCE = ../parser/parser.tab.c
PARSER_HEADER = ../parser/parser.tab.h
ERROR_SOURCE = ../shared/error/error.cpp
ERROR_HEADER = ../shared/error/error.h

.PHONY: all run clean

all: $(TARGET)

$(TARGET): lexer.o main.o error.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $(TARGET) lexer.o main.o error.o

lexer.o: lexer.c $(PARSER_HEADER) $(ERROR_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c lexer.c -o lexer.o

main.o: main.cpp $(PARSER_HEADER) $(ERROR_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c main.cpp -o main.o

error.o: $(ERROR_SOURCE) $(ERROR_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(ERROR_SOURCE) -o error.o

lexer.c: lexer.l $(PARSER_HEADER)
	$(LEX) -o lexer.c lexer.l

$(PARSER_HEADER): $(PARSER_GRAMMAR)
	$(BISON) -Wno-other -d -o $(PARSER_SOURCE) $(PARSER_GRAMMAR)

run: $(TARGET)
	./$(TARGET) $(TEST_INPUT)

clean:
	rm -f $(TARGET) lexer_test lexer.o main.o error.o lexer.c
	rm -f $(PARSER_SOURCE) $(PARSER_HEADER)
