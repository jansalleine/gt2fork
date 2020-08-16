#ifndef GSID_H
#define GSID_H

#define NUMSIDREGS 0x19
#define SIDWRITEDELAY 14 // lda $xxxx,x 4 cycles, sta $d400,x 5 cycles, dex 2 cycles, bpl 3 cycles
#define SIDWAVEDELAY 4 // and $xxxx,x 4 cycles extra

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float MOS6581;
    float MOS8580;
} FILTERCURVES;

void sid_init(
    int speed,
    unsigned m,
    unsigned ntsc,
    unsigned interpolate,
    unsigned customclockrate,
    unsigned usefp,
    unsigned numsids
);
int sid_fillbuffer(short *ptr, int samples);
int sid_fillbuffer_stereo(short *lptr, short *rptr, int samples);
unsigned char sid_getorder(unsigned char index);

#ifndef GSID_C
extern unsigned char sidreg[NUMSIDREGS];
extern unsigned char sidreg2[NUMSIDREGS];
extern FILTERCURVES filtercurves;
#endif

#ifdef __cplusplus
}
#endif

#endif
