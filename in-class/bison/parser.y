%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

int yylex(void);
void yyerror(const char *message);

namespace {

char *copyText(const char *text) {
    const std::size_t length = std::strlen(text) + 1;
    char *result = static_cast<char *>(std::malloc(length));
    if (result != nullptr) {
        std::memcpy(result, text, length);
    }
    return result;
}

char *makeBinaryExpression(const char *left, const char *op, const char *right) {
    const std::size_t length =
        std::strlen(left) + std::strlen(op) + std::strlen(right) + 5;
    char *result = static_cast<char *>(std::malloc(length));
    if (result != nullptr) {
        std::snprintf(result, length, "(%s %s %s)", left, op, right);
    }
    return result;
}

char *makeUnaryExpression(const char *op, const char *value) {
    const std::size_t length = std::strlen(op) + std::strlen(value) + 3;
    char *result = static_cast<char *>(std::malloc(length));
    if (result != nullptr) {
        std::snprintf(result, length, "(%s%s)", op, value);
    }
    return result;
}

} // namespace
%}

%define parse.error verbose

%union {
    int intValue;
    char *text;
}

%token <intValue> T_INT_LITERAL
%token <text> T_ID
%token T_PLUS
%token T_MINUS
%token T_LPAREN
%token T_RPAREN
%token T_LBRACE
%token T_RBRACE
%token T_SEMICOLON
%token T_COMMA
%token T_ASSIGN
%token T_LESS
%token T_INT
%token T_IF
%token T_ELSE
%token T_WHILE
%token T_RETURN

%type <text> expression
%destructor { std::free($$); } <text>

%nonassoc LOWER_THAN_ELSE
%nonassoc T_ELSE

%left T_LESS
%left T_PLUS T_MINUS
%right UMINUS

%start program

%%

program
    : statement_list
    ;

statement_list
    : /* empty */
    | statement_list statement
    ;

statement
    : declaration_statement 
      {
        std::cout << "declaration statement" << std::endl;
      }
    | assignment_statement 
      { 
        std::cout << "assignment statement" << std::endl;
      }
    | if_statement 
      {
        std::cout << "if statement" << std::endl;
      }
    | while_statement 
      {
        //COMPLETE ME
      }
    | return_statement 
      { 
        //COMPLETE ME
      }
    | block 
      {
        //COMPLETE ME
      }
    ;

block
    : T_LBRACE statement_list T_RBRACE
    ;

declaration_statement
    : T_INT declaration_list T_SEMICOLON
    ;

declaration_list
    : declaration_item
    | declaration_list T_COMMA declaration_item
    ;

declaration_item
    : T_ID
      {
          std::cout << "declared variable: " << $1 << std::endl;
          std::free($1);
      }
    | T_ID T_ASSIGN expression
      {
          // COMPLETE ME

          std::free($1);
          std::free($3);
      }
    ;

assignment_statement
    : T_ID T_ASSIGN expression T_SEMICOLON
      {
          std::cout << "assignment uses $1 = " << $1
                    << " and $3 = " << $3 << std::endl;
          std::free($1);
          std::free($3);
      }
    ;

if_statement
    : T_IF T_LPAREN expression T_RPAREN statement %prec LOWER_THAN_ELSE
      {
          std::cout << "if condition uses $3 = " << $3 << std::endl;
          std::free($3);
      }
    | T_IF T_LPAREN expression T_RPAREN statement T_ELSE statement
      {
          // COMPLETE ME

          std::free($3);
      }
    ;

while_statement
    : T_WHILE T_LPAREN expression T_RPAREN statement
      {
          // COMPLETE ME

          std::free($3);
      }
    ;

return_statement
    : T_RETURN expression T_SEMICOLON
      {
          std::cout << "returning expression: " << $2 << std::endl;
          std::free($2);
      }
    ;

expression
    : expression T_PLUS expression
      {
          $$ = makeBinaryExpression($1, "+", $3);
          std::free($1);
          std::free($3);
      }
    | expression T_MINUS expression
      {
          $$ = makeBinaryExpression($1, "-", $3);
          std::free($1);
          std::free($3);
      }
    | expression T_LESS expression
      {
          // COMPLETE ME

          std::free($1);
          std::free($3);
      }
    | T_MINUS expression %prec UMINUS
      {
          // COMPLETE ME
          
          std::free($2);
      }
    | T_LPAREN expression T_RPAREN
      {
          $$ = $2;
      }
    | T_ID
      {
          $$ = $1;
      }
    | T_INT_LITERAL
      {
          char buffer[32];
          std::snprintf(buffer, sizeof(buffer), "%d", $1);
          $$ = copyText(buffer);
      }
    ;

%%
