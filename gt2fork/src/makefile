#
# Linux makefile
#

CFLAGS=`sdl2-config --cflags`
PREFIX=../linux/
SUFFIX=
LIBS=`sdl2-config --libs`

CC?=gcc
CXX?=g++
CFLAGS+=-O3 -Wall -Ibme -Iasm
CXXFLAGS=$(CFLAGS) -fpermissive

RESIDOBJECTS=\
	resid/sid.o \
	resid/voice.o \
	resid/wave.o \
	resid/envelope.o \
	resid/filter.o \
	resid/dac.o \
	resid/extfilt.o \
	resid/pot.o

RESIDFPOBJECTS=\
	residfp/Dac.o \
	residfp/Filter6581.o \
	residfp/FilterModelConfig8580.o \
	residfp/Integrator.o \
	residfp/Spline.o \
	residfp/EnvelopeGenerator.o \
	residfp/Filter8580.o \
	residfp/FilterModelConfig.o \
	residfp/OpAmp.o \
	residfp/WaveformCalculator.o \
	residfp/ExternalFilter.o \
	residfp/Filter.o \
	residfp/Integrator8580.o \
	residfp/SID.o \
	residfp/WaveformGenerator.o \
	residfp/resample/SincResampler.o

EXE=$(PREFIX)gt2fork$(SUFFIX)

all: datafile dat2inc $(EXE)

$(PREFIX)gt2fork$(SUFFIX): goatdata.o gt2fork.o gsong.o gorder.o gpattern.o \
	ginstr.o gtable.o gplay.o gdisplay.o gfile.o greloc.o ghelp.o gsound.o \
	gconsole.o gsid.o gcolors.o $(RESIDOBJECTS) $(RESIDFPOBJECTS) \
	asm/asmtab.o asm/chnkpool.o asm/expr.o asm/lexyy.o asm/log.o asm/membuf.o \
	asm/membufio.o asm/namedbuf.o asm/parse.o asm/pc.o asm/vec.o \
	bme/bme_gfx.o bme/bme_snd.o bme/bme_win.o bme/bme_mou.o bme/bme_kbd.o \
	bme/bme_io.o bme/bme_end.o bme/bme.o
	$(CXX) -o $@ $^ $(LIBS)
	strip $@

goattrk2.dat: player.s altplayer.s player_s.s altplayer_s.s chargen.bin \
	palette.bin goattrk2.bmp goattrk2.seq
	./datafile $@ goattrk2.seq

goatdata.c: goattrk2.dat
	./dat2inc $< $@

datafile: bme/datafile.c bme/bme_end.c
	$(CC) -o datafile bme/datafile.c bme/bme_end.c $(CFLAGS) $(LIBS)
	strip datafile

dat2inc: bme/dat2inc.c
	$(CC) -o dat2inc bme/dat2inc.c $(CFLAGS) $(LIBS)
	strip dat2inc

clean:
	$(RM) *.o *~
	$(RM) asm/*.o
	$(RM) bme/*.o
	$(RM) resid/*.o
	$(RM) residfp/*.o
	$(RM) residfp/resample/*.o
	$(RM) $(EXE)
	$(RM) datafile
	$(RM) dat2inc
	$(RM) goatdata.c
	$(RM) goattrk2.dat
