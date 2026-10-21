# Phase: Semantic Analysis

## Learning Objective

Implement the semantic-analysis phase of SLangCC by extending your completed Bison parser to construct a scoped symbol table, implementing the `Symbol` and `SymbolTable` classes, and completing selected functions in `SemanticAnalyzer`. By completing this phase, you will be able to represent declarations and function signatures, manage function-level scopes, resolve identifiers and calls, infer expression types, detect type mismatches and other semantic errors, and attach semantic types to the AST created in the parser phase.

This is a **three-week phase**. You will continue using the lexer, parser, and `Node` class that you completed in the previous phases. You will **not** implement cast rewriting or print-argument rewriting; those are demonstrated by instructor-provided semantic-analyzer functions.

## Starter Files

The semantic-phase distribution adds these files to your **existing** SLangCC project:

- `semantic_analyzer/main.cpp`: Provided driver that parses a source file, runs semantic analysis, and writes the annotated AST as JSON when no errors occur.
- `semantic_analyzer/semanticAnalyzer.h`: Provided class declaration. Do not modify it.
- `semantic_analyzer/semanticAnalyzer.cpp`: Contains selected, completed instructor implementations. You must write the **12 missing member-function definitions**, using the signatures in the header. No function skeletons are supplied for those methods.
- `shared/symboltable/symbol.h`: Provided class declaration and field definitions. It contains method declarations only. Do not modify it.
- `shared/symboltable/symbol.cpp`: Contains the completed `Symbol::printSymbol()` function. You must write the remaining member function definitions from the header; no skeletons are provided.
- `shared/symboltable/symbolTable.h`: Provided class declaration. Do not modify it.
- `shared/symboltable/symbolTable.cpp`: Contains the completed `SymbolTable::printSymbolTable()` function. You must write the remaining member-function definitions from the header; no skeletons are supplied.
- `parser/parser_interface.h`, build files, shared error-handling support, and `benchmark/semantic/`: Provided infrastructure for integrating and testing this phase.

**`parser/parser.y` is not supplied as a replacement.** Keep the `parser.y` you completed during the parser phase. You must add the symbol-table integration described below to its existing grammar actions. Do not copy over or discard your completed AST-building actions.

## Directory Setup

At the start of this phase, your existing project should contain your completed lexer and parser work, including `parser/parser.y` and `shared/ast/node.h` and `node.cpp`. Commit or back up that work before copying new files.

Extract the semantic-phase starter archive into a temporary directory. Copy the provided **new and updated infrastructure files** into the appropriate paths in your existing `SLangCC/` directory. Merge directories with matching names. **Do not overwrite your existing `parser/parser.y`, `lexer/lexer.l`, or completed `Node` implementation.** If the archive contains any of those paths, retain your completed student versions instead.

After merging, the relevant structure should be:

```text
SLangCC/
├── benchmark/
│   ├── lexer/
│   ├── parser/
│   └── semantic/
├── lexer/
│   ...files from the earlier phase...
├── parser/
│   ...files from the earlier phase...
├── semantic_analyzer/
│   ├── main.cpp
│   ├── semanticAnalyzer.h
│   └── semanticAnalyzer.cpp
└── shared/
    ├── ast/
    ├── error/
    └── symboltable/
        ├── symbol.h
        ├── symbol.cpp
        ├── symbolTable.h
        └── symbolTable.cpp
```

Run build and test commands from the indicated project directory. The semantic-analyzer executable compiles the parser, lexer, AST, error handling, symbol table, and semantic analyzer into a single executable.

## Description

In the parser phase, you implemented grammar actions that produce the AST. In this phase, those same actions must also record declarations so that the semantic analyzer can resolve names and check types. The parser will build both the **AST** and the **symbol table**. After parsing completes successfully, the provided driver runs `SemanticAnalyzer` on the AST and writes the resulting JSON only if no errors are reported.

### Files You May Modify

You are expected to implement or update **exactly these four source files**:

| File | Required work |
|---|---|
| `parser/parser_interface.h` | Include `symbolTable.h` and add the `extern` declaration for the shared symbol table. Preserve all existing declarations. |
| `parser/parser.y` | Add symbol-table declarations and semantic-phase code to the existing `declaration` and `function_decl` actions. Preserve all parser-phase AST behavior. |
| `shared/symboltable/symbol.cpp` | Implement every declared method other than the provided `printSymbol()`. |
| `shared/symboltable/symbolTable.cpp` | Implement every declared method other than the provided `printSymbolTable()`. |
| `semantic_analyzer/semanticAnalyzer.cpp` | Implement the 12 student-assigned methods listed below. Leave the provided methods intact. |

