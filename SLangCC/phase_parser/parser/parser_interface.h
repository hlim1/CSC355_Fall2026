/**
 * parser_interface.h
 * 
 * Parser interface for handling the parsing logic.
 */

#ifndef PARSER_INTERFACE_H
#define PARSER_INTERFACE_H

#include <cstdio>
#include "../shared/ast/node.h"

/** Parser function.
 * 
 * @return The parse result.
 */
extern int yyparse();

/** Input file pointer. */
extern FILE *yyin;

/** Abstract syntax tree pointer. */
extern Node *ast;

#endif // PARSER_INTERFACE_H
