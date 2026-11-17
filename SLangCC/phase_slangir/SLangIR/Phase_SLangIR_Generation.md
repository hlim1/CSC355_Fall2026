# Phase: SLangIR Generation

## Learning Objective

Implement the intermediate representation (IR) generation phase of SLangCC by translating the semantic annotated abstract syntax tree (AST) into SLangIR, a three-address code representation organized into numbered basic blocks (from B1 to BN).
By completing this phase, you will be able to traverse a annotated AST, generate instructions for expressions and statements, assign temporary values, translate function calls, construct basic blocks for conditional and loop statements, and build a control-flow graph (CFG) for each function.

This is a **three-week phase**. You will continue using the lexer, parser, AST, and semantic analyzer completed in the earlier phases. 
In this phase, your task is to implement **`SLangIR/SLangIR.cpp`** only. Other files should remain the same.
The starter already implements the main entry points, instruction-emission and numbering helpers, and translation of `while` statements. You will implement the remaining functions declared in `SLangIR.h`.

## Starter Files

The SLangIR-phase distribution adds the following files and directories to your existing SLangCC project:

- `SLangIR/SLangIR.cpp`: Partially completed IR generator. Implement the missing member functions here, preserving all instructor-provided functions.
- `SLangIR/SLangIR.h`: Provided class declaration and method signatures. Read it but do not modify it.
- `SLangIR/main.cpp`: Provided driver that runs the earlier compiler phases, generates SLangIR, and writes the result to an `.ir` file. Do not modify it.
- `SLangIR/Makefile`: Provided build configuration. Do not modify it.
- `SLangIR/README.md`: Description of the IR design. Use it together with the benchmark outputs to understand the required instruction format.
- `shared/slangir-cfg/`: Provided classes for IR programs, functions, basic blocks, and instructions. Use their interfaces without modifying them.
- `benchmark/slangir/`: **30 benchmark programs**, their expected `.ir` files, and test scripts.

**`SLangIR.cpp` is incomplete.** The provided definitions include the entry points, helpers for emitting instructions and reserving numbers, and `visitWhileStatement()`.
The remaining functions declared in `SLangIR.h` must be defined in `SLangIR.cpp`. Until the required definitions exist, a complete build may fail at the linking stage.

## Directory Setup

Begin with your completed SLangCC project, including the lexer, parser, AST, and semantic analyzer from the previous phases. Keep those implementations intact when adding the SLangIR-phase files.

The relevant project structure should include:

```text
SLangCC/
├── benchmark/
│   ├── lexer/
│   ├── parser/
│   ├── semantic/
│   └── slangir/
│       ├── expected/
│       ├── test.sh
│       └── ...30 benchmark programs...
├── lexer/
├── parser/
├── semantic_analyzer/
├── SLangIR/
│   ├── SLangIR.h
│   ├── SLangIR.cpp
│   ├── main.cpp
│   ├── Makefile
│   └── README.md
└── shared/
    ├── ast/
    ├── error/
    ├── symboltable/
    └── slangir-cfg/
```

The SLangIR executable builds against the earlier compiler components and the provided CFG classes. Run build and benchmark commands from the directories indicated in the Tests section.

## Description

The earlier phases recognize the source program, build its AST, and check its semantics. This phase translates the resulting AST into an `IRProgram` containing functions, basic blocks, and three-address instructions. The provided driver then prints that representation to an `.ir` file.

```text
source.sl -> lexer -> parser -> semantic analyzer -> SLangIR::analyze(ast) -> IRProgram -> print -> source.ir
```

The input to this phase is the root `Program` node after successful semantic analysis. Because the preceding phases have already checked lexical, syntax, and semantic correctness, the IR generator translates the AST rather than repeating those checks. An impossible or malformed internal AST state may be treated as a compiler error; it is not a new source-level semantic diagnostic.

Translate each function in source order. Within a function, emit instructions as the AST is traversed. **Do not return to earlier instructions to rewrite them.** Each generated function contains its own numbered basic blocks, and each block contains an ordered sequence of instructions.

### Files You May Modify

