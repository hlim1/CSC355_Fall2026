#include "SLangIR.h"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../parser/parser_interface.h"
#include "../semantic_analyzer/semanticAnalyzer.h"
#include "../shared/error/error.h"

namespace {

void usage (const char* program) {
    std::cerr << "Usage: " << program
              << " <source.sl> [output-dir]\n";
}

std::filesystem::path outputPath (
    const char* source,
    const char* directory
) {
    const std::filesystem::path name =
        std::filesystem::path(source).stem().string() + ".ir";

    if (directory == nullptr) {
        return name;
    }

    return std::filesystem::path(directory) / name;
}

}  // namespace

int main (int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    Error::setSourceFile(argv[1]);

    yyin = std::fopen(argv[1], "r");

    if (yyin == nullptr) {
        std::perror(argv[1]);
        return EXIT_FAILURE;
    }

    const int parseResult = yyparse();

    std::fclose(yyin);

    if (parseResult != 0 || Error::hasErrors()) {
        return EXIT_FAILURE;
    }

    if (ast == nullptr) {
        std::cerr << "No AST generated.\n";
        return EXIT_FAILURE;
    }

    SemanticAnalyzer semanticAnalyzer(symbolTable);
    semanticAnalyzer.analyze(ast);

    if (Error::hasErrors()) {
        return EXIT_FAILURE;
    }

    const std::filesystem::path path =
        outputPath(argv[1], argc == 3 ? argv[2] : nullptr);

    std::ofstream file(path);

    if (!file) {
        std::cerr << "Could not open " << path << " for writing.\n";
        return EXIT_FAILURE;
    }

    try {
        SLangIR generator;
        const IRProgram& program = generator.analyze(ast);
        program.print(file);
    } catch (const std::exception& error) {
        std::cerr << "SLangIR error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "IR generation successful.\n"
              << "SLangIR written to " << path << ".\n";

    return EXIT_SUCCESS;
}
