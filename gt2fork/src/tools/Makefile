DIRS=ins2snd2 mod2sng sngspli2 ss2stereo

.PHONY: all clean

all:
	for d in $(DIRS); do $(MAKE) -C $$d; done

clean:
	for d in $(DIRS); do $(MAKE) -C $$d clean; done
