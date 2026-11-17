#ifndef SLANGIR_H
#define SLANGIR_H

#include <string>
#include <utility>
#include <vector>
#include "../shared/ast/node.h"
#include "../shared/slangir-cfg/ir-program.h"

class SLangIR
{
    public:
        /** Analyzes the given AST and stores the generated IR program. */
        const IRProgram& analyze(Node *ast);
        const IRProgram& getProgram() const { return program; }

    private:
        /** The generated IR and the block currently being populated. */
        IRProgram program;
        /** The current function being populated. */
        IRFunction* currentFunction = nullptr;
        /** The current basic block being populated. */
        IRBasicBlock* currentBlock = nullptr;
        /** Whether the next instruction should be separated from the previous one. */
        bool separateNextInstruction = false;
        /** The current temporary variable counter. */
        int temporaryCounter = 0;
        /** The current block counter. */
        int blockCounter = 0;
        /** A stack of loop start and end labels for nested loops. */
        std::vector<std::pair<int, int>> loops;

        /** Visits an AST node and generates corresponding IR code. */
        void visit(Node *node);
        /** Visits a function declaration node and generates corresponding IR code. */
        void visitFunctionDeclaration(Node *node);
        /** Visits a block node and generates corresponding IR code. */
        void visitBlock(Node *node);
        /** Visits a statement node and generates corresponding IR code. */
        void visitStatement(Node *node);
        /** Visits a declaration node and generates corresponding IR code. */
        void visitDeclaration(Node *node);
        /** Visits a while statement node and generates corresponding IR code. */
        void visitWhileStatement(Node *node);
        /** Visits a for statement node and generates corresponding IR code. */
        void visitForStatement(Node *node);
        /** Visits an if statement node and generates corresponding IR code. */
        void visitIfStatement(Node *node);
        
        /** Visits a return statement node and generates corresponding IR code. */
        std::string expression(Node *node);
        /** Visits a call statement node and generates corresponding IR code. */
        std::string call(Node *node);
        /** Visits an assignment node and generates corresponding IR code. */
        std::string lvalue(Node *node);
        /** Visits an expression node and generates corresponding IR code. */
        std::string literal(Node *node) const;
        /** Visits a unary operator node and generates corresponding IR code. */
        std::string negatedCondition(Node *node);
        /** Visits a binary operator node and generates corresponding IR code. */
        std::string newTemporary();

        /** Emits an assignment instruction into the current basic block. */
        void assign(const std::string &destination, Node *source);
        /** Emits a binary operation instruction to the output stream. */
        int newBlock();
        /** Selects a newly created basic block as the current block. */
        void emitLabel(int number);
        void separateInstructions();
        /** Adds an instruction to the current basic block. */
        void emit(const std::string &instruction);
        /** Returns whether the current block already ends control flow. */
        bool currentBlockIsTerminated() const;

        /** Emits a conditional jump instruction to the output stream. */
        static bool isLiteral(Node *node);
        /** Returns the arithmetic operator for an increment or decrement node kind. */
        static std::string updateOperator(const std::string &kind);
        /** Emits a conditional jump instruction to the output stream. */
        static bool endsInReturn(Node *block);
        /** Emits a conditional jump instruction to the output stream. */
        static std::string quote(const std::string &value, char delimiter);
};

#endif
