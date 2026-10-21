/**
 * symbolTable.h
 *
 * This file defines the SymbolTable class, which manages program symbols by scope.
 */

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

#include "../ast/node.h"
#include "symbol.h"

// Student starter: method declarations only; implement TODOs in symbolTable.cpp.
class SymbolTable {
    public:
        /** Constructor for the SymbolTable class. */
        SymbolTable();

        /** Destructor for the SymbolTable class. */
        ~SymbolTable();

        /** Inserts a new function symbol into the table. */
        void insertSymbol(const std::string& scope, int line, Symbol* symbol);

        /** Inserts a new variable symbol into the tables. */
        void insertSymbol(const std::string& scope, Symbol* symbol);

        /** Finds a symbol by scope and name. */
        const Symbol* lookup(const std::string& scope, const std::string& name, int line) const;

        /** Prints the entire symbol table for debugging. */
        void printSymbolTable() const;

    private:
        /** Map of symbol names to Symbol objects. */
        std::map<std::string, std::vector<Symbol*>> symbolTable;
};

#endif
