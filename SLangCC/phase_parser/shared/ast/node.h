/**
 * node.h
 * 
 * This file defines the Node class, which represents a node in the abstract syntax tree (AST).
 */

#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>
#include <vector>

/**
 * Represents one AST node.
 *
 * Each node has a required `kind`, an optional typed payload value, and zero
 * or more child nodes. A node owns every child pointer stored in `children`.
 *
 * The payload value type is separate from the semantic type represented by the
 * AST. For example, a `Type` node whose value is `void` stores that value as a
 * string payload, so its `valueType` is `ValueType::String`; that does not mean
 * the SLangCC type is `string`.
 */
class Node {
    public:
        /**
         * Identifies which C++ payload field is active for this node.
         *
         * This is not the SLangCC semantic type of the node. It only describes
         * how this node's optional `value` should be stored, converted, and
         * serialized.
         */
        enum class ValueType {
            None,
            String,
            Int,
            Double,
            Char,
            Bool
        };

        /** Creates a node with no value and no children. */
        Node(const std::string& kind, int line);

        /** Creates a node with a string payload already stored in std::string. */
        Node(const std::string& kind, const std::string& value, int line);

        /** Creates a node with an integer value. */
        Node(const std::string& kind, int value, int line);

        /** Creates a node with a double value. */
        Node(const std::string& kind, double value, int line);

        /** Creates a node with a character value. */
        Node(const std::string& kind, char value, int line);

        /** Creates a node with a boolean value. */
        Node(const std::string& kind, bool value, int line);

        /** Creates a node with one child. */
        Node(const std::string& kind, Node* child, int line);

        /** Creates a node with a string value and one child. */
        Node(const std::string& kind, const std::string& value, Node* child, int line);

        /** Creates a node with two children. */
        Node(const std::string& kind, Node* left, Node* right, int line);

        /** Creates a node with three children. */
        Node(const std::string& kind, Node* a, Node* b, Node* c, int line);

        /** Deletes this node and all child nodes it owns. */
        ~Node();

        /** Adds a non-null child and returns this node for chaining. */
        Node* addChild(Node* child);

        /** Returns true when this node has an active payload value. */
        bool hasValue() const;

        /** Returns true when the children vector is not empty. */
        bool hasChildren() const;

        /** Removes a child at the specified index. */
        void removeChild(size_t index);

        /** Replaces a child without deleting it and returns the previous child. */
        Node* replaceChild(size_t index, Node* child);

        /** Returns true when semantic analysis has assigned this node a type. */
        bool hasSemanticType() const;

        /** ======== Setter Functions ========*/

        /** Stores the inferred SLangCC semantic type for this node. */
        void setSemanticType(const std::string& type);

        /** Stores the AST node kind for this node. */
        void setKind(const std::string& kind);

        /** ======== Getter Functions ========*/

        /** Returns the inferred SLangCC semantic type for this node. */
        const std::string& getSemanticType() const { return semanticType; };

        /** Returns the AST node kind, such as `FunctionDeclaration`. */
        const std::string& getKind() const { return kind; };

        /** Returns which payload value field is active. */
        ValueType getValueType() const { return valueType; };

        /** Returns the string payload. Only meaningful for `ValueType::String`. */
        const std::string& getStringValue() const { return stringValue; };

        /** Returns the integer payload. Only meaningful for `ValueType::Int`. */
        int getIntValue() const { return intValue; };

        /** Returns the double payload. Only meaningful for `ValueType::Double`. */
        double getDoubleValue() const { return doubleValue; };

        /** Returns the character payload. Only meaningful for `ValueType::Char`. */
        char getCharValue() const { return charValue; };

        /** Returns the boolean payload. Only meaningful for `ValueType::Bool`. */
        bool getBoolValue() const { return boolValue; };

        /** Returns the ordered child nodes owned by this node. */
        const std::vector<Node*>& getChildren() const { return children; };

        /** Returns the source code line number. */
        int getLine() const { return line; };

        /** ======== Print-Related Functions ========*/
        
        /** Converts the active payload value to a readable string. */
        std::string valueAsString() const;

        /** Serializes this node and descendants as formatted JSON. */
        std::string toJSON(int level = 0) const;

        /** Prints this node as JSON to standard output. */
        void printJSON() const;

        /** Prints this node as JSON to the given stream. */
        void printJSON(std::ostream& out, int level = 0) const;

        /** Returns the active payload value type as a human-readable string. */
        std::string typeAsString() const;

        /** Prints debugging details for this node to standard output. */
        void printNode();

    private:
        /** AST node kind, such as `FunctionDeclaration` or `IntLiteral`. */
        std::string kind;

        /** Tag indicating which payload value field, if any, is active. */
        ValueType valueType;

        /** Active when `valueType == ValueType::String`. */
        std::string stringValue;

        /** Active when `valueType == ValueType::Int`. */
        int intValue;

        /** Active when `valueType == ValueType::Double`. */
        double doubleValue;

        /** Active when `valueType == ValueType::Char`. */
        char charValue;

        /** Active when `valueType == ValueType::Bool`. */
        bool boolValue;

        /** Inferred SLangCC semantic type, such as int, double, or string. */
        std::string semanticType;

        /** Child AST nodes owned by this node. */
        std::vector<Node*> children;

        /** Source code line number. */
        int line;

        /** Returns a two-space indentation string for the given JSON level. */
        std::string indent(int level) const;

        /** Escapes a string so it is safe to write as a JSON string value. */
        static std::string escapeJSON(const std::string& text);

        /** Writes the active payload value in JSON form without a field name. */
        void writeJSONValue(std::ostream& out) const;
};

#endif