You are expected to implement or update **exactly one source file**:

| File | Required work |
|---|---|
| `SLangIR/SLangIR.cpp` | Define the missing methods declared in `SLangIR.h`, preserving the supplied entry points, helpers, and `while` translation. |

Do **not** modify `SLangIR.h`, the driver, the Makefile, the shared CFG classes, the lexer, parser, AST, semantic analyzer, or the expected benchmark files. Temporary debugging output may be added while developing, but it must be removed before submission.

### Recommended Timeline

- **First Week — Basic instructions and expressions:** Generate function headers, entry blocks, parameters, literals, declarations, assignments, compound assignments, increment/decrement statements, returns, conversions, `print`, and `exit`. Complete expression translation. Target benchmarks **01–08, 13, and 14** (**10 tests**).
- **Second Week — Arrays, calls, and conditions:** Implement array access and `lvalue` handling, function calls, `negatedCondition()`, and `if`/`elseif`/`else` translation. The provided `while` translation should work once its dependencies are implemented. Target benchmarks **09–12, 15–21, 28, and 29**, bringing the total to **23 tests**.
- **Third Week — Remaining loops and integration:** Implement `for` translation, `break`, `continue`, and nested-loop behavior. Target benchmarks **22–27 and 30**, then run all 30 benchmarks, create your own tests, and complete a clean build.

## Program Requirements

### AST Input and Semantic-Analysis Results

The IR generator operates on the AST produced by the previous phases. Preserve its node kinds, child order, and semantic-type annotations; do not modify the AST while generating IR.

In particular, your semantic analyzer should already perform the following two transformations:

1. A valid `CastExpression` has been replaced by a conversion node with one expression child. The conversion kinds are `inttodouble`, `inttochar`, `chartoint`, `inttostring`, `doubletostring`, `booltostring`, and `chartostring`.
2. Every non-string argument to `print(...)` has been wrapped in an appropriate `...tostring` conversion node. Each print argument is therefore a string expression when IR generation begins.

A `CallStatement` also has a semantic type. 

Read its `getSemanticType()` result to determine whether the called function returns a value or `void`.

#### AST Structure

The tables below summarize the node shapes used by this phase. 

A node's *value* is the relevant text returned by `getStringValue()`, unless another accessor is specified. 

Structural wrappers such as `StatementList` are part of the AST and must be traversed.

**Program and function nodes**

| AST kind | Value | Children, in order |
|---|---|---|
| `Program` | — | `ExternalList` |
| `ExternalList` | — | One `FunctionDeclaration` per function |
| `FunctionDeclaration` | Function name | `Type`, `ParameterList`, `Block` |
| `ParameterList` | — | Ordered `Parameter` nodes; each parameter has a name and a `Type` child |
| `Block` | — | `StatementList` |
| `StatementList` | — | Statements in source order |

**Statement nodes**

| AST kind | Value | Children, in order |
|---|---|---|
| `Declaration` | — | `Type`, then `DeclaratorList` or `ArrayDeclarator` |
| `Declarator` | Variable name | `Initializer`, containing the initializing expression when present |
| `ArrayDeclarator` | Array name | Size (`IntLiteral` or `Identifier`), optional `InitializationList` |
| `Assignment` | — | Target, expression |
| `CompoundAssignment` | `+=` or `-=` | Target, expression |
| `PreIncrement`, `PostIncrement`, `PreDecrement`, `PostDecrement` | `++` or `--` | Target |
| `CallStatement` | — | Callee `Identifier`, `ArgumentList` |
| `PrintStatement`, `ExitStatement` | — | `ArgumentList` |
| `ReturnStatement` | — | No children, or one expression |
| `IfStatement` | — | Condition, `Block`, optional `ElseIfList`, optional `ElseClause` |
| `ElseIfList` | — | Ordered `ElseIfClause` nodes |
| `ElseIfClause` | — | Condition, `Block` |
| `ElseClause` | — | `Block` |
| `WhileStatement` | — | Condition, `Block` |
| `ForStatement` | — | `ForInit`, `ForCondition`, `ForUpdate`, `Block` |
| `ForInit`, `ForCondition`, `ForUpdate` | — | No children when omitted; otherwise one child |
| `BreakStatement`, `ContinueStatement` | — | No children |

