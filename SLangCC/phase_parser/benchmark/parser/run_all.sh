#!/bin/bash

PARSER=../../parser/parser
EXPECTDIR=./expected

mkdir -p "$EXPECTDIR"

if [ ! -x "$PARSER" ]; then
    echo "Error: parser executable not found"
    exit 1
fi

for file in *.sl; do
    base=$(basename "$file" .sl)

    json="${base}.json"
    errfile="${base}.err"

    echo "Running parser on $file ..."

    stderr_output=$("$PARSER" "$file" 2>&1 >/dev/null)

    if [ -f "$json" ]; then
        mv "$json" "$EXPECTDIR/$json"
        echo "Moved $json to $EXPECTDIR/$json"
    fi

    if [ -n "$stderr_output" ]; then
        echo "$stderr_output" > "$EXPECTDIR/$errfile"
        echo "Stored errors in $EXPECTDIR/$errfile"
    fi

    echo
done

echo "Done."
