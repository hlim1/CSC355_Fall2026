# SLangCC AST Design

The SLangCC abstract syntax tree is an owned hierarchy of `Node` objects shared
by the parser, semantic analyzer, and IR generator. The parser builds the tree
in `parser/parser.y`; semantic analysis annotates and may rewrite it;
`Node::toJSON()` serializes it.

## Data model and API

Every node has a `kind`, source `line`, optional typed payload, optional
inferred `semanticType`, and ordered `children`. `ValueType` is one of
`None`, `String`, `Int`, `Double`, `Char`, or `Bool`. It describes the
C++ payload representation, not the SLangCC language type.

Every constructor requires the source line:

```cpp
Node(kind, line)
Node(kind, std::stringValue, line)
Node(kind, cStringValue, line)
Node(kind, intValue, line)
Node(kind, doubleValue, line)
Node(kind, charValue, line)
Node(kind, boolValue, line)
Node(kind, child, line)
Node(kind, stringValue, child, line)
Node(kind, left, right, line)
Node(kind, a, b, c, line)
```

The `const char*` overload ensures literals such as `"+"` use string
storage rather than the `bool` overload. Child constructors use
`addChild()`, which ignores `nullptr` and returns the parent for chaining.

Use `getValueType()` before the typed payload getters
(`getStringValue()`, `getIntValue()`, `getDoubleValue()`,
`getCharValue()`, and `getBoolValue()`). `valueAsString()` gives a display
form; `typeAsString()` names the payload storage type. Language-type metadata
uses `setSemanticType()`, `getSemanticType()`, and `hasSemanticType()`.
Structural access uses `getKind()`, `setKind()`, `getLine()`,
`getChildren()`, `hasValue()`, and `hasChildren()`. Serialization and
diagnostics are provided by `toJSON()`, the two `printJSON()` overloads, and
`printNode()`.

A node owns every pointer in `children`; destruction recursively deletes them.
Adding a child transfers ownership. `removeChild(index)` detaches without
deleting or returning the pointer, so callers that need it must retain the pointer
before removal. `replaceChild(index, replacement)` installs a non-null
replacement and returns the detached child; the caller then owns that pointer. An
out-of-range index or null replacement returns `nullptr` and makes no change.

`Node` uses raw owning pointers and does not define safe copy or move operations.
Do not copy `Node` objects by value; pass pointers or references and maintain a
single owner for each child.

## JSON representation

Every object has `kind`. `Program` has `sourceFile`; every other node has
`sourceLine` (currently encoded as a JSON string). A payload is named `name`
for `Declarator`, `ArrayDeclarator`, `FunctionDeclaration`, `Identifier`,
and `Parameter`; other payloads use `value`. Inferred type uses `type`.
Absent payloads, semantic types, and child arrays are omitted. Strings are
escaped; numeric and boolean payloads retain their JSON types.

```json
{
  "kind": "Identifier",
  "sourceLine": "12",
  "name": "count",
  "type": "int"
}
```

## AST shapes

Child order is part of the AST contract.

### Program and declarations

| Kind | Payload | Ordered children |
| --- | --- | --- |
| `Program` | none | `ExternalList` |
| `ExternalList` | none | zero or more `FunctionDeclaration` |
| `FunctionDeclaration` | name | `Type`, `ParameterList`, `Block` |
| `Type` | `int`, `double`, `boolean`, `character`, `void`, or `string` | none |
| `ParameterList` | none | zero or more `Parameter` |
| `Parameter` | name | `Type` |
| `Declaration` | none | `Type`, then `DeclaratorList` or `ArrayDeclarator` |
| `DeclaratorList` | none | one or more `Declarator` |
| `Declarator` | name | `Initializer` |
| `ArrayDeclarator` | name | size, optional `InitializationList` |
| `Initializer` | none | expression |
| `InitializationList` | none | one or more expressions |

Scalar declarations currently require an initializer. Only one array
declarator is allowed per declaration; its initialization list is optional.

### Blocks, statements, and control flow

