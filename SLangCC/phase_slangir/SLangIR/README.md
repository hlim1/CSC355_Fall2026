# SLangIR

**SLangIR is a traditional compiler intermediate representation that organizes canonical, lowered three-address code (TAC) instructions into basic blocks connected by a control-flow graph (CFG), using one consistent instruction form for each source-language construct.**

The goal is to make the IR:

* deterministic,
* easy to generate,
* easy to inspect,
* and straightforward to translate into assembly.

SLangIR is stricter than general TAC. Traditional TAC permits multiple equivalent representations of the same source construct, while SLangIR selects one canonical form for implementation.

---

## 1. Naming Conventions

SLangIR uses the following naming conventions:

```text
x, y, z    = source-level variables
_tN        = compiler-generated temporary
B0, B1     = basic-block labels
T          = source-language type
literal    = constant value
op         = arithmetic or logical binary operator
relop      = relational operator
unary_op   = unary operator
```

Compiler-generated temporaries are numbered in creation order:

```text
_t0
_t1
_t2
...
```

Basic blocks are also numbered in creation order:

```text
B0
B1
B2
...
```

The entry block of a function is normally `B0`.

---

## 2. Canonical Instruction Forms

SLangIR supports the following core instruction forms:

```text
param T x

arg operand

x = literal

_tN = x
x = _tN

_tN = operand1 op operand2
_tN = unary_op operand
_tN = operand1 relop operand2

if _tN goto Bk
goto Bk

_tN = call function_name, argument_count
call function_name, argument_count

return
return literal
return _tN
```

where an `operand` may be:

```text
source-level variable
compiler-generated temporary
literal
```

Each instruction contains at most one operator.

---

## 3. Core Rules

SLangIR follows these canonical rules:

1. A literal may be assigned directly to a source-level variable.

2. A value copied from one source-level variable to another must pass through a compiler-generated temporary.

3. Every unary, binary, and relational expression produces a compiler-generated temporary.

4. The result of an expression must be assigned from the temporary into the destination variable.

5. Each operator in a nested expression receives its own temporary.

6. Temporaries may be used directly as operands in later instructions.

7. Unnecessary temporary-to-temporary copies must not be generated.

8. Function parameters are emitted at the beginning of the function entry block.

9. Function-call arguments are emitted immediately before the corresponding call.

10. Conditional branches test boolean temporaries.

11. A source condition may be logically complemented to create a convenient fall-through path.

12. Control flow is represented using basic blocks, conditional branches, unconditional jumps, and fall-through edges.

---

# 4. Translation Templates

## 4.1 Literal Declaration

### Source

```c
T x = literal;
```

### SLangIR

```text
x = literal
```

### Example

```c
int base = 2;
```

```text
base = 2
```

A temporary is not required for literal initialization.

Do not generate:

```text
_t0 = 2
base = _t0
```

---

## 4.2 Variable-Copy Declaration

### Source

```c
T x = y;
```

### SLangIR

```text
_tN = y
x = _tN
```

### Example

```c
int copied = base;
```

```text
_t0 = base
copied = _t0
```

Although the following is valid general TAC:

```text
copied = base
```

SLangIR uses the expanded form because it maps naturally to a register-mediated memory copy:

```asm
movl    -4(%rbp), %eax
movl    %eax, -8(%rbp)
```

In this example:

```text
base    -> -4(%rbp)
_t0     -> %eax
copied  -> -8(%rbp)
```

---

## 4.3 Binary-Expression Declaration

### Source

```c
T x = y op z;
```

### SLangIR

```text
_tN = y op z
x = _tN
```

### Example

```c
int doubled = copied + base;
```

```text
_t1 = copied + base
doubled = _t1
```

Do not generate:

```text
doubled = copied + base
```

The expression result must first be stored in a temporary.

---

## 4.4 Unary-Expression Declaration

### Source

```c
T x = unary_op y;
```

### SLangIR

```text
_tN = unary_op y
x = _tN
```

### Example

```c
int negative = -value;
```

```text
_t0 = -value
negative = _t0
```

---

## 4.5 Assignment from a Literal

### Source

```c
x = literal;
```

### SLangIR

```text
x = literal
```

---

## 4.6 Assignment from a Variable

### Source

```c
x = y;
```

### SLangIR

```text
_tN = y
x = _tN
```

---

## 4.7 Assignment from a Binary Expression

### Source

```c
x = y op z;
```

### SLangIR

```text
_tN = y op z
x = _tN
```

---

## 4.8 Assignment from a Unary Expression

