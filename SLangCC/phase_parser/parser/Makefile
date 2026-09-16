CXX = g++
LEX = flex
YACC = bison
CXXFLAGS = -std=c++17 -Wall -g -O0

PHASE := "PARSER"

ifeq ($(PHASE),"LEXER")
	CPPFLAGS = -DPHASE_LEXER
else ifeq ($(PHASE),"PARSER")
	CPPFLAGS = -DPHASE_LEXER -DPHASE_PARSER
else ifeq ($(PHASE),"SEMANTIC")
	CPPFLAGS = -DPHASE_LEXER -DPHASE_PARSER -DPHASE_SEMANTIC
endif

TARGET = parser

LEX_SRC = ../lexer/lexer.l
YACC_SRC = parser.y
MAIN = main.cpp
NODE = ../shared/ast/node.cpp
ERROR = ../shared/error/error.cpp
ERROR_HEADER = ../shared/error/error.h
DEBUG = parser.dSYM

LEX_OUT = lex.yy.c
YACC_OUT = parser.tab.c
YACC_HDR = parser.tab.h

all: $(TARGET)

$(TARGET): $(LEX_OUT) $(YACC_OUT) $(MAIN) $(NODE) $(ERROR) $(ERROR_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $(TARGET) $(MAIN) $(NODE) $(ERROR) $(YACC_OUT) $(LEX_OUT)

$(YACC_OUT) $(YACC_HDR): $(YACC_SRC)
	$(YACC) -d -o $(YACC_OUT) $(YACC_SRC)

$(LEX_OUT): $(LEX_SRC) $(YACC_HDR)
	$(LEX) -o $(LEX_OUT) $(LEX_SRC)

clean:
	rm -f $(TARGET) $(LEX_OUT) $(YACC_OUT) $(YACC_HDR)
	rm -rf $(DEBUG)
