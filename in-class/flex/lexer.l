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

DIGIT       [0-9]

%%

"int"           { return T_INT; }
"if"            { return T_IF; }

    /* Whitespace */
{WHITESPACE}    ; /* Ignore */
\n              ; /* Ignore */

    /* Single-line comments */
"//"[^\n]*      ; /* Ignore */

    /* Invalid input */
. { error(); }

%%
