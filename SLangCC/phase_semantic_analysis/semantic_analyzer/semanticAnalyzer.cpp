/**
 * semanticAnalyzer.cpp — Semantic Analysis Phase starter.
 *
 * Instructor-provided implementations appear below.
 * Students implement the remaining methods declared in semanticAnalyzer.h
 * in this file. No implementation stubs are supplied.
 */

#include "semanticAnalyzer.h"

#include "../shared/error/error.h"


/** Constructor for the semantic analyzer.
 * 
 * @param symbolTable The symbol table to use for semantic analysis.
 */
SemanticAnalyzer::SemanticAnalyzer(SymbolTable& symbolTable)
    : symbolTable(symbolTable),
      currentScope(""),
      currentReturnType("") {
}

/** Runs semantic checks over the whole AST.
 * 
 * @param ast The root node of the abstract syntax tree to analyze.
 * @return void
 */
void SemanticAnalyzer::analyze(Node* ast) {
    if (ast == nullptr) {
        return;
    }

    visit(ast);
}

/** Visits a node in the AST and performs semantic checks.
 * 
 * @param node The node to visit.
 * @return void
 */
void SemanticAnalyzer::visit(Node* node) {
    if (node == nullptr) {
        return;
    }

    // Get the kind of the node to determine which specific visit method to call. 
    const std::string& kind = node->getKind();

    // Dispatch to specific visit methods based on the node kind.
    if (kind == "Program") {
        visitProgram(node);
    } 
    else if (kind == "FunctionDeclaration") {
        visitFunctionDeclaration(node);
    } 
    else if (kind == "Declaration") {
        visitDeclaration(node);
    } 
    else if (kind == "Assignment") {
        visitAssignment(node);
    } 
    else if (kind == "ReturnStatement") {
        visitReturnStatement(node);
    }
    else if (kind == "CallStatement") {
        visitCallStatement(node);
    }
    else if (kind == "PrintStatement") {
        visitPrintStatement(node);
    }
    else {
        for (Node* child : node->getChildren()) {
            visit(child);
        }
    }
}

/** Visits a `Program` node and checks its children.
 * 
 * @param node The node to visit.
 * @return void
 */
void SemanticAnalyzer::visitProgram(Node* node) {
    for (Node* child : node->getChildren()) {
        visit(child);
    }
}

/** Visits a `FunctionDeclaration` node and checks its children.
 * 
 * @param node The node to visit.
 * @return void
 */
void SemanticAnalyzer::visitFunctionDeclaration(Node* node) {
    // Save the current scope and return type to restore them later.
    const std::string previousScope = currentScope;
    const std::string previousReturnType = currentReturnType;

    // Update the current scope and return type based on the function declaration.
    currentScope = node->getStringValue();

    const std::vector<Node*>& children = node->getChildren();
    if (children.empty() || children[0]->getKind() != "Type") {
        std::cerr << "Internal error: FunctionDeclaration node has no type child." << std::endl;
        std::abort();
    }

    // The first child of a FunctionDeclaration node is the return type.
    currentReturnType = children[0]->getStringValue();

    // Check if the 'main' function has a return type of 'void'.
    if (currentScope == "main" && currentReturnType != "void") {
        std::string message = "The 'main' function must have a return type of 'void'.";
        Error::semantic(message, node->getLine());
        return;
    }

    for (Node* child : children) {
        visit(child);
    }

    // Restore the previous scope and return type after visiting the function body.
    currentScope = previousScope;
    currentReturnType = previousReturnType;
}

/** Visits a `PrintStatement` node and checks the types of its arguments. 
 * 
 * @param node The `PrintStatement` node to visit.
 * @return void
 */
void SemanticAnalyzer::visitPrintStatement(Node* node)
{
    const std::vector<Node*>& children = node->getChildren();

    // Check if the `PrintStatement` node has exactly one child, which should be an `ArgumentList` node.
    if (children.size() != 1 || children[0]->getKind() != "ArgumentList") {
        Error::semantic("Malformed print statement.", node->getLine());
        return;
    }

    const std::vector<Node*>& arguments = children[0]->getChildren();

    if (arguments.empty()) {
        Error::semantic("Print statement must have at least one argument.", node->getLine());
        return;
    }

    for (size_t i = 0; i < arguments.size(); ++i) {
        Node* argument = arguments[i];
        const std::string type = inferExpressionType(argument);

        if (type.empty()) {
            return;
        }

        if (type != "string" && type != "int" && type != "double" &&
            type != "boolean" && type != "character") {
            Error::semantic("Print argument has unsupported type '" + type + "'.",argument->getLine());
            return;
        }

        // If the argument type is not a string, we need to insert an implicit cast to string.
        if (type != "string") {
            std::string castKind;

            if (type == "int") {
                castKind = "inttostring";
            } else if (type == "double") {
                castKind = "doubletostring";
            } else if (type == "boolean") {
                castKind = "booltostring";
            } else if (type == "character") {
                castKind = "chartostring";
            }

            // Create a new cast node and replace the original argument with the casted version.
            Node* cast = new Node(castKind, argument, argument->getLine());
            cast->setSemanticType("string");
            children[0]->replaceChild(i, cast);
        }
    }
}

/** Infers the SLangCC type of an expression node and returns it as a string.
 * 
 * @param expression The node for which to infer the type.
 * @return The inferred type as a string.
 */
