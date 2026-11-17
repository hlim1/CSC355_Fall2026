# SLangIR Program and Control-Flow Model

## 1. Description of SLangIR

SLangIR is the compiler's intermediate representation (IR). It lowers a parsed and semantically checked SLang program into a deterministic, textual, three-address-code-like form. Source-level expressions are decomposed into simple instructions, temporary values are given explicit names, and control-flow constructs are represented with labeled basic blocks and branch instructions.

A generated program is organized as follows:

```text
IRProgram
└── IRFunction (one per source function)
    └── IRBasicBlock (B0, B1, ...)
        └── IRInstruction
```

Each function has its own control-flow graph (CFG). The basic blocks are the graph's vertices, and each block's `successors` list describes its outgoing edges. `IRProgram` is therefore a program-wide container, not one large CFG spanning all functions.

An instruction is stored as canonical SLangIR text, for example:

```text
x = 1
_t0 = x + 1
if _t0 goto B2
goto B3
return _t0
```

The classes in this directory model and print that representation. AST traversal and instruction generation belong to `SLangIR`; these classes provide ownership, construction, CFG derivation, and serialization.

## 2. How the Classes Interact

### Ownership hierarchy

`IRProgram` is the root object. It owns a `std::vector<IRFunction>` in source/generation order. An `IRFunction` owns its `std::vector<IRBasicBlock>`, and an `IRBasicBlock` owns both its `std::vector<IRInstruction>` and its outgoing successor block numbers.

Ownership is by value at every level. Destroying or clearing an `IRProgram` consequently destroys every function, block, instruction, and recorded edge beneath it. No class in this directory owns objects through raw pointers.

The public getters return const references to their vectors. Consumers can inspect the representation without changing ownership or bypassing construction rules:

- `IRProgram::getFunctions()` exposes functions.
- `IRFunction::getBlocks()` exposes the function's blocks.
- `IRBasicBlock::getInstructions()` exposes instructions.
- `IRBasicBlock::getSuccessors()` exposes CFG edges by destination block number.

### Construction flow

`SLangIR` builds the hierarchy from the outside inward:

1. `IRProgram::clear()` removes the previously generated program.
2. For each function declaration, `IRProgram::addFunction(name)` constructs an `IRFunction` and returns a reference to it.
3. As control-flow regions are discovered, `IRFunction::addBlock(number)` constructs blocks and returns the newly added block.
4. As statements and expressions are lowered, `IRBasicBlock::addInstruction(text, separated)` appends instructions to the current block.
5. After that function has been generated, `IRFunction::rebuildCFG()` derives its outgoing block edges from terminators and block order.
6. `IRProgram::print()` or `IRProgram::toString()` serializes the completed program.

The returned references let the generator immediately populate a newly created object. Because the owning containers are `std::vector`s, adding another element can reallocate storage and invalidate earlier references or pointers. A caller must not retain a function reference across later `addFunction()` calls or a block reference across later `addBlock()` calls unless it knows no reallocation occurs. The current generator finishes each function before adding the next and updates its current block whenever it creates a block.

### Access between classes

`IRFunction` is a friend of `IRBasicBlock`. This narrow friendship permits `IRFunction::rebuildCFG()` to clear and populate a block's private `successors` vector and efficiently inspect its private instruction storage. Other clients receive read-only access through getters.

`IRProgram` does not need friendship. It creates functions through their public constructor and prints them through `IRFunction` and `IRBasicBlock` public getters. This keeps program-level ownership separate from function-level control-flow construction.

### Printing flow

`IRProgram::print(std::ostream&)` walks functions, blocks, and instructions in vector order. It prints a function label, each block label, and then each instruction indented by four spaces. `IRInstruction::isSeparated()` requests a blank line before that instruction for readable grouping. This flag affects formatting only; it has no CFG meaning.

`IRProgram::toString()` delegates to `print()` through a `std::ostringstream`, ensuring stream and string output use identical formatting rules.

## 3. Class Logic

### `IRProgram`

`IRProgram` represents one complete generated SLangIR program.

- `clear()` empties the function vector and transitively removes all generated IR.
- `addFunction(name)` explicitly constructs an `IRFunction`, appends it, and returns the last element.
- `getFunctions()` provides read-only traversal in generation order.
- `print(out)` produces the textual program, separating functions and blocks with blank lines and ending a nonempty program with a newline.
- `toString()` returns the same output as `print()`.

`IRProgram` does not build CFG edges because control cannot flow directly from a block in one function to a block in another. Calls are instructions, not inter-function CFG edges in this representation.

### `IRFunction`

`IRFunction` represents a named function and that function's CFG.

The constructor takes its name by value and moves it into storage. `addBlock(number)` first scans existing blocks and throws `std::runtime_error` when the number is already present. Unique block numbers are required because successors identify destinations by number. If the number is unique, the method appends and returns the new block.

