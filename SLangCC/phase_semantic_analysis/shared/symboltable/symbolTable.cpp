#include "symbolTable.h"
#include "symbol.h"
#include "../error/error.h"

#include <stdexcept>

/** Prints the entire symbol table to standard output for debugging.
 *
 * @return void.
 */
void SymbolTable::printSymbolTable() const {
    std::cout << std::endl;
    std::cout << "Symbol Table:" << std::endl;
    for (const auto& entry : symbolTable) {
        const std::string& scope = entry.first;
        const std::vector<Symbol*>& symbols = entry.second;

        std::cout << "Scope: " << scope << std::endl;
        for (const Symbol* symbol : symbols) {
            symbol->printSymbol();
        }
    }
}
