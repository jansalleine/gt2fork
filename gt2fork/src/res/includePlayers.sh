#!/bin/sh

OUTFILE="../players.c"

xxd -i player.s > $OUTFILE
xxd -i altplayer.s >> $OUTFILE
xxd -i player_s.s >> $OUTFILE
xxd -i altplayer_s.s >> $OUTFILE

sed -i 's/_len/Length/g' $OUTFILE
sed -i 's/altplayer_s_s/playRoutineStereoAlt/g' $OUTFILE
sed -i 's/player_s_s/playRoutineStereo/g' $OUTFILE
sed -i 's/altplayer_s/playRoutineAlt/g' $OUTFILE
sed -i 's/player_s/playRoutine/g' $OUTFILE
