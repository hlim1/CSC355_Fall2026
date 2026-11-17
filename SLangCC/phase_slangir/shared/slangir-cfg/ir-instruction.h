/**
 * ir-instruction.h
 *
 * This file defines the IRInstruction class, which represents one SLangIR instruction.
 */

#ifndef SLANGIR_CFG_IR_INSTRUCTION_H
#define SLANGIR_CFG_IR_INSTRUCTION_H

#include <string>

class IRInstruction {
    public:
        /** Constructs an instruction with the specified text and separation flag. */
        explicit IRInstruction(std::string text, bool separated = false);
        /** Returns the text of the instruction. */ 
        const std::string& getText() const { return text; }
        /** Returns whether the instruction is separated from the previous instruction. */
        bool isSeparated() const { return separated; }

    private:
        /** The text of the instruction. It stores the complete textual IR instruction. */
        std::string text;
        /** Whether the instruction is separated from the previous instruction,
         * i.e., when it's true, the printer inserts a blank line before the instruction. */
        bool separated;
};

#endif
