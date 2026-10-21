/**
 * semanticAnalyzer.h
 * 
 * Header file for the SemanticAnalyzer class, which performs semantic analysis on the AST.
 */

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <string>

#include "../shared/ast/node.h"
#include "../shared/symboltable/symbolTable.h"

class SemanticAnalyzer {
    public:
        /** Creates an analyzer that can query the parser-built symbol table. */
        explicit SemanticAnalyzer(SymbolTable& symbolTable);

        /** Runs semantic checks over the whole AST. */
        void analyze(Node* ast);

    private:
        /** The symbol table to query during semantic analysis. */
        SymbolTable& symbolTable;
        /** The current scope during semantic analysis. */
        std::string currentScope;
        /** The return type of the current function. */
        std::string currentReturnType;

        /** Generic dispatcher for AST nodes. */
        void visit(Node* node);

        /** Node-specific checks. */
        void visitProgram(Node* node);
        void visitFunctionDeclaration(Node* node);
        void visitDeclaration(Node* node);
        void visitAssignment(Node* node);
        void visitReturnStatement(Node* node);
        void visitCallStatement(Node* node);
        void visitPrintStatement(Node* node);

        /** Infers the SLangCC type of an expression where possible. */
        std::string inferExpressionType(Node* expression);
        /** Infers the SLangCC type of a literal node and returns it as a string. */
        std::string inferLiteralType(Node* expression);
        /** Infers the SLangCC type of a parenthesized expression node and returns it as a string. */
        std::string inferParenthesizedExpressionType(Node* expression);
        /** Infers the SLangCC type of an identifier node and returns it as a string. */
        std::string inferIdentifierType(Node* expression);
        /** Infers the SLangCC type of an array access node and returns it as a string. */
        std::string inferArrayAccessType(Node* expression);
        /** Infers the SLangCC type of a function call node and returns it as a string. */
        std::string inferCallStatementType(Node* expression);
        /** Infers the SLangCC type of a unary operator node and returns it as a string. */
        std::string inferUnaryOpType(Node* expression);
        /** Infers the SLangCC type of a binary operator node and returns it as a string. */
        std::string inferBinaryOpType(Node* expression);
        /** Infers the SLangCC type of a cast expression node and returns it as a string. */
        std::string inferCastExpressionType(Node* expression);

        /** Stores an inferred type on an expression node and returns it. */
        std::string setExpressionType(Node* expression, const std::string& type) const;

        /** Helpers for basic type rules. */
        bool isNumericType(const std::string& type) const;
        bool isAssignable(const std::string& expected, const std::string& actual) const;
        void reportTypeMismatch(const std::string& expected, const std::string& actual, int line) const;
        bool isLiteralKind(const std::string& kind) const;
        bool isCastable(const std::string& targetType, const std::string& sourceType) const;
};

#endif