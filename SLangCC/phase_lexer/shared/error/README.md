# SLangCC Error Reporting

The error module provides centralized lexer, parser, and semantic diagnostics.
It consists of a static-only `Error` class declared in `error.h` and implemented
in `error.cpp`. Compiler stages report failures through this class and use
`hasErrors()` to decide whether the current process should continue.

## Basic usage

Set the source path once before lexing or parsing:

```cpp
Error::setSourceFile(sourcePath);
```

Then report diagnostics with their one-based source line when available:

```cpp
Error::lexer("illegal character '@'", line);
Error::parser("Standalone variable or array access.", line);
Error::semantic("Undeclared identifier: 'count'.", line);
```

After a compiler phase, test the accumulated state:

```cpp
if (Error::hasErrors()) {
    return EXIT_FAILURE;
}
```

All diagnostics are written to `std::cerr`; the API does not throw exceptions
or terminate the process itself.

## Public API

| Function | Behavior |
| --- | --- |
| `setSourceFile(path)` | Stores only the final filename component of `path` for future diagnostics. Both `/` and `\\` are recognized as separators. |
| `getSourceFile()` | Returns a const reference to the stored filename. The AST serializer uses it for the root `sourceFile` field. |
| `lexer(message, line)` | Emits a `Lexer error` diagnostic. |
| `parser(message, line)` | Emits a `Syntax error` diagnostic. An empty message or exactly `"syntax error"` is suppressed as generic parser text. |
| `semantic(message, line)` | Emits a `Semantic error` diagnostic. An empty message or exactly `"syntax error"` is suppressed. |
| `hasErrors()` | Returns `true` after any diagnostic has been emitted during the process. |

There are no warning, note, column, source-range, or diagnostic-code APIs.
Lexer, syntax, and semantic reports all increment the same error counter.

## Output format

Diagnostics follow this layout:

```text
<kind> [in <source-file>] [at line <line>] [: <message>]
```

For example:

```text
Semantic error in example.sl at line 12: Undeclared identifier: 'count'.
```

The source-file segment is omitted until a non-empty filename has been set.
The line segment is omitted when `line <= 0`, and the message segment is
omitted when the effective message is empty. Each diagnostic ends with a
newline.

`setSourceFile("path/to/example.sl")` stores `example.sl`, not the full path.
This same basename appears in diagnostics and in serialized `Program` nodes.

## State and lifecycle

`sourceFile` and `errorCount` are process-wide static state. They are initialized
to an empty filename and zero errors when the program starts. The current API
has no reset function, and `setSourceFile()` changes only the filename—it does
not clear the error count. The existing compiler executables therefore process
one source file per process.

This design is not thread-safe. Concurrent calls can race while reading or
writing `sourceFile` and `errorCount`, and diagnostic text written to
`std::cerr` may interleave. If compilation becomes concurrent or a process must
compile multiple independent files, error state should be moved into a
per-compilation diagnostic context or protected with synchronization.

## Integration

- `lexer/lexer.l` reports illegal characters with `Error::lexer()`.
- `parser/parser.y` reports grammar-related and parser-enforced language errors
  with `Error::parser()` or `Error::semantic()`.
- `shared/symboltable` and `semantic_analyzer` report declaration and type errors
  with `Error::semantic()`.
- Parser, semantic-analyzer, IR, code-generation, and driver entry points call
  `hasErrors()` to prevent later phases from running after a diagnostic.
- `Node::toJSON()` reads `getSourceFile()` when serializing the `Program` root.
