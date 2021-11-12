/**
 * =============================================================================
 * screen display routines
 * =============================================================================
 */

#define GT2F_DISPLAY_C

#include "gt2fork.h"

char *notename[] =
{   "C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
    "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
    "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
    "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
    "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
    "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
    "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
    "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "...", "---", "+++"
};

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
    int visibleOrderlist = 14;
    int maxChns = MAX_CHN;
    if (numsids == 1)
    {
        maxChns = 3;
        visibleOrderlist = VISIBLEORDERLIST;
    }
    menu = 0;

    if ((mouseb > MOUSEB_LEFT) && (mousey <= 1) && (!eamode)) menu = 1;

    printblankc(0, 0, colscheme.statusTop, MAX_COLUMNS);

    if (!menu)
    {
        if (!strlen(loadedsongfilename))
            sprintf(textbuffer, "%s", programname);
        else
            sprintf(textbuffer, "%s - %s", programname, loadedsongfilename);
        textbuffer[49] = 0;
        printtext(dpos.statusTopX+1, dpos.statusTopY, colscheme.statusTop, textbuffer);

        if ((numsids == 2) && monomode)
            printtext(dpos.statusTopFvX-2, dpos.statusTopY, colscheme.statusTop, "M");

        if (usefinevib)
            printtext(dpos.statusTopFvX, dpos.statusTopY, colscheme.statusTop, "FV");

        if (optimizepulse)
            printtext(dpos.statusTopFvX+3, dpos.statusTopY, colscheme.statusTop, "PO");

        if (optimizerealtime)
            printtext(dpos.statusTopFvX+6, dpos.statusTopY, colscheme.statusTop, "RO");

        if (ntsc)
            printtext(dpos.statusTopFvX+9, dpos.statusTopY, colscheme.statusTop, "NTSC");
        else
            printtext(dpos.statusTopFvX+9, dpos.statusTopY, colscheme.statusTop, " PAL");

        if (!sidmodel)
            printtext(dpos.statusTopFvX+14, dpos.statusTopY, colscheme.statusTop, "6581");
        else
            printtext(dpos.statusTopFvX+14, dpos.statusTopY, colscheme.statusTop, "8580");

        sprintf(textbuffer, "HR:%04X", adparam);
        printtext(dpos.statusTopFvX+19, dpos.statusTopY, colscheme.statusTop, textbuffer);
        if (eamode && !ebmode) printbg(dpos.statusTopFvX+22+eacolumn, dpos.statusTopY, cc, 1);

        if (multiplier)
        {
            sprintf(textbuffer, "%2dX", multiplier);
            printtext(dpos.statusTopFvX+27, dpos.statusTopY, colscheme.statusTop, textbuffer);
        }
        else printtext(dpos.statusTopFvX+27, dpos.statusTopY, colscheme.statusTop, "25Hz");

        if (multiplier == 1)
        {
            sprintf(textbuffer, "%03dBPM", snd_bpmtempo);
            printtext(dpos.statusTopFvX+31, dpos.statusTopY, colscheme.statusTop, textbuffer);

            if (eamode && ebmode) printbg(dpos.statusTopFvX+31+eacolumn, dpos.statusTopY, cc, 1);
        }

        printtext(dpos.statusTopEndX-8, dpos.statusTopY, colscheme.statusTop, "F12=HELP");
    }
    else
    {
        printtext(0, dpos.statusTopY, colscheme.statusTop, " PLAY | PLAYPOS | PLAYPATT | STOP | LOAD | SAVE | PACK/RL | HELP | CLEAR | QUIT |");
    }

    if ((followplay) && (isplaying()))
    {
        for (c = 0; c < maxChns; c++)
        {
            int currentSonglen = 0;
            if (numsids == 1)
            {
                currentSonglen = songlen[esnum][c];
            }
            else if (numsids == 2)
            {
                currentSonglen = songlen_stereo[esnum][c];
            }
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
            if (newpos > currentSonglen) newpos = currentSonglen;

            if ((c == eschn) && (chn[c].advance))
            {
                eseditpos = newpos;
                if (newpos - esview < 0)
                {
                    esview = newpos;
                }
                if (newpos - esview >= visibleOrderlist)
                {
                    esview = newpos - visibleOrderlist + 1;
                }
            }
        }
    }

    for (c = 0; c < maxChns; c++)
    {
        sprintf(textbuffer, "CH.");
        printtext(dpos.patternsX+c*13, dpos.patternsY, colscheme.pattcol, textbuffer);
        sprintf(textbuffer, "%d", c+1);
        printtext(dpos.patternsX+3+c*13, dpos.patternsY, colscheme.edit, textbuffer);
        sprintf(textbuffer, "PATT.");
        printtext(dpos.patternsX+5+c*13, dpos.patternsY, colscheme.pattcol, textbuffer);
        sprintf(textbuffer, "%02X", epnum[c]);
        printtext(dpos.patternsX+10+c*13, dpos.patternsY, colscheme.edit, textbuffer);

        for (d = 0; d < VISIBLEPATTROWS; d++)
        {
            int p = epview+d;
            color = colscheme.normal;
            if ((epnum[c] == chn[c].pattnum) && (isplaying()))
            {
                int chnrow = chn[c].pattptr / 4;
                if (chnrow > pattlen[chn[c].pattnum]) chnrow = pattlen[chn[c].pattnum];

                if (chnrow == p) color = colscheme.playingInv;
            }

            if (chn[c].mute) color = colscheme.mute;
            if (p == eppos) color = colscheme.edit;
            //
            if ((p == eppos) && (followplay)) color = colscheme.playingInv;
            //
            if ((p < 0) || (p > pattlen[epnum[c]]))
            {
                sprintf(textbuffer, "             ");
            }
            else
            {
                if (!(patterndispmode & 1))
                {
                    if (p < 100)
                        sprintf(textbuffer, " %02d", p);
                    else
                        sprintf(textbuffer, "%03d", p);
                }
                else
                    sprintf(textbuffer, " %02X", p);

                if (pattern[epnum[c]][p*4] == ENDPATT)
                {
                    sprintf(&textbuffer[3], " PATT. END");

                    if (color == colscheme.normal) color = colscheme.tableEnd;
                }
                else
                {
                    sprintf(&textbuffer[3], " %s ",
                            notename[pattern[epnum[c]][p*4]-FIRSTNOTE]);
                    if ( pattern[epnum[c]][p*4+1] )
                    {
                        sprintf(&textbuffer[8], "%02X", pattern[epnum[c]][p*4+1]);
                    }
                    else
                    {
                        sprintf(&textbuffer[8], "..");
                    }
                    if ( pattern[epnum[c]][p*4+2] )
                    {
                        sprintf(&textbuffer[10], "%01X%02X", pattern[epnum[c]][p*4+2], pattern[epnum[c]][p*4+3]);
                    }
                    else
                    {
                        sprintf(&textbuffer[10], "...");
                    }
                }
            }
            textbuffer[3] = 0;

            if (!(p%stepsize) && !((p < 0) || (p > pattlen[epnum[c]])))
            {
                if (patterndispmode)
                {
                    printtext(dpos.patternsX+c*13, dpos.patternsY+1+d, colscheme.indexesInv, &textbuffer[1]);
                }
                else
                {
                    printtext(dpos.patternsX-1+c*13, dpos.patternsY+1+d, colscheme.indexesInv, textbuffer);
                }
            }
            else
            {
                printtext(dpos.patternsX-1+c*13, dpos.patternsY+1+d, colscheme.indexes, textbuffer);
            }
            int tmp, tmpc;
            tmp = textbuffer[8];
            textbuffer[8] = 0;
            printtext(dpos.patternsX+3+c*13, dpos.patternsY+1+d, color, &textbuffer[4]);
            textbuffer[8] = tmp;
            tmp = textbuffer[10];
            textbuffer[10] = 0;
            tmpc = ( color == colscheme.mute ) || ( color == colscheme.edit ) || ( color == colscheme.tableEnd ) || ( color == colscheme.playingInv ) ? color : colscheme.instrnum;
            printtext(dpos.patternsX+7+c*13, dpos.patternsY+1+d, tmpc, &textbuffer[8]);
            textbuffer[10] = tmp;
            tmp = textbuffer[11];
            textbuffer[11] = 0;
            tmpc = ( color == colscheme.mute ) || ( color == colscheme.edit ) || ( color == colscheme.tableEnd ) || ( color == colscheme.playingInv ) ? color : colscheme.command;
            printtext(dpos.patternsX+9+c*13, dpos.patternsY+1+d, tmpc, &textbuffer[10]);
            textbuffer[11] = tmp;
            tmpc = ( color == colscheme.mute ) || ( color == colscheme.edit ) || ( color == colscheme.tableEnd ) || ( color == colscheme.playingInv ) ? color : colscheme.commandValue;
            printtext(dpos.patternsX+10+c*13, dpos.patternsY+1+d, tmpc, &textbuffer[11]);

            /* ====================================== */

            if (c == epmarkchn)
            {
                if (epmarkstart <= epmarkend)
                {
                    if ((p >= epmarkstart) && (p <= epmarkend))
                        printbg(dpos.patternsX+c*13+3, dpos.patternsY+1+d, colscheme.markBg, 9);
                }
                else
                {
                    if ((p <= epmarkstart) && (p >= epmarkend))
                        printbg(dpos.patternsX+c*13+3, dpos.patternsY+1+d, colscheme.markBg, 9);
                }
            }
            if ((color == colscheme.edit) && (editmode == EDIT_PATTERN) && (epchn == c))
            {
                switch(epcolumn)
                {
                case 0:
                    if (!eamode) printbg(dpos.patternsX+c*13+3, dpos.patternsY+1+d, cc, 3);
                    break;

                default:
                    if (!eamode) printbg(dpos.patternsX+c*13+6+epcolumn, dpos.patternsY+1+d, cc, 1);
                    break;
                }
            }
        }
    }

    sprintf(textbuffer, "CHN ORDERLIST (SUBTUNE ");
    printtext(dpos.orderlistX, dpos.orderlistY, colscheme.title, textbuffer);
    sprintf(textbuffer, "%02X", esnum);
    printtext(dpos.orderlistX+23, dpos.orderlistY, colscheme.edit, textbuffer);
    sprintf(textbuffer, ", POS ");
    printtext(dpos.orderlistX+25, dpos.orderlistY, colscheme.title, textbuffer);
    sprintf(textbuffer, "%02X", eseditpos);
    printtext(dpos.orderlistX+31, dpos.orderlistY, colscheme.edit, textbuffer);
    sprintf(textbuffer, ")");
    printtext(dpos.orderlistX+33, dpos.orderlistY, colscheme.title, textbuffer);

    for (c = 0; c < maxChns; c++)
    {
        sprintf(textbuffer, " %d ", c+1);
        printtext(dpos.orderlistX, dpos.orderlistY+1+c, colscheme.title, textbuffer);
        for ( d = 0; d < visibleOrderlist; d++ )
        {
            int p = esview+d;
            unsigned char currentSongorder = 0;
            int currentSonglen = 0;
            if (numsids == 1)
            {
                currentSongorder = songorder[esnum][c][p];
                currentSonglen = songlen[esnum][c];
            }
            else if (numsids == 2)
            {
                currentSongorder = songorder_stereo[esnum][c][p];
                currentSonglen = songlen_stereo[esnum][c];
            }
            color = colscheme.normal;
            if (isplaying())
            {
                int chnpos = chn[c].songptr;
                chnpos--;
                if (chnpos < 0) chnpos = 0;
                if ((p == chnpos) && (chn[c].advance)) color = colscheme.playing;
            }
            if (p == espos[c]) color = colscheme.edit;
            if ((esend[c]) && (p == esend[c])) color = colscheme.edit;

            if ((p < 0) || (p > (currentSonglen+1)) || (p > MAX_SONGLEN+1))
            {
                sprintf(textbuffer, "   ");
            }
            else
            {
                if (currentSongorder < LOOPSONG)
                {
                    if ((currentSongorder < REPEAT) || (p >= currentSonglen))
                    {
                        sprintf(textbuffer, "%02X ", currentSongorder);
                        if ((p >= currentSonglen) && (color == colscheme.normal)) color = colscheme.command;
                    }
                    else
                    {
                        if (currentSongorder >= TRANSUP)
                        {
                            sprintf(textbuffer, "+%01X ", currentSongorder & 0xf);
                            if (color == colscheme.normal) color = colscheme.command;
                        }
                        else
                        {
                            if (currentSongorder >= TRANSDOWN)
                            {
                                sprintf(textbuffer, "-%01X ", 16-(currentSongorder & 0x0f));
                                if (color == colscheme.normal) color = colscheme.command;
                            }
                            else
                            {
                                sprintf(textbuffer, "R%01X ", (currentSongorder+1) & 0x0f);
                                if (color == colscheme.normal) color = colscheme.command;
                            }
                        }
                    }
                }
                if (currentSongorder == LOOPSONG)
                {
                    sprintf(textbuffer, "RST");
                    if (color == colscheme.normal) color = colscheme.command;
                }
            }
            if (chn[c].mute) color = colscheme.mute;
            printtext(dpos.orderlistX+4+d*3, dpos.orderlistY+1+c, color, textbuffer);
            if (c == esmarkchn)
            {
                if (esmarkstart <= esmarkend)
                {
                    if ((p >= esmarkstart) && (p <= esmarkend))
                    {
                        if (p != esmarkend)
                            printbg(dpos.orderlistX+4+d*3, dpos.orderlistY+1+c, colscheme.markBg, 3);
                        else
                            printbg(dpos.orderlistX+4+d*3, dpos.orderlistY+1+c, colscheme.markBg, 2);
                    }
                }
                else
                {
                    if ((p <= esmarkstart) && (p >= esmarkend))
                    {
                        if (p != esmarkstart)
                            printbg(dpos.orderlistX+4+d*3, dpos.orderlistY+1+c, colscheme.markBg, 3);
                        else
                            printbg(dpos.orderlistX+4+d*3, dpos.orderlistY+1+c, colscheme.markBg, 2);
                    }
                }
            }
            if ((p == eseditpos) && (editmode == EDIT_ORDERLIST) && (eschn == c))
            {
                if (!eamode) printbg(dpos.orderlistX+4+d*3+escolumn, dpos.orderlistY+1+c, cc, 1);
            }
        }
    }

    sprintf(textbuffer, "INSTRUMENT NUM. %02X  %-16s", einum, instr[einum].name);
    printtext(dpos.instrumentsX, dpos.instrumentsY, colscheme.title, textbuffer);

    sprintf(textbuffer, "Attack/Decay");
    if (eipos == 0) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX, dpos.instrumentsY+1, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ad);
    if (eipos == 0) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16, dpos.instrumentsY+1, color, textbuffer);

    sprintf(textbuffer, "Sustain/Release");
    if (eipos == 1) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX, dpos.instrumentsY+2, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].sr);
    if (eipos == 1) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16, dpos.instrumentsY+2, color, textbuffer);

    sprintf(textbuffer, "Wavetable Pos");
    if (eipos == 2) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX, dpos.instrumentsY+3, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[WTBL]);
    if (eipos == 2) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16, dpos.instrumentsY+3, color, textbuffer);

    sprintf(textbuffer, "Pulsetable Pos");
    if (eipos == 3) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX, dpos.instrumentsY+4, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[PTBL]);
    if (eipos == 3) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16, dpos.instrumentsY+4, color, textbuffer);

    sprintf(textbuffer, "Filtertable Pos");
    if (eipos == 4) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX, dpos.instrumentsY+5, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[FTBL]);
    if (eipos == 4) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16, dpos.instrumentsY+5, color, textbuffer);

    sprintf(textbuffer, "Vibrato Param");
    if (eipos == 5) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX+20, dpos.instrumentsY+1, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[STBL]);
    if (eipos == 5) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16+20, dpos.instrumentsY+1, color, textbuffer);

    sprintf(textbuffer, "Vibrato Delay");
    if (eipos == 6) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX+20, dpos.instrumentsY+2, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].vibdelay);
    if (eipos == 6) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16+20, dpos.instrumentsY+2, color, textbuffer);

    sprintf(textbuffer, "HR/Gate Timer");
    if (eipos == 7) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX+20, dpos.instrumentsY+3, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].gatetimer);
    if (eipos == 7) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16+20, dpos.instrumentsY+3, color, textbuffer);

    sprintf(textbuffer, "1stFrame Wave");
    if (eipos == 8) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(dpos.instrumentsX+20, dpos.instrumentsY+4, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].firstwave);
    if (eipos == 8) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(dpos.instrumentsX+16+20, dpos.instrumentsY+4, color, textbuffer);

    if (editmode == EDIT_INSTRUMENT)
    {
        if (eipos < 9)
        {
            if (!eamode) printbg(dpos.instrumentsX+16+eicolumn+20*(eipos/5), dpos.instrumentsY+1+(eipos%5), cc, 1);
        }
        else
        {
            if (!eamode) printbg(dpos.instrumentsX+20+strlen(instr[einum].name), dpos.instrumentsY, cc, 1);
        }
    }

    sprintf(textbuffer, "WAVE TBL  PULSETBL  FILT.TBL  SPEEDTBL");
    printtext(dpos.instrumentsX, dpos.instrumentsY+7, colscheme.title, textbuffer);

    for (c = 0; c < MAX_TABLES; c++)
    {
        for (d = 0; d < VISIBLETABLEROWS; d++)
        {
            int p = etview[c]+d;

            color = colscheme.normal;
            switch (c)
            {
            case WTBL:
                if (ltable[c][p] >= WAVECMD) color = colscheme.tableModVal;
                if (ltable[c][p] <= 0x0F) color = colscheme.tableModVal;
                break;

            case PTBL:
                if (ltable[c][p] >= 0x80) color = colscheme.normal;
                else color = colscheme.tableModVal;
                break;

            case FTBL:
                if ((ltable[c][p] >= 0x80) || ((!ltable[c][p]) && (rtable[c][p]))) color = colscheme.normal;
                else color = colscheme.tableModVal;
                break;
            }

            if (ltable[c][p] == 0xFF) color = colscheme.tableEnd;

            if ((p == etpos) && (etnum == c)) color = colscheme.edit;

            sprintf(textbuffer, "%02X:", p+1);

            printtext(dpos.instrumentsX+10*c, dpos.instrumentsY+8+d, colscheme.indexes, textbuffer);

            // allow single 00 00 columns:
            if ( ltable[c][p] || rtable[c][p] || ltable[c][p+1] || rtable[c][p+1] )
            {
                sprintf(textbuffer, "%02X %02X", ltable[c][p], rtable[c][p]);
            }
            else
            {
                sprintf(textbuffer, ".. ..");
            }
            printtext(dpos.instrumentsX+10*c+3, dpos.instrumentsY+8+d, color, textbuffer);

            if (etmarknum == c)
            {
                if (etmarkstart <= etmarkend)
                {
                    if ((p >= etmarkstart) && (p <= etmarkend))
                        printbg(dpos.instrumentsX+10*c+3, dpos.instrumentsY+8+d, colscheme.markBg, 5);
                }
                else
                {
                    if ((p <= etmarkstart) && (p >= etmarkend))
                        printbg(dpos.instrumentsX+10*c+3, dpos.instrumentsY+8+d, colscheme.markBg, 5);
                }
            }
        }
    }

    if (editmode == EDIT_TABLES)
    {
        if (!eamode) printbg(dpos.instrumentsX+3+etnum*10+(etcolumn & 1)+(etcolumn/2)*3, dpos.instrumentsY+8+etpos-etview[etnum], cc, 1);
    }

    printtext(dpos.instrumentsX, dpos.instrumentsY+8+VISIBLETABLEROWS+1, colscheme.title, "TITLE    ");
    sprintf(textbuffer, "%-32s", songname);
    printtext(dpos.instrumentsX+9, dpos.instrumentsY+8+VISIBLETABLEROWS+1, colscheme.edit, textbuffer);

    printtext(dpos.instrumentsX, dpos.instrumentsY+8+VISIBLETABLEROWS+2, colscheme.title, "AUTHOR   ");
    sprintf(textbuffer, "%-32s", authorname);
    printtext(dpos.instrumentsX+9, dpos.instrumentsY+8+VISIBLETABLEROWS+2, colscheme.edit, textbuffer);

    printtext(dpos.instrumentsX, dpos.instrumentsY+8+VISIBLETABLEROWS+3, colscheme.title, "RELEASED ");
    sprintf(textbuffer, "%-32s", copyrightname);
    printtext(dpos.instrumentsX+9, dpos.instrumentsY+8+VISIBLETABLEROWS+3, colscheme.edit, textbuffer);

    if ((editmode == EDIT_NAMES) && (!eamode))
    {
        switch(enpos)
        {
        case 0:
            printbg(dpos.instrumentsX+9+strlen(songname), dpos.instrumentsY+8+VISIBLETABLEROWS+1, cc, 1);
            break;
        case 1:
            printbg(dpos.instrumentsX+9+strlen(authorname), dpos.instrumentsY+8+VISIBLETABLEROWS+2, cc, 1);
            break;
        case 2:
            printbg(dpos.instrumentsX+9+strlen(copyrightname), dpos.instrumentsY+8+VISIBLETABLEROWS+3, cc, 1);
            break;
        }
    }

    sprintf(textbuffer, "OCTAVE");
    printtext(dpos.octaveX, dpos.octaveY, colscheme.title, textbuffer);

    sprintf(textbuffer, "%d", epoctave);
    printtext(dpos.octaveX+7, dpos.octaveY, colscheme.playing, textbuffer);

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

    if (recordmode) printtext(dpos.octaveX, dpos.octaveY+1, color, "EDITMODE");
    else printtext(dpos.octaveX, dpos.octaveY+1, colscheme.playing, "JAM MODE");

    if (isplaying()) printtext(dpos.octaveX+10, dpos.octaveY, colscheme.title, "PLAYING");
    else printtext(dpos.octaveX+10, dpos.octaveY, colscheme.normal, "STOPPED");

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

    printtext(dpos.octaveX+10, dpos.octaveY+1, colscheme.edit, textbuffer);

    if (numsids == 1)
    {
        printtext(
            dpos.channelsX,
            dpos.channelsY,
            colscheme.title,
            " CHN1   CHN2   CHN3 "
        );
    }
    else if (numsids == 2)
    {
        printtext(
            dpos.channelsX,
            dpos.channelsY,
            colscheme.title,
            " CHN1   CHN2   CHN3   CHN4   CHN5   CHN6 "
        );
    }
    for (c = 0; c < maxChns; c++)
    {
        int chnpos = chn[c].songptr;
        int chnrow = chn[c].pattptr/4;
        chnpos--;
        if (chnpos < 0) chnpos = 0;
        if (chnrow > pattlen[chn[c].pattnum]) chnrow = pattlen[chn[c].pattnum];
        if (chnrow >= 100) chnrow -= 100;

        sprintf(textbuffer, "%03X/%02X",
                chnpos,chnrow);
        printtext(
            dpos.channelsX+7*c,
            dpos.channelsY+1,
            chn[c].mute ? colscheme.mute : colscheme.edit,
            textbuffer
        );
    }

    if (etlock) printtext(dpos.channelsX-2, dpos.channelsY+1, colscheme.statusBottom, " ");
    else printtext(dpos.channelsX-2, dpos.channelsY+1, colscheme.statusBottom, "U");
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

