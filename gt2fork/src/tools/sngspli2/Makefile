PREFIX?=../../../linux/
SUFFIX?=

CC?=gcc
CFLAGS?=`sdl2-config --cflags` -O3 -Wall
CFLAGS+=-I'../bme'

EXE=$(PREFIX)sngspli2$(SUFFIX)

.PHONY: clean

all: $(EXE)

$(PREFIX)sngspli2$(SUFFIX): sngspli2.o ../bme/bme_end.o
	$(CC) -o $@ $^
	strip $@

clean:
	$(RM) *.o *~
	$(RM) ../bme/*.o
	$(RM) $(EXE)
