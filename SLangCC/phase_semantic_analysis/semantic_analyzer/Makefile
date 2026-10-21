CXX = g++
LEX = flex
YACC = bison

CXXFLAGS = -std=c++17 -Wall -g -O0

PHASE := "SEMANTIC"

ifeq ($(PHASE),"LEXER")
	CPPFLAGS = -DPHASE_LEXER
else ifeq ($(PHASE),"PARSER")
	CPPFLAGS = -DPHASE_LEXER -DPHASE_PARSER
else ifeq ($(PHASE),"SEMANTIC")
	CPPFLAGS = -DPHASE_LEXER -DPHASE_PARSER -DPHASE_SEMANTIC
endif

TARGET = semantic_analyzer

PARSER_DIR = ../parser
LEXER_DIR = ../lexer
SHARED_DIR = ../shared

YACC_SRC = $(PARSER_DIR)/parser.y
YACC_OUT = $(PARSER_DIR)/parser.tab.c
YACC_HDR = $(PARSER_DIR)/parser.tab.h
LEX_SRC = $(LEXER_DIR)/lexer.l
LEX_OUT = $(PARSER_DIR)/lex.yy.c

SOURCES = main.cpp \
    semanticAnalyzer.cpp \
    $(SHARED_DIR)/ast/node.cpp \
    $(SHARED_DIR)/error/error.cpp \
    $(SHARED_DIR)/symboltable/symbol.cpp \
    $(SHARED_DIR)/symboltable/symbolTable.cpp \
    $(YACC_OUT) \
    $(LEX_OUT)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

$(YACC_OUT) $(YACC_HDR): $(YACC_SRC)
	$(YACC) -d -o $(YACC_OUT) $(YACC_SRC)

$(LEX_OUT): $(LEX_SRC) $(YACC_HDR)
	$(LEX) -o $(LEX_OUT) $(LEX_SRC)

clean:
	rm -f $(TARGET)