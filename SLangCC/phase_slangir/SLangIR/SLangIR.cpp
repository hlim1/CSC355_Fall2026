#include "SLangIR.h"

#include <map>
#include <sstream>
#include <stdexcept>

/** Analyzes the AST and returns the generated IR program.
 * 
 * @param ast The abstract syntax tree to analyze.
 * @return The generated IR program.
 */
const IRProgram& SLangIR::analyze (Node* ast) {
    program.clear();
    currentFunction = nullptr;
    currentBlock = nullptr;

    if (ast != nullptr) {
        visit(ast);
    }

    return program;
}

/** Visits the given node and generates IR for it. 
 * 
 * @param node The AST node to visit.
 * @return void
 */
void SLangIR::visit (Node* node) {
    if (node == nullptr) {
        return;
    }

    // Visit the node based on its kind. If it's a function declaration, handle it specifically; 
    // otherwise, recursively visit its children.
    if (node->getKind() == "FunctionDeclaration") {
        visitFunctionDeclaration(node);
    } else {
        for (Node* child : node->getChildren()) {
            visit(child);
        }
    }
}

/** Visits a while statement node and generates corresponding IR code. 
 * 
 * @param node The AST node representing the while statement.
 * @return void
 */
void SLangIR::visitWhileStatement (Node* node) {
    const std::vector<Node*>& children = node->getChildren();

    const int condition = newBlock(); // Create a new basic block for the loop condition, which will be evaluated before each iteration of the loop.
    const int body = newBlock(); // Create a new basic block for the loop body, which will contain the statements to execute in each iteration of the loop.
    const int done = newBlock(); // Create a new basic block for the "done" label, which will be the target of the conditional jump when the loop condition is false.

    // Emit the label for the condition block, which will be evaluated before each iteration of the loop.
    emitLabel(condition);

    // Emit a conditional jump instruction to the done block if the negated condition evaluates to true.
    std::string negatedCond = negatedCondition(children.at(0));
    std::string conditionInstruction = "if " + negatedCond + " goto B" + std::to_string(done);

    emit(conditionInstruction);

    emitLabel(body);

    // Push the loop start and end labels onto the loop stack to handle nested loops correctly.
    loops.push_back({condition, done});
    visitBlock(children.at(1));
    loops.pop_back();

    std::string gotoConditionInstruction = "goto B" + std::to_string(condition);
    emit(gotoConditionInstruction);

    emitLabel(done);
}

/** Emits a label for a basic block. 
 * 
 * @param number The number of the basic block to emit a label for.
 * @return void
 */
void SLangIR::emitLabel (int number) {
    if (currentFunction == nullptr) {
        std::cerr << "Internal error: IR block emitted outside a function" << std::endl;
        std::abort();
    }
    // Add a new basic block to the current function and set it as the current block.
    currentBlock = &currentFunction->addBlock(number);
    // Reset the flag for separating the next instruction, as we are starting a new basic block.
    separateNextInstruction = false;
}

/** Emits an instruction to the output stream.
 * 
 * @param instruction The instruction to emit.
 * @return void
 */
void SLangIR::emit (const std::string& instruction) {
    if (currentBlock == nullptr) {
        std::cerr << "Internal error: IR instruction emitted outside a basic block" << std::endl;
        std::abort();
    }
    currentBlock->addInstruction(instruction, separateNextInstruction);
    separateNextInstruction = false;
}

/** Returns whether the current block ends in an unconditional terminator.
 *
 * @return True if the current block cannot fall through, false otherwise.
 */
bool SLangIR::currentBlockIsTerminated () const {
    if (currentBlock == nullptr || currentBlock->getInstructions().empty()) {
        return false;
    }

    const std::string& instruction =
        currentBlock->getInstructions().back().getText();

    return instruction.rfind("goto B", 0) == 0 ||
        instruction.rfind("return", 0) == 0 ||
        instruction.rfind("call exit,", 0) == 0;
}

/** Separates the next instruction from the previous one.
 *
 * @return void
 */
void SLangIR::separateInstructions () {
    separateNextInstruction = currentBlock != nullptr &&
        !currentBlock->getInstructions().empty();
}

/** Checks if a node represents a literal value.
 * 
 * @param node The AST node to check.
 * @return True if the node represents a literal value, false otherwise.
 */
bool SLangIR::isLiteral (Node* node) {
    const std::string& kind = node->getKind();

    return (
        kind == "IntLiteral" ||
        kind == "DecimalLiteral" ||
        kind == "StringLiteral" ||
        kind == "CharacterLiteral" ||
        kind == "BooleanLiteral"
    );
}

/** Quotes a string value with the specified delimiter and escapes special characters.
 * 
 * @param value The string value to quote.
 * @param delimiter The character to use as the delimiter for quoting.
 * @return The quoted string with escaped special characters.
 */
std::string SLangIR::quote (const std::string& value, char delimiter) {
    std::ostringstream outputStream;

    outputStream << delimiter;

    for (char character : value) {
        if (character == '\\' || character == delimiter) {
            outputStream << '\\' << character;
        } else if (character == '\n') {
            outputStream << "\\n";
        } else if (character == '\r') {
            outputStream << "\\r";
        } else if (character == '\t') {
            outputStream << "\\t";
        } else {
            outputStream << character;
        }
    }

    outputStream << delimiter;

    return outputStream.str();
}

/** Returns the arithmetic operator for an increment or decrement node kind.
 * 
 * @param kind The kind of the node.
 * @return The arithmetic operator for the node.
 */
std::string SLangIR::updateOperator(const std::string& kind)
{
    if (kind.find("Increment") != std::string::npos) {
        return "+";
    }

    return "-";
}

/** Creates a new temporary variable and returns its name.
 * 
 * @return The name of the newly created temporary variable.
 */
std::string SLangIR::newTemporary()
{
    return "_t" + std::to_string(temporaryCounter++);
}

/** Creates a new basic block and returns its number.
 * 
 * @return The number of the newly created basic block.
 */
int SLangIR::newBlock() {
    return ++blockCounter;
}
