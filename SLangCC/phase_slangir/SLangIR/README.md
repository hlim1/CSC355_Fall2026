# SLangIR

**SLangIR is a traditional compiler intermediate representation that organizes canonical, lowered three-address code (TAC) instructions into basic blocks connected by a control-flow graph (CFG), using one consistent instruction form for each source-language construct.**

The goal is to make the IR:

* deterministic,
* easy to generate,
* easy to inspect,
* and straightforward to translate into assembly.

SLangIR is stricter than general TAC. Traditional TAC permits multiple equivalent representations of the same source construct, while SLangIR selects one canonical form for implementation.

Every example in this document is the exact output of the reference SLangIR generator. The expected files in `benchmark/slangir/expected/` are the final authority on the output format.

---

## 1. Naming Conventions

SLangIR uses the following naming conventions:

```text
x, y, z    = source-level variables
_tN        = compiler-generated temporary
B1, B2     = basic-block labels
T          = source-language type
literal    = constant value
op         = arithmetic or logical binary operator
relop      = relational operator
unary_op   = unary operator
```

Compiler-generated temporaries are numbered in creation order, starting at `_t0`:

```text
_t0
_t1
_t2
...
```

Basic blocks are numbered in **reservation order**, starting at `B1`:

```text
B1
B2
B3
...
```

The entry block of every function is `B1`. Both counters restart in every function, so each function begins with `B1` and `_t0`.

Note that a block number is reserved before its block is created, so blocks do not always appear in the output in numeric order (see section 10).

---

## 2. Canonical Instruction Forms

SLangIR supports the following instruction forms:

```text
param T x

arg operand

x = literal
x[index] = operand

_tN = x
x = _tN

_tN = operand1 op operand2
_tN = operand1 relop operand2
_tN = unary_op operand
_tN = !operand
_tN = conversion operand
_tN = x[index]

if operand goto Bk
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

and `conversion` is one of `inttodouble`, `inttochar`, `chartoint`, `inttostring`, `doubletostring`, `booltostring`, or `chartostring`.

Each instruction contains at most one operator.

**Literals.** Integer, decimal and boolean literals are written as in the source (`5`, `2.5`, `true`). String literals are written in double quotes (`"hi"`) and character literals in single quotes (`'x'`). Backslash, the quote character, newline, carriage return and tab are written as escapes (`\\`, `\"`, `\n`, `\r`, `\t`).

**Spacing.** An operator is separated from its operands by single spaces, including a value-level unary operator: `_t0 = - value`, `_t0 = ! value`. The branch-negation form is the one exception and has **no** space: `_t0 = !operand` (see section 9.3).

---

## 3. Core Rules

SLangIR follows these canonical rules:

1. A literal may be assigned directly to a source-level variable or array element.

2. A value copied from one source-level variable or array element to another must pass through a compiler-generated temporary.

3. Every unary, binary, and relational expression, every conversion, and every array read used as a value produces a compiler-generated temporary.

4. The result of an expression must be assigned from the temporary into the destination variable.

5. Each operator in a nested expression receives its own temporary.

6. Temporaries may be used directly as operands in later instructions.

7. Unnecessary temporary-to-temporary copies must not be generated.

8. Function parameters are emitted at the beginning of the function entry block.

9. Function-call arguments are evaluated first, left to right, and the `arg` instructions are emitted immediately before the corresponding call.

10. A conditional branch tests a single operand. That operand is usually a temporary, but it can be a source-level variable (see section 9.3).

11. A branch jumps to the **false** path of its source condition, so the source condition is always complemented (section 9.2).

12. Control flow is represented using basic blocks, conditional branches, unconditional jumps, and fall-through edges.

13. A void function ends with a `return` unless its body already ends with a `return` statement.

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

