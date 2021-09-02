#!/bin/sh

OUTFILEC="../legacy.c"
OUTFILEH="../legacy.h"

echo "/**" > $OUTFILEC
echo "/**" > $OUTFILEH

for i in *.c
do
    echo "/* =============================================================================" >> $OUTFILEC
    echo "/* FILE $i: " >> $OUTFILEC
    echo "/* =============================================================================" >> $OUTFILEC
    echo " */" >> $OUTFILEC
    cat $i >> $OUTFILEC
done

for i in *.h
do
    echo "/* =============================================================================" >> $OUTFILEH
    echo "/* FILE $i: " >> $OUTFILEH
    echo "/* =============================================================================" >> $OUTFILEH
    echo " */" >> $OUTFILEH
    cat $i >> $OUTFILEH
done
