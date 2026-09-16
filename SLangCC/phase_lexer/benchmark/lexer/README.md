# Lexer Test Suite

These files are designed for the provided `lexer.l` Flex lexer. They focus on token recognition, maximal-munch behavior, comments, whitespace, literals, delimiters, invalid characters, and boundary cases.

## Suggested use

If your project builds an executable scanner, run each `.sl` file through it. 

```
$<path>/<to>/lexer <test file>.sl
```

Example:

```
$../../lexer/lexer 01_keywords_all.sl
```

The output from the lexer is either the list of tokens or error message.

```
<unit> (<token class>)
```

Example:

```
int (T_INT)
double (T_DOUBLE)
boolean (T_BOOLEAN)
character (T_CHARACTER)
void (T_VOID)
string (T_STRING)
if (T_IF)
else (T_ELSE)
elseif (T_ELSEIF)
exit (T_EXIT)
print (T_PRINT)
true (T_TRUE)
false (T_FALSE)
for (T_FOR)
while (T_WHILE)
continue (T_CONTINUE)
break (T_BREAK)
return (T_RETURN)
```

or

```
Illegal character '<character>'
```

Example:

```
Illegal character '@'
```

## Automated Testing

The `expected/` directory contains all the ouputs from running test files from 01 to 50 in files.

Run `test.sh` to generate new set of output files, which will be stored in the `out/` directory, and compare these file with the expected output files.

To run `test.sh`, make sure the script is executable:

```
$chmod +x test.sh
```

## Coverage map

- `01` to `04`: keywords, identifiers, whitespace, comments
- `05` to `09`: comments with code, operators, delimiters
- `10` to `12`: integer and decimal literals, including malformed decimal-looking input
- `13` to `17`: string and character literals, including escape and error cases
- `18` to `24`: expressions, arrays, blocks, control flow, keyword boundaries
- `25` to `32`: illegal characters, invalid/valid mixes, hash in strings, Unicode, dense input
- `33` to `37`: long identifiers, large numbers, line ending cases, all-token stress
- `38` to `50`: empty input, only-invalid input, nested constructs, sign/number behavior, realistic sample program, maximal-munch cases

## Notes

This lexer ignores spaces, tabs, and newlines, ignores `#` comments, returns tokens for keywords/operators/delimiters/literals, and prints `Illegal character` for unmatched single characters.