An x86 `mov` can store an immediate directly into memory (`movl $2, -4(%rbp)`), but it cannot copy memory to memory. This is why a literal is assigned directly and a variable copy goes through a temporary. It differs from the load-store style used in lecture, where every value passes through a temporary.

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
_t0 = - value
negative = _t0
```

Note the space between the operator and its operand.

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

## 4.9 Compound Assignment

### Source

```c
x += e;
x -= e;
```

### SLangIR

```text
_tN = x + e
x = _tN
```

```text
_tN = x - e
x = _tN
```

---

## 4.10 Increment and Decrement

Pre- and post-increment and decrement statements produce the same code.

### Source

```c
i++;
```

### SLangIR

```text
_tN = i + 1
i = _tN
```

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

The binary operators are:

```text
+   -   *   /
<   <=  >   >=  ==  !=
&&  ||
```

Both operands are translated first, left and then right, and only then is the result temporary created.

---

## 5.2 Unary Expressions

A unary expression produces one temporary.

### Source

```c
-x
```

### SLangIR

```text
_tN = - x
```

The unary operators are `-`, `+` and `!`:

```text
_tN = - x
_tN = + x
_tN = ! x
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

The relational operators are:

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

A relational expression used as a value is **not** complemented. Complementing happens only for branch conditions (section 9.2).

---

## 5.4 Logical Expressions

`&&` and `||` are translated like any other binary operator.

### Source

```c
p && q
```

### SLangIR

```text
_tN = p && q
```

**`&&` and `||` do not short-circuit.** Both operands are always evaluated, then combined into one temporary. This is a deliberate simplification, and it differs from the jumping-code translation in textbooks. One consequence is that `f() && g()` always calls both functions.

---

## 5.5 Nested Expressions

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

Parentheses produce no instructions of their own.

---

## 5.6 Temporary Reuse

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

## 5.7 Type Conversions

After semantic analysis, every cast is one of the following conversion operations. Each produces a temporary.

| Source | SLangIR |
| ------ | ------- |
| `(double) k` | `_t0 = inttodouble k` |
| `(character) k` | `_t0 = inttochar k` |
| `(int) c` | `_t0 = chartoint c` |
| `(string) k` (int) | `_t0 = inttostring k` |
| `(string) d` (double) | `_t0 = doubletostring d` |
| `(string) b` (boolean) | `_t0 = booltostring b` |
| `(string) c` (character) | `_t0 = chartostring c` |

### Example

```c
double d = (double) k;
```

```text
_t0 = inttodouble k
d = _t0
```

---

## 5.8 Arrays

An array element is written `name[index]`, where the index is a variable or an integer literal and stays inside the brackets.

### Source

```c
arr[0] = 10;
```

### SLangIR

```text
arr[0] = 10
```

### Source

```c
arr[x] = arr[0];
int v = arr[1];
```

### SLangIR

```text
_t0 = arr[0]
arr[x] = _t0

_t1 = arr[1]
v = _t1
```

(The blank line is only for readability here; the real output has none.) A literal may be stored into an element directly. Reading an element, or copying one to another location, goes through a temporary.

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
B1:
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
B1:
    param int x
    param int y

```

The printed output contains a blank line after the last `param` instruction.

Rules:

1. `param` instructions appear at the beginning of `B1`.

2. Their order must match the source-level parameter order.

3. Each `param` instruction records both the type and parameter name.

General form:

```text
param T x
```

---

## 6.3 Function End

A `void` function ends with a `return` instruction. If the last statement of the body is already a `return` statement, no extra `return` is added. A call to `exit()` does not count as a `return`, so the closing `return` is still added after it.

---

# 7. Function Calls

SLangIR distinguishes between:

* formal parameters declared by the called function; and
* actual arguments supplied at the call site.

Formal parameters use `param`.

Actual arguments use `arg`.

---

## 7.1 Function-Call Arguments

Every actual argument is evaluated first, left to right. Only then are the `arg` instructions emitted, immediately before the corresponding call.

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
foo(10, true);
```

### SLangIR

