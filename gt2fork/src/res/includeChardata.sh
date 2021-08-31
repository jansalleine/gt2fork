#!/bin/sh

OUTFILE="../chardata.c"

xxd -i chargen.bin > $OUTFILE

sed -i 's/_len/Length/g' $OUTFILE
sed -i 's/chargen_bin/chardata/g' $OUTFILE
