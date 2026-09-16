#include <cstdlib>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "parser_interface.h"
#include "../shared/error/error.h"

int main(int argc, char **argv) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source.sl> [output-dir]\n";
        return EXIT_FAILURE;
    }

    Error::setSourceFile(argv[1]);

    yyin = std::fopen(argv[1], "r");
    if (!yyin) {
        std::perror(argv[1]);
        return EXIT_FAILURE;
    }

    int result = yyparse();
    std::fclose(yyin);

    if (result == 0 && !Error::hasErrors()) {
        std::cout << "Parse successful.\n";

        if (ast) {
            namespace fs = std::filesystem;

            fs::path inputPath(argv[1]);
            fs::path outputName = inputPath.stem().string() + ".json";
            fs::path outputPath = argc == 3
                ? fs::path(argv[2]) / outputName
                : outputName;

            std::ofstream astFile(outputPath);

            if (!astFile) {
                std::cerr << "Could not open "
                          << outputPath
                          << " for writing.\n";
                return EXIT_FAILURE;
            }

            ast->printJSON(astFile, 0);

            std::cout << "AST written to "
                      << outputPath
                      << ".\n";
        }

        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
