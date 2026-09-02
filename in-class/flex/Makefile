CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
FLEX     := flex

TARGET   := lexer
LEXER_L  := lexer.l
DRIVER   := lexer.cpp
TOKENS   := tokens.h
FLEX_OUT := lex.yy.cpp

.PHONY: all clean run

all: $(TARGET)

$(FLEX_OUT): $(LEXER_L) $(TOKENS)
	$(FLEX) -o $(FLEX_OUT) $(LEXER_L)

$(TARGET): $(FLEX_OUT) $(DRIVER) $(TOKENS)
	$(CXX) $(CXXFLAGS) $(FLEX_OUT) $(DRIVER) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(FLEX_OUT)
