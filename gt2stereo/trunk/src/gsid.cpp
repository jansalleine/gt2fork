/*
 * GOATTRACKER reSID interface
 */

#define GSID_C

#include <stdlib.h>
#include "resid/sid.h"
#include "resid-fp/sidfp.h"

#include "gsid.h"
#include "gsound.h"

int clockrate;
int samplerate;
unsigned char sidreg[NUMSIDREGS];
unsigned char sidreg2[NUMSIDREGS];

unsigned char sidorder[] =
  {0x15,0x16,0x18,0x17,
   0x05,0x06,0x02,0x03,0x00,0x01,0x04,
   0x0c,0x0d,0x09,0x0a,0x07,0x08,0x0b,
   0x13,0x14,0x10,0x11,0x0e,0x0f,0x12};

unsigned char altsidorder[] =
  {0x15,0x16,0x18,0x17,
   0x04,0x00,0x01,0x02,0x03,0x05,0x06,
   0x0b,0x07,0x08,0x09,0x0a,0x0c,0x0d,
   0x12,0x0e,0x0f,0x10,0x11,0x13,0x14};

FILTERPARAMS filterparams =
  {0.50f, 3.3e6f, 1.0e-4f,
   1147036.4394268463f, 274228796.97550374f, 1.0066634233403395f, 16125.154840564108f,
   5.5f, 20.f,
   0.9613160610660189f};

SID *sid = 0;
SID *sid2 = 0;
SIDFP *sidfp = 0;
SIDFP *sidfp2 = 0;

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
    if (!sid) sid = new SID;
    if (!sid2) sid2 = new SID;
    
    if (sidfp) 
    { 
      delete sidfp; 
      sidfp = NULL;
    }
    if (sidfp2)
    { 
      delete sidfp2;
      sidfp = NULL;
    }
  }
  else
  {
    if (!sidfp) sidfp = new SIDFP;
    if (!sidfp2) sidfp2 = new SIDFP;
    
    if (sid)
    {
      delete sid;
      sid = NULL;
    }
    if (sid2)
    {
      delete sid2;
      sid2 = NULL;
    }
  }

  switch(interpolate)
  {
    case 0:
    if (sid) sid->set_sampling_parameters(clockrate, SAMPLE_FAST, speed);
    if (sid2) sid2->set_sampling_parameters(clockrate, SAMPLE_FAST, speed);
    if (sidfp) sidfp->set_sampling_parameters(clockrate, SAMPLE_INTERPOLATE, speed);
    if (sidfp2) sidfp2->set_sampling_parameters(clockrate, SAMPLE_INTERPOLATE, speed);
    break;

    default:
    if (sid) sid->set_sampling_parameters(clockrate, SAMPLE_INTERPOLATE, speed);
    if (sid2) sid2->set_sampling_parameters(clockrate, SAMPLE_INTERPOLATE, speed);
    if (sidfp) sidfp->set_sampling_parameters(clockrate, SAMPLE_RESAMPLE_INTERPOLATE, speed);
    if (sidfp2) sidfp2->set_sampling_parameters(clockrate, SAMPLE_RESAMPLE_INTERPOLATE, speed);
    break;
  }

  if (sid) sid->reset();
  if (sid2) sid2->reset();
  if (sidfp) sidfp->reset();
  if (sidfp2) sidfp2->reset();

  for (c = 0; c < NUMSIDREGS; c++)
  {
    sidreg[c] = 0x00;
    sidreg2[c] = 0x00;
  }
  if (m == 1)
  {
    if (sid) sid->set_chip_model(MOS8580);
    if (sid2) sid2->set_chip_model(MOS8580);
    if (sidfp) sidfp->set_chip_model(MOS8580);
    if (sidfp2) sidfp2->set_chip_model(MOS8580);
  }
  else
  {
    if (sid) sid->set_chip_model(MOS6581);
    if (sid2) sid2->set_chip_model(MOS6581);
    if (sidfp) sidfp->set_chip_model(MOS6581);
    if (sidfp2) sidfp2->set_chip_model(MOS6581);
  }
  
  if (sidfp)
  {
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
  }
  
  if (sidfp2)
  {
    sidfp2->get_filter().set_distortion_properties(
      filterparams.distortionrate,
      filterparams.distortionpoint,
      filterparams.distortioncfthreshold);
    sidfp2->get_filter().set_type3_properties(
      filterparams.type3baseresistance,
      filterparams.type3offset,
      filterparams.type3steepness,
      filterparams.type3minimumfetresistance);
    sidfp2->get_filter().set_type4_properties(
      filterparams.type4k,
      filterparams.type4b);
    sidfp2->set_voice_nonlinearity(
      filterparams.voicenonlinearity);
  }
}

