# Phase: Parser

## Learning Objective

Implement a syntax analyzer for SLangCC using Bison. By completing this phase, you will be able to translate a language specification into context-free grammar productions and parser actions, recognize declarations, functions, statements, expressions, and control-flow constructs, apply precedence and associativity rules to resolve ambiguity, construct an abstract syntax tree from parsed input, and report syntax errors with useful source-location information.

## Starter Files

- `parser/parser_interface.h`: Declares the parser entry point, input stream, and generated AST shared with the parser driver.
- `parser/main.cpp`: Runs the parser on a source file and writes a successful parse result as an AST JSON file.
- `shared/ast/node.h`: Declares the `Node` class used to represent typed values, source locations, and parent-child relationships in the AST.
- `shared/ast/node.cpp`: Implements AST node construction, ownership, accessors, and JSON serialization.

## Directory Setup

At the start of this phase, your existing project should already have the following structure from the lexer phase:

```text
SLangCC/
├── benchmark/
│   └── lexer/
├── lexer/
└── shared/
    └── ast/
```

Before copying the parser-phase files, commit or back up your existing work. Extract the provided parser-phase archive into a temporary directory. Then move or copy the extracted top-level files and directories into your existing `SLangCC/` directory—the directory that already contains `lexer/`. Merge directories with matching names, such as `benchmark/` and `shared/`; do not place the extracted directory inside `lexer/` and do not create a second nested `SLangCC/` directory.

For example, from the course repository's `SLangCC` directory, copy its contents as follows:

```sh
cp -R <path>/<to>/<course directory>/SLangCC/phase_parser/* <path>/<to>/<student directory>/SLangCC/
```

`-R` option recursively copies the source directory and all of its contents to the destination directory.

After merging the files, the relevant directory structure should be:

```text
SLangCC/
├── benchmark/
│   ├── lexer/
│   └── parser/
├── lexer/
├── parser/
└── shared/
    ├── ast/
    └── error/
```

Your earlier lexer files and lexer benchmarks must remain in place. All commands for this phase should be run from the appropriate location inside this combined `SLangCC` project.

## Description

In this phase, you will complete the SLangCC syntax analyzer by adding semantic actions to the remaining grammar productions in the Bison grammar file: `parser/parser.y`. These actions must construct the required abstract syntax tree (AST), preserve source locations and child order, manage token values correctly, and report parser-level errors for invalid constructs.

You will also complete the selected constructors and tree-management functions in `Node` class (under the `shared/ast/` directory) that support AST creation, ownership, and inspection. A completed `program` grammar action and one `Node` constructor are provided as examples. Symbol-table construction and semantic type analysis are reserved for a later phase.

### Recommended Timeline

- **First Week:** Complete and test the assigned constructors and tree-management functions in `shared/ast/node.cpp`. Build small ASTs manually to verify initialization, payload values, child ownership, and JSON output.
- **Second Week:** Complete the foundational grammar actions for types, identifiers, literals, expressions, casts, parameters, lists, and blocks. Run the smallest related legal tests after each group.
- **Third Week:** Complete declarations, initialization, assignments, function calls, returns, print and exit statements, and increment/decrement actions. Verify node kinds, values, and child order against expected JSON files.
- **Fourth Week:** Complete functions, conditionals, loops, optional productions, and required parser-level error checks. Run the corresponding legal and illegal benchmark groups.
- **Fifth Week:** Run the complete test suite, correct AST and diagnostic differences, check memory ownership, perform clean rebuilds, and submit only after both local and GitHub workflow tests pass.

## Program Requirements

### `Node` Class

Complete only the constructor overloads and methods listed in this section. Implement them in `shared/ast/node.cpp`; do not place `Node` implementations in `parser/parser.y` and do not change the public declarations in `shared/ast/node.h`.

The following functions are supplied and must not be rewritten: the one-child constructor, destructor, `addChild`, `hasValue`, and `hasSemanticType`. Use them as examples of initialization, ownership, null handling, and query behavior.

#### Shared constructor requirements

Every constructor must set `kind` and `line` from its arguments and initialize `semanticType` to the empty string. Semantic types are assigned in a later phase. Unless a constructor receives children, its `children` vector must begin empty.

Every payload field must be initialized, including fields that are inactive:

- `stringValue = ""`
- `intValue = 0`
- `doubleValue = 0.0`
- `charValue` is the null character
- `boolValue = false`