A `Block` may also occur directly within a `StatementList`, representing a block written as `{ ... }` without an enclosing control statement.

**Expression nodes**

| AST kind | Value | Children, in order |
|---|---|---|
| `BinaryOp` | `+`, `-`, `*`, `/`, comparison, `&&`, or `\|\|` operator | Left, right |
| `UnaryOp` | `!`, `-`, or `+` | Operand |
| `ParenthesizedExpression` | — | Enclosed expression |
| `Identifier` | Variable name | No children |
| `ArrayAccess` | — | Array `Identifier`, index (`Identifier` or `IntLiteral`) |
| `IntLiteral`, `DecimalLiteral`, `BooleanLiteral` | Use `valueAsString()` | No children |
| `StringLiteral` | Decoded string | No children |
| `CharacterLiteral` | Use `getCharValue()` | No children |
| `inttodouble`, `inttochar`, `chartoint`, `inttostring`, `doubletostring`, `booltostring`, `chartostring` | — | Expression being converted |
| `CallStatement` used as an expression | — | Callee `Identifier`, `ArgumentList` |

Use the AST produced by your completed parser and semantic analyzer as the source of truth for traversal. To inspect a particular AST, you may temporarily call `ast->printJSON();` from `SLangIR::analyze()` while debugging.

### Provided IR Classes and Helpers

The supplied classes in `shared/slangir-cfg/` represent the generated IR. Do not modify their implementations.

| Class | Relevant interface | Behavior |
|---|---|---|
| `IRProgram` | `addFunction(name)` | Creates a function and returns an `IRFunction&`. |
| `IRFunction` | `addBlock(number)`, `rebuildCFG()` | Creates numbered blocks; duplicate block numbers are rejected. `rebuildCFG()` constructs CFG edges from the completed instructions. |
| `IRBasicBlock` | `addInstruction(text, separated)` | Appends instruction text; `separated = true` adds a blank line before it in the printed output. |
| `IRInstruction` | — | Stores instruction text. |

The provided `SLangIR.cpp` already includes helpers that simplify most interactions with these classes:

- `emit(text)` appends an instruction to the current basic block.
- `newBlock()` reserves the next basic-block number without creating the block.
- `emitLabel(number)` creates and selects the numbered block (`B<number>`).
- `separateInstructions()` controls the required blank-line separation in printed output.
- The temporary-numbering helpers allocate names such as `_t0`, `_t1`, and `_t2`.

**Block references:** `IRFunction::addBlock()` returns a reference to an element stored in a `std::vector`. Creating another block may invalidate earlier references or pointers. Use the `currentBlock` pointer maintained by `emitLabel()`. Do not retain your own block pointer across calls that create blocks.

**Reserved blocks:** Reserving a number with `newBlock()` does not create an `IRBasicBlock`. A corresponding `emitLabel()` call must create the block later. This permits a forward branch to an exit block whose instructions have not yet been emitted.

After generating all blocks and instructions for a function, call **`currentFunction->rebuildCFG()` exactly once**. The method derives control-flow edges from the completed blocks; students do not construct the CFG edges directly.

### Node Accessors

Use the existing `Node` interface to examine the AST:

| Accessor | Required use |
|---|---|
| `getKind()` | Obtain the AST kind, such as `"BinaryOp"`. |
| `getChildren()` | Traverse the ordered child list. |
| `hasChildren()` | Determine whether a node has children. |
| `getStringValue()` | Read an identifier, function name, or operator string. |
| `valueAsString()` | Format a literal value such as `7`, `2.5`, or `true`. |
| `getCharValue()` | Read the character stored by a `CharacterLiteral`. |
| `getSemanticType()` | Read the semantic type attached to an expression or call. |

### Instruction Format and Numbering

The expected IR text in `benchmark/slangir/expected/` defines the required output format. Read `SLangIR/README.md` for the design, but **follow the benchmark expected files if an example in the README differs**.

