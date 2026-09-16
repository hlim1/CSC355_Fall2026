# Phase: Lexer

## Learning Objective

Implement a lexical analyzer for SLangCC using Flex. By completing this phase, you will be able to translate a language specification into regular expressions and scanner actions, recognize keywords, identifiers, literals, operators, delimiters, and comments, apply longest-match and rule-priority behavior, associate lexemes with token values, and report invalid characters with useful source-location information.

## Starter Files

- `lexer/lexer.h`: declares the scanner entry point and input stream used by the lexer driver.
- `lexer/lexer.l`: contains the Flex patterns and actions that recognize SLangCC tokens. **This is the only file students are expected to edit during this phase.**
- `lexer/main.cpp`: provides the standalone driver that opens a source file, invokes the lexer, and prints the recognized tokens.
- `lexer/Makefile`: generates the scanner from `lexer.l` and builds the standalone lexer executable.
- `shared/error/error.h`: declares the shared error-reporting interface used to report invalid lexer input.
- `shared/error/error.cpp`: implements diagnostic formatting, source-file tracking, and error counting.
- `parser/parser.y`: defines the Bison parser grammar, token names, and semantic-value types shared with the lexer During this phase, use it as the authoritative reference for the tokens and values that `lexer/lexer.l` must return; do not modify it.

Students must not modify any starter file other than `lexer/lexer.l`. The remaining files provide the fixed interfaces, driver, build configuration, and error-reporting support used to compile and evaluate the lexer.

## Description

In this phase, you will complete the lexical analyzer in `lexer/lexer.l`. The starter file already contains the required C/C++ includes, Flex configuration, source-location tracking, whitespace handling, and labeled sections for the remaining token categories.

Your task is to add the Flex regular expressions and actions needed to recognize SLangCC source text. 

## Program Requirements

The lexer must...
- ignore comments and whitespace,
- return the appropriate token for each valid lexeme,
- store semantic values for identifiers and literals, and 
- report invalid characters with their source line.

The complete set of token names and semantic-value types is declared in `parser/parser.y`.

## Tests

From the `lexer/lexer` directory, build the standalone lexer:

```bash
make
```

A successful build creates the `lexer/lexer` executable. If compilation fails, read the first reported error, correct your rules or actions in `lexer/lexer.l`, and rebuild.

The lexer test inputs are located in `benchmark/lexer/`. To run one test and print its tokens or diagnostics in the terminal, pass the source-file path to the executable:

```bash
<path>/<to>/lexer <path>/<to>/benchmark/lexer/01_keywords_all.sl
```

To save the output for inspection, redirect both standard output and standard error to a file:

```bash
<path>/<to>/lexer/lexer <path>/<to>/benchmark/lexer/01_keywords_all.sl > lexer-output.txt 2>&1
```

To run the complete local test suite, enter the benchmark directory and execute its test script:

```bash
cd benchmark/lexer
./test.sh
```

The script runs every `.sl` test, stores the generated results in `benchmark/lexer/out/`, and compares them with the reference files in `benchmark/lexer/expected/`. It prints a pass/fail summary and shows a diff when a test does not match the expected output.

## Hint

### Error Handling
Use `Error::lexer` to report a character that does not match any valid SLangCC token. The function accepts an error-message string and the source line number:

```cpp
Error::lexer(message, lineNumber);
```

Its declaration is in `shared/error/error.h`, and its implementation is in `shared/error/error.cpp`. These files are already included and linked by the lexer starter project.

Within a Flex rule action:

- `yytext` refers to the text matched by the current rule. Its contents are temporary and may be replaced when the scanner finds the next match.
- `yylineno` contains the current input line number. Flex maintains this variable because the starter file enables `%option yylineno`.

Use these values to construct a useful diagnostic message and identify the line on which invalid input occurs. For more information, see the official Flex documentation for 
[`yytext`](https://westes.github.io/flex/manual/User-Values.html#:~:text=char%20*yytext,%2D%2B%E2%80%99%20flag)
and the 
[`yylineno`](https://westes.github.io/flex/manual/Options-Affecting-Scanner-Behavior.html#:~:text=%E2%80%98%2D%2Dyylineno%2C,yylineno%20is%20enabled).

### Identifiers and Comments in SLangCC

An identifier must begin with an English letter or an underscore. After the first character, it may also contain digits. Reserved words such as `int`, `if`, and `return` must be recognized as keywords rather than identifiers.

SLangCC supports two comment forms. A single-line comment begins with `#` and continues to the end of the line. A multiline comment begins with `#{` and ends with `}#`. Comments are ignored by the lexer and do not produce tokens.

See the [SLangCC syntax documentation](https://slangcc.com/language/syntax/) for more detail.