Do **not** modify the class headers, `Node` files, parser grammar productions, lexer rules, driver files, error-handling support, or expected benchmark files. In particular, the `Node::setSemanticType()` and `Node::replaceChild()` methods are prerequisites from the parser phase, not new assignments.

### Recommended Timeline

- **First Week — Symbols and parser integration:** Implement `Symbol` and `SymbolTable`, then extend `declaration` and `function_decl` in `parser.y` to queue, scope, insert, and link symbols. Test insertion, lookup, duplicates, function scopes, and array metadata using small standalone symbol-table tests; verify that the existing parser actions remain intact. Benchmarks **06–19** are the later end-to-end targets for this work.
- **Second Week — Expression inference and assignments:** Implement `inferLiteralType`, `inferParenthesizedExpressionType`, `inferIdentifierType`, `inferArrayAccessType`, `inferUnaryOpType`, `inferBinaryOpType`, `isAssignable`, `visitDeclaration`, and `visitAssignment`. Use targeted checks and compare intended annotations and diagnostics against benchmarks **13–32** (cast functionality is supplied). The complete semantic executable may not link yet because Week 3 methods are still missing.
- **Third Week — Functions and integration:** Implement `inferCallStatementType`, `visitCallStatement`, and `visitReturnStatement`. Complete the first successful full link; test function signatures, argument checking, calls, returns, and `main` restrictions (benchmarks **01–05** and **33–48**). Then run **all 48** semantic benchmarks, correct output differences, perform a clean rebuild, and re-run parser benchmarks to detect regressions.

## Program Requirements

### Pipeline and Scope Rules

The semantic analyzer follows this order:

1. The lexer and parser recognize the source and construct the AST.
2. The parser's semantic-phase actions create and register symbols in a shared `SymbolTable`.
3. If parsing or symbol insertion reported errors, the driver stops without producing JSON.
4. Otherwise, the provided `SemanticAnalyzer::analyze()` traverses the AST and performs semantic checks.
5. If any semantic errors were reported, the driver does not write JSON. For valid programs, it serializes the **annotated and, where needed, rewritten AST**.

SLangCC uses **one scope per function** in this phase. There are **no global variables** and **no nested block scopes**. All parameters and local declarations of a function share a single namespace, including declarations inside nested blocks. Duplicate names in the same function are invalid. The same local name is permitted in different functions.

A function symbol uses its **function name as both the scope key and symbol name**. Its parameters and locals use the enclosing function name as their scope. Lookup does not search parent scopes or other functions.

### `Symbol` Class — `shared/symboltable/symbol.cpp`

Complete every `Symbol` member function declared in `symbol.h` except `printSymbol()`, which is supplied. Write full definitions in `symbol.cpp`, outside the class declaration. Do not add implementations to the header.

#### `Symbol::Symbol(Node* type, Node* decl)`

**Purpose:** Construct one symbol from the AST nodes created by the parser.

Initialize every data field deterministically (including inactive payloads, flags, pointers, array sizes, and scope). Read the declared type from the `Type` node and the name and source line from the declaration node. Support these exact AST node kinds:

| Declaration node kind | Required symbol information |
|---|---|
| `FunctionDeclaration` | Name, return type, source line, function flag, scope equal to its name, and an ordered list of parameter symbols obtained from its `ParameterList` child. |
| `Parameter` | Parameter name, declared type, source line; its enclosing scope will be set later by the parser. |
| `Declarator` | Scalar name, declared type, line, and an immediately available initializer payload when applicable. |
| `ArrayDeclarator` | Array name, element type, line, array flag, and either a literal integer size or the name of an identifier used as its size. |

For a function, the parser-phase `FunctionDeclaration` node has the children `Type`, `ParameterList`, and `Block`, in that order. Each `Parameter` contains its own `Type` child. Preserve parameter order because call argument types are checked positionally.

