%option noyywrap
%option yylineno

%{
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "parser.tab.hpp"

namespace {

char *copyText(const char *text) {
    const std::size_t length = std::strlen(text) + 1;
    char *result = static_cast<char *>(std::malloc(length));
    if (result != nullptr) {
        std::memcpy(result, text, length);
    }
    return result;
}

} // namespace

void error() {

    std::cerr << "Lexer error on line " << yylineno
              << ": illegal character '" << yytext << "'\n";
}
%}

/* ---------- Regular definitions ---------- */

DIGIT       [0-9]
LETTER      [A-Za-z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INTEGER     {DIGIT}+
WHITESPACE  [ \t\r]+

%%

    /* Keywords */
"int"           { return T_INT; }
"if"            { return T_IF; }
"else"          { return T_ELSE; }
"while"         { return T_WHILE; }
"return"        { return T_RETURN; }

    /* Identifiers and integer literals */
{ID}            { yylval.text = copyText(yytext); return T_ID; }
{INTEGER}       { yylval.intValue = std::atoi(yytext); return T_INT_LITERAL; }

    /* Arithmetic operators */
"+"             { return T_PLUS; }
"-"             { return T_MINUS; }

    /* Delimiters */
"("             { return T_LPAREN; }
")"             { return T_RPAREN; }
"{"             { return T_LBRACE; }
"}"             { return T_RBRACE; }
";"             { return T_SEMICOLON; }
","             { return T_COMMA; }

    /* Assignment and comparison operators */
"="             { return T_ASSIGN; }
"<"             { return T_LESS; }

    /* Whitespace */
{WHITESPACE}    ; /* Ignore */
\n              ; /* Ignore */

    /* Single-line comments */
"//"[^\n]*      ; /* Ignore */

    /* Invalid input */
. { error(); }

%%