`valueType` identifies the active payload field. `ValueType::None` means that the node has no payload. Default-looking values do not mean that a payload is absent: an empty string, zero, the null character, and `false` can be legitimate values when the corresponding non-`None` tag is active.

`valueType` and `semanticType` are independent. For example, an identifier named `count` has a string payload and therefore `ValueType::String`; semantic analysis may later assign it semantic type `int`. Changing one type field must not change the other.

#### `Node(const std::string& kind, int line)`

**Purpose:** Use this overload for structural nodes that have no payload and no initial child, such as `BreakStatement`, `ContinueStatement`, or an empty list/wrapper node.

Create a node with no payload and no children. Set `valueType` to `ValueType::None`, apply all shared default initialization, and preserve the supplied kind and source line. This overload is used for nodes such as empty lists and statements that carry neither a value nor an initial child.

#### `Node(const std::string& kind, const std::string& value, int line)`

**Purpose:** Use this overload when a node stores textual information but does not receive a child during construction. Examples include identifiers, type names, function or variable names, and operator nodes whose operands will be appended afterward.

Create a string-valued node with no children. Set `valueType` to `ValueType::String`, copy `value` into `stringValue`, and initialize all non-string payload fields to their defaults. This is the only textual-payload constructor. Fixed text in parser actions must therefore be passed explicitly as `std::string`, while lexer-owned text must be copied or decoded before its token buffer is freed.

#### `Node(const std::string& kind, int value, int line)`

**Purpose:** Use this overload for nodes whose payload is an integer, primarily `IntLiteral` nodes and integer array-size values.

Create an integer-valued node with no children. Set `valueType` to `ValueType::Int`, store `value` in `intValue`, and initialize every other payload field to its default. A value of zero is still an active integer payload.

#### `Node(const std::string& kind, double value, int line)`

**Purpose:** Use this overload for nodes whose payload is a floating-point value, primarily `DecimalLiteral` nodes.

Create a double-valued node with no children. Set `valueType` to `ValueType::Double`, store `value` in `doubleValue`, and initialize every other payload field to its default. A value of `0.0` is still an active double payload.

#### `Node(const std::string& kind, char value, int line)`

**Purpose:** Use this overload for `CharacterLiteral` nodes after the parser has removed quotes and decoded any escape sequence.

Create a character-valued node with no children. Set `valueType` to `ValueType::Char`, store `value` in `charValue`, and initialize every other payload field to its default. The null character is permitted as an active character payload.

#### `Node(const std::string& kind, bool value, int line)`

**Purpose:** Use this overload for `BooleanLiteral` nodes representing the SLangCC values `true` and `false`.

Create a boolean-valued node with no children. Set `valueType` to `ValueType::Bool`, store `value` in `boolValue`, and initialize every other payload field to its default. `false` is an active boolean payload and must not be treated as no value.

#### `Node(const std::string& kind, const std::string& value, Node* child, int line)`

**Purpose:** Use this overload when a node needs both a textual payload and one initial child. In this parser, prefix and postfix increment/decrement nodes use it to store the operator while owning the target expression.

Create a string-valued node with one optional child. Follow the string-constructor requirements, then add `child` through the supplied `addChild` method so a null pointer is ignored consistently. A non-null child becomes owned by this node.

#### `Node(const std::string& kind, Node* left, Node* right, int line)`

**Purpose:** Use this overload for a structural node that begins with two ordered children, such as an assignment target and value, an array base and index, or a condition and body.

Create a node with no payload and up to two children. Set `valueType` to `ValueType::None`, apply all shared defaults, and add `left` followed by `right` through `addChild`. Preserve argument order and do not insert placeholders for null pointers.

#### `Node(const std::string& kind, Node* a, Node* b, Node* c, int line)`

**Purpose:** Use this overload for a structural node that naturally begins with three ordered children. It avoids constructing an empty node and then calling `addChild` three times.

Create a node with no payload and up to three children. Set `valueType` to `ValueType::None`, apply all shared defaults, and add `a`, `b`, and `c` in that order through `addChild`. Ignore null pointers without disturbing the relative order of non-null children.

#### `bool Node::hasChildren() const`

**Purpose:** Use this query when traversal, serialization, or validation needs to know whether a node owns at least one child.

Return `true` exactly when the `children` vector contains at least one node; otherwise return `false`. Do not inspect child contents and do not modify the vector.

