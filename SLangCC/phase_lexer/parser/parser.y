%{
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>

#ifdef PHASE_PARSER
    #include "parser_interface.h"
    #include "../shared/ast/node.h"
#endif

#ifdef PHASE_SEMANTIC
    #include "../shared/symboltable/symbol.h"
    #include "../shared/symboltable/symbolTable.h"
#endif

#include "../shared/error/error.h"

extern int yylex(); // Lexer function that returns the next token.
extern FILE *yyin;  // Input file stream used by the lexer.
int yyerror(const char *s); // Error handling function. See the definition at the bottom of this file.

#ifdef PHASE_PARSER
    Node *ast = nullptr;
#endif

#ifdef PHASE_SEMANTIC
    std::queue<Symbol*> symbols;
    SymbolTable symbolTable;
#endif

/**
 * Converts an escaped character into its actual character value.
 * 
 * This function is used when processing escape sequences inside
 * string or character literals. For example, the sequence '\n'
 * is converted into a newline character.
 * 
 * @param c The escaped character following the backslash.
 * @return The decoded character value.
 */
static char decodeEscape(char c) {
    switch (c) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case 'b': return '\b';
        case 'f': return '\f';
        case '0': return '\0';
        case '\\': return '\\';
        case '"': return '"';
        case '\'': return '\'';
        default: return c;
    }
}

/**
 * Removes surrounding quotes from a string or character literal
 * and decodes any escape sequences inside the literal.
 * 
 * For example, the text "\"hello\\n\"" becomes the string
 * "hello\n" with an actual newline character.
 * 
 * @param text The quoted literal text.
 * @return A decoded std::string without surrounding quotes.
 */
static std::string decodeQuotedLiteral(const char *text) {
    std::string value;
    if (text == nullptr) {
        return value;
    }

    std::size_t length = std::strlen(text);
    std::size_t start = 0;
    std::size_t end = length;

    if (length >= 2 &&
        ((text[0] == '"' && text[length - 1] == '"') ||
         (text[0] == '\'' && text[length - 1] == '\''))) {
        start = 1;
        end = length - 1;
    }

    for (std::size_t i = start; i < end; ++i) {
        if (text[i] == '\\' && i + 1 < end) {
            value.push_back(decodeEscape(text[++i]));
        } else {
            value.push_back(text[i]);
        }
    }

    return value;
}

/**
 * Decodes a character literal and returns its character value.
 * 
 * This function removes surrounding quotes, processes escape
 * sequences, and returns the first decoded character. If the
 * literal is empty, the null character is returned.
 * 
 * @param text The character literal text.
 * @return The decoded character value.
 */
static char decodeCharacterLiteral(const char *text) {
    std::string value = decodeQuotedLiteral(text);
    return value.empty() ? '\0' : value[0];
}

/**
 * Returns whether a node is a valid target for ++ or --.
 *
 * Parenthesized expressions are unwrapped once so constructs like ++(x)
 * and (values[i])-- can be accepted, while computed expressions like
 * ++(x + 1) and (x + 1)++ are rejected.
 *
 * @param node The parsed target node, optionally wrapped in parentheses.
 * @return true when the target is an identifier or array access.
 */
static bool isIncrementDecrementTarget(Node *node) {
    if (node == nullptr) {
        return false;
    }

    Node *target = node;
    if (target->getKind() == "ParenthesizedExpression") {
        const std::vector<Node*>& children = target->getChildren();
        if (children.size() != 1) {
            return false;
        }
        target = children[0];
    }

    return target->getKind() == "Identifier" ||
           target->getKind() == "ArrayAccess";
}
%}

/* Required for tracking token and grammar rule locations in Bison. */
%locations
%define parse.error detailed

%union {
    int ival;
    char *sval;
    double fval;
    class Node *node;
}

