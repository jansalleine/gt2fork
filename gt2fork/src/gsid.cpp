/*
 * GOATTRACKER reSID interface
 */

#define GSID_C

#include <stdlib.h>
#include <stdio.h>
#include "resid/sid.h"
#include "residfp/SID.h"

#include "gsid.h"
#include "gsound.h"

int clockrate;
int samplerate;
unsigned char sidreg[NUMSIDREGS];
unsigned char sidorder[] =
{
    0x18,0x17,0x16,0x15,
    0x14,0x13,0x12,0x11,0x10,0x0f,0x0e,
    0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,
    0x06,0x05,0x04,0x03,0x02,0x01,0x00
};

unsigned char altsidorder[] =
{
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,
    0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,
    0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
    0x15,0x16,0x17,0x18
};

FILTERCURVES filtercurves =
{
    0.50f,
    0.50f
};

reSID::SID *sid = 0;
reSIDfp::SID *sidfp = 0;

extern unsigned residdelay;
extern unsigned adparam;

void sid_init(
        int speed,
        unsigned m,
        unsigned ntsc,
        unsigned interpolate,
        unsigned customclockrate,
        unsigned usefp
    )
{
    int c;
    /*
     * highestAccurateFrequency
     * I acutally don't know what highestAccurateFrequency actually is
     * I took the value from residfp/resample/test.cpp
     * and it seems to be the only value that works *shrug*
    */
    const double highestAccurateFrequency = 20000.0;

    if (ntsc) clockrate = NTSCCLOCKRATE;
    else clockrate = PALCLOCKRATE;

    if (customclockrate) clockrate = customclockrate;

    samplerate = speed;

    if (!usefp)
    {
        if (sidfp)
        {
            delete sidfp;
            sidfp = NULL;
        }

        if (!sid) sid = new reSID::SID;
    }
    else
    {
        if (sid)
        {
            delete sid;
            sid = NULL;
        }

        if (!sidfp) sidfp = new reSIDfp::SID;
    }

    switch (interpolate)
    {
        case 0:
            if (sid)
            {
                sid->set_sampling_parameters(
                    clockrate,
                    reSID::SAMPLE_FAST,
                    speed
                );
            }
            if (sidfp)
            {
                sidfp->setSamplingParameters(
                    (double)clockrate,
                    reSIDfp::DECIMATE,
                    (double)speed,
                    highestAccurateFrequency
                );
            }
            break;

        default:
            if (sid)
            {
                sid->set_sampling_parameters(
                    clockrate,
                    reSID::SAMPLE_RESAMPLE,
                    speed
                );
            }
            if (sidfp)
            {
                sidfp->setSamplingParameters(
                    (double)clockrate,
                    reSIDfp::RESAMPLE,
                    (double)speed,
                    highestAccurateFrequency
                );
            }
            break;
    }

    if (sid) sid->reset();
    if (sidfp) sidfp->reset();

    for (c = 0; c < NUMSIDREGS; c++)
    {
        sidreg[c] = 0x00;
    }

    if (m == 1)
    {
        if (sid) sid->set_chip_model(reSID::MOS8580);
        if (sidfp)
        {
            sidfp->setChipModel(reSIDfp::MOS8580);
            sidfp->setFilter8580Curve((double)filtercurves.MOS8580);
        }
    }
    else
    {
        if (sid) sid->set_chip_model(reSID::MOS6581);
        if (sidfp)
        {
            sidfp->setChipModel(reSIDfp::MOS6581);
            sidfp->setFilter6581Curve((double)filtercurves.MOS6581);
        }
    }
}

unsigned char sid_getorder(unsigned char index)
{
    if (adparam >= 0xf000)
    {
        return altsidorder[index];
    }
    else
    {
        return sidorder[index];
    }
}

int sid_fillbuffer(short *ptr, int samples)
{
    int tdelta;
    int tdelta2;
    int result = 0;
    int total = 0;
    int c;

    int badline = rand() % NUMSIDREGS;

    tdelta = clockrate * samples / samplerate;
    if (tdelta <= 0) return total;


    for (c = 0; c < NUMSIDREGS; c++)
    {
        unsigned char o = sid_getorder(c);

        // Possible random badline delay once per writing
        if ((badline == c) && (residdelay))
        {
            tdelta2 = residdelay;

            if (sid) result = sid->clock(tdelta2, ptr, samples);
            if (sidfp) result = sidfp->clock(tdelta2, ptr);

            total += result;
            ptr += result;
            samples -= result;
            tdelta -= residdelay;
        }

        if (sid) sid->write(o, sidreg[o]);
        if (sidfp) sidfp->write(o, sidreg[o]);

        tdelta2 = SIDWRITEDELAY;

        if (sid) result = sid->clock(tdelta2, ptr, samples);
        if (sidfp) result = sidfp->clock(tdelta2, ptr);

        total += result;
        ptr += result;
        samples -= result;
        tdelta -= SIDWRITEDELAY;

        if (tdelta <= 0) return total;
    }

    if (sid) result = sid->clock(tdelta, ptr, samples);
    if (sidfp) result = sidfp->clock(tdelta, ptr);

    total += result;
    ptr += result;
    samples -= result;

    // Loop extra cycles until all samples produced
    while (samples)
    {
        tdelta = clockrate * samples / samplerate;
        if (tdelta <= 0) return total;

        if (sid) result = sid->clock(tdelta, ptr, samples);
        if (sidfp) result = sidfp->clock(tdelta, ptr);

        total += result;
        ptr += result;
        samples -= result;
    }

    return total;
}

