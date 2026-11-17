#!/bin/bash

SLANGIR=../../SLangIR/slangir
OUTDIR=./out
EXPECTDIR=./expected

mkdir -p "$OUTDIR"

if [ ! -x "$SLANGIR" ]; then
    echo "Error: SLangIR executable not found"
    exit 1
fi

if [ ! -d "$EXPECTDIR" ]; then
    echo "Error: expected directory not found"
    exit 1
fi

PASS_COUNT=0
FAIL_COUNT=0

for file in *.sl; do
    base=$(basename "$file" .sl)

    ir="${base}.ir"
    err="${base}.err"

    out_ir="$OUTDIR/$ir"
    out_err="$OUTDIR/$err"

    expected_ir="$EXPECTDIR/$ir"
    expected_err="$EXPECTDIR/$err"

    rm -f "$out_ir" "$out_err" "$ir" "$err"

    echo "========================================"
    echo "Running test: $file"
    echo "========================================"

    stderr_output=$("$SLANGIR" "$file" 2>&1 >/dev/null)

    if [ -f "$ir" ]; then
        mv "$ir" "$out_ir"
    fi

    if [ -n "$stderr_output" ]; then
        echo "$stderr_output" > "$out_err"
    fi

    if [ -f "$expected_ir" ]; then
        if [ ! -f "$out_ir" ]; then
            echo "[FAIL] $base"
            echo "Expected IR file, but SLangIR did not produce one."
            FAIL_COUNT=$((FAIL_COUNT + 1))
            echo
            continue
        fi

        if diff -u "$expected_ir" "$out_ir" > /dev/null; then
            echo "[PASS] $base"
            PASS_COUNT=$((PASS_COUNT + 1))
        else
            echo "[FAIL] $base"
            echo "IR differences:"
            diff -u "$expected_ir" "$out_ir"
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi

    elif [ -f "$expected_err" ]; then
        if [ ! -f "$out_err" ]; then
            echo "[FAIL] $base"
            echo "Expected error file, but SLangIR did not produce one."
            FAIL_COUNT=$((FAIL_COUNT + 1))
            echo
            continue
        fi

        if diff -u "$expected_err" "$out_err" > /dev/null; then
            echo "[PASS] $base"
            PASS_COUNT=$((PASS_COUNT + 1))
        else
            echo "[FAIL] $base"
            echo "Error-output differences:"
            diff -u "$expected_err" "$out_err"
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi

    else
        echo "[FAIL] $base"
        echo "No expected file found: $expected_ir or $expected_err"
        FAIL_COUNT=$((FAIL_COUNT + 1))
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
