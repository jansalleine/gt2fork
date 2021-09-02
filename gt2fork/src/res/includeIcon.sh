#!/bin/sh

OUTFILE="../icon.c"

xxd -i gt2fork.png > $OUTFILE

sed -i 's/_len/Length/g' $OUTFILE
sed -i 's/gt2fork_png/icon/g' $OUTFILE