Use the following numbering and layout rules:

- Temporary names are `_t0`, `_t1`, `_t2`, and so on, in creation order. Restart the temporary counter for every function.
- Basic-block labels are `B1`, `B2`, `B3`, and so on, in **reservation order**. Restart the block counter for every function. Although some README examples begin at `B0`, the benchmark outputs begin at `B1`.
- Print the function name followed by `:`, then the entry block label `B1:`.
- Emit parameter instructions at the beginning of `B1`, in parameter order, using `param <type> <name>`.
- If a function has parameters, call `separateInstructions()` after emitting them to produce the required blank line.

#### Required Instruction Forms

The following examples illustrate the instruction text and temporary usage. Each instruction appears on its own line. The exact spaces, punctuation, block numbers, and temporary names matter.

| SLangCC source or construct | Required SLangIR form |
|---|---|
| `int x = 5;` or `x = 5;` | `x = 5` |
| `int x = y;` or `x = y;` | `_t0 = y`, then `x = _t0` |
| `x = a + b;` | `_t0 = a + b`, then `x = _t0` |
| `-v` or `+v` | `_t0 = - v` or `_t0 = + v` |
| `!v` used as a value | `_t0 = ! v` |
| `a[i] = 10;` | `a[i] = 10` |
| `a[i] = a[0];` | `_t0 = a[0]`, then `a[i] = _t0` |
| `x += e;` | `_t0 = x + e`, then `x = _t0` (`-=` is analogous) |
| `x++;` or `++x;` | `_t0 = x + 1`, then `x = _t0` (`--` uses `-`) |
| `f(a, 3);` where `f` returns `void` | `arg a`, `arg 3`, `call f, 2` |
| `int r = f(a);` | `arg a`, `_t0 = call f, 1`, `r = _t0` |
| `print(a, b);` | Convert arguments as needed, concatenate them into temporaries, emit one `arg`, then `call print, 1` |
| `exit();` | `call exit, 0` |
| `return;` | `return` |
| `return 0;` | `return 0` |
| `return x;` | `_t0 = x`, then `return _t0` |
| `return a[i];` | `_t0 = a[i]`, then `return _t0` |
| `return a + b;` | `_t0 = a + b`, then `return _t0` |
| `inttostring` conversion node | `_t0 = inttostring x` |

Follow these rules when selecting operands and temporaries:

1. **Literal values** may be assigned to variables or returned directly. Print strings in double quotes and characters in single quotes. Escape backslashes, quote characters, newlines, carriage returns, and tabs as required by the expected output.
2. **Variable copies** require a fresh temporary. For example, `x = y;` emits `_t0 = y` followed by `x = _t0`. The same rule applies to `return y;`.
3. **Operators and value-returning calls** produce a fresh temporary. If a child expression already produces a temporary, use it directly rather than copying it into another temporary.
4. **Call arguments** are evaluated from left to right before any `arg` instructions for that call are emitted. Emit all `arg` instructions immediately before `call`, even when an argument contains a nested function call.
5. **Void functions** receive a final `return` unless the last statement in the function body is already `return`. An `exit()` call does not replace this final return.
6. **Instruction spacing** must match the expected files. Preserve source operand order and put one space around binary operators and between a value-level unary operator and its operand.

#### Why Literal Assignments Differ from Variable Copies

SLangIR uses direct literal assignments but copies variables through temporaries. This differs from the load-store model introduced in lectures, in which every value passes through a temporary before it is stored.

The distinction reflects the intended x86 target. An x86 `mov` instruction can place an immediate value directly into memory, but a memory-to-memory copy requires an intermediate register. For example:

```text
x = 5          movl  $5, -4(%rbp)           # immediate to memory

_t0 = y        movl  -8(%rbp), %eax         # memory to register
x = _t0        movl  %eax, -4(%rbp)         # register to memory
```

These assembly instructions illustrate the reason for the IR convention; you are not implementing x86 code generation in this phase.

### Branch Conditions

