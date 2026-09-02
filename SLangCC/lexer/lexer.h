/**
 * lexer.h
 * 
 * Lexer header file for the project.
 */

#ifndef LEXER_H
#define LEXER_H

#include <cstdio>

/** Lexical analyzer function.
 * 
 * @return The token type.
 */
int yylex();

/** Input file pointer. */
extern FILE *yyin;

#endif
