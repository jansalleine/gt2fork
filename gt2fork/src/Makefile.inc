.PHONY: datafiles clean tools

EXE=$(PREFIX)gt2fork$(SUFFIX)

GTOBJECTS=\
	gt2forkdata.o \
	gt2fork.o \
	gsong.o \
	gorder.o \
	gpattern.o \
	ginstr.o \
	gtable.o \
	gplay.o \
	gdisplay.o \
	gfile.o \
	greloc.o \
	ghelp.o \
	gsound.o \
	gconsole.o \
	gsid.o \
	gcolors.o

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

ASMOBJECTS=\
	asm/asmtab.o \
	asm/chnkpool.o \
	asm/expr.o \
	asm/lexyy.o \
	asm/log.o \
	asm/membuf.o \
	asm/membufio.o \
	asm/namedbuf.o \
	asm/parse.o \
	asm/pc.o \
	asm/vec.o

BMEOBJECTS=\
	bme/bme_gfx.o \
	bme/bme_snd.o \
	bme/bme_win.o \
	bme/bme_mou.o \
	bme/bme_kbd.o \
	bme/bme_io.o \
	bme/bme_end.o \
	bme/bme.o

all: datafiles $(EXE) tools

$(PREFIX)gt2fork$(SUFFIX): $(GTOBJECTS) $(RESIDOBJECTS) $(RESIDFPOBJECTS) \
	$(ASMOBJECTS) $(BMEOBJECTS) $(OPTOBJECTS)
	$(CXX) -o $@ $^ $(LIBS)
	strip $@

datafiles:
	$(MAKE) -C bme all

gt2fork.dat: player.s altplayer.s player_s.s altplayer_s.s chargen.bin \
	palette.bin gt2fork.png gt2fork.seq
	./bme/datafile $@ gt2fork.seq

gt2forkdata.c: gt2fork.dat
	./bme/dat2inc $< $@

tools:
	$(MAKE) -C tools all

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C bme clean
	$(RM) *.o *~
	$(RM) asm/*.o
	$(RM) resid/*.o
	$(RM) residfp/*.o
	$(RM) residfp/resample/*.o
	$(RM) $(EXE)
	$(RM) ../win32/*.cfg
	$(RM) gt2forkdata.c
	$(RM) gt2fork.dat
