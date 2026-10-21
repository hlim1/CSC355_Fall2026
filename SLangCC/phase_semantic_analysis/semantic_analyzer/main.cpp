#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../parser/parser_interface.h"
#include "../shared/error/error.h"
#include "semanticAnalyzer.h"

namespace {

void printUsage(const char* program) {
    std::cerr << "Usage: " << program << " <source.sl> [output-dir]\n";
}

std::filesystem::path astOutputPath(const char* sourcePath, const char* outputDir) {
    namespace fs = std::filesystem;

    fs::path inputPath(sourcePath);
    fs::path outputName = inputPath.stem().string() + ".json";

    return outputDir == nullptr ? outputName : fs::path(outputDir) / outputName;
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* sourcePath = argv[1];
    const char* outputDir = argc == 3 ? argv[2] : nullptr;
    Error::setSourceFile(sourcePath);

    yyin = std::fopen(sourcePath, "r");
    if (yyin == nullptr) {
        std::perror(sourcePath);
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

    SemanticAnalyzer analyzer(symbolTable);
    analyzer.analyze(ast);

    if (Error::hasErrors()) {
        return EXIT_FAILURE;
    }

    const std::filesystem::path outputPath = astOutputPath(sourcePath, outputDir);
    std::ofstream astFile(outputPath);

    if (!astFile) {
        std::cerr << "Could not open " << outputPath << " for writing.\n";
        return EXIT_FAILURE;
    }

    ast->printJSON(astFile, 0);

    std::cout << "Semantic analysis successful.\n";
    std::cout << "AST written to " << outputPath << ".\n";
    return EXIT_SUCCESS;
}