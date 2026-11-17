/**
 * ir-basic-block.h
 *
 * This file defines the IRBasicBlock class, which represents a basic block in a SLangIR function.
 */

#ifndef SLANGIR_CFG_IR_BASIC_BLOCK_H
#define SLANGIR_CFG_IR_BASIC_BLOCK_H

#include "ir-instruction.h"

#include <string>
#include <vector>

class IRBasicBlock {
    public:
        /** Constructs a basic block with the specified number. */
        explicit IRBasicBlock(int number);
        /** Returns the number of the basic block. */
        int getNumber() const { return number; }
        /** Returns a reference to the vector of instructions in the basic block. */
        const std::vector<IRInstruction>& getInstructions() const { return instructions; }
        /** Returns a reference to the vector of successors of the basic block. */
        const std::vector<int>& getSuccessors() const { return successors; }
        /** Adds an instruction to the basic block. */
        void addInstruction(const std::string& text, bool separated = false);

    private:
        /** Allows IRFunction to access private members of IRBasicBlock. */
        friend class IRFunction;
        /** The number of the basic block. */
        int number;
        /** The instructions in the basic block. */
        std::vector<IRInstruction> instructions;
        /** The successors of the basic block. */
        std::vector<int> successors;
};

#endif