### Source

```c
x = unary_op y;
```

### SLangIR

```text
_tN = unary_op y
x = _tN
```

---

# 5. Expressions

## 5.1 Binary Expressions

A binary expression produces one temporary.

### Source

```c
x + y
```

### SLangIR

```text
_tN = x + y
```

Supported binary operators may include:

```text
+   -   *   /   %
&&  ||
```

The exact set depends on the SLangC language definition.

---

## 5.2 Unary Expressions

A unary expression produces one temporary.

### Source

```c
-x
```

### SLangIR

```text
_tN = -x
```

Examples of unary operators may include:

```text
-
!
```

---

## 5.3 Relational Expressions

A relational expression produces a boolean temporary.

### Source

```c
x relop y
```

### SLangIR

```text
_tN = x relop y
```

Supported relational operators are:

```text
<   <=   >   >=   ==   !=
```

### Example

```c
x <= y
```

```text
_t0 = x <= y
```

The resulting temporary may be used by a conditional branch:

```text
if _t0 goto B2
```

---

## 5.4 Nested Expressions

Each operator in a nested expression receives its own temporary.

### Example 1

#### Source

```c
int result = a + b * c;
```

#### SLangIR

```text
_t0 = b * c
_t1 = a + _t0
result = _t1
```

The evaluation order follows the abstract syntax tree and operator precedence.

### Example 2

#### Source

```c
int result = (a + b) * (c - d);
```

#### SLangIR

```text
_t0 = a + b
_t1 = c - d
_t2 = _t0 * _t1
result = _t2
```

---

## 5.5 Temporary Reuse

A temporary may be used directly as an operand in a later instruction.

Preferred:

```text
_t0 = b * c
_t1 = a + _t0
result = _t1
```

Avoid:

```text
_t0 = b * c
_t1 = _t0
_t2 = a + _t1
result = _t2
```

The copy from `_t0` to `_t1` is unnecessary.

---

# 6. Function Definitions and Parameters

## 6.1 Function Header

A function begins with its name followed by the entry block.

### Source

```c
T function(...)
```

### SLangIR

```text
function:
B0:
```

---

## 6.2 Formal Parameters

Formal parameters use `param` instructions.

### Source

```c
int foo(int x, int y)
```

### SLangIR

```text
foo:
B0:
    param int x
    param int y
```

Rules:

1. `param` instructions appear at the beginning of `B0`.

2. Their order must match the source-level parameter order.

3. Each `param` instruction records both the type and parameter name.

General form:

```text
param T x
```

---

# 7. Function Calls

SLangIR distinguishes between:

* formal parameters declared by the called function; and
* actual arguments supplied at the call site.

Formal parameters use `param`.

Actual arguments use `arg`.

---

## 7.1 Function-Call Arguments

Each actual argument is emitted before the corresponding call.

### Source

```c
function(a, b);
```

### SLangIR

```text
arg a
arg b
call function, 2
```

The integer after the function name is the number of arguments.

Rules:

1. Argument order must match source-level argument order.

2. Arguments for a call must appear immediately before that call.

3. Arguments from different calls must not be interleaved.

4. Literals, variables, and temporaries may be used as arguments.

---

## 7.2 Void Function Call

A call to a `void` function does not produce a result temporary.

### Source

```c
print(result);
```

### SLangIR

```text
arg result
call print, 1
```

General form:

```text
arg operand1
arg operand2
...
call function_name, argument_count
```

---

## 7.3 Value-Returning Function Call

A value-returning function call must store its result in a temporary.

### Source

```c
int result = fibonacciIter(10);
```

### SLangIR

```text
arg 10
_t0 = call fibonacciIter, 1
result = _t0
```

Do not generate:

```text
result = call fibonacciIter, 1
```

The canonical form is:

```text
_tN = call function_name, argument_count
destination = _tN
```

---

## 7.4 Function Call Used in an Assignment

### Source

```c
x = function(a, b);
```

### SLangIR

```text
arg a
arg b
_tN = call function, 2
x = _tN
```

---

## 7.5 Function Call Used in an Expression

A function call used as part of an expression must first produce a temporary.

### Source

```c
int result = function(a) + b;
```

### SLangIR

```text
arg a
_t0 = call function, 1
_t1 = _t0 + b
result = _t1
```

Each function call and each operator receives its own temporary.

---

## 7.6 Multiple Function Calls in an Expression

Function calls are emitted in the required source-language evaluation order.

### Source

```c
int result = first(a) + second(b);
```

### SLangIR