/* All declared terminal tokens. */
%token <ival> T_INT_LITERAL
%token <sval> T_ID T_STRING_LITERAL T_CHARACTER_LITERAL
%token <fval> T_DECIMAL_LITERAL

%token T_PLUS T_MINUS T_TIMES T_DIVIDE
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET
%token T_SEMICOLON T_COMMA
%token T_ASSIGN T_PLUS_ASSIGN T_MINUS_ASSIGN T_INCREMENT T_DECREMENT
%token T_LESS T_GREATER T_LESS_EQUAL T_GREATER_EQUAL T_EQUAL T_NOT_EQUAL
%token T_NOT T_AND T_OR
%token T_INT T_DOUBLE T_BOOLEAN T_CHARACTER T_VOID T_STRING
%token T_IF T_ELSE T_ELSEIF T_EXIT T_PRINT T_TRUE T_FALSE
%token T_FOR T_WHILE T_CONTINUE T_BREAK T_RETURN

/* Types of non-terminal tokens */
%type <node> program external_list external_decl function_decl
%type <node> type param_list_opt param_list param
%type <node> block statement_list statement
%type <node> declaration init_var init_array declarator declarator_list
%type <node> return_stmt if_stmt elseif_list elseif_clause else_opt while_stmt for_stmt call_stmt
%type <node> for_init_opt for_cond_opt for_update_opt
%type <node> assignment increment_stmt decrement_stmt print_stmt exit_stmt
%type <node> expression expression_list argument_list_opt argument_list primary unary paren_expression
%type <node> opvalue bracket_value cast_expression cast_value

/* Associativity and Precedence (later declaration has higher precedence) */
%left T_OR
%left T_AND
%left T_EQUAL T_NOT_EQUAL
%left T_LESS T_GREATER T_LESS_EQUAL T_GREATER_EQUAL
%left T_PLUS T_MINUS
%left T_TIMES T_DIVIDE
%right T_NOT UMINUS UPLUS

%start program

%%

program: 
    external_list
    {
        ast = new Node("Program", $1, @1.first_line);

        $$ = ast;
    }
    ;

external_list: 
    external_list external_decl
    | empty
    ;

external_decl:
    function_decl
    | statement
    ;

function_decl: 
    type T_ID T_LPAREN param_list_opt T_RPAREN block
    ;

type: 
    T_INT
    | T_DOUBLE    
    | T_BOOLEAN   
    | T_CHARACTER 
    | T_VOID      
    | T_STRING    
    ;

param_list_opt: 
    param_list
    | empty
    ;

param_list:
    param
    | param_list T_COMMA param
    ;

param: 
    type T_ID
    ;

block:
    T_LBRACE statement_list T_RBRACE
;

statement_list:
    statement_list statement
    | empty
    ;

statement: 
    declaration T_SEMICOLON 
    | assignment T_SEMICOLON
    | increment_stmt T_SEMICOLON
    | decrement_stmt T_SEMICOLON
    | call_stmt T_SEMICOLON
    | paren_expression T_SEMICOLON
    | return_stmt T_SEMICOLON
    | opvalue T_SEMICOLON
    | if_stmt
    | while_stmt
    | for_stmt
    | print_stmt T_SEMICOLON
    | exit_stmt T_SEMICOLON
    | T_CONTINUE T_SEMICOLON
    | T_BREAK T_SEMICOLON
    | block
    ;

declaration:
    type declarator_list
    ;

declarator_list:
    declarator
    | declarator_list T_COMMA declarator
    | T_ID T_LBRACKET bracket_value T_RBRACKET init_array
    ;

declarator:
    T_ID init_var
    ;

init_var:
    T_ASSIGN expression
    | T_ASSIGN cast_expression
    /* empty is a parsing error, as the compiler does not allow non-initialized variable */
    ;
  
init_array:
    T_ASSIGN T_LBRACE expression_list T_RBRACE
    | empty
    ;

expression_list:
    expression
    | expression_list T_COMMA expression
    ;
    
