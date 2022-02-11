#!/bin/sh
set -e

CPP=cpp
CPP_OPT="-fdirectives-only -E -P -C $CPP_INC $CPP_OPT_EXTRA"

TMP="."

FILE="$1"
OUT="$2"

if [ -z "$FILE" ] || [ -z "$OUT" ]; then
    echo "Usage: $0 [INFILE] [OUTFILE]"
    exit 1
fi

if [ ! -r "$FILE" ]; then
    echo "$FILE is not readable"
    exit 1
fi

# Mark the start of the file
echo "/* @StartFile@ */" >"$TMP/.tmp.c"

# Put markers before and after #include
awk '/^#\s*include/{
    print "//", $0;
    print "/* @PreInclude@ */";
    print;
    print "/* @PostInclude@ */";
    next
} 1' "$FILE" >>"$TMP/.tmp.c"

# Run pre-processor
# shellcheck disable=SC2086
$CPP $CPP_OPT "$TMP/.tmp.c" -o "$TMP/.tmp.i"

# Delete inlined #include
sed '/@PreInclude@/,/@PostInclude@/d' "$TMP/.tmp.i" >"$OUT"

# Delete extra stuff before original start of file
sed --in-place '1,/@StartFile@/d' "$OUT"

# Add #include back in
sed --in-place 's_// \(#\s*include .*\)_\1_' "$OUT"

# Clean up extra newlines
awk -v RS= -v ORS='\n\n' '1' "$OUT" >.tmp2.c
mv .tmp2.c "$OUT"

# Clean up
rm -f "$TMP/.tmp.i"
rm -f "$TMP/.tmp.c"
rm -f "$TMP/.tmp2.c"