```text
arg 10
arg true
call foo, 2
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

Function calls are emitted in left-to-right evaluation order.

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

## 7.8 Variable Arguments

Passing a source-level variable or a literal as an argument does not require a temporary.

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

---

## 7.9 `print`

`print` is a call to a one-argument function named `print`. Before IR generation, semantic analysis has wrapped every non-string argument in a `...tostring` conversion, so every argument is a string.

With one argument, the argument is passed directly:

```c
print("on");
```

```text
arg "on"
call print, 1
```

```c
print(result);
```

```text
_t0 = inttostring result
arg _t0
call print, 1
```

With several arguments, each argument is converted first, in order. Then the strings are joined with `+`, each `+` in its own temporary, and the final value is passed:

```c
print("a = ", a);
```

```text
_t0 = inttostring a
_t1 = "a = " + _t0
arg _t1
call print, 1
```

---

## 7.10 `exit`

### Source

```c
exit();
```

### SLangIR

```text
call exit, 0
```

A `call exit,` instruction ends a basic block, just like `return` and `goto` (see sections 14 and 15). In a `void` function a final `return` is still emitted after it.

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

## 9.1 Conditional Branch and Unconditional Jump

A conditional branch tests a single operand:

```text
if _tN goto Bk
```

This means:

* jump to `Bk` when the operand is true;
* otherwise continue with the next block in the output.

An unconditional jump transfers control to another block:

```text
goto Bk
```

---

## 9.2 Branch on the Negated Condition

A branch jumps to the **false** path of the source condition so that the **true** path falls through. The generator therefore branches on the negation of the source condition.

For a relational operator, the negation is the complementary comparison, with no separate logical-not instruction. For example:

```c
if (x > y)
```

is lowered to:

```text
_t0 = x <= y
if _t0 goto B_else
```

When `x > y` is true, `x <= y` is false, so execution falls through to the then-block. When `x > y` is false, `x <= y` is true, so execution jumps to the else-block.

### Relational Complements

| Original condition | Complement |
| ------------------ | ---------- |
| `x < y`            | `x >= y`   |
| `x <= y`           | `x > y`    |
| `x > y`            | `x <= y`   |
| `x >= y`           | `x < y`    |
| `x == y`           | `x != y`   |
| `x != y`           | `x == y`   |

Both operands are translated first, and then the complement's temporary is created.

---

## 9.3 Other Conditions

Two other kinds of condition are handled differently:

* **Source-level negation `!e`.** The negation of `!e` is `e`, so the generator translates `e` as an ordinary expression and branches directly on its value. `e` is not complemented, and no temporary is created for the `!`.

```c
if (!flag) { ... }
```

```text
if flag goto B2
```

```c
if (!(a < b)) { ... }
```

```text
_t0 = a < b
if _t0 goto B2
```

* **Anything else** (a boolean variable, a literal, a call, `&&`, `||`). The generator reserves a temporary for the negation **first**, then evaluates the expression, then emits `_tN = !operand`. The branch-negation form has no space after `!`.

```c
if (flag) { ... }
```

```text
_t0 = !flag
if _t0 goto B2
```

```c
if (p && q) { ... }
```

```text
_t1 = p && q
_t0 = !_t1
if _t0 goto B2
```

Note that the negation's temporary (`_t0`) has a lower number than the temporaries of the expression it negates. This is the only place where a result temporary is created before the operands are evaluated.

---

## 9.4 Unconditional Jumps

Typical uses of `goto` are:

* skipping an else-block after finishing a then-block;
* returning from the end of a loop body to the loop condition (or update) block;
* jumping to a loop exit (`break`) or to the next iteration (`continue`).

Whether the generator emits a jump or lets control fall through is fixed for each construct (sections 10 to 12); the examples show exactly where `goto` instructions appear.

---

# 10. If Statements

Block numbers are reserved in this order:

1. the common **done** block;
2. for each clause in order: its condition block (only for clauses after the first), then its body block;
3. the **else** block, if there is one.

The blocks are printed in this order: the first condition stays in the current block, then body 1, condition 2, body 2, ..., the else block, and finally **done**. Because `done` is reserved first, it has the smallest number of the group but is printed last.

A then-body ends with `goto done` unless its block already ends in `goto`, `return`, or `call exit,`. The else body falls through into `done`.

## 10.1 If Without Else

### Source

```c
void main()
{
    int x = 1;
    int y = 2;
    int z = 3;
    if (x > y)
    {
        z = y;
    }
}
```

### SLangIR

```text
main:
B1:
    x = 1
    y = 2
    z = 3
    _t0 = x <= y
    if _t0 goto B2

