.PHONY: clean tools

EXE=$(PREFIX)gt2fork$(SUFFIX)

GTOBJECTS=\
	gt2fork.o \
	players.o \
	chardata.o \
	palette.o \
	icon.o \
	song.o \
	order.o \
	pattern.o \
	instr.o \
	table.o \
	play.o \
	display.o \
	file.o \
	reloc.o \
	help.o \
	sound.o \
	console.o \
	sid.o \
	colors.o

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

LEGACYOBJECTS=\
	legacy.o

all: $(EXE) tools

$(PREFIX)gt2fork$(SUFFIX): $(GTOBJECTS) $(RESIDOBJECTS) $(RESIDFPOBJECTS) \
	$(ASMOBJECTS) $(LEGACYOBJECTS) $(OPTOBJECTS)
	$(CXX) -o $@ $^ $(LIBS)
	strip $@
	cp -f res/custom.pal.example $(PREFIX)custom.pal.example

tools:
	$(MAKE) -C tools all

clean:
	$(MAKE) -C tools clean
	$(RM) *.o *~
	$(RM) asm/*.o
	$(RM) resid/*.o
	$(RM) residfp/*.o
	$(RM) residfp/resample/*.o
	$(RM) $(EXE)
	$(RM) $(PREFIX)*.cfg
	$(RM) $(PREFIX)custom.pal.example
