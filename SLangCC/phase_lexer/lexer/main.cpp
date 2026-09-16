#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../shared/error/error.h"
#include "../parser/parser.tab.h"

extern int yylex();
extern FILE *yyin;
extern char *yytext;

YYSTYPE yylval;
YYLTYPE yylloc;

namespace {

    /** Get the name of a token.
     * 
     * @param token The token to get the name of.
     * @return The name of the token.
     */
    const char *tokenName(int token) {
        switch (token) {
            case T_INT_LITERAL: return "T_INT_LITERAL";
            case T_DECIMAL_LITERAL: return "T_DECIMAL_LITERAL";
            case T_ID: return "T_ID";
            case T_STRING_LITERAL: return "T_STRING_LITERAL";
            case T_CHARACTER_LITERAL: return "T_CHARACTER_LITERAL";
            case T_PLUS: return "T_PLUS";
            case T_MINUS: return "T_MINUS";
            case T_TIMES: return "T_TIMES";
            case T_DIVIDE: return "T_DIVIDE";
            case T_LPAREN: return "T_LPAREN";
            case T_RPAREN: return "T_RPAREN";
            case T_LBRACE: return "T_LBRACE";
            case T_RBRACE: return "T_RBRACE";
            case T_LBRACKET: return "T_LBRACKET";
            case T_RBRACKET: return "T_RBRACKET";
            case T_SEMICOLON: return "T_SEMICOLON";
            case T_COMMA: return "T_COMMA";
            case T_ASSIGN: return "T_ASSIGN";
            case T_PLUS_ASSIGN: return "T_PLUS_ASSIGN";
            case T_MINUS_ASSIGN: return "T_MINUS_ASSIGN";
            case T_INCREMENT: return "T_INCREMENT";
            case T_DECREMENT: return "T_DECREMENT";
            case T_LESS: return "T_LESS";
            case T_GREATER: return "T_GREATER";
            case T_LESS_EQUAL: return "T_LESS_EQUAL";
            case T_GREATER_EQUAL: return "T_GREATER_EQUAL";
            case T_EQUAL: return "T_EQUAL";
            case T_NOT_EQUAL: return "T_NOT_EQUAL";
            case T_NOT: return "T_NOT";
            case T_AND: return "T_AND";
            case T_OR: return "T_OR";
            case T_INT: return "T_INT";
            case T_DOUBLE: return "T_DOUBLE";
            case T_BOOLEAN: return "T_BOOLEAN";
            case T_CHARACTER: return "T_CHARACTER";
            case T_STRING: return "T_STRING";
            case T_IF: return "T_IF";
            case T_ELSE: return "T_ELSE";
            case T_ELSEIF: return "T_ELSEIF";
            case T_EXIT: return "T_EXIT";
            case T_PRINT: return "T_PRINT";
            case T_TRUE: return "T_TRUE";
            case T_FALSE: return "T_FALSE";
            case T_FOR: return "T_FOR";
            case T_WHILE: return "T_WHILE";
            case T_RETURN: return "T_RETURN";
            case T_VOID: return "T_VOID";
            case T_CONTINUE: return "T_CONTINUE";
            case T_BREAK: return "T_BREAK";
            default:
                fprintf(stderr, "Lexer error: unknown token (%s)\n", yytext);
                exit(EXIT_FAILURE);
        }
    }

    /** Check if a token has a string value.
     * 
     * @param token The token to check.
     * @return True if the token has a string value, false otherwise.
     */
    bool hasStringValue(int token) {
        return token == T_ID || token == T_STRING_LITERAL || token == T_CHARACTER_LITERAL;
    }

} // namespace

int main(int argc, char **argv) {
    const char *inputPath = argc > 1 ? argv[1] : "test.sl";
    Error::setSourceFile(inputPath);

    yyin = std::fopen(inputPath, "r");
    if (!yyin) {
        std::cerr << "Cannot open " << inputPath << std::endl;
        return 1;
    }

    int token;
    while ((token = yylex()) != 0) {
        std::cout << yytext << " (" << tokenName(token) << ")" << std::endl;

        if (hasStringValue(token)) {
            std::free(yylval.sval);
            yylval.sval = nullptr;
        }
    }

    std::fclose(yyin);
    return Error::hasErrors() ? 1 : 0;
}
