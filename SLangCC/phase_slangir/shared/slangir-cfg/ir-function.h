/**
 * ir-function.h
 *
 * This file defines the IRFunction class, which represents a function in a SLangIR program.
 */

#ifndef SLANGIR_CFG_IR_FUNCTION_H
#define SLANGIR_CFG_IR_FUNCTION_H

#include "ir-basic-block.h"

#include <string>
#include <vector>

class IRFunction {
    public:
        /** Constructs a function with the specified name. */
        explicit IRFunction(std::string name);
        /** Returns the name of the function. */
        const std::string& getName() const { return name; }
        /** Returns a reference to the vector of basic blocks in the function. */
        const std::vector<IRBasicBlock>& getBlocks() const { return blocks; }
        /** Adds a basic block to the function. */
        IRBasicBlock& addBlock(int number);
        /** Rebuilds the control-flow edges between this function's basic blocks. */
        void rebuildCFG();

    private:
        /** The name of the function. */
        std::string name;
        /** The basic blocks in the function. */
        std::vector<IRBasicBlock> blocks;
};

#endif
