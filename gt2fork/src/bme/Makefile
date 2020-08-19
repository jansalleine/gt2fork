CC=gcc
CFLAGS=`sdl-config --cflags` -O3 -Wall
LIBS=`sdl-config --libs`

.PHONY: clean

all: dat2inc datafile

datafile: datafile.c bme_end.c
	$(CC) $(CFLAGS) -o datafile datafile.c bme_end.c
	strip datafile

dat2inc: dat2inc.c
	$(CC) $(LIBS) -o dat2inc dat2inc.c
	strip dat2inc

clean:
	$(RM) *.o
	$(RM) datafile
	$(RM) dat2inc

