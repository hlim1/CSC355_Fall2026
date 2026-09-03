SLangCC Lecture Support Files
============================

Valid programs
--------------
01_hello.sl             Basic compile/run demonstration.
02_types.sl             Fundamental scalar types and initialization.
03_expressions.sl       Arithmetic precedence, comparison, and logic.
04_if_else.sl           Brace-delimited conditional statement.
05_while.sl             Basic while loop.
06_functions.sl         Functions, parameters, returns, function calls.
07_recursion.sl         Recursive function example.
08_arrays.sl            One-dimensional array initialization/access.
09_comments.sl          SLang # and #{ ... }# comments.
10_lexer_stress.sl      Keyword-vs-identifier and <= longest-match examples.
11_explicit_cast.sl     Explicit casts supported by the language.

Intentional error programs
--------------------------
12_invalid_uninitialized.sl  Scalar variable without initializer.
13_invalid_type_mismatch.sl  int literal used where double is required.
14_invalid_missing_braces.sl Control-flow body without braces.
15_invalid_comment_style.sl  Uses // instead of SLang's # comment syntax.
16_invalid_character.sl      Contains @ to exercise lexical error handling.

Suggested lecture use
---------------------
Compile a valid file with:
    ./slangcc 01_hello.sl

Run the resulting executable with:
    ./01_hello

The invalid files are intended to demonstrate diagnostics. Their exact
messages may depend on the compiler build, but each file deliberately violates
one documented SLang rule.