For a scalar, read the initializer through the `Declarator` → `Initializer` → expression structure. Store a directly available literal payload when available, including valid zero-like values. A node's `Node::ValueType` tells you what payload it stores; it is **not** the SLangCC semantic type. Do not attempt to evaluate arbitrary expressions in this constructor.

For an array, the first child of `ArrayDeclarator` is its size node. Record an integer size for `IntLiteral`, or the identifier text for an identifier-sized array. Do not treat a size identifier's name as a numeric value.

Malformed AST shapes are programming/internal errors; ordinary invalid source constructs must be reported using the semantic diagnostic mechanism at the appropriate stage.

#### Other `Symbol` functions

Implement the following functions using the declarations in `symbol.h`:

| Function | Required behavior |
|---|---|
| `~Symbol()` | Release resources owned **by this symbol** as appropriate; do not delete the AST or a linked array-size symbol owned by the table. |
| `getName()` | Return the symbol name. |
| `getType()` | Return its declared SLangCC type. |
| `getIntValue()` | Return the stored integer payload used, among other things, to resolve integer-named array sizes. |
| `updateScope(const std::string&)` | Set the function scope for a parameter or local. |
| `updateArraySizeSymbol(const Symbol*)` | Store the link to the symbol used as an array's size. |
| `getLine()` | Return the declaration's source line. |
| `getScope()` | Return the symbol's scope. |
| `getArraySizeSymbolName()` | Return the stored identifier name, if the array size was given by an identifier. |
| `getParameters()` | Return the function's parameter-symbol list in source order. |
| `isArraySymbol()` | Indicate whether the symbol represents an array. |
| `getArraySize()` | Use the linked size symbol's integer value when available; otherwise return the stored literal size. |

**Provided example:** `printSymbol()` is already implemented and must not be rewritten. It calls several getters and therefore also helps you test their implementations.

### `SymbolTable` Class — `shared/symboltable/symbolTable.cpp`

Complete the following missing methods; leave `printSymbolTable()` unchanged:

#### `SymbolTable::SymbolTable()`

**Purpose:** Construct an initially empty symbol table. The private storage is a `std::map<std::string, std::vector<Symbol*>>` keyed by function scope.

#### `SymbolTable::~SymbolTable()`

**Purpose:** Iterate through every function scope in the symbol table and delete each stored symbol exactly once. Do not delete AST nodes or pointers returned by `lookup()`.

#### `void insertSymbol(const std::string& scope, int line, Symbol* symbol)`

**Purpose:** Insert the **function symbol** and establish a new scope.

A new function creates a new map entry whose first symbol is the function itself. Reject another function with the same scope/name and report its declaration line with `Error::semantic` using the existing diagnostic conventions. The function scope must exist before its parameters or locals are inserted.

#### `void insertSymbol(const std::string& scope, Symbol* symbol)`

**Purpose:** Insert a **parameter or local declaration** into a function's existing scope.

Reject insertion if the function scope does not exist. Within the scope, reject an existing symbol with the same name; this includes collisions between parameters and locals, or between two locals, anywhere in that function. Different functions may have locals with identical names. Report errors through `Error::semantic` using the symbol's source line.

#### `const Symbol* lookup(const std::string& scope, const std::string& name, int line) const`

**Purpose:** Find a symbol in exactly the specified scope.

Return a pointer to the matching symbol, or `nullptr` when either the scope or name is missing. Do not search other scopes or create a new symbol. Do not report an undeclared-name error inside `lookup()` itself; the semantic analyzer or other caller chooses the appropriate diagnostic. The `line` argument is available to the caller's lookup context.

**Pointer handling:** The symbol table stores pointers to dynamically allocated symbols. Successfully inserted symbols must be deleted by the symbol table's destructor. If insertion fails, ensure the rejected symbol is deleted exactly once, either by `insertSymbol()` or by the calling parser code. Do not delete symbols during lookup. Do not copy a symbol table by simply copying its stored pointers.

**Provided example:** `printSymbolTable()` is complete and must not be rewritten.

### Updating `parser/parser_interface.h`

You already have `parser/parser_interface.h` from the parser phase. For semantic analysis, update this file to expose the symbol table constructed by the parser.

**1.** Add the following header alongside the existing includes:

```cpp
#include "../shared/symboltable/symbolTable.h"
```

**2.** Add the following declaration after `extern Node *ast;`:

