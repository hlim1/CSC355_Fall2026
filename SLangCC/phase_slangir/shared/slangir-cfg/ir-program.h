/**
 * ir-program.h
 *
 * This file defines the IRProgram class, which represents a generated SLangIR program.
 */

#ifndef SLANGIR_CFG_IR_PROGRAM_H
#define SLANGIR_CFG_IR_PROGRAM_H

#include "ir-basic-block.h"
#include "ir-function.h"
#include "ir-instruction.h"

#include <iosfwd>
#include <string>
#include <vector>

/** Owns all functions in a generated SLangIR program. */
class IRProgram {
    public:
        /** Clears all functions and their IR. */
        void clear();
        /** Adds a function to the IR program. */
        IRFunction& addFunction(const std::string& name);
        /** Returns a reference to the vector of functions in the IR program. */
        const std::vector<IRFunction>& getFunctions() const { return functions; }
        /** Returns a string representation of the IR program. */
        std::string toString() const;
        /** Prints the IR program to the specified output stream. */
        void print(std::ostream& out) const;

    private:
        /** The functions in the IR program. */
        std::vector<IRFunction> functions;
};

#endif
