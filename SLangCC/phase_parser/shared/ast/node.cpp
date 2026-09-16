#include "node.h"
#include "../error/error.h"

#include <iomanip>
#include <iostream>
#include <sstream>

/** Creates a node with one child.
 *
 * @param kind The AST node kind.
 * @param child The child node to add; null is ignored.
 * @param line The source line where the node occurs.
 */
Node::Node(const std::string& kind, Node* child, int line)
{
    this->kind = kind;
    this->valueType = ValueType::None;

    this->addChild(child);

    this->line = line;
    this->semanticType = "";

    // Initialize unused fields to default values.
    this->intValue = 0;
    this->doubleValue = 0.0;
    this->charValue = '\0';
    this->boolValue = false;
}

/** Deletes all descendant nodes owned by this node. */
Node::~Node() {
    for (Node* child : children) {
        delete child;
    }
}

/** Adds a child unless the pointer is null.
 *
 * @param child The child node whose ownership is transferred to this node.
 * @return This node, for call chaining.
 */
Node* Node::addChild(Node* child) {
    if (child != nullptr) {
        children.push_back(child);
    }
    return this;
}

/** Checks whether this node has an active typed value.
 *
 * @return True if the node has a value, false otherwise.
 */
bool Node::hasValue() const {
    return valueType != ValueType::None;
}

/** Checks whether semantic analysis has assigned this node a type.
 *
 * @return True if the node has a semantic type, false otherwise.
 */
bool Node::hasSemanticType() const {
    return !semanticType.empty();
}

/** Converts the active payload value to a readable string.
 *
 * @return The formatted value, or an empty string if the node has no value.
 */
std::string Node::valueAsString() const {
    std::ostringstream out;

    switch (valueType) {
        case ValueType::String:
            return stringValue;
        case ValueType::Int:
            return std::to_string(intValue);
        case ValueType::Double:
            out << std::setprecision(15) << doubleValue;
            return out.str();
        case ValueType::Char:
            return std::string(1, charValue);
        case ValueType::Bool:
            return boolValue ? "true" : "false";
        case ValueType::None:
            return "";
    }

    return "";
}

/** Gets a readable name for the active payload value type.
 *
 * This describes the stored payload rather than the node semantic type.
 *
 * @return The payload type name, or an empty string if the node has no value.
 */
std::string Node::typeAsString() const {

    switch (valueType) {
        case ValueType::String:
            return "string";
        case ValueType::Int:
            return "integer";
        case ValueType::Double:
            return "double";
        case ValueType::Char:
            return "character";
        case ValueType::Bool:
            return "boolean";
        case ValueType::None:
            return "";
    }

    return "";
}

/** Prints all local node fields and immediate children for debugging.
 *
 * @return void.
 */
void Node::printNode() {
    std::cout << "kind: " << this->kind << std::endl;
    std::cout << "type: " << typeAsString() << std::endl;
    std::cout << "semantic type: " << semanticType << std::endl;
    std::cout << "string value: " << this->stringValue << std::endl;
    std::cout << "integer value: " << this->intValue << std::endl;
    std::cout << "double value: " << this->doubleValue << std::endl;
    std::cout << "character value: " << std::string(1, this->charValue) << std::endl;
    std::cout << "boolean value: " << this->boolValue << std::endl;

    std::cout << "children:" << std::endl;
    for (std::size_t i = 0; i < children.size(); ++i) {
        std::cout << "  child " << i << std::endl;
        std::cout << "      kind: " << children[i]->kind << std::endl;
        std::cout << "      string value: " << children[i]->stringValue << std::endl;
        std::cout << "      integer value: " << children[i]->intValue << std::endl;
        std::cout << "      double value: " << children[i]->doubleValue << std::endl;
        std::cout << "      character value: " << std::string(1, children[i]->charValue) << std::endl;
        std::cout << "      boolean value: " << children[i]->boolValue << std::endl;
    }
}

/** Builds indentation for formatted JSON output.
 *
 * @param level The non-negative indentation level.
 * @return A string containing two spaces per indentation level.
 */
std::string Node::indent(int level) const {
    return std::string(level * 2, ' ');
}

/** Escapes characters that have special meaning in JSON strings.
 *
 * @param text The unescaped text.
 * @return The JSON-safe escaped text without surrounding quotation marks.
 */
std::string Node::escapeJSON(const std::string& text) {
    std::ostringstream out;

    for (char c : text) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\t': out << "\\t"; break;
            case '\r': out << "\\r"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\0': out << "\\u0000"; break;
            default: out << c; break;
        }
    }

    return out.str();
}

/** Writes the active value as a JSON value without a field name.
 *
 * @param out The output stream to write to.
 * @return void.
 */
void Node::writeJSONValue(std::ostream& out) const {
    switch (valueType) {
        case ValueType::String:
            out << "\"" << escapeJSON(stringValue) << "\"";
            break;
        case ValueType::Int:
            out << intValue;
            break;
        case ValueType::Double:
            out << std::setprecision(15) << doubleValue;
            break;
        case ValueType::Char:
            out << "\"" << escapeJSON(std::string(1, charValue)) << "\"";
            break;
        case ValueType::Bool:
            out << (boolValue ? "true" : "false");
            break;
        case ValueType::None:
            break;
    }
}

/** Serializes this node and all descendants to formatted JSON.
 *
 * @param level The initial indentation level.
 * @return The formatted JSON representation of this node hierarchy.
 */
std::string Node::toJSON(int level) const {
    std::ostringstream out;

    out << indent(level) << "{\n";
    out << indent(level + 1) << "\"kind\": \"" << escapeJSON(kind) << "\"" << ",\n";
    if (kind == "Program") {
        out << indent(level + 1) << "\"sourceFile\": \"" << escapeJSON(Error::getSourceFile()) << "\"";
    }
    else {
        out << indent(level + 1) << "\"sourceLine\": \"" << line << "\"";
    }

    if (hasValue()) {
        out << ",\n";
        if (
            kind == "Declarator" ||
            kind == "ArrayDeclarator" ||
            kind == "FunctionDeclaration" ||
            kind == "Identifier" ||
            kind == "Parameter"
        ) {
            out << indent(level + 1) << "\"name\": ";
        }
        else {
            out << indent(level + 1) << "\"value\": ";
        }
        writeJSONValue(out);
    }

    if (hasSemanticType()) {
        out << ",\n";
        out << indent(level + 1) << "\"type\": \"" << escapeJSON(semanticType) << "\"";
    }

    if (!children.empty()) {
        out << ",\n";
        out << indent(level + 1) << "\"children\": [\n";

        for (std::size_t i = 0; i < children.size(); ++i) {
            out << children[i]->toJSON(level + 2);
            if (i + 1 < children.size()) {
                out << ",";
            }
            out << "\n";
        }

        out << indent(level + 1) << "]";
    }

    out << "\n" << indent(level) << "}";
    return out.str();
}

/** Prints this node as JSON to standard output.
 *
 * @return void.
 */
void Node::printJSON() const {
    printJSON(std::cout, 0);
}

/** Prints this node as JSON to a caller-provided stream.
 *
 * @param out The output stream to write to.
 * @param level The initial indentation level.
 * @return void.
 */
void Node::printJSON(std::ostream& out, int level) const {
    out << toJSON(level) << std::endl;
}