```cpp
/** Symbol table shared with the semantic analyzer. */
extern SymbolTable symbolTable;
```

The `extern` declaration allows other compiler components to access the symbol table defined in `parser.y`. Do not create another symbol table instance in this header.

Preserve all existing declarations and functionality. No other changes to `parser_interface.h` are required.

### Semantic Work in Your Existing `parser/parser.y`

**Purpose:** Extend your parser-phase actions so that parsing constructs both the AST and a symbol table. You are not being given a new `parser.y`.

Do not modify the grammar productions, tokens, precedence declarations, parser interface, AST node kinds, existing child order, or lexer-owned memory handling. The completed AST actions from the parser phase must continue to work exactly as before.

#### 1. Add the symbol-table dependencies and shared state

In the C/C++ prologue of `parser.y` (the first `%{ ... %}` section), add the headers needed to use `Symbol` and `SymbolTable` and a container such as `std::queue<Symbol*>` to temporarily retain local symbols. Define the shared **`SymbolTable symbolTable`** object required by the provided `parser_interface.h` and the semantic-analyzer driver; ensure it is defined **once**, not separately in multiple files.

The pending-symbol container is needed because Bison's reductions are bottom-up: the `declaration` action runs **before** the enclosing `function_decl` action, so the function name may not be available when a local symbol is first created. Do not confuse the AST pointer `ast` with the symbol-table object.

#### 2. Extend the `declaration` action

The parser-phase `declaration` action already builds a `Declaration` node with its `Type` and declarator child. **Keep those AST operations.** Afterward, perform these additional semantic-phase operations:

- When the declarator child is `DeclaratorList`, traverse its `Declarator` children **in source order**. Create one `Symbol` for each using the declaration's `Type` and that individual declarator node.
- When the declarator child is `ArrayDeclarator`, create one array symbol using the same `Type` node and that array declarator.
- Place each created symbol into the pending container; do not insert it into the table yet, because its function scope has not been established.
- Leave the AST result (`$$`) equal to the completed `Declaration` node; do not attach symbol objects as AST children.

Do not change the parser's existing handling of scalar initializers, array initialization lists, line numbers, token, or declaration-list child order.

#### 3. Extend the `function_decl` action

The parser-phase action already constructs `FunctionDeclaration` with the ordered children `Type`, `ParameterList`, and `Block`. Preserve that AST construction, then perform the following operations **before releasing the function-name token**:

1. **Register the function:** Construct its `Symbol` from the return-type `Type` node and the completed `FunctionDeclaration` node. Call the function-insertion overload of `insertSymbol`, using the function name as its scope and the source line for diagnostics.
2. **Register the parameters:** Walk the `ParameterList` in source order. For each `Parameter`, create a symbol from the parameter's `Type` child and the parameter node, call `updateScope()` with the function name, and use the local-symbol insertion overload. Reject duplicates in the same function.
3. **Register queued locals:** In first-in, first-out order, take each pending scalar or array symbol, set its scope with `updateScope()`, and insert it into that function's table entry. This includes declarations nested inside blocks; they share the function scope.
4. **Link identifier-sized arrays:** For a queued array whose size was written as an identifier, look up that name in the same function scope and store the result with `updateArraySizeSymbol()`. A size declaration preceding the array can therefore be resolved during this pass. Leave legality and positivity checks to semantic analysis.
5. **Clear the pending container:** Every queued declaration must be removed before the next function is parsed. Keep the existing function-name token cleanup and return the completed `FunctionDeclaration` AST node.

The **order matters**: create the function scope first, insert parameters before locals, and process queued locals in the order they were encountered (see the Hint section for more detail). This ensures duplicate names are detected in the correct namespace and that earlier integer declarations can be used as array sizes.

Do not add symbol insertion to unrelated grammar actions. A parser-level error such as an illegal top-level statement remains a parser-phase behavior and must be preserved.

#### 4. Required parser-side checks

Test at least these conditions while developing your integration:

- Two functions with the same name must be rejected.
- Duplicate parameters, duplicate locals, and parameter/local name conflicts in one function must be rejected.
- The same local name in two different functions is permitted.
- Each scalar in a comma-separated declaration is registered independently.
- Function signatures retain parameter order.
- Literal-sized and identifier-sized arrays are recorded correctly; the latter can be linked to an earlier integer variable in the same function.
- Completing one function does not leave its pending declarations attached to the next function.

