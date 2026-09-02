/**
 * error.h
 *
 * This file defines the Error class, which reports compiler diagnostics.
 */

#ifndef ERROR_H
#define ERROR_H

#include <string>

/**
 * Reports lexer and parser diagnostics.
 *
 * Error keeps shared diagnostic state for the current compiler run, including
 * the optional source file path and a count of reported errors. 
 */
class Error {
    public:
        /** Stores the source file name to include in future diagnostics. */
        static void setSourceFile(const std::string& path);

        /** Returns true once at least one error has been reported. */
        static bool hasErrors();

        /** Reports a parser (syntax) diagnostic at the given source line. */
        static void parser(const std::string& message, int line);

        /** Reports a semantic diagnostic at the given source line. */
        static void semantic(const std::string& message, int line);

        /** Reports a lexer diagnostic at the given source line. */
        static void lexer(const std::string& message, int line);

        /** Returns source file name. */
        static const std::string& getSourceFile();

    private:
        /** Source file name printed with diagnostics when available. */
        static std::string sourceFile;

        /** Number of diagnostics reported during this compiler run. */
        static int errorCount;

        /** Formats and prints one diagnostic, then increments the error count. */
        static void report(const std::string& kind, const std::string& message, int line);
};

#endif
