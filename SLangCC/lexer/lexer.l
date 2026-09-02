%{
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../parser/parser.tab.h"
#include "../shared/error/error.h"

#define YY_USER_ACTION \
    yylloc.first_line = yylineno; \
    yylloc.last_line = yylineno; \
    yylloc.first_column = 0; \
    yylloc.last_column = 0;
%}

    /* Enable current input line tracker variable. Flex automatically increments */
    /* yylineno every time the scanner matches a newline character (\n). */
%option yylineno

%%
    /* Whitespace */
[ \t\n]+         ;

    /* Comments */

    /* Compound operators */

    /* Arithmetic operators */

    /* Delimiters */

    /* Assignment and comparison operators */

    /* Types */

    /* Control flow and built-ins */

    /* Identifiers */

    /* Literals */

    /* Invalid input */

%%

int yywrap() { return 1; }