Use the Bison source locations already present in your parser actions; do not substitute the number of a token occurring later in the production when reporting a declaration error.

### `SemanticAnalyzer` — `semantic_analyzer/semanticAnalyzer.cpp`

Keep `semanticAnalyzer.h` unchanged. The distributed `.cpp` contains **13 completed instructor implementations**. The following **12 functions are missing** and must be defined by you with exactly the signatures declared in the header:

| Student-implemented function | Main responsibility |
|---|---|
| `inferLiteralType()` | Infer and annotate literal types. |
| `inferParenthesizedExpressionType()` | Infer and annotate the enclosed expression's type. |
| `inferIdentifierType()` | Resolve a local/parameter name and annotate its declared type. |
| `inferArrayAccessType()` | Resolve an array access and annotate its element type. |
| `inferUnaryOpType()` | Infer an operand and the enclosing unary expression. |
| `inferBinaryOpType()` | Infer both operands and enforce the phase's type-compatibility rule. |
| `isAssignable()` | Check exact type compatibility. |
| `visitDeclaration()` | Validate scalar initializers and array sizes. |
| `visitAssignment()` | Validate the target's declared type against the assigned expression. |
| `visitReturnStatement()` | Check `return` against the current function's return type. |
| `inferCallStatementType()` | Resolve a function, check its arguments, and infer its return type. |
| `visitCallStatement()` | Require a call used as a statement to return `void`. |

#### Supplied `SemanticAnalyzer` methods — do not rewrite

The starter file already contains:

- `SemanticAnalyzer(SymbolTable&)`, `analyze()`, `visit()`, `visitProgram()`, and `visitFunctionDeclaration()` for initialization, dispatch, traversal, and function context;
- `inferExpressionType()` for dispatching among the specialized inference methods;
- `setExpressionType()` for recording semantic types on AST nodes;
- `visitPrintStatement()` and `inferCastExpressionType()` for validation and AST transformations;
- `isNumericType()`, `isCastable()`, `reportTypeMismatch()`, and `isLiteralKind()` as helpers.

Read these implementations before writing your functions. For example, `inferExpressionType()` calls the student-implemented inference methods, and `visitFunctionDeclaration()` establishes `currentScope` and `currentReturnType` before student-implemented visitors run.

#### `inferLiteralType(Node* expression)`

**Purpose:** Return and store the type represented by a literal node. Use the exact mappings:

| AST kind | SLangCC semantic type |
|---|---|
| `IntLiteral` | `int` |
| `DecimalLiteral` | `double` |
| `CharacterLiteral` | `character` |
| `BooleanLiteral` | `boolean` |
| `StringLiteral` | `string` |

Use the supplied `setExpressionType()` helper so the inferred type appears in the AST JSON. Return an empty type for an unsupported node kind.

#### `inferParenthesizedExpressionType(Node* expression)`

**Purpose:** Obtain the type of the wrapped expression without inventing a new type. The parser's `ParenthesizedExpression` node contains its enclosed expression as a child. Reject an empty/malformed wrapper through the semantic diagnostic mechanism, infer its child recursively with `inferExpressionType()`, and store the resulting type on the wrapper using `setExpressionType()`.

#### `inferIdentifierType(Node* expression)`

**Purpose:** Resolve an identifier in the current function. Read its name from the AST node, query `symbolTable.lookup(currentScope, name, line)`, report an undeclared identifier when lookup fails, and otherwise annotate and return the symbol's declared type. Function names used as calls are resolved separately in `inferCallStatementType()`.

#### `inferArrayAccessType(Node* expression)`

**Purpose:** Determine the declared element type for an array access. The first child of an `ArrayAccess` is its base `Identifier`; its second child represents the index. Extract the array name, look it up in the current function, report the appropriate undeclared-array diagnostic if missing, and annotate the `ArrayAccess` with its element type. Preserve the AST structure. Do not create a second symbol for each access.

The supplied benchmark suite directly checks for successful and undeclared array access. It does **not** establish additional requirements for rejecting indexing a scalar or for operator-style index checking; do not invent new diagnostic text or AST annotations beyond the supplied output contract.

#### `inferUnaryOpType(Node* expression)`