assignment:
    opvalue T_ASSIGN expression
    | opvalue T_PLUS_ASSIGN expression
    | opvalue T_MINUS_ASSIGN expression
    ;

return_stmt:
    T_RETURN expression
    | T_RETURN
    ;

print_stmt:
    T_PRINT T_LPAREN argument_list_opt T_RPAREN
    ;

exit_stmt:
    T_EXIT T_LPAREN argument_list_opt T_RPAREN
    ;

if_stmt:
    T_IF T_LPAREN expression T_RPAREN block elseif_list else_opt
    ;
    
elseif_list:
    elseif_list elseif_clause
    | empty
    ;

elseif_clause:
    T_ELSEIF T_LPAREN expression T_RPAREN block
    ;

else_opt:
    T_ELSE block
    | empty
    ;

while_stmt:
    T_WHILE T_LPAREN expression T_RPAREN block
    ;

for_stmt: 
    T_FOR T_LPAREN for_init_opt T_SEMICOLON for_cond_opt T_SEMICOLON for_update_opt T_RPAREN block
    ;

for_init_opt: 
    declaration
    | assignment
    | increment_stmt
    | decrement_stmt
    | expression
    | empty
    ;

for_cond_opt:
    expression
    | empty
    ;

for_update_opt:
    assignment
    | increment_stmt
    | decrement_stmt
    | expression
    | empty 
    ;

call_stmt:
    T_ID T_LPAREN argument_list_opt T_RPAREN
    ;

increment_stmt:
    T_INCREMENT opvalue
    | opvalue T_INCREMENT
    | T_INCREMENT paren_expression
    | paren_expression T_INCREMENT
    | T_LPAREN T_INCREMENT opvalue T_RPAREN
    | T_LPAREN opvalue T_INCREMENT T_RPAREN
    ;

decrement_stmt:
    T_DECREMENT opvalue
    | opvalue T_DECREMENT
    |
    T_DECREMENT paren_expression
    | paren_expression T_DECREMENT
    | T_LPAREN T_DECREMENT opvalue T_RPAREN
    | T_LPAREN opvalue T_DECREMENT T_RPAREN
    ;

opvalue:
    T_ID
    | T_ID T_LBRACKET bracket_value T_RBRACKET
  ;

bracket_value:
    T_ID
    | T_INT_LITERAL
    ;

paren_expression:
    T_LPAREN expression T_RPAREN
    | T_LPAREN assignment T_RPAREN
    ;

expression: 
    expression T_OR expression
    | expression T_AND expression
    | expression T_EQUAL expression
    | expression T_NOT_EQUAL expression
    | expression T_LESS expression
    | expression T_GREATER expression
    | expression T_LESS_EQUAL expression
    | expression T_GREATER_EQUAL expression
    | expression T_PLUS expression
    | expression T_MINUS expression
    | expression T_TIMES expression
    | expression T_DIVIDE expression
    | unary
    | call_stmt
    ;

cast_expression:
    T_LPAREN type T_RPAREN cast_value
    ;

cast_value:
    T_ID
    | T_INT_LITERAL
    | T_DECIMAL_LITERAL
    | T_CHARACTER_LITERAL
    ;

unary:
    T_NOT unary
    | T_MINUS unary %prec UMINUS
    | T_PLUS unary %prec UPLUS
    | primary
    ;

primary:
    opvalue 
    | T_INT_LITERAL
    | T_DECIMAL_LITERAL
    | T_STRING_LITERAL
    | T_CHARACTER_LITERAL
    | T_TRUE
    | T_FALSE
    | T_LPAREN expression T_RPAREN
    ;

argument_list_opt:
    argument_list
    | empty
    ;

argument_list:
    expression
    | argument_list T_COMMA expression
    ;

empty:
    ;

%%

int yyerror(const char *s) {
    Error::parser(s, yylloc.first_line);
    return 1;
}