```text
arg a
_t0 = call first, 1

arg b
_t1 = call second, 1

_t2 = _t0 + _t1
result = _t2
```

---

## 7.7 Nested Function Calls

An inner call must complete before its result is passed to the outer call.

### Source

```c
int result = outer(inner(x));
```

### SLangIR

```text
arg x
_t0 = call inner, 1

arg _t0
_t1 = call outer, 1

result = _t1
```

---

## 7.8 Function Call with Literal Arguments

### Source

```c
foo(10, true);
```

### SLangIR

```text
arg 10
arg true
call foo, 2
```

A temporary is not required solely to pass a literal.

---

## 7.9 Function Call with Variable Arguments

### Source

```c
foo(x, y);
```

### SLangIR

```text
arg x
arg y
call foo, 2
```

Passing a source-level variable as an argument does not require an additional temporary.

---

# 8. Return Statements

## 8.1 Return Without a Value

### Source

```c
return;
```

### SLangIR

```text
return
```

---

## 8.2 Return of a Literal

### Source

```c
return literal;
```

### SLangIR

```text
return literal
```

### Example

```c
return 0;
```

```text
return 0
```

---

## 8.3 Return of a Variable

A source-level variable must first be copied into a temporary.

### Source

```c
return x;
```

### SLangIR

```text
_tN = x
return _tN
```

### Example

```c
return z;
```

```text
_t3 = z
return _t3
```

---

## 8.4 Return of an Expression

### Source

```c
return x op y;
```

### SLangIR

```text
_tN = x op y
return _tN
```

---

## 8.5 Return of a Function Call

### Source

```c
return function(x);
```

### SLangIR

```text
arg x
_tN = call function, 1
return _tN
```

---

# 9. Control Flow

## 9.1 Conditional Branch

A conditional branch tests a boolean temporary.

### SLangIR

```text
_tN = condition
if _tN goto Bk
```

### Example

```text
_t0 = x <= y
if _t0 goto B2
```

This means:

* jump to `B2` when `_t0` is true;
* otherwise continue to the next basic block.

The unselected path is represented by fall-through.

---

## 9.2 Complementing Conditions

SLangIR may complement a source-level relational condition so that one path becomes the fall-through path.

For example:

```c
if (x > y)
```

may be lowered as:

```text
_t0 = x <= y
if _t0 goto B_else
```

When `x > y` is true:

```text
x <= y
```

is false, so execution falls through to the then-block.

When `x > y` is false:

```text
x <= y
```

is true, so execution jumps to the else-block.

### Relational Complements

| Original condition | Complement |
| ------------------ | ---------- |
| `x < y`            | `x >= y`   |
| `x <= y`           | `x > y`    |
| `x > y`            | `x <= y`   |
| `x >= y`           | `x < y`    |
| `x == y`           | `x != y`   |
| `x != y`           | `x == y`   |

---

## 9.3 Unconditional Jump

An unconditional jump transfers control to another basic block.

### SLangIR

```text
goto Bk
```

### Example

```text
goto B3
```

Typical uses include:

* skipping an else-block after completing a then-block;
* returning from a loop body to the loop-condition block;
* transferring control when the target block is not the next block.

A jump may be omitted when execution naturally falls through to the next block.

---

# 10. If Statements

## 10.1 If Without Else

### Source

```c
if (x > y)
{
    z = y;
}
```

### SLangIR

```text
B0:
    _t0 = x <= y
    if _t0 goto B2

B1:
    _t1 = y
    z = _t1

B2:
```

CFG edges:

```text
B0 -> B1
B0 -> B2
B1 -> B2
```

---

## 10.2 If-Else Statement

### Source

```c
if (x > y)
{
    z = y + z;
}
else
{
    z = x + z;
}
```

### SLangIR

```text
B0:
    _t0 = x <= y
    if _t0 goto B2

B1:
    _t1 = y + z
    z = _t1
    goto B3

B2:
    _t2 = x + z
    z = _t2

B3:
```

CFG edges:

```text
B0 -> B1
B0 -> B2
B1 -> B3
B2 -> B3
```

`B2` does not require `goto B3` because `B3` is the immediate fall-through block.

---

# 11. While Loops

A `while` loop is represented using:

* a condition block;
* a loop-body block;
* an exit block;
* and a back edge from the body to the condition.

### Source

```c
while (i < n)
{
    i++;
}
```

### SLangIR

```text
B1:
    _t0 = i >= n
    if _t0 goto B3

B2:
    _t1 = i + 1
    i = _t1
    goto B1

B3:
```