**Purpose:** Infer the type of a `UnaryOp`. Require one operand child; report a semantic error for a malformed unary expression. Infer the operand recursively, propagate an unsuccessful inference as an empty type, and otherwise store and return the operand type for the unary node. For this phase, no additional unary-operator-specific type rules are required by the supplied reference implementation.

#### `inferBinaryOpType(Node* expression)`

**Purpose:** Infer the types of the two operands of a `BinaryOp`. Reject a malformed node with an incorrect number of children. Infer left and right operands; if either fails, propagate the failure. For this phase's **current reference/benchmark contract**, matching operand types produce that same type on the binary node. Different operand types produce the specified incompatible-operands semantic diagnostic.

Do not silently add implicit conversions. Operator-specific result typing (such as always making comparison operators `boolean`) is **not part of the current reference implementation**; follow the distributed expected AST JSON until an updated specification is supplied.

#### `bool isAssignable(const std::string& expected, const std::string& actual) const`

**Purpose:** Apply the phase's assignment-compatibility rule. Return `true` only when the source and destination types are identical (e.g., both are `int` or both are `double`). Return `false` otherwise. SLangCC does not support implicit type conversions, such as assigning an `int` value to a `double` variable. Do not implicitly widen `int` to `double` or narrow `double` to `int`. The same exact-type rule is used when validating initializers and returns, and function arguments must match their corresponding parameter types.

#### `visitDeclaration(Node* node)`

**Purpose:** Validate a parser-built declaration without changing its AST structure. A `Declaration` contains `Type` followed by either `DeclaratorList` (scalars) or `ArrayDeclarator` (an array).

For each scalar `Declarator`, locate its `Initializer` expression, infer its type with `inferExpressionType()`, and use `isAssignable()` to compare it with the declared `Type`. Use the provided `reportTypeMismatch()` helper and the declaration's source location when types differ.

For an `ArrayDeclarator`, examine its first child (the size): a literal size must be a **positive integer**; an identifier size must resolve to an integer symbol whose recorded value is positive. Report missing identifiers, non-integer types, and zero or negative sizes using the error messages and line numbers required by `benchmark/semantic/expected/`. Invalid AST shapes should be diagnosed without dereferencing absent children.

No new array-initialization conversion or AST rewrite is assigned in this method. Preserve any existing initialization children.

#### `visitAssignment(Node* node)`

**Purpose:** Validate assignment to an identifier or array element. In an `Assignment` node, the left child names the target (`Identifier` or `ArrayAccess`) and the right child is the value expression. Resolve the target symbol in the current function; report an undeclared target if missing. Infer the right-hand side with `inferExpressionType()` and compare its type with the symbol's declared type using `isAssignable()`. Report mismatches through `reportTypeMismatch()` at the assignment's source line.

#### `inferCallStatementType(Node* expression)`

**Purpose:** Resolve and type-check a function call used as an expression or statement. A `CallStatement` contains an `Identifier` child for the function name and an `ArgumentList` child in source order.

- Reject calls to `main`.
- Look up the function using **its own name as both scope and name**; report an undeclared function if missing.
- Infer each argument through `inferExpressionType()`.
- Compare the number of arguments to the function's ordered parameter list.
- Compare each argument's inferred type with the corresponding parameter's declared type; report count and type errors as required by the benchmark expected diagnostics.
- On a valid call, annotate the `CallStatement` with the function's declared return type, including `void`, and return that type.

Calls may appear inside initializers, binary expressions, arguments to other functions, and standalone statements. Do not reorder arguments or modify the function signature stored in its symbol.

#### `visitCallStatement(Node* node)`

**Purpose:** Validate a call used as a standalone statement. Invoke `inferCallStatementType()` and require a `void` return type. A non-void function call used as a statement is invalid. Do not reject a non-void call **solely because** it appears inside a larger expression; that context is handled by the caller of `inferExpressionType()`.

#### `visitReturnStatement(Node* node)`

**Purpose:** Check a return statement against the function context established by the provided `visitFunctionDeclaration()`.

- In a `void` function, a `return` with a value is invalid; a bare `return` is allowed.
- In a non-void function, a bare `return` is invalid; a return expression must be present.
- When a non-void return expression exists, infer its type and check exact compatibility with `currentReturnType`. Report a return-type mismatch at the return statement's source line.