#### `void Node::removeChild(size_t index)`

**Purpose:** Use this method when restructuring an AST requires detaching a child from a specific position without destroying that child inside the method.

Remove the child at the zero-based `index` only when the index is valid. An invalid index must leave the node unchanged. Detach the pointer from the vector without deleting the child; this function does not destroy the removed subtree. Remember that `size_t` is unsigned, so a lower-bound check against zero is unnecessary.

#### `Node* Node::replaceChild(size_t index, Node* child)`

**Purpose:** Use this method when an AST transformation must substitute one child while preserving its position and recovering the displaced subtree.

Replace the child at `index` only when the index is valid and `child` is non-null. Return the displaced pointer without deleting it so the caller can reuse or destroy it. The replacement becomes owned by this node. If the request is invalid, leave the vector unchanged and return `nullptr`.

#### `void Node::setSemanticType(const std::string& type)`

**Purpose:** Use this setter during the later semantic-analysis phase to record the inferred or declared SLangCC type of a node.

Copy `type` into `semanticType`. Do not modify `valueType`, any payload field, the node kind, its source line, or its children. An empty string restores the unassigned semantic-type state reported by the supplied `hasSemanticType`.

**Note**: This function is not used in this phase. Complete the function and use in the following semantic analysis phase.

#### `void Node::setKind(const std::string& kind)`

**Purpose:** Use this setter when a compiler transformation or controlled test needs to reclassify an existing AST node without rebuilding it.

Copy the supplied value into the node `kind`. Do not modify its payload, `valueType`, `semanticType`, source line, or children.

#### Recommended validation for `Node`

Test every payload constructor with both ordinary and zero-like values. Confirm that `hasValue`, `getValueType`, the typed getter, `hasChildren`, and `hasSemanticType` agree with the constructed state. Test child constructors with non-null and null arguments and verify child order. For removal and replacement, test the first and last valid indices, an out-of-range index, a null replacement, and ownership of the displaced pointer. Finally, serialize representative trees and compare their JSON structure with parser expectations.

### Bison Grammar File: Parser.y

Complete the remaining semantic actions in `parser/parser.y`.

The `program` action is already implemented as an example, along with the grammar productions, token declarations, nonterminal types, precedence declarations, helper functions, and error handler. 

**Do not redesign the grammar or change the public parser interface.** 
Every successful production must return the appropriate `Node*`, pass through an existing node when no wrapper is required, or deliberately use `nullptr` for an absent optional construct.

#### AST construction

- Study the supplied `program` action as the model for constructing a node from a reduced nonterminal, using a Bison location value, assigning the left-hand-side semantic value, and publishing the final tree through the global `ast` pointer. Complete the other productions using the same conventions.
- Construct nodes with the exact kinds, payloads, and child order expected by the supplied JSON files. This includes programs and lists; functions, parameters, blocks, and declarations; assignments and initializers; control-flow and loops; calls and argument lists; identifiers, arrays, casts, unary and binary operations; increment and decrement operations; and typed literals.
- Preserve source order when accumulating lists. Append each newly parsed item to the existing list instead of creating nested list nodes.
- Preserve distinctions such as prefix versus postfix increment/decrement, simple versus compound assignment, the three `for` components, `elseif` clauses, optional `else` clauses, and parenthesized increment/decrement targets.
- Add children in source order. Do not add null children for absent optional clauses unless the AST contract requires an empty wrapper. Empty parameter, argument, statement, and `for` components require valid empty nodes; absent array initialization, `elseif`, and `else` components may use `nullptr`.
- Assign each node the line on which its construct begins by using the supplied Bison location values.

#### Grammar action requirements

The `program` action is supplied. Implement the action for every remaining alternative as follows. Node names shown in backticks are exact and must match the expected AST output.

##### Program and top-level declarations

- `program` (provided example): creates the `Program` root around `external_list`, assigns it to the global AST pointer, and returns it. Use this action as a pattern; do not replace it.
- `external_list`: append each `external_decl` to the existing `ExternalList`. The empty alternative creates an empty `ExternalList`.
- `external_decl`: pass a `function_decl` through unchanged. If the alternative is a top-level `statement`, report that all statements must be inside a function and cause the parse to fail.
- `function_decl`: create a string-valued `FunctionDeclaration` whose value is the function name. Add the return `Type`, `ParameterList`, and `Block` in that order. Release the identifier text and return the function node.
- `type`: return a `Type` node containing the exact source type name: `int`, `double`, `boolean`, `character`, `void`, or `string`.

