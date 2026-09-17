CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
FLEX     := flex
BISON    := bison

TARGET   := parser
LEXER_L  := lexer.l
PARSER_Y := parser.y
DRIVER   := parser.cpp
FLEX_OUT := lex.yy.cpp
PARSER_OUT := parser.tab.cpp
PARSER_HDR := parser.tab.hpp

.PHONY: all clean run

all: $(TARGET)

$(PARSER_OUT) $(PARSER_HDR): $(PARSER_Y)
	$(BISON) -d -o $(PARSER_OUT) $(PARSER_Y)

$(FLEX_OUT): $(LEXER_L) $(PARSER_HDR)
	$(FLEX) -o $(FLEX_OUT) $(LEXER_L)

$(TARGET): $(PARSER_OUT) $(FLEX_OUT) $(DRIVER)
	$(CXX) $(CXXFLAGS) $(PARSER_OUT) $(FLEX_OUT) $(DRIVER) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(FLEX_OUT) $(PARSER_OUT) $(PARSER_HDR)