B3:
    _t1 = y
    z = _t1
    goto B2

B2:
    return
```

CFG edges:

```text
B1 -> B2
B1 -> B3
B3 -> B2
```

---

## 10.2 If-Else Statement

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
B1:
    param int x
    param int y

    z = 10
    _t0 = x <= y
    if _t0 goto B4

B3:
    _t1 = y + z
    z = _t1
    goto B2

B4:
    _t2 = x + z
    z = _t2

B2:
    _t3 = z
    return _t3
```

CFG edges:

```text
B1 -> B4
B1 -> B3
B3 -> B2
B4 -> B2
```

The else block `B4` has no `goto`: it falls through into the next printed block, `B2`.

---

## 10.3 If / Elseif / Else

For a three-clause chain, the numbers are: done `B2`, body 1 `B3`, condition 2 `B4`, body 2 `B5`, else `B6`.

### Source

```c
if (x < n) { x = 2; }
elseif (x == n) { x = 3; }
else { x = 4; }
```

### SLangIR

```text
    _t0 = x >= n
    if _t0 goto B4

B3:
    x = 2
    goto B2

B4:
    _t1 = x != n
    if _t1 goto B6

B5:
    x = 3
    goto B2

B6:
    x = 4

B2:
```

Each clause branches to the next clause's condition block, to the else block, or to `done` when there is none.

---

# 11. While Loops

A `while` loop is represented using:

* a condition block;
* a loop-body block;
* an exit block;
* and a back edge from the body to the condition.

Block numbers are reserved in the order condition, body, exit, and printed in the same order. The body always ends with `goto condition`.

### Source

```c
while (i < n)
{
    i++;
}
```

### SLangIR

```text
B2:
    _t0 = i >= n
    if _t0 goto B4

B3:
    _t1 = i + 1
    i = _t1
    goto B2

B4:
```

(The entry block is `B1`, so the loop starts at `B2`.)

The source condition `i < n` is complemented as `i >= n`, and the complemented condition branches to the loop exit.

CFG edges:

```text
B2 -> B4
B2 -> B3
B3 -> B2
```

---

# 12. For Loops, Break and Continue

## 12.1 For Loops

The init clause is translated into the **current** block before any loop block is reserved. Then the blocks are reserved and printed in the order condition, body, update, exit. The body always ends with `goto update`, and the update block ends with `goto condition`. If the condition clause is omitted, the condition block contains no branch. An omitted init or update clause produces no instructions.

### Source

```c
void main()
{
    for (int index = 0; index < 5; index++) {
        print(index);
    }
}
```

### SLangIR

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

---

## 12.2 Break and Continue

`break` jumps to the exit block of the innermost loop. `continue` jumps to the condition block of a `while` loop, and to the **update** block of a `for` loop, so that the update still runs.

### Source

```c
void main()
{
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        if (i == 3) {
            continue;
        }
        if (i == 7) {
            break;
        }
        sum += i;
    }
}
```

### SLangIR

```text
main:
B1:
    sum = 0
    i = 0

B2:
    _t0 = i >= 10
    if _t0 goto B5

B3:
    _t1 = i != 3
    if _t1 goto B6

B7:
    goto B4

B6:
    _t2 = i != 7
    if _t2 goto B8

B9:
    goto B5

B8:
    _t3 = sum + i
    sum = _t3
    goto B4

B4:
    _t4 = i + 1
    i = _t4
    goto B2

B5:
    return
```