This phase checks **encountered return statements**; it does not require a control-flow proof that every possible path through a non-void function returns a value.

### Instructor-Provided Checks and Transformations

You must **understand** these behaviors and preserve them, but you do **not** implement their methods:

- `visitFunctionDeclaration()` sets the function's current scope and return type. `main` must return `void`.
- `inferCastExpressionType()` validates explicit casts, annotates their target type, renames a valid `CastExpression` to the required conversion kind, and removes the old `Type` child. Allowed conversions are `int`→`double`, `int`→`character`, `character`→`int`, `character`→`string`, `int`→`string`, `double`→`string`, and `boolean`→`string`.
- `visitPrintStatement()` requires print arguments, validates supported types, and wraps non-string print arguments in the appropriate string-conversion node.
- `setExpressionType()` uses the `Node::setSemanticType()` method you completed during the parser phase. It records SLangCC types independently of `Node::ValueType`.

These supplied methods depend on the types inferred by your functions. Incorrect or missing inference can therefore cause incorrect cast/print JSON even though the transformation code was provided.

### Semantic Types and AST Output

Use the exact SLangCC type strings `int`, `double`, `character`, `boolean`, `string`, and `void`. A successfully inferred expression type must be recorded with `setExpressionType()` **when the relevant inference function is called**; do not alter a node's literal payload or `Node::ValueType` to represent semantic type.

For example, an `Identifier` may have a string payload containing `count`, while its semantic type is `int`. The JSON serializer you received in the parser phase already knows how to include semantic type annotations. You do not need to implement serialization again.

Keep AST kinds, source lines, child order, and the supplied cast/print rewrite behavior consistent with `benchmark/semantic/expected/*.json`. Do not add arbitrary annotations to structural nodes that the expected output leaves untyped.

### Diagnostics and Ownership

Use `Error::semantic(message, line)` for invalid SLangCC programs. Use the original source line recorded in the relevant AST node or Bison location. The provided `Error` facility determines the error category, source filename, and formatting. Match the exact spelling and punctuation of diagnostics in `benchmark/semantic/expected/*.err`.

Some current expected `.err` files contain **multiple diagnostics for a single input**, so match the supplied output rather than assuming that every error stops semantic traversal immediately. Do not cause crashes when a lookup fails or an expression cannot be typed. Null results and empty inferred type strings should be handled carefully in the student-implemented methods.

The AST owns its child nodes. Symbol-table lookup pointers and resolved array-size pointers are non-owning. Do not attach the same AST subtree to two parents, delete a table-owned symbol while it is still stored, or leak rejected allocations. The `Node::removeChild()` and `Node::replaceChild()` behavior comes from your parser-phase implementation; provided transformations depend on it.

### Completion Criteria

The completed compiler must:

- Build the semantic-analyzer executable without linker errors from missing student-implemented methods.
- Preserve all previously completed lexer/parser behavior and AST shapes.
- Build exactly one function-level symbol scope per function, with correct function, parameter, scalar, and array symbols.
- Reject duplicate declarations and report undeclared names at the expected source line.
- Infer and annotate types through the assigned methods, validate assignments, arguments, and returns, and preserve instructor-provided cast and print rewriting.
- Accept every legal program and reject every illegal program in `benchmark/semantic/`.
- Match the corresponding expected **JSON or diagnostics exactly** without producing JSON for invalid programs.

## Tests

Because the student starter `.cpp` files intentionally omit function definitions, **initial full linking failures are expected** until the required methods have been implemented. During early development, compile individual source files or run small, focused tests to check interfaces, then switch to the full build once dependencies are complete.

From the project root, build the semantic analyzer:

```sh
cd semantic_analyzer
make clean
make
```

A successful build creates the `semantic_analyzer/semantic_analyzer` executable. The semantic Makefile builds the compiler using `parser/parser.y`, `lexer/lexer.l`, the AST, the symbol table, and your semantic-analyzer implementation.

Run a single semantic benchmark from the **project root**:

```sh
./semantic_analyzer/semantic_analyzer benchmark/semantic/06_legal_scalar_declarations_all_types.sl
```

When analysis succeeds, the executable creates `06_legal_scalar_declarations_all_types.json` in the current working directory. An optional second argument provides an output directory:

```sh
./semantic_analyzer/semantic_analyzer benchmark/semantic/06_legal_scalar_declarations_all_types.sl benchmark/semantic/out
```

