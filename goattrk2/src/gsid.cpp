/*
 * GOATTRACKER reSID interface
 */

#define GSID_C

#include <stdlib.h>
#include <stdio.h>
#include "resid/sid.h"
// #include "resid-fp/sidfp.h"
#include "residfp/SID.h"

#include "gsid.h"
#include "gsound.h"

int clockrate;
int samplerate;
unsigned char sidreg[NUMSIDREGS];
unsigned char sidorder[] =
  {0x18,0x17,0x16,0x15,
   0x14,0x13,0x12,0x11,0x10,0x0f,0x0e,
   0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,
   0x06,0x05,0x04,0x03,0x02,0x01,0x00};

unsigned char altsidorder[] =
  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,
   0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,
   0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
   0x15,0x16,0x17,0x18};

reSID::SID *sid = 0;
reSIDfp::SID *sidfp = 0;

FILTERPARAMS filterparams =
  {0.50f, 3.3e6f, 1.0e-4f,
   1147036.4394268463f, 274228796.97550374f, 1.0066634233403395f, 16125.154840564108f,
   5.5f, 20.f,
   0.9613160610660189f};

extern unsigned residdelay;
extern unsigned adparam;

void sid_init(int speed, unsigned m, unsigned ntsc, unsigned interpolate, unsigned customclockrate, unsigned usefp)
{
  int c;

  if (ntsc) clockrate = NTSCCLOCKRATE;
    else clockrate = PALCLOCKRATE;

  if (customclockrate)
    clockrate = customclockrate;

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

  printf("clockrate: %d speed: %d\n", clockrate, speed);

  switch(interpolate)
  {
    case 0:
    if (sid) sid->set_sampling_parameters(clockrate, reSID::SAMPLE_FAST, speed);
    if (sidfp) sidfp->setSamplingParameters((double)clockrate, reSIDfp::DECIMATE, (double)speed, 20000.0);
    break;

    default:
    if (sid) sid->set_sampling_parameters(clockrate, reSID::SAMPLE_RESAMPLE, speed);
    if (sidfp) sidfp->setSamplingParameters((double)clockrate, reSIDfp::RESAMPLE, (double)speed, 20000.0);
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
    if (sidfp) sidfp->setChipModel(reSIDfp::MOS8580);
  }
  else
  {
    if (sid) sid->set_chip_model(reSID::MOS6581);
    if (sidfp) sidfp->setChipModel(reSIDfp::MOS6581);
  }

  if (sidfp)
  {
    /*
    sidfp->get_filter().set_distortion_properties(
      filterparams.distortionrate,
      filterparams.distortionpoint,
      filterparams.distortioncfthreshold);
    sidfp->get_filter().set_type3_properties(
      filterparams.type3baseresistance,
      filterparams.type3offset,
      filterparams.type3steepness,
      filterparams.type3minimumfetresistance);
    sidfp->get_filter().set_type4_properties(
      filterparams.type4k,
      filterparams.type4b);
    sidfp->set_voice_nonlinearity(
      filterparams.voicenonlinearity);
    */
  }
}

unsigned char sid_getorder(unsigned char index)
{
  if (adparam >= 0xf000)
    return altsidorder[index];
  else
    return sidorder[index];
}

int residfp_fake_result(int n)
{
  int s;

  for (s = 0; s < n; s++) {

  }

  return s;
}

int residfp_fake_clock(int c)
{
    int result = 0;

    switch (c)
    {
        case 1:
        case 2:
        case 4:
        case 5:
        case 7:
        case 8:
        case 10:
        case 11:
        case 13:
        case 14:
        case 16:
        case 17:
        case 19:
        case 20:
        case 21:
        case 23:
        case 24:
            result = 1;
            break;
        default:
            break;
    }

    return result;
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

    if (sid)
    {

        for (c = 0; c < NUMSIDREGS; c++)
        {
            unsigned char o = sid_getorder(c);
            /*
            // Possible random badline delay once per writing
            if ((badline == c) && (residdelay))
            {
              tdelta2 = residdelay;
              if (sid) result = sid->clock(tdelta2, ptr, samples);
              // if (sidfp) result = sidfp->clock(tdelta2, ptr, samples);
              if (sidfp)
              {
                sidfp->clock(tdelta2, ptr);
                result = residfp_fake_result(samples);
                // sidfp->clockSilent(samples);
              }
              total += result;
              ptr += result;
              samples -= result;
              tdelta -= residdelay;
            }
            */
            // printf("o: %04d sidreg[o]: %04d  ", o, sidreg[o]);
            if (sid) sid->write(o, sidreg[o]);
            if (sidfp) sidfp->write(o, sidreg[o]);

            tdelta2 = SIDWRITEDELAY;

            if (sid) result = sid->clock(tdelta2, ptr, samples);
            // if (sidfp) result = sidfp->clock(tdelta2, ptr, samples);
            if (sidfp)
            {
              sidfp->clock(tdelta2, ptr);
              result = residfp_fake_clock(c);
              // sidfp->clockSilent(samples);
            }
            // printf("$D4%02X R0: %d tdelta: %d tdelta2: %d \n", c, result, tdelta, tdelta2);

            total += result;
            ptr += result;
            samples -= result;
            tdelta -= SIDWRITEDELAY;

            if (tdelta <= 0) return total;
        }

        if (sid) result = sid->clock(tdelta, ptr, samples);
        // if (sidfp) result = sidfp->clock(tdelta, ptr, samples);
        if (sidfp)
        {
            sidfp->clock(tdelta, ptr);
            result = samples-1;
            // sidfp->clockSilent(samples);
        }
        // printf("Samples: %d R1: %d\n", samples, result);

        total += result;
        ptr += result;
        samples -= result;

        // Loop extra cycles until all samples produced
        while (samples)
        {
            tdelta = clockrate * samples / samplerate;
            if (tdelta <= 0) return total;

            if (sid) result = sid->clock(tdelta, ptr, samples);
            // if (sidfp) result = sidfp->clock(tdelta, ptr, samples);
            if (sidfp)
            {
              sidfp->clock(tdelta, ptr);
              result = 1;
              // sidfp->clockSilent(samples);
            }
            // if (result > 1) printf("R2: %d\n", result);
            total += result;
            ptr += result;
            samples -= result;
        }

        return total;
    }

    if (sidfp)
    {
        for (c = 0; c < NUMSIDREGS; c++)
        {
            unsigned char o = sid_getorder(c);
            sidfp->write(o, sidreg[o]);

            tdelta2 = SIDWRITEDELAY;
            sidfp->clock(tdelta2, ptr);

            tdelta -= SIDWRITEDELAY;
        }
        sidfp->clock(tdelta, ptr);

        return total;
    }

    return total;
}