SLangIR conditional branches jump to the **false** path, allowing the true path to continue into the next block. To implement this layout, `negatedCondition()` generates a condition that is true when the source condition is false.

For relational operators, emit the complementary comparison instead of a separate logical-not instruction:

| Source condition | Negated SLangIR condition |
|---|---|
| `a < b` | `_t0 = a >= b` |
| `a <= b` | `_t0 = a > b` |
| `a > b` | `_t0 = a <= b` |
| `a >= b` | `_t0 = a < b` |
| `a == b` | `_t0 = a != b` |
| `a != b` | `_t0 = a == b` |

Follow the comparison with a branch such as `if _t0 goto B3`. Parenthesized conditions use the condition inside the parentheses without introducing an additional operator.

Two other cases require different handling:

- **Source-level negation (`!e`):** Since the negation of `!e` is `e`, evaluate `e` and branch on its value directly. Do not allocate a temporary solely for the removed `!`. The operand `e` is translated as an ordinary expression, so it is not complemented. For `if (!(a < b))`, emit `_t0 = a < b` and then `if _t0 goto ...`.
- **Other expressions:** For a boolean variable, literal, call, `&&`, `||`, or other condition not handled above, evaluate the expression and then emit a logical-not instruction in the form `_tN = !operand`. There is **no space after `!`** in this branch-negation form.

**IMPORTANT: `&&` and `||` do not short-circuit.** SLangIR evaluates them like any other binary operator: both operands are computed, then combined into a single temporary (`_t2 = enabled && _t1`). `negatedCondition()` therefore needs no special case for them. They fall under "other expressions" above and produce one value to negate and branch on. This is a deliberate simplification, and it differs from the textbook's jumping-code translation, which turns `&&` and `||` into a chain of conditional branches. One consequence is that `f() && g()` always calls both functions. Do not add extra blocks or branches for `&&` or `||`.

For the "other expressions" case, **reserve the negation temporary before evaluating the expression**. This special allocation order is required by the expected temporary numbering. For example, benchmark 19 translates the condition in `if (enabled && !blocked)` as:

```text
_t1 = ! blocked
_t2 = enabled && _t1
_t0 = !_t2
if _t0 goto B2
```

By contrast, for a relational comparison, translate its left operand and then its right operand **before** reserving the comparison-result temporary.

### Basic-Block Construction and Control Flow

Block numbers are assigned when `newBlock()` is called, which may be earlier than the corresponding `emitLabel()` call. The reservation order and label-emission order must both match the benchmark outputs.

| Construct | Block reservation order | Label-emission order |
|---|---|---|
| Function | Entry | Entry block, containing any `param` instructions |
| `while` | Condition, body, done | Condition, body, done |
| `for` | Condition, body, update, done; emit the initialization in the current block first | Condition, body, update, done |
| `if` / `elseif` / `else` | Done first; for each clause in order, reserve its condition block (except for the first clause), then its body block; reserve an optional else block last | First condition in the current block; first body, subsequent condition/body pairs, optional else, done |

#### `while` Statements — Provided Implementation

The starter's `visitWhileStatement()` demonstrates the required structure. Reserve blocks for the condition, body, and exit. In the condition block, emit the negated condition and a conditional jump to the exit. Translate the body, emit an unconditional jump back to the condition block, and finally label the exit block.

**Do not rewrite `visitWhileStatement()`.** Read it as the worked example for the other control-flow constructs.

#### `for` Statements

Translate `ForInit` in the current block **before** reserving the loop's new block numbers. Reserve blocks for the condition, body, update, and exit, in that order.

1. Label the condition block. If `ForCondition` is present, emit its negated condition and branch to the exit block when false.
2. Label the body block, translate its statements, and emit `goto` to the update block.
3. Label the update block, translate `ForUpdate`, and emit `goto` to the condition block.
4. Label the exit block so subsequent instructions follow the loop.

Omitted `ForInit`, `ForCondition`, and `ForUpdate` clauses are represented by wrapper nodes with no children. Handle them without attempting to translate a missing child.

#### `if`, `elseif`, and `else` Statements