##### Parameters, blocks, and statements

- `param_list_opt`: pass through a populated parameter list; otherwise return an empty `ParameterList`.
- `param_list`: wrap the first parameter in `ParameterList`, then append later parameters in source order.
- `param`: create a string-valued `Parameter` containing the parameter name, add its `Type` child, and release the identifier text.
- `block`: create a `Block` containing its `StatementList`.
- `statement_list`: append each statement to the existing `StatementList`. The empty alternative creates an empty `StatementList`.
- `statement`: pass through declaration, assignment, increment, decrement, call, parenthesized expression, return, control-flow, print, exit, and nested-block nodes. Create `ContinueStatement` and `BreakStatement` nodes for those keyword alternatives. Reject a bare identifier or array access used as a complete statement.

##### Declarations and initialization

- `declaration`: create `Declaration` with the declared `Type` followed by the declarator node.
- `declarator_list`: create `DeclaratorList` around the first scalar declarator and append subsequent scalar declarators. The array alternative instead creates a string-valued `ArrayDeclarator` with the array name, size node, and optional initialization list in that order. Release the array identifier text.
- `declarator`: create a string-valued `Declarator` containing the variable name. Add its initializer when present, release the identifier text, and return the node.
- `init_var`: wrap the assigned expression or cast expression in `Initializer`. SLangCC does not permit the scalar initializer to be omitted.
- `init_array`: return the parsed `InitializationList` when braces and values are present; return `nullptr` when array initialization is absent.
- `expression_list`: wrap the first expression in `InitializationList`, then append additional expressions in source order.

##### Assignment and simple statements

- `assignment`: for `=`, create `Assignment` with the target followed by the value expression. For `+=` and `-=`, create a `CompoundAssignment` carrying the operator string and add the target and expression in that order.
- `return_stmt`: create `ReturnStatement` with the returned expression when supplied, or an empty `ReturnStatement` for a bare return.
- `print_stmt`: create `PrintStatement` containing its `ArgumentList`.
- `exit_stmt`: ensure the argument list contains no more than one expression, report a parser error otherwise, and create `ExitStatement` containing the argument list when valid.

##### Conditional and loop statements

- `if_stmt`: create `IfStatement` with condition and `Block` first. Append `ElseIfList` and `ElseClause` only when present, preserving that order.
- `elseif_list`: return `nullptr` when empty. On the first clause, create `ElseIfList`; append every later `ElseIfClause` to it.
- `elseif_clause`: create `ElseIfClause` with condition followed by its block.
- `else_opt`: create `ElseClause` containing its block when present; otherwise return `nullptr`.
- `while_stmt`: create `WhileStatement` with condition followed by its block.
- `for_stmt`: create `ForStatement` with `ForInit`, `ForCondition`, `ForUpdate`, and body `Block` in that exact order.
- `for_init_opt`: wrap a declaration, assignment, increment, decrement, or expression in `ForInit`; create an empty `ForInit` when omitted.
- `for_cond_opt`: wrap the expression in `ForCondition`; create an empty `ForCondition` when omitted.
- `for_update_opt`: wrap an assignment, increment, decrement, or expression in `ForUpdate`; create an empty `ForUpdate` when omitted.

##### Calls, assignable values, and increment/decrement

- `call_stmt`: create `CallStatement` with an `Identifier` child for the function name followed by its `ArgumentList`. Release the identifier text.
- `increment_stmt`: create `PreIncrement` or `PostIncrement`, store `++` as its value, and attach the target. For a parenthesized expression target, use the supplied helper to reject computed expressions while allowing identifiers and array accesses.
- `decrement_stmt`: mirror the increment behavior with `PreDecrement`, `PostDecrement`, and the `--` value. Apply the same target validation to parenthesized expressions. No decrement alternative should behave as an empty production.
- `opvalue`: convert a plain identifier to `Identifier`. For indexed access, create `ArrayAccess` with the base `Identifier` followed by the index node. Release identifier text in both cases.
- `bracket_value`: convert an identifier index to `Identifier` or an integer index to `IntLiteral`.
- `paren_expression`: wrap a parenthesized expression in `ParenthesizedExpression`. For a parenthesized assignment, pass the `Assignment` node through without adding that wrapper.

##### Expressions, casts, and literals