| Kind | Payload | Ordered children |
| --- | --- | --- |
| `Block` | none | `StatementList` |
| `StatementList` | none | zero or more statements |
| `Assignment` | none | target, expression |
| `CompoundAssignment` | `+=` or `-=` | target, expression |
| `ReturnStatement` | none | zero or one expression |
| `PrintStatement` | none | `ArgumentList` |
| `ExitStatement` | none | `ArgumentList` with at most one expression |
| `ContinueStatement`, `BreakStatement` | none | none |
| `CallStatement` | none | callee `Identifier`, `ArgumentList` |
| `PreIncrement`, `PostIncrement` | `++` | target |
| `PreDecrement`, `PostDecrement` | `--` | target |
| `IfStatement` | none | condition, `Block`, optional `ElseIfList`, optional `ElseClause` |
| `ElseIfList` | none | one or more `ElseIfClause` |
| `ElseIfClause` | none | condition, `Block` |
| `ElseClause` | none | `Block` |
| `WhileStatement` | none | condition, `Block` |
| `ForStatement` | none | `ForInit`, `ForCondition`, `ForUpdate`, `Block` |
| `ForInit` | none | zero or one declaration, assignment, update, or expression |
| `ForCondition` | none | zero or one expression |
| `ForUpdate` | none | zero or one assignment, update, or expression |

All three `For*` wrappers exist even for omitted clauses. The parser can put a
`Block` or `ParenthesizedExpression` directly in a statement list and does
not currently emit an `ExpressionStatement` wrapper.

### Expressions

| Kind | Payload | Ordered children |
| --- | --- | --- |
| `Identifier` | name | none |
| `IntLiteral`, `DecimalLiteral` | number | none |
| `StringLiteral`, `CharacterLiteral` | decoded value | none |
| `BooleanLiteral` | boolean | none |
| `ArrayAccess` | none | array `Identifier`, index `Identifier` or `IntLiteral` |
| `ArgumentList` | none | zero or more expressions |
| `ParenthesizedExpression` | none | expression (standalone/increment-target form) |
| `BinaryOp` | operator | left, right |
| `UnaryOp` | operator | operand |
| `CastExpression` | none | target `Type`, then source `Identifier`, `IntLiteral`, `DecimalLiteral`, or `CharacterLiteral` |

`BinaryOp` supports `||`, `&&`, `==`, `!=`, `<`, `>`, `<=`, `>=`,
`+`, `-`, `*`, and `/`. `UnaryOp` supports `!`, `-`, and `+`. Ordinary
parentheses used inside an expression are collapsed by the parser; a
`ParenthesizedExpression` wrapper is retained only by the dedicated
parenthesized-statement/increment-target production. Explicit cast syntax is
restricted to the source kinds shown in the table.

Semantic analysis resolves `CastExpression` by replacing its kind with a
concrete conversion and removing the `Type` child. It also inserts conversion
nodes for non-string print arguments. Conversion nodes have one source
expression child.

## All node kinds currently handled

This exhaustive list includes parser-emitted kinds, semantic-rewrite kinds, and
the IR compatibility kind `ExpressionStatement` (which the current parser does
not emit):

- `ArgumentList`
- `ArrayAccess`
- `ArrayDeclarator`
- `Assignment`
- `BinaryOp`
- `Block`
- `BooleanLiteral`
- `BreakStatement`
- `CallStatement`
- `CastExpression`
- `CharacterLiteral`
- `CompoundAssignment`
- `ContinueStatement`
- `DecimalLiteral`
- `Declaration`
- `Declarator`
- `DeclaratorList`
- `ElseClause`
- `ElseIfClause`
- `ElseIfList`
- `ExitStatement`
- `ExpressionStatement`
- `ExternalList`
- `ForCondition`
- `ForInit`
- `ForStatement`
- `ForUpdate`
- `FunctionDeclaration`
- `Identifier`
- `IfStatement`
- `InitializationList`
- `Initializer`
- `IntLiteral`
- `Parameter`
- `ParameterList`
- `ParenthesizedExpression`
- `PostDecrement`
- `PostIncrement`
- `PreDecrement`
- `PreIncrement`
- `PrintStatement`
- `Program`
- `ReturnStatement`
- `StatementList`
- `StringLiteral`
- `Type`
- `UnaryOp`
- `WhileStatement`
- `booltostring`
- `chartoint`
- `chartostring`
- `doubletostring`
- `inttochar`
- `inttodouble`
- `inttostring`