`rebuildCFG()` recalculates all edges from scratch. For every block, it first clears stale successors, then examines the block as follows:

1. An empty block falls through to the next block, if one exists.
2. A final instruction beginning with `return` or `call exit,` terminates control flow and produces no successor.
3. A final instruction beginning with `goto B` produces only the explicit target edge.
4. A final instruction beginning with `if ` and containing `goto B` produces an edge to the branch target, followed by a fall-through edge to the next block when one exists.
5. Any other final instruction falls through to the next block when one exists.

#### Why rebuilding is necessary

IR generation initially creates basic blocks and appends instructions to them. Those operations establish the function's block layout and branch text, but they do not directly populate each block's `successors` vector. The outgoing edges can only be derived reliably after the generator knows:

- the final instruction in every block;
- the target of every conditional and unconditional branch; and
- the final block order, which determines fall-through.

For that reason, `SLangIR::visitFunctionDeclaration()` calls:

```cpp
currentFunction->rebuildCFG();
```

after it has generated all IR for the current function. Rebuilding earlier could produce incomplete edges, and changing instructions or block order afterward would leave the stored edges stale.

For example, suppose generation produces this function:

```text
B0:
    _t0 = x <= y
    if _t0 goto B2

B1:
    x = 1
    goto B3

B2:
    x = 2

B3:
    return
```

Before `rebuildCFG()` runs, the four blocks and their instructions exist, but their successor vectors are empty. The method then examines the final instruction in each block:

- `B0` ends with a conditional branch. It can jump to `B2`, and it can fall through to the next block, `B1`.
- `B1` ends with an unconditional branch to `B3`, so it has no fall-through edge.
- `B2` ends with an ordinary instruction, so it falls through to the next block, `B3`.
- `B3` ends with `return`, so control leaves the function and there is no successor.

The resulting graph is:

```text
B0 --branch------> B2
 |
 +--fall-through-> B1

B1 --goto--------> B3
B2 --fall-through-> B3
B3 --return------> function exit
```

The corresponding successor vectors contain block numbers:

```cpp
B0.successors = {2, 1};
B1.successors = {3};
B2.successors = {3};
B3.successors = {};
```

The order `{2, 1}` reflects the implementation: it inserts the explicit conditional target first and the fall-through block second. A successor list represents possible outgoing edges; its order does not represent the order in which both paths execute, because only one path is selected at runtime.

Branch targets are parsed from the final `goto B` marker in the instruction using `std::stoi`. If a branch-shaped instruction lacks that marker, target parsing throws `std::runtime_error`. Successors are inserted only if absent, so a branch target equal to the fall-through block is recorded once.

Block order is semantically important: the next vector element defines fall-through. Consequently, callers must add blocks in intended layout order before rebuilding the CFG. If instructions or block layout change afterward, callers must invoke `rebuildCFG()` again.

### `IRBasicBlock`

`IRBasicBlock` represents one CFG vertex.

- `number` is the printed label and edge identity (`B<number>`).
- `instructions` preserves instruction order within the block.
- `successors` stores outgoing CFG destinations as block numbers.
- `addInstruction(text, separated)` explicitly constructs and appends an `IRInstruction`.

The class does not decide where control flows. It stores edges calculated by its owning `IRFunction`. Its public API intentionally exposes successors as read-only data, preventing clients from creating edges inconsistent with the block's terminator.

### `IRInstruction`

`IRInstruction` is the leaf value object. It stores:

- `text`: the complete canonical textual instruction.
- `separated`: whether printing should insert a blank line immediately before it.

The constructor takes the text by value and moves it into the object. `getText()` and `isSeparated()` provide read-only access. Instructions are not parsed into an opcode/operand class hierarchy; CFG reconstruction recognizes the relevant terminator prefixes directly from their text. This makes exact canonical instruction spelling part of the contract between the generator and `IRFunction::rebuildCFG()`.

### Invariants and responsibilities

The design relies on these invariants:

- Each function name identifies the function being printed; duplicate-name validation is handled before IR generation rather than by `IRProgram`.
- Block numbers are unique within a function.
- Blocks are stored in layout order so fall-through can be inferred.
- Branch targets refer to valid block numbers in the same function. CFG reconstruction records targets but does not currently validate their existence.
- Only the final instruction of a block controls its outgoing edges.
- CFG edges must be rebuilt after relevant instruction or layout changes.
- Canonical terminator text must remain synchronized with the prefix checks in `rebuildCFG()`.

These boundaries keep responsibilities clear: `SLangIR` decides what IR and block layout to generate, `IRProgram` owns the program, `IRFunction` owns and derives one CFG, `IRBasicBlock` stores a vertex and its edges, and `IRInstruction` stores one printable operation.
