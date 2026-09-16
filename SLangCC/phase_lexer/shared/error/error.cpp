#include "error.h"

#include <iostream>

std::string Error::sourceFile;
int Error::errorCount = 0;

/** Stores the source file name used by future diagnostics.
-
- @param path The source path; only its file name is retained.
- @return void.
 */
void Error::setSourceFile(const std::string& path) {
    std::string::size_type separator = path.find_last_of("/\\");
    sourceFile = separator == std::string::npos ? path : path.substr(separator + 1);
}

/** Checks whether any diagnostics have been emitted.
-
- @return True if at least one error has been reported, false otherwise.
 */
bool Error::hasErrors() {
    return errorCount > 0;
}

/** Reports a parser diagnostic.
-
- @param message The diagnostic details, or an empty string for a generic message.
- @param line The source line where the error occurred.
- @return void.
 */
void Error::parser(const std::string& message, int line) {
    if (message.empty() || message == "syntax error") {
        report("Syntax error", "", line);
    } else {
        report("Syntax error", message, line);
    }
}

/** Reports a semantic diagnostic.
-
- @param message The diagnostic details, or an empty string for a generic message.
- @param line The source line where the error occurred.
- @return void.
 */
void Error::semantic(const std::string& message, int line) {
    if (message.empty() || message == "syntax error") {
        report("Semantic error", "", line);
    } else {
        report("Semantic error", message, line);
    }
}

/** Reports a lexer diagnostic.
-
- @param message The diagnostic details.
- @param line The source line where the error occurred.
- @return void.
 */
void Error::lexer(const std::string& message, int line) {
    report("Lexer error", message, line);
}

/** Prints a formatted diagnostic and records that an error occurred.
-
- @param kind The diagnostic category to print.
- @param message The optional diagnostic details.
- @param line The source line where the error occurred, or a non-positive value to omit it.
- @return void.
 */
void Error::report(const std::string& kind, const std::string& message, int line) {
    ++errorCount;

    std::cerr << kind;

    if (!sourceFile.empty()) {
        std::cerr << " in " << sourceFile;
    }

    if (line > 0) {
        std::cerr << " at line " << line;
    }

    if (!message.empty()) {
        std::cerr << ": " << message;
    }

    std::cerr << '\n';
}

/** Gets the source file name used in diagnostics.
-
- @return A reference to the stored source file name.
 */
const std::string& Error::getSourceFile() {
    return sourceFile;
}