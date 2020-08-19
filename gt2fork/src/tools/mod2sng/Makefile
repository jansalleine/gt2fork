PREFIX?=../../../linux/
SUFFIX?=

CC?=gcc
CFLAGS?=`sdl2-config --cflags` -O3 -Wall
CFLAGS+=-I'../bme'

EXE=$(PREFIX)mod2sng$(SUFFIX)

.PHONY: clean

all: $(EXE)

$(PREFIX)mod2sng$(SUFFIX): mod2sng.o ../bme/bme_end.o
	$(CC) -o $@ $^
	strip $@

clean:
	$(RM) *.o *~
	$(RM) ../bme/*.o
	$(RM) $(EXE)
