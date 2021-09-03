PREFIX?=../../../linux/
SUFFIX?=

CC?=gcc
CFLAGS?=`sdl2-config --cflags` -O3 -Wall
CFLAGS+=-I'../bme'

EXE=$(PREFIX)ss2stereo$(SUFFIX)

.PHONY: clean

all: $(EXE)

$(PREFIX)ss2stereo$(SUFFIX): ss2stereo.o ../bme/bme_end.o
	$(CC) -o $@ $^
	strip $@

clean:
	$(RM) *.o *~
	$(RM) ../bme/*.o
	$(RM) $(EXE)