Here `B7` and `B9` are the bodies of the two `if` statements. Their blocks already end in `goto`, so no extra `goto done` is added. `continue` jumps to the update block `B4`, and `break` jumps to the exit block `B5`.

---

# 13. Basic Blocks

A basic block is a maximal sequence of instructions with:

* one entry point;
* no branch into the middle of the block;
* and control leaving only through the final instruction or fall-through.

A new basic block begins at:

1. the first instruction of a function;

2. the target of a conditional or unconditional jump;

3. the instruction immediately following a conditional jump;

4. the first instruction of the code after a loop or an `if`.

Each block is labeled:

```text
B1:
B2:
B3:
```

The final instruction of a block may be:

```text
if operand goto Bk
goto Bk
call exit, 0
return
return literal
return _tN
```

A block may also fall through to the next block when no explicit terminator is needed. A block may be empty, for example when an `if` statement is followed directly by a loop: the `done` block of the `if` is then empty and falls through into the loop's condition block.

**Printing.** Each function prints its name and a colon, then its blocks. A blank line separates blocks. Inside a block there are no blank lines, except one blank line after the `param` instructions at the start of a function.

---

# 14. Control-Flow Graph Edges

A CFG edge represents a possible transfer of control between basic blocks. The edges are derived from the final instruction of each block, using the blocks **in the order they are printed** (not in numeric order).

## Conditional Branch

```text
if _t0 goto B2
```

creates:

* one edge to `B2`; and
* one fall-through edge to the next printed block.

## Unconditional Jump

```text
goto B3
```

creates one edge to `B3` and no fall-through edge.

## Fall-Through

If a block contains no terminating instruction, it has one edge to the next printed block. An empty block falls through in the same way.

## Return and Exit

A `return` instruction, and a `call exit,` instruction, have no successor within the function CFG.

## Order and Duplicates

For a conditional branch, the branch target is listed first, then the fall-through successor. A successor is listed once even if both edges reach the same block.

The edges do not appear in the `.ir` output. They are built by `IRFunction::rebuildCFG()`, which must be called once for each function after its last instruction has been emitted.

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
B1:
    base = 2
    _t0 = base
    copied = _t0
    _t1 = copied + base
    doubled = _t1
    return
```

---

## 15.2 Function with If-Else

See section 10.2.

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
B1:
    param int n

    previous = 0
    current = 1
    i = 0

B2:
    _t0 = i >= n
    if _t0 goto B4

B3:
    _t1 = previous + current
    next = _t1
    _t2 = current
    previous = _t2
    _t3 = next
    current = _t3
    _t4 = i + 1
    i = _t4
    goto B2

B4:
    _t5 = previous
    return _t5
```

### CFG Edges

```text
B1 -> B2
B2 -> B4
B2 -> B3
B3 -> B2
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
B1:
    arg 10
    _t0 = call fibonacciIter, 1
    result = _t0
    _t1 = inttostring result
    arg _t1
    call print, 1
    return
```

---

# 16. Formal SLangIR Rule

> SLangIR uses one canonical, lowered TAC representation for each source-language construct. Literals may be assigned directly to source-level variables, but variable-to-variable copies must pass through compiler-generated temporaries. Every unary, binary, and relational expression produces a temporary, and each operator in a nested expression receives its own temporary. `&&` and `||` are ordinary binary operators and do not short-circuit. Formal parameters use `param`, while call-site arguments use `arg`. Value-returning calls store their result in a temporary, whereas void calls do not produce a result. A conditional branch tests one operand and jumps to the false path of the source condition, so the source condition is complemented to create a fall-through path for the true case. Control flow is represented using labeled basic blocks, conditional branches, unconditional jumps, fall-through edges, and return instructions.