The source condition:

```text
i < n
```

is complemented as:

```text
i >= n
```

The complemented condition branches to the loop exit.

CFG edges:

```text
B1 -> B2
B1 -> B3
B2 -> B1
```

---

# 12. Increment and Decrement

## 12.1 Increment

### Source

```c
i++;
```

### SLangIR

```text
_tN = i + 1
i = _tN
```

---

## 12.2 Decrement

### Source

```c
i--;
```

### SLangIR

```text
_tN = i - 1
i = _tN
```

---

# 13. Basic Blocks

A basic block is a maximal sequence of instructions with:

* one entry point;
* no branch into the middle of the block;
* and control leaving only through the final instruction or fall-through.

A new basic block begins at:

1. the first instruction of a function;

2. the target of a conditional jump;

3. the target of an unconditional jump;

4. the instruction immediately following a conditional jump;

5. the instruction immediately following an unconditional jump.

Each block is labeled:

```text
B0:
B1:
B2:
```

The final instruction of a block may be:

```text
if _tN goto Bk
goto Bk
return
return literal
return _tN
```

A block may also fall through to the next block when no explicit terminator is needed.

---

# 14. Control-Flow Graph Edges

A CFG edge represents a possible transfer of control between basic blocks.

## Conditional Branch

```text
if _t0 goto B2
```

creates:

* one edge to `B2`; and
* one fall-through edge to the next block.

## Unconditional Jump

```text
goto B3
```

creates one edge to `B3`.

## Fall-Through

If block `B2` is immediately followed by `B3` and contains no terminating branch, then:

```text
B2 -> B3
```

is an implicit fall-through edge.

## Return

A `return` instruction has no successor within the function CFG.

---

# 15. Complete Examples

## 15.1 Variable Declarations and Expressions

### Source

```c
void main()
{
    int base = 2;
    int copied = base;
    int doubled = copied + base;
}
```

### SLangIR

```text
main:
B0:
    base = 2

    _t0 = base
    copied = _t0

    _t1 = copied + base
    doubled = _t1

    return
```

---

## 15.2 Function with If-Else

### Source

```c
int foo(int x, int y)
{
    int z = 10;

    if (x > y)
    {
        z = y + z;
    }
    else
    {
        z = x + z;
    }

    return z;
}
```

### SLangIR

```text
foo:
B0:
    param int x
    param int y

    z = 10

    _t0 = x <= y
    if _t0 goto B2

B1:
    _t1 = y + z
    z = _t1
    goto B3

B2:
    _t2 = x + z
    z = _t2

B3:
    _t3 = z
    return _t3
```

### CFG Edges

```text
B0 -> B1
B0 -> B2
B1 -> B3
B2 -> B3
```

---

## 15.3 Iterative Fibonacci

### Source

```c
int fibonacciIter(int n)
{
    int previous = 0;
    int current = 1;
    int i = 0;

    while (i < n)
    {
        int next = previous + current;
        previous = current;
        current = next;
        i++;
    }

    return previous;
}
```

### SLangIR

```text
fibonacciIter:
B0:
    param int n

    previous = 0
    current = 1
    i = 0

B1:
    _t0 = i >= n
    if _t0 goto B3

B2:
    _t1 = previous + current
    next = _t1

    _t2 = current
    previous = _t2

    _t3 = next
    current = _t3

    _t4 = i + 1
    i = _t4

    goto B1

B3:
    _t5 = previous
    return _t5
```

### CFG Edges

```text
B0 -> B1
B1 -> B2
B1 -> B3
B2 -> B1
```

---

## 15.4 Function Call and Print

### Source

```c
void main()
{
    int result = fibonacciIter(10);
    print(result);
}
```

### SLangIR

```text
main:
B0:
    arg 10
    _t0 = call fibonacciIter, 1
    result = _t0

    arg result
    call print, 1

    return
```

---

# 16. Formal SLangIR Rule

> SLangIR uses one canonical, lowered TAC representation for each source-language construct. Literals may be assigned directly to source-level variables, but variable-to-variable copies must pass through compiler-generated temporaries. Every unary, binary, and relational expression produces a temporary, and each operator in a nested expression receives its own temporary. Formal parameters use `param`, while call-site arguments use `arg`. Value-returning calls store their result in a temporary, whereas void calls do not produce a result. Conditional branches test boolean temporaries, and relational conditions may be complemented to create a convenient fall-through path. Control flow is represented using labeled basic blocks, conditional branches, unconditional jumps, fall-through edges, and return instructions.
