#include <iostream>
#include <stdexcept>

#include "symbol.h"

/** Prints all symbol fields to standard output for debugging.
 *
 * @return void.
 */
void Symbol::printSymbol() const {
    std::cout << "name: " << this->name << std::endl;
    std::cout << "type: " << this->type << std::endl;

    if (!this->isArray && this->scope != "global") {
        if (this->type == "int") {
            std::cout << "integer value: " << this->intValue << std::endl;
        }
        else if (this->type == "double") {
            std::cout << "double value: " << this->doubleValue << std::endl;
        }
        else if (this->type == "character") {
            std::cout << "character value: " << this->charValue << std::endl;
        }
        else if (this->type == "boolean") {
            std::cout << "boolean value: " << (this->boolValue ? "true" : "false") << std::endl;
        }
        else if (this->type == "string"){
            std::cout << "string value: " << this->stringValue << std::endl;
        }
        else if (this->type == "void") {
            std::cout << "void type" << std::endl;
        }
        else {
            std::cout << "value: <unknown type>" << std::endl;
        }
    }
    else if (this->isArray && this->arraySizeValue > 0) {
        std::cout << "array size: " << this->arraySizeValue << std::endl;
    }
    else if (this->isArray && this->arraySizeSymbol != nullptr) {
        std::cout << "array size symbol: " << this->arraySizeSymbol->getName() << std::endl;
    }

    if (this->isFunction) {
        std::cout << "function parameters: ";
        for (const Symbol* param : this->parameters) {
            std::cout << param->getName() << " (" << param->getType() << "), ";
        }
        std::cout << std::endl;
    }

    std::cout << "source line: " << this->line << std::endl;
    std::cout << "--------------------------" << std::endl;
}