- `expression`: for every logical, equality, relational, additive, and multiplicative alternative, create a string-valued `BinaryOp` containing the exact operator and add the left and right operands in that order. Pass `unary` and `call_stmt` through unchanged.
- `cast_expression`: create `CastExpression` with the destination `Type` followed by the converted value.
- `cast_value`: convert an identifier, integer, decimal, or character token to `Identifier`, `IntLiteral`, `DecimalLiteral`, or decoded `CharacterLiteral`, respectively. Release lexer-owned `char*` token storage after copying or decoding it.
- `unary`: create a string-valued `UnaryOp` for `!`, unary `-`, or unary `+` and attach its operand. Pass `primary` through unchanged.
- `primary`: pass an `opvalue` through; create the appropriate `IntLiteral`, `DecimalLiteral`, decoded `StringLiteral`, decoded `CharacterLiteral`, or boolean-valued `BooleanLiteral`; and pass an expression in ordinary grouping parentheses through without adding a wrapper. Release string and character token storage after decoding it.
- `argument_list_opt`: pass through a populated argument list; otherwise create an empty `ArgumentList`.
- `argument_list`: wrap the first expression in `ArgumentList`, then append later expressions in source order.
- `empty`: perform no AST construction. The production that uses `empty` is responsible for creating its required empty wrapper or returning `nullptr`.

#### Values and memory

- Copy identifier text into string-valued nodes before releasing the lexer-owned `char*` token buffer.
- Preserve integer and decimal token values in `IntLiteral` and `DecimalLiteral` nodes.
- Decode quotes and escape sequences before storing string and character literals. Use the supplied decoding helpers instead of duplicating that logic in grammar actions.
- Store `true` and `false` as boolean payloads rather than strings.
- Release every lexer-owned `char*` value received from identifier, string-literal, and character-literal tokens after copying or decoding it. Do not free nodes after transferring them to a parent; the AST owns its children.

##### Choosing the correct `Node` constructor

The constructor overload is selected by the payload argument after `kind`. The first argument always names the AST node; the second argument, when present, determines how its payload is stored.

| Value being stored | Constructor payload type | Example |
|---|---|---|
| Any textual payload, including fixed type/operator text, identifiers, and decoded strings | `const std::string&` | `new Node("Type", std::string("int"), line)` |
| Integer token value | `int` | `new Node("IntLiteral", integerValue, line)` |
| Decimal token value | `double` | `new Node("DecimalLiteral", decimalValue, line)` |
| Decoded character value | `char` | `new Node("CharacterLiteral", characterValue, line)` |
| Boolean language value | `bool` | `new Node("BooleanLiteral", true, line)` |
| No payload | no payload argument | `new Node("BreakStatement", line)` |

Use the `std::string` overload for **all textual payloads**. Fixed text written directly in a grammar action must be wrapped explicitly:

```cpp
new Node("Type", std::string("int"), line);
new Node("BinaryOp", std::string("+"), line);
new Node("CompoundAssignment", std::string("+="), line);
```

The explicit `std::string` is required because the class also has a `bool` overload. Passing a raw string literal as the payload could select the boolean constructor.

Text received from the lexer also becomes a `std::string`, but lexer ownership must be handled separately. For an identifier, copy the dynamically allocated token text before freeing it:

```cpp
std::string name(identifierText);
Node* identifier = new Node("Identifier", name, line);
free(identifierText);
```

Decode string-literal text before constructing its node, then free the original token allocation:

```cpp
std::string value = decodeQuotedLiteral(literalText);
Node* literal = new Node("StringLiteral", value, line);
free(literalText);
```

The lexer may still supply text through `char*`, but `Node` no longer has a C-string payload constructor. Every textual payload passed to `Node` must be an explicit `std::string`, and a node never retains lexer-owned memory.

#### Required parser checks

Report the relevant source line with `Error::parser` or `Error::semantic`, then invoke Bison error handling, for constructs that the grammar recognizes but SLangCC forbids:

- executable statements and declarations at the top level, since only function declarations may appear outside a function;
- a standalone identifier or array access used as a complete statement;
- an `exit` call with more than one argument;
- increment or decrement applied to a computed parenthesized expression. A valid target is an identifier or array access, optionally enclosed by the supported parentheses. Use the supplied target-checking helper.

#### Completion criteria