Reserve the common **done** block first. The first clause evaluates its condition in the current block; subsequent `elseif` clauses use newly reserved condition blocks. Reserve the body blocks and, if needed, the `else` block in the specified order.

For each conditional clause, emit the negated condition and branch to the next clause's condition block, the `else` block, or the common done block. Then label and translate the corresponding body.

After a conditional body's instructions, emit `goto done` **only if the current block is not already terminated** by a `goto`, `return`, or `call exit,` instruction. Use the provided `currentBlockIsTerminated()` helper. An `else` body falls through to the done block and does not need its own unconditional `goto done`.

Finish by labeling the done block.

#### `break`, `continue`, and Nested Loops

Use the provided `loops` stack to track each enclosing loop's `{continue target, break target}` pair. Push the targets before translating a loop body and pop them after completing that body. The top entry always represents the innermost loop.

- `break` emits an unconditional jump to the innermost loop's exit block.
- `continue` in a `while` loop jumps to the condition block.
- `continue` in a `for` loop jumps to the update block, so the update runs before the next condition check.

#### Example: `for` Loop (Benchmark 24)

Consider the following loop in `main`:

```cpp
for (int index = 0; index < 5; index++) {
    print(index);
}
```

The function entry uses `B1`. The loop reserves `B2` for the condition, `B3` for the body, `B4` for the update, and `B5` for the exit. The required IR is:

```text
main:
B1:
    index = 0

B2:
    _t0 = index >= 5
    if _t0 goto B5

B3:
    _t1 = inttostring index
    arg _t1
    call print, 1
    goto B4

B4:
    _t2 = index + 1
    index = _t2
    goto B2

B5:
    return
```

The initialization remains in `B1` because it is translated before the loop blocks are labeled. The final `return` is emitted in `B5`, the current block after the loop completes.

### Completion Criteria

The completed IR generator must:

- Build and link with the provided `Makefile` and the compiler components from the earlier phases.
- Preserve the supplied header, CFG classes, driver, parser, and semantic-analyzer behavior.
- Generate source-order functions with correctly numbered blocks and temporaries, resetting both counters for each function.
- Translate expressions, declarations, assignments, array accesses, function calls, returns, conversions, `print`, and `exit` using the specified instruction forms.
- Generate control flow for conditional statements, `while`, `for`, `break`, `continue`, and nested loops.
- Call `rebuildCFG()` once for each completed function.
- Match all **30** expected SLangIR outputs, including instruction text, blank lines, block labels, and temporary numbers.
- Compile `SLangIR.cpp` without warnings under `-Wall -Wextra` and contain no leftover debugging output.

## Tests

The starter may not link until all required methods have definitions. During early development, you may first add definitions with the exact signatures from `SLangIR.h` and then implement their behavior incrementally. A definition that only returns a placeholder is not a completed implementation.

The build requires `g++`, `flex`, and `bison`. From the project root, build the IR generator:

```sh
cd SLangIR
make
```

A successful build creates **`SLangIR/slangir`**. From the `SLangIR/` directory, run one benchmark:

```sh
./slangir ../benchmark/slangir/17_elseif_chain.sl
```

By default, the executable writes `17_elseif_chain.ir` in the current working directory. An optional second argument selects an output directory:

```sh
./slangir ../benchmark/slangir/17_elseif_chain.sl /tmp
```

To run the complete suite from the project root:

```sh
cd benchmark/slangir
./test.sh
```

The script runs all 30 source programs, compares generated `.ir` files against `benchmark/slangir/expected/`, and reports a pass or failure for each test. For failures it shows a unified diff: lines beginning with `-` are expected output, while lines beginning with `+` are generated output.

To inspect one benchmark directly while working in `benchmark/slangir/`:

```sh
../../SLangIR/slangir 17_elseif_chain.sl
diff -u expected/17_elseif_chain.ir 17_elseif_chain.ir
```

### Suggested Incremental Benchmark Groups

