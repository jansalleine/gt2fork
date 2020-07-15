CC=g++
CFLAGS=-Wall -D_GNU_SOURCE=1 -D_REENTRANT
RM=rm -f

SOURCES=sid.cc \
	voice.cc \
	wave.cc \
	envelope.cc \
	filter.cc \
	dac.cc \
	extfilt.cc \
	pot.cc

all: $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	$(RM) *.o
