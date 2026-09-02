#!/bin/bash

LEXER=../../lexer/lexer
OUTDIR=./expected

mkdir -p "$OUTDIR"

if [ ! -x "$LEXER" ]; then
    echo "Error: lexer executable not found"
    exit 1
fi

for file in *.sl; do
    base=$(basename "$file" .sl)

    echo "Running $file ..."

    $LEXER "$file" > "$OUTDIR/${base}.out" 2>&1

    echo "Saved output to $OUTDIR/${base}.out"
done

echo "All tests completed."
