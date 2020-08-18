//
// GOATTRACKER sound routines
//

#define GSOUND_C

#ifdef __WIN32__
#include <windows.h>
#endif

#include "gt2fork.h"

// General / reSID output
int playspeed;
int initted = 0;
unsigned framerate = PALFRAMERATE;
Sint16 *buffer = NULL;
Sint16 *lbuffer = NULL;
Sint16 *rbuffer = NULL;

FILE *writehandle = NULL;
// SDL_TimerID timer = 0;

void sound_playrout(void);
void sound_mixer(Sint32 *dest, unsigned samples);
Uint32 sound_timer(Uint32 interval, void *param);

int sound_init(
        unsigned b,
        unsigned mr,
        unsigned writer,
        unsigned m,
        unsigned ntsc,
        unsigned multiplier,
        unsigned interpolate,
        unsigned customclockrate
    )
{
    sound_uninit();

    if (multiplier)
    {
        if (ntsc)
        {
            framerate = NTSCFRAMERATE * multiplier;
            snd_bpmtempo = 150 * multiplier;
        }
        else
        {
            framerate = PALFRAMERATE * multiplier;
            snd_bpmtempo = 125 * multiplier;
        }
    }
    else
    {
        if (ntsc)
        {
            framerate = NTSCFRAMERATE / 2;
            snd_bpmtempo = 150 / 2;
        }
        else
        {
            framerate = PALFRAMERATE / 2;
            snd_bpmtempo = 125 / 2;
        }
    }

    if (!buffer) buffer = (Sint16*)malloc(MIXBUFFERSIZE * sizeof(Sint16));
    if (!lbuffer) lbuffer = (Sint16*)malloc(MIXBUFFERSIZE * sizeof(Sint16));
    if (!rbuffer) rbuffer = (Sint16*)malloc(MIXBUFFERSIZE * sizeof(Sint16));
    if ((!buffer) || (!lbuffer) || (!rbuffer)) return 0;

    if (writer)
    {
        writehandle = fopen("sidaudio.raw", "wb");
    }

    playspeed = mr;
    if (playspeed < MINMIXRATE) playspeed = MINMIXRATE;
    if (playspeed > MAXMIXRATE) playspeed = MAXMIXRATE;
    if (b < MINBUF) b = MINBUF;
    if (b > MAXBUF) b = MAXBUF;


    if (numsids == 1 && !snd_init(mr, SIXTEENBIT|MONO, b, 1, 0))
    {
        return 0;
    }
    else if (numsids == 2 && !snd_init(mr, SIXTEENBIT|STEREO, b, 1, 0))
    {
        return 0;
    }

    playspeed = snd_mixrate;

    sid_init(
        playspeed,
        m,
        ntsc,
        interpolate & 1,
        customclockrate,
        interpolate >> 1,
        numsids
    );

    snd_player = &sound_playrout;
    snd_setcustommixer(sound_mixer);

    // SOUNDOK:
    initted = 1;
    return 1;
}

void sound_uninit(void)
{
    if (!initted) return;
    initted = 0;

    // Apparently a delay is needed to make sure the sound timer thread is
    // not mixing stuff anymore, and we can safely delete related structures
    SDL_Delay(50);

    snd_setcustommixer(NULL);
    snd_player = NULL;

    if (writehandle)
    {
        fclose(writehandle);
        writehandle = NULL;
    }

    if (buffer)
    {
        free(buffer);
        buffer = NULL;
    }

    if (lbuffer)
    {
        free(lbuffer);
        lbuffer = NULL;
    }

    if (rbuffer)
    {
        free(rbuffer);
        rbuffer = NULL;
    }
}

Uint32 sound_timer(Uint32 interval, void *param)
{
    if (!initted) return interval;
    sound_playrout();
    return interval;
}

void sound_playrout(void)
{
    if (numsids == 1)
    {
        playroutine();
    }
    else if (numsids == 2)
    {
        playroutine_stereo();
    }
}

void sound_mixer(Sint32 *dest, unsigned samples)
{
    int c;

    if (!initted) return;
    if (samples > MIXBUFFERSIZE) return;

    if (numsids == 1)
    {
        if (!buffer) return;

        sid_fillbuffer(buffer, samples);
        if (writehandle)
        {
            fwrite(buffer, samples * sizeof(Uint16), 1, writehandle);
        }

        for (c = 0; c < samples; c++)
        {
            dest[c] = buffer[c];
        }
    }
    else if (numsids == 2)
    {
        sid_fillbuffer_stereo(lbuffer, rbuffer, samples);
        if (writehandle)
        {
            for (c = 0; c < samples; c++)
            {
                fwrite(&lbuffer[c], sizeof(Sint16), 1, writehandle);
                fwrite(&rbuffer[c], sizeof(Sint16), 1, writehandle);
            }
        }
        if (monomode)
        {
            for (c = 0; c < samples; c++)
            {
                dest[c*2] = lbuffer[c] / 2 + rbuffer[c] / 2;
                dest[c*2+1] = dest[c*2];
            }
        }
        else
        {
            for (c = 0; c < samples; c++)
            {
                dest[c*2] = lbuffer[c];
                dest[c*2+1] = rbuffer[c];
            }
        }
    }
}
