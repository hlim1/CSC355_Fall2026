# SLangCC Symbol Table

The symbol-table module records functions, parameters, scalar variables, and
arrays discovered while `parser/parser.y` builds the AST. Semantic analysis and
code generation then query the same table for declaration types, function
signatures, and array sizes.

The implementation consists of:

- `Symbol`: metadata for one declared program entity.
- `SymbolTable`: an owning collection of symbols grouped by function scope.

## Scope model and table layout

`SymbolTable` stores a `std::map<std::string, std::vector<Symbol*>>`. Each map
key is a function name. The first entry in that vector is normally the function
symbol itself, followed by its parameters and local declarations.

There is currently no nested block-scope model and no global-variable scope;
the parser rejects statements and declarations outside functions. Consequently,
all parameters and local declarations in a function share one namespace and
must have unique names throughout that function.

Function symbols use their own name as both the map key and symbol scope.
Parameters and local variables initially have an empty scope and are updated to
the enclosing function name when parsing reaches the function declaration.

## `Symbol`

Construct a symbol with:

```cpp
Symbol(Node* type, Node* declaration)
```

`type` must be a `Type` node. `declaration` must be one of `Declarator`,
`ArrayDeclarator`, `Parameter`, or `FunctionDeclaration`; any other kind causes
an internal error and aborts the process.

Every symbol stores its name, declared SLangCC type, source line, and scope.
Additional metadata depends on the declaration kind:

| Declaration kind | Additional metadata |
| --- | --- |
| `FunctionDeclaration` | Function marker and a parameter-symbol snapshot |
| `Declarator` | Initializer payload when the initializer is a literal-valued node |
| `ArrayDeclarator` | Literal size or the identifier naming the size symbol |
| `Parameter` | Name and declared type |

The public query API provides `getName()`, `getType()`, `getLine()`,
`getScope()`, `getIntValue()`, `isArraySymbol()`, `getArraySize()`,
`getArraySizeSymbolName()`, and `getParameters()`. `updateScope()` assigns the
enclosing function, while `updateArraySizeSymbol()` links an identifier-sized
array to the symbol used to resolve its length. `printSymbol()` writes debugging
details to standard output.

`getArraySize()` returns the linked size symbol's stored integer value when a
link exists; otherwise it returns the literal size stored on the array symbol.
The linked `Symbol` is non-owning and must not outlive the table entry it points
to.

## `SymbolTable`

### Insertion

```cpp
void insertSymbol(const std::string& scope, int line, Symbol* function);
void insertSymbol(const std::string& scope, Symbol* variable);
```

The first overload creates a function scope and rejects a duplicate function
name. The second inserts a parameter or local declaration into an existing
function scope and rejects duplicate names in that scope.

On successful insertion, the table takes ownership of the pointer and deletes
it in `~SymbolTable()`. On failed insertion, the current implementation reports
a semantic error but does not delete the supplied pointer; the caller retains
responsibility for it.

### Lookup

```cpp
const Symbol* lookup(const std::string& scope,
                     const std::string& name,
                     int line) const;
```

Lookup searches only the requested scope and returns a non-owning pointer to the
matching symbol, or `nullptr` when the scope or name is absent. It does not
search parent or global scopes and currently does not report a diagnostic. The
`line` parameter is accepted for call-site context but is currently unused.

Function lookup uses the function name for both `scope` and `name`:

```cpp
const Symbol* function = table.lookup("sum", "sum", sourceLine);
const Symbol* local = table.lookup("sum", "total", sourceLine);
```

`printSymbolTable()` prints all scopes and symbols for debugging.
