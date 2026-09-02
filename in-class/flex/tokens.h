#ifndef TOKENS_H
#define TOKENS_H

enum Token {
    T_INT_LITERAL = 256,
    T_ID,
    T_PLUS,
    T_MINUS,
    T_TIMES,
    T_DIVIDE,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_COMMA,
    T_ASSIGN,
    T_LESS,
    T_GREATER,
    T_LESS_EQUAL,
    T_GREATER_EQUAL,
    T_EQUAL,
    T_NOT_EQUAL,
    T_INT,
    T_IF,
    T_ELSE,
    T_WHILE,
    T_RETURN
};

#endif
