#!/bin/bash

SLANGIR=../../SLangIR/slangir
EXPECTDIR=./expected

mkdir -p "$EXPECTDIR"

if [ ! -x "$SLANGIR" ]; then
    echo "Error: SLangIR executable not found"
    exit 1
fi

for file in *.sl; do
    base=$(basename "$file" .sl)

    ir="${base}.ir"
    errfile="${base}.err"

    echo "Running SLangIR on $file ..."

    stderr_output=$("$SLANGIR" "$file" 2>&1 >/dev/null)

    if [ -f "$ir" ]; then
        mv "$ir" "$EXPECTDIR/$ir"
        echo "Moved $ir to $EXPECTDIR/$ir"
    fi

    if [ -n "$stderr_output" ]; then
        echo "$stderr_output" > "$EXPECTDIR/$errfile"
        echo "Stored errors in $EXPECTDIR/$errfile"
    fi

    echo
done

echo "Done."