| Benchmarks | What to verify |
|---|---|
| **01–08, 13–14** | Entry blocks, parameters, literals, declarations, expressions, assignments, conversion instructions, returns, `print`, and `exit` (**10 tests**). |
| **09–12, 15–21, 28–29** | Arrays, function calls, negated conditions, conditional branches, and working `while` translation (**23 cumulative tests**). |
| **22–27, 30** | `for` loops, `break`, `continue`, nested loops, and final integration (**30 cumulative tests**). |

Begin with benchmark **01** to check the function header, `B1`, and final `return`. Then test parameters and returns with **02**, and literals with **03**, before adding operators and print statements. For conditions, test `negatedCondition()` with benchmarks **15** and **20** before **19**, which checks the special temporary-allocation order. Use the provided `visitWhileStatement()` as a model before implementing `visitForStatement()`.

### Diagnosing Test Failures

| Symptom | What to check |
|---|---|
| Missing instructions or unexpectedly short output | A translation method may still be incomplete or may fail to traverse a child node. |
| `SLangIR error: duplicate IR basic block B4` | The same block number may have been passed to `emitLabel()` more than once. |
| `Internal error: Unsupported ... node: K` | The traversal or expression dispatcher may not handle AST kind `K`. |
| Correct instructions but different `_t` or `B` numbers | Compare temporary allocation and block reservation order. |
| Only spacing differs | Check spaces around operators, after value-level unary operators, and after `!` in a branch negation. |
| Segmentation fault | Check child-vector bounds and whether a block pointer became invalid after a new block was created. |

A missing `rebuildCFG()` call may not change the printed `.ir` output, but it is still required because the control-flow graph is part of the generated program representation.

## Hints

### Start with the Header and Provided Functions

Read `SLangIR.h` and identify the methods that lack definitions in `SLangIR.cpp`. Copy their exact signatures, including parameter types and qualifiers. Do not add missing definitions to the header or alter provided functions. It may be useful to define every required method before completing the implementation so that missing linker symbols do not obscure later errors.

### Check Counter and Block Lifetime Rules

Reset the temporary counter to `_t0` and the basic-block counter to `B1` for each function. Remember that `newBlock()` allocates only a number; `emitLabel()` creates the actual block. Every referenced block must eventually be labeled, and a number must not be labeled twice.

Do not preserve pointers or references to blocks across operations that add another block. Always use the updated `currentBlock` pointer maintained by the supplied helper.

### Generate Expressions Before Their Consumers

For a binary expression, translate the left operand first, then the right operand, and only then allocate its result temporary. Reuse any temporary returned by a subexpression; do not introduce an unnecessary copy such as `_t4 = _t3`. The generic branch-negation case in `negatedCondition()` is the exception: it reserves the negation temporary before evaluating the expression.

Translate and collect all function-argument values before emitting the `arg` instructions. This is important when an argument contains another call: the nested call must complete before the outer call's argument instructions appear. A single-argument `print` requires no concatenation instruction.

### Check Control-Flow Targets and Termination

For a `for` statement, `continue` must target the update block, not the condition block. For a `while` statement, it targets the condition block. A `break` always targets the innermost loop's exit block.

Within an `if` or `elseif` body, call `currentBlockIsTerminated()` before emitting `goto done`. Do not append an extra jump after an existing `goto`, `return`, or `call exit,` terminator. A `call exit, 0` does not eliminate the required final `return` of a void function.

### Compare Exact Instruction Text

When an expected file differs, examine its first difference rather than debugging all 30 tests at once. Distinguish value-level unary syntax (`_t0 = ! value`) from branch negation (`_t0 = !value`). Preserve the exact spacing and blank lines in the benchmark outputs.

To inspect the AST during development, temporarily print it with `ast->printJSON();` inside `SLangIR::analyze()`. You may also add temporary `std::cerr` messages to identify which translation function is running. Remove these changes before submitting the assignment.

If a large benchmark fails, make a smaller `.sl` program containing only the relevant construct. For a crash, check child-vector bounds and pointer validity; compiling with `-g` and using `gdb` may help locate the problem.

Finally, call `currentFunction->rebuildCFG()` after generating the last instruction of each function, even though its effect may not be visible in the printed `.ir` files.