std::string SemanticAnalyzer::inferExpressionType(Node* expression) {
    if (expression == nullptr) {
        return "";
    }

    // Check the kind of the expression node to determine its type.
    const std::string& kind = expression->getKind();

    // Handle literal nodes and set their types accordingly.
    if (isLiteralKind(kind)) {
        return inferLiteralType(expression);
    }

    // Handle parenthesized expressions by inferring the type of the inner expression.
    if (kind == "ParenthesizedExpression") {
        return inferParenthesizedExpressionType(expression);
    }

    // Handle identifiers, array accesses, function calls, and call statements by looking up their symbols in the symbol table.
    if (kind == "Identifier") {
        return inferIdentifierType(expression);
    }

    if (kind == "ArrayAccess") {
        return inferArrayAccessType(expression);
    }

    if (kind == "CallStatement") {
        return inferCallStatementType(expression);
    }

    if (kind == "UnaryOp") {
        return inferUnaryOpType(expression);
    }

    if (kind == "BinaryOp") {
        return inferBinaryOpType(expression);
    }

    if (kind == "CastExpression") {
        return inferCastExpressionType(expression);
    }

    return "";
}

/** Stores an inferred type on an expression node and returns it.
 * 
 * @param expression The node on which to store the type.
 * @param type The type to store.
 * @return The stored type as a string.
 */
std::string SemanticAnalyzer::setExpressionType(Node* expression, const std::string& type) const {
    if (expression != nullptr && !type.empty()) {
        expression->setSemanticType(type);
    }

    return type;
}

/** Infers the SLangCC type of a cast expression node and returns it as a string.
 * 
 * @param expression The node for which to infer the type.
 * @return The inferred type as a string.
 */
std::string SemanticAnalyzer::inferCastExpressionType(Node* expression) {
    if (expression->getChildren().size() != 2) {
        Error::semantic("Invalid cast expression.", expression->getLine());
        return "";
    }

    const std::string targetType = expression->getChildren()[0]->getStringValue();
    const std::string sourceType = inferExpressionType(expression->getChildren()[1]);

    // If either the target type or source type is empty, report an internal error and abort the program.
    if (targetType.empty() || sourceType.empty()) {
        std::cerr << "Internal error: Cast expression has invalid target or source type." << std::endl;
        std::abort();
    }

    // If the source type can be assigned to the target type, return the target type as the result of the cast.
    if (isCastable(targetType, sourceType))
    {
        std::string typeStr = setExpressionType(expression, targetType);

        std::string castKind = "";
        if (sourceType == "int" and targetType == "double")
        {
            castKind = "inttodouble";
        }
        else if (sourceType == "int" and targetType == "character")
        {
            castKind = "inttochar";
        }
        else if (sourceType == "character" and targetType == "string")
        {
            castKind = "chartostring";
        }
        else if (sourceType == "int" and targetType == "string")
        {
            castKind = "inttostring";
        }
        else if (sourceType == "double" and targetType == "string")
        {
            castKind = "doubletostring";
        }
        else if (sourceType == "boolean" and targetType == "string")
        {
            castKind = "booltostring";
        }
        else if (sourceType == "character" and targetType == "int")
        {
            castKind = "chartoint";
        }
        else
        {
            std::string message = "Invalid casting from \"" + sourceType + "\" to " + targetType + ".";
            Error::semantic(message, expression->getLine());
            return "";
        }

        // Update the cast node's kind.
        expression->setKind(castKind);

        // Remove the first child, which is the target type information.
        expression->removeChild(0);

        return typeStr;
    }

    // If the source type cannot be assigned to the target type, report a semantic error indicating that the cast is invalid.
    std::string message = "Cannot cast from type '" + sourceType + "' to type '" + targetType + "'.";
    Error::semantic(message, expression->getLine());

    return "";
}

/** Checks if a given type is numeric (int or double).
 * 
 * @param type The type to check.
 * @return True if the type is numeric, false otherwise.
 */
bool SemanticAnalyzer::isNumericType(const std::string& type) const {
    return type == "int" || type == "double";
}

/** Checks if a value of the source type can be cast to the target type.
 * 
 * @param targetType The target type.
 * @param sourceType The source type.
 * @return True if the cast is valid, false otherwise.
 */
bool SemanticAnalyzer::isCastable(const std::string& targetType, const std::string& sourceType) const {

    if (sourceType == "int" && targetType == "double") {
        return true;
    }

    if (sourceType == "character" && targetType == "string") {
        return true;
    }

    if (sourceType == "int" && targetType == "string") {
        return true;
    }

    if (sourceType == "double" && targetType == "string") {
        return true;
    }

    if (sourceType == "boolean" && targetType == "string") {
        return true;
    }

    if (sourceType == "character" && targetType == "int") {
        return true;
    }

    if (sourceType == "int" && targetType == "character") {
        return true;
    }

    return false;
}

/** Reports a type mismatch error with the expected and actual types at the given source line.
 * 
 * @param expected The expected type.
 * @param actual The actual type.
 * @param line The source line number.
 */
void SemanticAnalyzer::reportTypeMismatch(const std::string& expected,
                                          const std::string& actual,
                                          int line) const {
    Error::semantic("Cannot assign value of type '" + actual +
                    "' to type '" + expected + "'.", line);
}

/** Checks if a given node kind represents a literal (int, decimal, character, string, or boolean).
 * 
 * @param kind The node kind to check.
 * @return True if the node kind represents a literal, false otherwise.
 */
bool SemanticAnalyzer::isLiteralKind(const std::string& kind) const {
    return kind == "IntLiteral" ||
           kind == "DecimalLiteral" ||
           kind == "CharacterLiteral" ||
           kind == "StringLiteral" ||
           kind == "BooleanLiteral";
}