The parser must build without new Bison conflicts, accept every legal parser benchmark, reject every illegal benchmark with the expected diagnostic, and serialize legal input to AST JSON matching `benchmark/parser/expected/`. Merely accepting an input is insufficient: node kinds, values, line numbers, list structure, and child order are all part of the required output.


## Tests

From the `lexer/lexer` directory, build the standalone lexer:

```bash
make
```

A successful build creates the `parser/parser` executable. If compilation fails, read the first reported error, correct your rules or actions in `parser/parser.y`, and rebuild.

The parser test inputs are located in `benchmark/parser/`. To run one test and print its tokens or diagnostics in the terminal, pass the source-file path to the executable:

```bash
<path>/<to>/parser <path>/<to>/benchmark/parser/01_legal_minimal_function.sl
```

When parsing succeeds, the parser writes the abstract syntax tree (AST) to a JSON file named after the input source file. For example, parsing `01_legal_minimal_function.sl` generates `01_legal_minimal_function.json` in the current directory.

To run the complete local test suite, enter the benchmark directory and execute its test script:

```bash
cd benchmark/parser
./test.sh
```

The script runs every `.sl` test, stores the generated results in `benchmark/parser/out/`, and compares them with the reference files in `benchmark/parser/expected/`. It prints a pass/fail summary and shows a diff when a test does not match the expected output.

## Hint

### Begin with the `Node` class

Complete and test the assigned functions in `shared/ast/node.cpp` before writing many parser actions. Nearly every grammar action depends on a correctly initialized `Node`, working `addChild`, and predictable ownership behavior. Use the provided one-child constructor as a checklist: initialize the kind, payload type, source line, semantic type, and every unused primitive field.

Build a few small trees manually and inspect their getters or JSON output. Include leaf nodes, nodes with zero-like values, multiple children, and null child arguments. This isolates `Node` bugs before they appear as confusing parser-test failures.

### Read Bison action values carefully

In a grammar action, `$$` is the value produced by the nonterminal on the left side of the rule. `$1`, `$2`, and later numbered values refer to symbols on the right side, while `@1`, `@2`, and later numbered values contain their source locations. Study the supplied `program` action to see how a child node and line number are used to create and return a parent node. See the [Bison semantic actions documentation](https://www.gnu.org/software/bison/manual/html_node/Semantic-Actions.html) and [Bison location tracking documentation](https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html) for additional details.

Count the symbols in each production before using a numbered value. Punctuation tokens count too. For example, parentheses and commas affect which number refers to an expression or list. A wrong number may compile but create an incorrect tree or crash at runtime.

### Implement the grammar incrementally

Work from simpler, lower-level productions toward larger constructs:

1. Types, identifiers, and literals.
2. Primary, unary, binary, and cast expressions.
3. Lists, parameters, blocks, and declarations.
4. Assignments, calls, and simple statements.
5. Conditionals, loops, and complete functions.
6. Parser-level error checks and edge cases.

After finishing a small group, rebuild and run only the relevant benchmark tests. Do not wait until every action is written before compiling. Small commits also make it easier to locate a regression.

### Match the required AST shape

Use the files in `benchmark/parser/expected/` as the output contract. When a legal test differs, compare the first mismatched node and check its kind, payload type and value, line number, child order, and whether an unnecessary wrapper was introduced.

List productions should normally create one list node and append later items to it in source order. For pass-through productions, return the existing child instead of creating a redundant node. For optional productions, distinguish between a required empty wrapper and a truly absent child represented by `nullptr`; follow the production requirements in this document.

### Watch ownership and token memory

Once a node is added to a parent, the parent owns it. Do not delete that child separately or attach the same pointer to multiple parents. The AST root will eventually release the complete tree through recursive destruction.

Identifier, string-literal, and character-literal token values are dynamically allocated by the lexer. Copy or decode their contents before freeing the token storage. Numeric and boolean values do not require this step. If a parser crashes or produces corrupted text, check pointer ownership and the order in which text is copied and released.

### Debug one failure at a time

Start with the smallest legal test for the feature being implemented. Confirm that it parses, then compare its generated JSON with the expected file. Next run the related illegal tests and verify that they fail at the correct line. The first difference in a test-suite diff is usually more useful than the final pass count.

When changing grammar actions, perform a clean rebuild if generated parser files appear stale. Address the first compiler or Bison error before later messages, since many later errors may be consequences of the first one. Symbol-table construction and semantic type checking are not part of this phase.
