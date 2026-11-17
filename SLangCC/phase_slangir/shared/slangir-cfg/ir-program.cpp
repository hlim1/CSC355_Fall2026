#include "ir-program.h"

#include <algorithm>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace
{
    /** Checks whether text begins with a prefix.
     *
     * @param text The text to inspect.
     * @param prefix The prefix to search for.
     * @return True if the text begins with the prefix, false otherwise.
     */
    bool startsWith(const std::string &text, const std::string &prefix)
    {
        return text.rfind(prefix, 0) == 0;
    }

    /** Extracts the target block number from a branch instruction.
     *
     * @param instruction The branch instruction containing a `goto B` target.
     * @return The numeric target block identifier.
     * @throws std::runtime_error If the instruction contains no branch target.
     */
    int branchTarget(const std::string &instruction)
    {
        const std::string marker = "goto B";
        const std::size_t position = instruction.rfind(marker);
        if (position == std::string::npos)
        {
            throw std::runtime_error("branch instruction has no target: " + instruction);
        }
        return std::stoi(instruction.substr(position + marker.size()));
    }
} // namespace

/** Creates an IR instruction.
 *
 * @param text The instruction text.
 * @param separated Whether a blank line should precede the instruction when printed.
 */
IRInstruction::IRInstruction(std::string text, bool separated)
    : text(std::move(text)), separated(separated) {}

/** Creates an empty IR basic block.
 *
 * @param number The numeric block identifier.
 */
IRBasicBlock::IRBasicBlock(int number) : number(number) {}

/** Appends an instruction to this basic block.
 *
 * @param text The instruction text.
 * @param separated Whether a blank line should precede the instruction when printed.
 * @return void.
 */
void IRBasicBlock::addInstruction(const std::string &text, bool separated)
{
    instructions.push_back(IRInstruction(text, separated));
}

/** Creates an empty IR function.
 *
 * @param name The function name.
 */
IRFunction::IRFunction(std::string name) : name(std::move(name)) {}

/** Adds a uniquely numbered basic block to this function.
 *
 * @param number The numeric block identifier.
 * @return A reference to the newly added basic block.
 * @throws std::runtime_error If a block with the same number already exists.
 */
IRBasicBlock &IRFunction::addBlock(int number)
{
    for (const IRBasicBlock &block : blocks)
    {
        if (block.getNumber() == number)
        {
            throw std::runtime_error("duplicate IR basic block B" + std::to_string(number));
        }
    }
    blocks.push_back(IRBasicBlock(number));
    return blocks.back();
}

/** Rebuilds each basic block successor list from its terminating instruction.
 *
 * @return void.
 */
void IRFunction::rebuildCFG()
{
    for (std::size_t i = 0; i < blocks.size(); ++i)
    {
        IRBasicBlock &block = blocks[i];
        block.successors.clear();
        const auto addSuccessor = [&block](int successor)
        {
            if (std::find(block.successors.begin(), block.successors.end(), successor) ==
                block.successors.end())
            {
                block.successors.push_back(successor);
            }
        };

        if (block.instructions.empty())
        {
            if (i + 1 < blocks.size())
                addSuccessor(blocks[i + 1].number);
            continue;
        }

        const std::string &last = block.instructions.back().getText();
        if (startsWith(last, "return") || startsWith(last, "call exit,"))
            continue;
        if (startsWith(last, "goto B"))
        {
            addSuccessor(branchTarget(last));
            continue;
        }
        if (startsWith(last, "if ") && last.find("goto B") != std::string::npos)
            addSuccessor(branchTarget(last));
        if (i + 1 < blocks.size())
            addSuccessor(blocks[i + 1].number);
    }
}

/** Removes all functions and their IR from this program.
 *
 * @return void.
 */
void IRProgram::clear() { functions.clear(); }

/** Adds a function to this IR program.
 *
 * @param name The function name.
 * @return A reference to the newly added function.
 */
IRFunction &IRProgram::addFunction(const std::string &name)
{
    functions.push_back(IRFunction(name));
    return functions.back();
}

/** Prints this IR program.
 *
 * @param out The output stream to write to.
 * @return void.
 */
void IRProgram::print(std::ostream &out) const
{
    for (std::size_t fi = 0; fi < functions.size(); ++fi)
    {
        const IRFunction &function = functions[fi];
        out << function.getName() << ":\n";
        for (std::size_t bi = 0; bi < function.getBlocks().size(); ++bi)
        {
            const IRBasicBlock &block = function.getBlocks()[bi];
            if (bi != 0)
                out << '\n';
            out << 'B' << block.getNumber() << ":\n";
            for (const IRInstruction &instruction : block.getInstructions())
            {
                if (instruction.isSeparated())
                    out << '\n';
                out << "    " << instruction.getText() << '\n';
            }
        }
        if (fi + 1 < functions.size())
            out << '\n';
    }
    if (!functions.empty())
        out << '\n';
}

/** Converts this IR program to its textual representation.
 *
 * @return The formatted SLangIR program text.
 */
std::string IRProgram::toString() const
{
    std::ostringstream out;
    print(out);
    return out.str();
}
