%option noyywrap
%option yylineno

%{
#include <iostream>
#include "tokens.h"

void error() {

    std::cerr << "Lexer error on line " << yylineno
              << ": illegal character '" << yytext << "'\n";
}
%}

/* ---------- Regular definitions ---------- */

%%

    /* Whitespace */
{WHITESPACE}    ; /* Ignore */
\n              ; /* Ignore */

    /* Single-line comments */
"//"[^\n]*      ; /* Ignore */

    /* Invalid input */
. { error(); }

%%
