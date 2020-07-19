//
// GOATTRACKER v2 screen display routines
//

#define GDISPLAY_C

#include "goattrk2.h"

char *notename[] =
 {"C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
  "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
  "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
  "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
  "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
  "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
  "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
  "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "...", "---", "+++"};

char timechar[] = {':', ' '};

int timemin = 0;
int timesec = 0;
int timeframe = 0;

void printmainscreen(void)
{
  clearscreen();
  printstatus();
  fliptoscreen();
}

void displayupdate(void)
{
  if (cursorflashdelay >= 6)
  {
    cursorflashdelay %= 6;
    cursorflash++;
    cursorflash &= 3;
  }
  printstatus();
  fliptoscreen();
}

void printstatus(void)
{
  int c, d, color;
  int cc = cursorcolortable[cursorflash];
  int maxpattlen = 0;
  menu = 0;

  if ((mouseb > MOUSEB_LEFT) && (mousey <= 1) && (!eamode)) menu = 1;

  printblankc(0, 0, CSTATUSBAR, MAX_COLUMNS);

  if (!menu)
  {
    if (!strlen(loadedsongfilename))
      sprintf(textbuffer, "%s", programname);
    else
      sprintf(textbuffer, "%s - %s", programname, loadedsongfilename);
    textbuffer[59] = 0;
    printtext(0, 0, CSTATUSBAR, textbuffer);

    if (monomode)
      printtext(38+20, 0, CSTATUSBAR, "M");

    if (usefinevib)
      printtext(40+20, 0, CSTATUSBAR, "FV");

    if (optimizepulse)
      printtext(43+20, 0, CSTATUSBAR, "PO");

    if (optimizerealtime)
      printtext(46+20, 0, CSTATUSBAR, "RO");

    if (ntsc)
      printtext(49+20, 0, CSTATUSBAR, "NTSC");
    else
      printtext(49+20, 0, CSTATUSBAR, " PAL");

    if (!sidmodel)
      printtext(54+20, 0, CSTATUSBAR, "6581");
    else
      printtext(54+20, 0, CSTATUSBAR, "8580");

    sprintf(textbuffer, "HR:%04X", adparam);
    printtext(59+20, 0, CSTATUSBAR, textbuffer);
    if (eamode) printbg(62+20+eacolumn, 0, cc, 1);

    if (multiplier)
    {
      sprintf(textbuffer, "%2dX", multiplier);
      printtext(67+20, 0, CSTATUSBAR, textbuffer);
    }
    else printtext(67+20, 0, CSTATUSBAR, "25Hz");

    printtext(72+20, 0, CSTATUSBAR, "F12=HELP");
  }
  else
  {
    printtext(0, 0, CSTATUSBAR, " PLAY | PLAYPOS | PLAYPATT | STOP | LOAD | SAVE | PACK/RL | HELP | CLEAR | QUIT |");
  }

  if ((followplay) && (isplaying()))
  {
    for (c = 0; c < MAX_CHN; c++)
    {
      int newpos = chn[c].pattptr / 4;
      if (chn[c].advance) epnum[c] = chn[c].pattnum;

      if (newpos > pattlen[epnum[c]]) newpos = pattlen[epnum[c]];

      if (c == epchn)
      {
        eppos = newpos;
        epview = newpos-VISIBLEPATTROWS/2;
      }

      newpos = chn[c].songptr;
      newpos--;
      if (newpos < 0) newpos = 0;
      if (newpos > songlen[esnum][c]) newpos = songlen[esnum][c];

      if ((c == eschn) && (chn[c].advance))
      {
        eseditpos = newpos;
        if (newpos - esview < 0)
        {
          esview = newpos;
        }
        if (newpos - esview >= VISIBLEORDERLIST)
        {
          esview = newpos - VISIBLEORDERLIST + 1;
        }
      }
    }
  }

  for (c = 0; c < MAX_CHN; c++)
  {
    if (pattlen[epnum[c]] > maxpattlen)
      maxpattlen = pattlen[epnum[c]];
  }

  printtext(1, 2, CTITLE, "POS");
  for (d = 0; d < VISIBLEPATTROWS; d++)
  {
    int p = epview+d;
    int tpx = 1;
    color = CINDEXES;
    if (!(p%stepsize)) color = CINDEXESINV;

    if ((p < 0) || (p > maxpattlen))
    {
      sprintf(textbuffer, "   ");
      color = CINDEXES;
    }
    else
    {
      int dp = p;

      if (!(patterndispmode & 1))
      {
        if (dp < 100)
          sprintf(textbuffer, " %02d", dp);
        else
          sprintf(textbuffer, "%03d", dp);
      }
      else
      {
        sprintf(textbuffer, "%02X", dp);
        tpx = 2;
      }
    }
    printtext(tpx, 3+d, color, textbuffer);
  }

  for (c = 0; c < MAX_CHN; c++)
  {
    sprintf(textbuffer, "CH%d PT", c+1);
    printtext(5+c*9, 2, CTITLE, textbuffer);
    sprintf(textbuffer, "%02X", epnum[c]);
    printtext(11+c*9, 2, CEDIT, textbuffer);

    for (d = 0; d < VISIBLEPATTROWS; d++)
    {
      int p = epview+d;
      color = CNORMAL;
      if ((epnum[c] == chn[c].pattnum) && (isplaying()))
      {
        int chnrow = chn[c].pattptr / 4;
        if (chnrow > pattlen[chn[c].pattnum]) chnrow = pattlen[chn[c].pattnum];
        // if (chnrow == p) color = CPLAYING;
        if (chnrow == p) color = CPLAYINGINV;
      }

      if (chn[c].mute) color = CMUTE;
      if (p == eppos) color = CEDIT;
      //
      if ((p == eppos) && (followplay)) color = CPLAYINGINV;
      //
      if ((p < 0) || (p > pattlen[epnum[c]]))
      {
        sprintf(textbuffer, "        ");
        printtext(5+c*9, 3+d, color, textbuffer);
      }
      else
      {
        if (pattern[epnum[c]][p*4] == ENDPATT)
        {
          sprintf(textbuffer, "PATT.END");
          if (color == CNORMAL) color = CTBLEND;
          printtext(5+c*9, 3+d, color, textbuffer);
        }
        else
        {
          sprintf(textbuffer, "%s", notename[pattern[epnum[c]][p*4]-FIRSTNOTE]);
          printtext(5+c*9, 3+d, color, textbuffer);

          int tmpc;
          if ( pattern[epnum[c]][p*4+1] )
          {
            sprintf(textbuffer, "%02X", pattern[epnum[c]][p*4+1]);
          }
          else
          {
            sprintf(textbuffer, "..");
          }
          tmpc = ( color == CMUTE ) || ( color == CEDIT ) || ( color == CTBLEND ) || ( color == CPLAYINGINV ) ? color : CINSTRNUM;
          printtext(5+3+c*9, 3+d, tmpc, textbuffer);
          if ( pattern[epnum[c]][p*4+2] )
          {
            sprintf(textbuffer, "%01X", pattern[epnum[c]][p*4+2]);
            tmpc = ( color == CMUTE ) || ( color == CEDIT ) || ( color == CTBLEND ) || ( color == CPLAYINGINV ) ? color : CCOMMAND;
            printtext(5+5+c*9, 3+d, tmpc, textbuffer);
            sprintf(textbuffer, "%02X", pattern[epnum[c]][p*4+3]);
            tmpc = ( color == CMUTE ) || ( color == CEDIT ) || ( color == CTBLEND ) || ( color == CPLAYINGINV ) ? color : CCOMMANDVAL;
            printtext(6+5+c*9, 3+d, tmpc, textbuffer);
          }
          else
          {
            sprintf(textbuffer, ".");
            tmpc = ( color == CMUTE ) || ( color == CEDIT ) || ( color == CTBLEND ) || ( color == CPLAYINGINV ) ? color : CCOMMAND;
            printtext(5+5+c*9, 3+d, tmpc, textbuffer);
            sprintf(textbuffer, "..");
            tmpc = ( color == CMUTE ) || ( color == CEDIT ) || ( color == CTBLEND ) || ( color == CPLAYINGINV ) ? color : CCOMMANDVAL;
            printtext(6+5+c*9, 3+d, tmpc, textbuffer);
          }
        }
      }

      if (c == epmarkchn)
      {
        if (epmarkstart <= epmarkend)
        {
          if ((p >= epmarkstart) && (p <= epmarkend))
            printbg(5+c*9, 3+d, CMARKBG, 8);
        }
        else
        {
          if ((p <= epmarkstart) && (p >= epmarkend))
            printbg(5+c*9, 3+d, CMARKBG, 8);
        }
      }
      if ((color == CEDIT) && (editmode == EDIT_PATTERN) && (epchn == c))
      {
        switch(epcolumn)
        {
          case 0:
          if (!eamode) printbg(5+c*9, 3+d, cc, 3);
          break;

          default:
          if (!eamode) printbg(5+c*9+2+epcolumn, 3+d, cc, 1);
          break;
        }
      }
    }
  }

  sprintf(textbuffer, "CHN ORDERLIST (SUBTUNE %02X, POS %02X)", esnum, eseditpos);
  printtext(40+20, 2, CTITLE, textbuffer);
  for (c = 0; c < MAX_CHN; c++)
  {
    sprintf(textbuffer, " %d ", c+1);
    printtext(40+20, 3+c, CTITLE, textbuffer);
    for (d = 0; d < VISIBLEORDERLIST; d++)
    {
      int p = esview+d;
      color = CNORMAL;
      if (isplaying())
      {
        int chnpos = chn[c].songptr;
        chnpos--;
        if (chnpos < 0) chnpos = 0;
        if ((p == chnpos) && (chn[c].advance)) color = CPLAYING;
      }
      if (p == espos[c]) color = CEDIT;
      if ((esend[c]) && (p == esend[c])) color = CEDIT;

      if ((p < 0) || (p > (songlen[esnum][c]+1)) || (p > MAX_SONGLEN+1))
      {
        sprintf(textbuffer, "   ");
      }
      else
      {
        if (songorder[esnum][c][p] < LOOPSONG)
        {
          if ((songorder[esnum][c][p] < REPEAT) || (p >= songlen[esnum][c]))
          {
            sprintf(textbuffer, "%02X ", songorder[esnum][c][p]);
            if ((p >= songlen[esnum][c]) && (color == CNORMAL)) color = CCOMMAND;
          }
          else
          {
            if (songorder[esnum][c][p] >= TRANSUP)
            {
              sprintf(textbuffer, "+%01X ", songorder[esnum][c][p]&0xf);
              if (color == CNORMAL) color = CCOMMAND;
            }
            else
            {
              if (songorder[esnum][c][p] >= TRANSDOWN)
              {
                sprintf(textbuffer, "-%01X ", 16-(songorder[esnum][c][p] & 0x0f));
                if (color == CNORMAL) color = CCOMMAND;
              }
              else
              {
                sprintf(textbuffer, "R%01X ", (songorder[esnum][c][p]+1) & 0x0f);
                if (color == CNORMAL) color = CCOMMAND;
              }
            }
          }
        }
        if (songorder[esnum][c][p] == LOOPSONG)
        {
          sprintf(textbuffer, "RST");
          if (color == CNORMAL) color = CCOMMAND;
        }
      }
      printtext(44+20+d*3, 3+c, color, textbuffer);
      if (c == esmarkchn)
      {
        if (esmarkstart <= esmarkend)
        {
          if ((p >= esmarkstart) && (p <= esmarkend))
          {
            if (p != esmarkend)
              printbg(44+20+d*3, 3+c, CMARKBG, 3);
            else
              printbg(44+20+d*3, 3+c, CMARKBG, 2);
          }
        }
        else
        {
          if ((p <= esmarkstart) && (p >= esmarkend))
          {
            if (p != esmarkstart)
              printbg(44+20+d*3, 3+c, CMARKBG, 3);
            else
              printbg(44+20+d*3, 3+c, CMARKBG, 2);
          }
        }
      }
      if ((p == eseditpos) && (editmode == EDIT_ORDERLIST) && (eschn == c))
      {
        if (!eamode) printbg(44+20+d*3+escolumn, 3+c, cc, 1);
      }
    }
  }

  sprintf(textbuffer, "INSTRUMENT NUM. %02X  %-16s", einum, instr[einum].name);
  printtext(40+20, 7+3, CTITLE, textbuffer);

  sprintf(textbuffer, "Attack/Decay");
  if (eipos == 0) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(40+20, 8+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].ad);
  if (eipos == 0) color = CNORMAL; else color = CPLAYING;
  printtext(40+20+16, 8+3, color, textbuffer);

  sprintf(textbuffer, "Sustain/Release");
  if (eipos == 1) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(40+20, 9+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].sr);
  if (eipos == 1) color = CNORMAL; else color = CPLAYING;
  printtext(40+36, 9+3, color, textbuffer);

  sprintf(textbuffer, "Wavetable Pos");
  if (eipos == 2) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(40+20, 10+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].ptr[WTBL]);
  if (eipos == 2) color = CNORMAL; else color = CPLAYING;
  printtext(40+36, 10+3, color, textbuffer);

  sprintf(textbuffer, "Pulsetable Pos");
  if (eipos == 3) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(40+20, 11+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].ptr[PTBL]);
  if (eipos == 3) color = CNORMAL; else color = CPLAYING;
  printtext(40+36, 11+3, color, textbuffer);

  sprintf(textbuffer, "Filtertable Pos");
  if (eipos == 4) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(40+20, 12+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].ptr[FTBL]);
  if (eipos == 4) color = CNORMAL; else color = CPLAYING;
  printtext(40+36, 12+3, color, textbuffer);

  sprintf(textbuffer, "Vibrato Param");
  if (eipos == 5) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(60+20, 8+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].ptr[STBL]);
  if (eipos == 5) color = CNORMAL; else color = CPLAYING;
  printtext(60+36, 8+3, color, textbuffer);

  sprintf(textbuffer, "Vibrato Delay");
  if (eipos == 6) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(60+20, 9+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].vibdelay);
  if (eipos == 6) color = CNORMAL; else color = CPLAYING;
  printtext(60+36, 9+3, color, textbuffer);

  sprintf(textbuffer, "HR/Gate Timer");
  if (eipos == 7) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(60+20, 10+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].gatetimer);
  if (eipos == 7) color = CNORMAL; else color = CPLAYING;
  printtext(60+36, 10+3, color, textbuffer);

  sprintf(textbuffer, "1stFrame Wave");
  if (eipos == 8) color = CEDIT; else color = CNORMAL & 0xF7;
  printtext(60+20, 11+3, color, textbuffer);

  sprintf(textbuffer, "%02X", instr[einum].firstwave);
  if (eipos == 8) color = CNORMAL; else color = CPLAYING;
  printtext(60+36, 11+3, color, textbuffer);

  if (editmode == EDIT_INSTRUMENT)
  {
    if (eipos < 9)
    {
      if (!eamode) printbg(56+20+eicolumn+20*(eipos/5), 8+3+(eipos%5), cc, 1);
    }
    else
    {
      if (!eamode) printbg(60+20+strlen(instr[einum].name), 7+3, cc, 1);
    }
  }

  sprintf(textbuffer, "WAVE TBL  PULSETBL  FILT.TBL  SPEEDTBL");
  printtext(40+20, 14+3, CTITLE, textbuffer);

  for (c = 0; c < MAX_TABLES; c++)
  {
    for (d = 0; d < VISIBLETABLEROWS; d++)
    {
      int p = etview[c]+d;

      color = CNORMAL;
      switch (c)
      {
        case WTBL:
        // if (ltable[c][p] >= WAVECMD) color = CCOMMAND;
        if (ltable[c][p] >= WAVECMD) color = CTBLMODVAL;
        if (ltable[c][p] <= 0x10) color = CTBLMODVAL;
        break;

        case PTBL:
        // if (ltable[c][p] >= 0x80) color = CCOMMAND;
        if (ltable[c][p] >= 0x80) color = CNORMAL;
        else color = CTBLMODVAL;
        break;

        case FTBL:
        // if ((ltable[c][p] >= 0x80) || ((!ltable[c][p]) && (rtable[c][p]))) color = CCOMMAND;
        if ((ltable[c][p] >= 0x80) || ((!ltable[c][p]) && (rtable[c][p]))) color = CNORMAL;
        else color = CTBLMODVAL;
        break;
      }
      // added
      if (ltable[c][p] == 0xFF) color = CTBLEND;
      // end
      if ((p == etpos) && (etnum == c)) color = CEDIT;
      sprintf(textbuffer, "%02X:", p+1);
      // @TODO
      printtext(40+20+10*c, 15+3+d, CINDEXES, textbuffer);
      // allow single 00 00 columns:
      if ( ltable[c][p] || rtable[c][p] || ltable[c][p+1] || rtable[c][p+1] )
      {
        sprintf(textbuffer, "%02X %02X", ltable[c][p], rtable[c][p]);
      }
      else
      {
        sprintf(textbuffer, ".. ..");
      }
      printtext(40+20+10*c+3, 15+3+d, color, textbuffer);

      if (etmarknum == c)
      {
        if (etmarkstart <= etmarkend)
        {
          if ((p >= etmarkstart) && (p <= etmarkend))
            printbg(40+20+10*c+3, 15+3+d, CMARKBG, 5);
        }
        else
        {
          if ((p <= etmarkstart) && (p >= etmarkend))
            printbg(40+20+10*c+3, 15+3+d, CMARKBG, 5);
        }
      }
    }
  }

  if (editmode == EDIT_TABLES)
  {
    if (!eamode) printbg(43+20+etnum*10+(etcolumn & 1)+(etcolumn/2)*3, 15+3+etpos-etview[etnum], cc, 1);
  }

  if (enpos == 0)
  {
    printtext(40+20, 21+3+9, CTITLE, "NAME   ");
    sprintf(textbuffer, "%-32s", songname);
    printtext(47+20, 21+3+9, CEDIT, textbuffer);
  }

  if (enpos == 1)
  {
    printtext(40+20, 21+3+9, CTITLE, "AUTHOR ");
    sprintf(textbuffer, "%-32s", authorname);
    printtext(47+20, 21+3+9, CEDIT, textbuffer);
  }

  if (enpos == 2)
  {
    printtext(40+20, 21+3+9, CTITLE, "COPYR. ");
    sprintf(textbuffer, "%-32s", copyrightname);
    printtext(47+20, 21+3+9, CEDIT, textbuffer);
  }

  if ((editmode == EDIT_NAMES) && (!eamode))
  {
    switch(enpos)
    {
      case 0:
      printbg(47+20+strlen(songname), 21+3+9, cc, 1);
      break;
      case 1:
      printbg(47+20+strlen(authorname), 21+3+9, cc, 1);
      break;
      case 2:
      printbg(47+20+strlen(copyrightname), 21+3+9, cc, 1);
      break;
    }
  }
  sprintf(textbuffer, "OCTAVE");
  printtext(0, 35, CTITLE, textbuffer);

  sprintf(textbuffer, "%d", epoctave);
  printtext(0+7, 35, CPLAYING, textbuffer);

  switch(autoadvance)
  {
    // @TODO: what does this do?
    case 0:
    color = ( color & 0xF0 ) + 10;
    break;

    case 1:
    color = ( color & 0xF0 ) + 14;
    break;

    case 2:
    color = ( color & 0xF0 ) + 12;
    break;
  }

  if (recordmode) printtext(0, 36, color, "EDITMODE");
  else printtext(0, 36, color, "JAM MODE");

  if (isplaying()) printtext(10, 35, CTITLE, "PLAYING");
  else printtext(10, 35, CTITLE, "STOPPED");
  if (multiplier)
  {
    if (!ntsc)
      sprintf(textbuffer, " %02d%c%02d ", timemin, timechar[timeframe/(25*multiplier) & 1], timesec);
    else
      sprintf(textbuffer, " %02d%c%02d ", timemin, timechar[timeframe/(30*multiplier) & 1], timesec);
  }
  else
  {
    if (!ntsc)
      sprintf(textbuffer, " %02d%c%02d ", timemin, timechar[(timeframe/13) & 1], timesec);
    else
      sprintf(textbuffer, " %02d%c%02d ", timemin, timechar[(timeframe/15) & 1], timesec);
  }

  printtext(10, 36, CEDIT, textbuffer);

  printtext(59, 35, CTITLE, " CHN1   CHN2   CHN3   CHN4   CHN5   CHN6");
  for (c = 0; c < MAX_CHN; c++)
  {
    int chnpos = chn[c].songptr;
    int chnrow = chn[c].pattptr/4;
    chnpos--;
    if (chnpos < 0) chnpos = 0;
    if (chnrow > pattlen[chn[c].pattnum]) chnrow = pattlen[chn[c].pattnum];
    if (chnrow >= 100) chnrow -= 100;

    sprintf(textbuffer, "%03d/%02d",
      chnpos,chnrow);
    printtext(59+7*c, 36, CEDIT, textbuffer);
  }

  if (etlock) printtext(57, 36, CTITLE, " ");
    else printtext(57, 36, CTITLE, "U");
}

void resettime(void)
{
  timemin = 0;
  timesec = 0;
  timeframe = 0;
}

void incrementtime(void)
{
  {
    timeframe++;
    if (!ntsc)
    {
      if (((multiplier) && (timeframe >= PALFRAMERATE*multiplier))
          || ((!multiplier) && (timeframe >= PALFRAMERATE/2)))
      {
        timeframe = 0;
        timesec++;
      }
    }
    else
    {
      if (((multiplier) && (timeframe >= NTSCFRAMERATE*multiplier))
          || ((!multiplier) && (timeframe >= NTSCFRAMERATE/2)))
      {
        timeframe = 0;
        timesec++;
      }
    }
    if (timesec == 60)
    {
      timesec = 0;
      timemin++;
      timemin %= 60;
    }
  }
}

