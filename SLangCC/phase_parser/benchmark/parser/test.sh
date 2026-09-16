#!/bin/bash

PARSER=../../parser/parser
OUTDIR=./out
EXPECTDIR=./expected

mkdir -p "$OUTDIR"

if [ ! -x "$PARSER" ]; then
    echo "Error: parser executable not found"
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

    json="${base}.json"
    err="${base}.err"

    out_json="$OUTDIR/$json"
    out_err="$OUTDIR/$err"

    expected_json="$EXPECTDIR/$json"
    expected_err="$EXPECTDIR/$err"

    rm -f "$out_json" "$out_err" "$json" "$err"

    echo "========================================"
    echo "Running test: $file"
    echo "========================================"

    stderr_output=$("$PARSER" "$file" 2>&1 >/dev/null)

    if [ -f "$json" ]; then
        mv "$json" "$out_json"
    fi

    if [ -n "$stderr_output" ]; then
        echo "$stderr_output" > "$out_err"
    fi

    if [ -f "$expected_json" ]; then
        if [ ! -f "$out_json" ]; then
            echo "[FAIL] $base"
            echo "Expected JSON file, but parser did not produce one."
            FAIL_COUNT=$((FAIL_COUNT + 1))
            echo
            continue
        fi

        if diff -u "$expected_json" "$out_json" > /dev/null; then
            echo "[PASS] $base"
            PASS_COUNT=$((PASS_COUNT + 1))
        else
            echo "[FAIL] $base"
            echo "JSON differences:"
            diff -u "$expected_json" "$out_json"
            FAIL_COUNT=$((FAIL_COUNT + 1))
        fi

    elif [ -f "$expected_err" ]; then
        if [ ! -f "$out_err" ]; then
            echo "[FAIL] $base"
            echo "Expected error file, but parser did not produce one."
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
        echo "No expected file found: $expected_json or $expected_err"
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
