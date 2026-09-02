#!/bin/bash

LEXER=../../lexer/lexer
OUTDIR=./out
EXPECTDIR=./expected

mkdir -p "$OUTDIR"

if [ ! -x "$LEXER" ]; then
    echo "Error: lexer executable not found"
    exit 1
fi

PASS_COUNT=0
FAIL_COUNT=0

for file in *.sl; do
    base=$(basename "$file" .sl)

    OUTFILE="$OUTDIR/${base}.out"
    EXPECTFILE="$EXPECTDIR/${base}.out"

    echo "========================================"
    echo "Running test: $file"
    echo "========================================"

    $LEXER "$file" > "$OUTFILE" 2>&1

    echo "Output saved to: $OUTFILE"

    if [ -f "$EXPECTFILE" ]; then
        if diff -u "$EXPECTFILE" "$OUTFILE" > /dev/null; then
            echo "[PASS] $base"
            PASS_COUNT=$((PASS_COUNT + 1))
        else
            echo "[FAIL] $base"
            echo "Differences:"
            diff -u "$EXPECTFILE" "$OUTFILE"
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi
    else
        echo "[WARNING] Missing expected file: $EXPECTFILE"
    fi

    echo
done

echo "========================================"
echo "Summary"
echo "========================================"
echo "Passed: $PASS_COUNT"
echo "Failed: $FAIL_COUNT"

if [ "$FAIL_COUNT" -ne 0 ]; then
    exit 1
fi

exit 0
