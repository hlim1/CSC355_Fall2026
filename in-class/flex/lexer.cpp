#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "tokens.h"

extern int yylex();
extern FILE *yyin;
extern char *yytext;

namespace {

    /** Get the name of a token.
     *
     * @param token The token to get the name of.
     * @return The name of the token.
     */
    const char *tokenName(int token) {
        switch (token) {
            case T_INT_LITERAL:   return "T_INT_LITERAL";
            case T_ID:            return "T_ID";
            case T_PLUS:          return "T_PLUS";
            case T_MINUS:         return "T_MINUS";
            case T_TIMES:         return "T_TIMES";
            case T_DIVIDE:        return "T_DIVIDE";
            case T_LPAREN:        return "T_LPAREN";
            case T_RPAREN:        return "T_RPAREN";
            case T_LBRACE:        return "T_LBRACE";
            case T_RBRACE:        return "T_RBRACE";
            case T_SEMICOLON:     return "T_SEMICOLON";
            case T_COMMA:         return "T_COMMA";
            case T_ASSIGN:        return "T_ASSIGN";
            case T_LESS:          return "T_LESS";
            case T_GREATER:       return "T_GREATER";
            case T_LESS_EQUAL:    return "T_LESS_EQUAL";
            case T_GREATER_EQUAL: return "T_GREATER_EQUAL";
            case T_EQUAL:         return "T_EQUAL";
            case T_NOT_EQUAL:     return "T_NOT_EQUAL";
            case T_INT:           return "T_INT";
            case T_IF:            return "T_IF";
            case T_ELSE:          return "T_ELSE";
            case T_WHILE:         return "T_WHILE";
            case T_RETURN:        return "T_RETURN";
            default:
                std::cerr << "Lexer error: unknown token (" << yytext << ")\n";
                std::exit(EXIT_FAILURE);
        }
    }

} // namespace

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [input-file]\n";
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        yyin = std::fopen(argv[1], "r");
        if (!yyin) {
            std::cerr << "Cannot open " << argv[1] << '\n';
            return EXIT_FAILURE;
        }
    } else {
        yyin = stdin;
    }

    int token;
    while ((token = yylex()) != 0) {
        std::cout << yytext << " (" << tokenName(token) << ")\n";
    }

    if (yyin != stdin) {
        std::fclose(yyin);
    }

    return EXIT_SUCCESS;
}