unsigned char sid_getorder(unsigned char index)
{
  if (adparam >= 0xf000)
    return altsidorder[index];
  else
    return sidorder[index];
}

int sid_fillbuffer(short *lptr, short *rptr, int samples)
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

  	// Extra delay for loading the waveform (and mt_chngate,x)
  	if ((o == 4) || (o == 11) || (o == 18))
  	{
  	  tdelta2 = SIDWAVEDELAY;
      if (sid) result = sid->clock(tdelta2, lptr, samples);
      if (sidfp) result = sidfp->clock(tdelta2, lptr, samples);
  	  tdelta2 = SIDWAVEDELAY;
      if (sid2) sid2->clock(tdelta2, rptr, samples);
      if (sidfp2) sidfp2->clock(tdelta2, rptr, samples);

      total += result;
      lptr += result;
      rptr += result;
      samples -= result;
      tdelta -= SIDWAVEDELAY;
    }

    // Possible random badline delay once per writing
    if ((badline == c) && (residdelay))
  	{
      tdelta2 = residdelay;
      if (sid) result = sid->clock(tdelta2, lptr, samples);
      if (sidfp) result = sidfp->clock(tdelta2, lptr, samples);
      tdelta2 = residdelay;
      if (sid2) sid2->clock(tdelta2, rptr, samples);
      if (sidfp2) sidfp2->clock(tdelta2, rptr, samples);

      total += result;
      lptr += result;
      rptr += result;
      samples -= result;
      tdelta -= residdelay;
    }

    if (sid) sid->write(o, sidreg[o]);
    if (sidfp) sidfp->write(o, sidreg[o]);
    if (sid2) sid2->write(o, sidreg2[o]);
    if (sidfp2) sidfp2->write(o, sidreg2[o]);

    tdelta2 = SIDWRITEDELAY;
    if (sid) result = sid->clock(tdelta2, lptr, samples);
    if (sidfp) result = sidfp->clock(tdelta2, lptr, samples);
    tdelta2 = SIDWRITEDELAY;
    if (sid2) sid2->clock(tdelta2, rptr, samples);
    if (sidfp2) sidfp2->clock(tdelta2, rptr, samples);

    total += result;
    lptr += result;
    rptr += result;
    samples -= result;
    tdelta -= SIDWRITEDELAY;

    if (tdelta <= 0) return total;
  }

  tdelta2 = tdelta;
  if (sid) result = sid->clock(tdelta2, lptr, samples);
  if (sidfp) result = sidfp->clock(tdelta2, lptr, samples);
  tdelta2 = tdelta;
  if (sid2) result = sid2->clock(tdelta2, rptr, samples);
  if (sidfp2) result = sidfp2->clock(tdelta2, rptr, samples);

  total += result;
  lptr += result;
  rptr += result;
  samples -= result;

  // Loop extra cycles until all samples produced
  while (samples)
  {
    tdelta = clockrate * samples / samplerate;
    if (tdelta <= 0) return total;
        
    if (sid) result = sid->clock(tdelta, lptr, samples);
    if (sidfp) result = sidfp->clock(tdelta, lptr, samples);
    tdelta = clockrate * samples / samplerate;
    if (sid2) result = sid2->clock(tdelta, rptr, samples);
    if (sidfp2) result = sidfp2->clock(tdelta, rptr, samples);
    total += result;
    lptr += result;
    rptr += result;
    samples -= result;
  }

  return total;
}
