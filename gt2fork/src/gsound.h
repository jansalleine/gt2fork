#ifndef GSOUND_H
#define GSOUND_H

#define MINMIXRATE 11025
#define MAXMIXRATE 48000
#define MINBUF 20
#define MAXBUF 2000
#define DEFAULTBUF 100
#define DEFAULTMIXRATE 48000

#define PALFRAMERATE 50
#define PALCLOCKRATE 985248
#define NTSCFRAMERATE 60
#define NTSCCLOCKRATE 1022727

#define MIXBUFFERSIZE 65536

int sound_init(
    unsigned b,
    unsigned mr,
    unsigned writer,
    unsigned m,
    unsigned ntsc,
    unsigned multiplier,
    unsigned interpolate,
    unsigned customclockrate
);
void sound_uninit(void);

#endif