To run the complete suite after building, enter the benchmark directory:

```sh
cd benchmark/semantic
./test.sh
```

The script runs all 48 `.sl` inputs, places results in `benchmark/semantic/out/`, and compares each result against the corresponding `benchmark/semantic/expected/` file. Legal cases are compared as JSON; illegal cases are compared as `.err` diagnostic text. **Do not edit the expected output files or run instructor-only reference-generation scripts to replace them.**

#### Suggested incremental benchmark groups

| Benchmarks | What to verify |
|---|---|
| **01–05** | Top-level restrictions, valid `main` signature, and prohibition on calling `main`. |
| **06–12** | Scalar declarations, function scopes, and duplicate symbols. |
| **13–19** | Arrays, literal and identifier sizes, and assignment/lookup failures. |
| **20–30** | Identifier inference, binary expressions, assignments, and exact-type compatibility. |
| **31–32** | Supplied explicit-cast inference and rewriting; verify your inferred source types feed those methods. |
| **33–43** | Function signatures, return-type annotation of calls, argument matching, and call contexts. |
| **44–48** | Return statements and return-type checking. |

After semantic tests pass, rerun the parser benchmark suite from `benchmark/parser/` to check that your new `parser.y` actions have not changed parsing or AST construction. A clean rebuild is useful when regenerated Bison/Flex files appear stale.

## Hints

### Start with the Missing Method Lists

Read the three provided header files first. Each `.cpp` starter supplies only selected completed functions; the missing method bodies are not marked with TODO stubs. Copy the exact method signatures, including `const` qualifiers, into your own definitions. Do not define the same method twice or attempt to solve undefined references by editing the headers.

### Trace One Declaration from AST to Symbol Table

Follow a declaration such as `int count = 4;` from the parser's `Declaration` node through `Symbol` construction, the pending queue, `function_decl` scope assignment, table insertion, and the later `inferIdentifierType()` lookup. This helps separate errors in AST construction, symbol registration, and semantic inference.

### Remember Why Locals Must Be Queued

Bison uses bottom-up parsing, so a `declaration` action executes before its enclosing `function_decl` action. At declaration time, the function scope has not yet been assigned to the local symbols.

Use a `std::queue<Symbol*>` to temporarily store variable and array symbols in declaration order. In `parser/parser.y`, include the `<queue>` header and declare the following global variable:

```cpp
std::queue<Symbol*> symbols;
```

During each `declaration` action, use `symbols.push()` to add newly created symbols to the queue. When `function_decl` executes, process the queue in first-in, first-out order using `front()` and `pop()`. Assign each symbol's function scope using `updateScope()` and insert it into `symbolTable`.

Ensure the queue is empty after processing each function to prevent symbols from different functions from being mixed.

### Distinguish Function Symbols from Local Symbols

The function-insertion overload of `insertSymbol()` creates a scope. The local-insertion overload requires that scope to exist. A call to function `sum` uses `lookup("sum", "sum", line)`, while a reference to local `count` in `sum` uses `lookup("sum", "count", line)`. Do not treat the function's parameter list as a separate scope.

### Infer from Leaf Nodes Toward Their Parents

Test literal and identifier inference before implementing operators, assignments, calls, and returns. Each successful inference should call the supplied `setExpressionType()`, so missing JSON `type` fields often indicate an inference method that was not invoked or did not store its result.

### Keep the Parser AST Unchanged

The AST structure remains the one required in the parser phase. In `parser.y`, build symbols alongside AST nodes rather than changing the AST to store symbols. Preserve `std::string` conversion and freeing of lexer-owned identifier text, as well as all existing child orders and source lines.

### Compare the First Unexpected Output Difference

If legal JSON differs, inspect the first missing or incorrect semantic `type`, AST node kind, or transformed child. If an illegal program's `.err` differs, compare the category, filename, line number, and exact message, including whether the expected file contains multiple diagnostic lines. Use the smallest related benchmark rather than debugging all failures at once.

### Respect Pointer Ownership

The AST owns AST children. The table owns successfully inserted symbols, but symbol references from `lookup()` and array-size links do not own their targets. Function parameter-signature symbols and the actual parameters inserted into the table are separate objects. Review destructor and failure paths carefully to avoid leaks and double deletion.
