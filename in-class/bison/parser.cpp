#include <cstdio>
#include <cstdlib>
#include <iostream>

extern FILE *yyin;
extern char *yytext;
extern int yylineno;

int yyparse(void);

void yyerror(const char *message) {
    std::cerr << "Parser error on line " << yylineno << ": " << message;
    if (yytext && *yytext != '\0') {
        std::cerr << " near '" << yytext << "'";
    }
    std::cerr << '\n';
}

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [input-file]\n";
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        yyin = std::fopen(argv[1], "r");
        if (!yyin) {
            std::cerr << "Cannot open " << argv[1] << '\n';
            return EXIT_FAILURE;
        }
    } else {
        yyin = stdin;
    }

    const int result = yyparse();

    if (yyin != stdin) {
        std::fclose(yyin);
    }

    if (result == 0) {
        std::cout << "Parse successful\n";
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
