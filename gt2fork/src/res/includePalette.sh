#!/bin/sh

OUTFILE="../palette.c"

xxd -i palette.bin > $OUTFILE

sed -i 's/_len/Length/g' $OUTFILE
sed -i 's/palette_bin/palette/g' $OUTFILE
