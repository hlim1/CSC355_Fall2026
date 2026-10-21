/**
 * symbol.h
 *
 * This file defines the Symbol class, which represents a declared program symbol.
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include "../ast/node.h"
#include "../error/error.h"

#include <string>

// Student starter: method declarations only; implement TODOs in symbol.cpp.
class Symbol {
    public:

        /** Creates a symbol from a declarator node. */
        Symbol(Node *type, Node* decl);

        /** Destructor for the Symbol class. */
        ~Symbol();

        /** Returns the name of the symbol. */
        std::string getName() const;

        /** Returns the declared type of the symbol. */
        std::string getType() const;

        /** Returns the integer value of the symbol if it is an integer variable. */
        int getIntValue() const;

        /** Updates current symbol's scope. */
        void updateScope(const std::string& scope);

        /** Updates the array size symbol. */
        void updateArraySizeSymbol(const Symbol* symbol);

        /** Returns the line number where the symbol is defined. */
        int getLine() const;

        /** Returns the scope of the symbol. */
        std::string getScope() const;

        /** Returns the name of the symbol for the array size if it is a variable. */
        std::string getArraySizeSymbolName() const;

        /** Returns the list of parameters for the function if the symbol is a function. */
        std::vector<Symbol*> getParameters() const;

        /** Returns true if the symbol is an array, false otherwise. */
        bool isArraySymbol() const;

        /** Returns the resolved number of elements in this array. */
        int getArraySize() const;

        /** Prints all symbol fields for debugging. */
        void printSymbol() const;
    private:
        /** Current symbol's name. */
        std::string name;

        /** Type (int, double, character, boolean, or string) of the symbol. */
        std::string type;
        
        /** Active when ast node's `valueType == ValueType::Int`. */
        int intValue;

        /** Active when ast node's `valueType == ValueType::Double`. */
        double doubleValue;

        /** Active when ast node's `valueType == ValueType::Char`. */
        char charValue;

        /** Active when ast node's `valueType == ValueType::Bool`. */
        bool boolValue;

        /** Active when `valueType == ValueType::String`. */
        std::string stringValue;

        /** Scope of the current symbol. Function name if variable or program if a function.*/
        std::string scope;

        /** Line number in the source file where the symbol is defined. */
        int line;

        /** Size of the array if the symbol is an array. */
        int arraySizeValue;

        /** Symbol for the array size if it is a variable. */
        const Symbol* arraySizeSymbol;

        /** Name of the symbol for the array size if it is a variable. */
        std::string arraySizeSymbolName;

        /** True if the symbol is an array, false otherwise. */
        bool isArray;

        /** True if the symbol is a function, false otherwise. */
        bool isFunction;

        /** List of parameters for the function if the symbol is a function. */
        std::vector<Symbol*> parameters;
};

#endif