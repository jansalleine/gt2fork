//
// GOATTRACKER v2 screen display routines
//

#define GDISPLAY_C

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
    menu = 0;

    if ((mouseb > MOUSEB_LEFT) && (mousey <= 1) && (!eamode)) menu = 1;

    printblankc(0, 0, colscheme.status_top, MAX_COLUMNS);

    if (!menu)
    {
        if (!strlen(loadedsongfilename))
            sprintf(textbuffer, "%s", programname);
        else
            sprintf(textbuffer, "%s - %s", programname, loadedsongfilename);
        textbuffer[49] = 0;
        printtext(POSSTATUSTOPX+1, POSSTATUSTOPY, colscheme.status_top, textbuffer);

        if (usefinevib)
            printtext(POSSTATUSTOPFVX, POSSTATUSTOPY, colscheme.status_top, "FV");

        if (optimizepulse)
            printtext(POSSTATUSTOPFVX+3, POSSTATUSTOPY, colscheme.status_top, "PO");

        if (optimizerealtime)
            printtext(POSSTATUSTOPFVX+6, POSSTATUSTOPY, colscheme.status_top, "RO");

        if (ntsc)
            printtext(POSSTATUSTOPFVX+9, POSSTATUSTOPY, colscheme.status_top, "NTSC");
        else
            printtext(POSSTATUSTOPFVX+9, POSSTATUSTOPY, colscheme.status_top, " PAL");

        if (!sidmodel)
            printtext(POSSTATUSTOPFVX+14, POSSTATUSTOPY, colscheme.status_top, "6581");
        else
            printtext(POSSTATUSTOPFVX+19, POSSTATUSTOPY, colscheme.status_top, "8580");

        sprintf(textbuffer, "HR:%04X", adparam);
        printtext(POSSTATUSTOPFVX+19, POSSTATUSTOPY, colscheme.status_top, textbuffer);
        if (eamode) printbg(POSSTATUSTOPFVX+22+eacolumn, POSSTATUSTOPY, cc, 1);

        if (multiplier)
        {
            sprintf(textbuffer, "%2dX", multiplier);
            printtext(POSSTATUSTOPFVX+27, POSSTATUSTOPY, colscheme.status_top, textbuffer);
        }
        else printtext(POSSTATUSTOPFVX+27, POSSTATUSTOPY, colscheme.status_top, "25Hz");

        printtext(POSSTATUSTOPENDX-8, POSSTATUSTOPY, colscheme.status_top, "F12=HELP");
    }
    else
    {
        printtext(0, POSSTATUSTOPY, colscheme.status_top, " PLAY | PLAYPOS | PLAYPATT | STOP | LOAD | SAVE | PACK/RL | HELP | CLEAR | QUIT |");
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
                if (newpos - esview >= VISIBLEORDERLISTMONO)
                {
                    esview = newpos - VISIBLEORDERLISTMONO + 1;
                }
            }
        }
    }

    for (c = 0; c < MAX_CHN; c++)
    {
        sprintf(textbuffer, "CH.");
        printtext(POSMONOPATTERNSX+c*13, POSMONOPATTERNSY, colscheme.pattcol, textbuffer);
        sprintf(textbuffer, "%d", c+1);
        printtext(POSMONOPATTERNSX+3+c*13, POSMONOPATTERNSY, colscheme.edit, textbuffer);
        sprintf(textbuffer, "PATT.");
        printtext(POSMONOPATTERNSX+5+c*13, POSMONOPATTERNSY, colscheme.pattcol, textbuffer);
        sprintf(textbuffer, "%02X", epnum[c]);
        printtext(POSMONOPATTERNSX+10+c*13, POSMONOPATTERNSY, colscheme.edit, textbuffer);

        for (d = 0; d < VISIBLEPATTROWS; d++)
        {
            int p = epview+d;
            color = colscheme.normal;
            if ((epnum[c] == chn[c].pattnum) && (isplaying()))
            {
                int chnrow = chn[c].pattptr / 4;
                if (chnrow > pattlen[chn[c].pattnum]) chnrow = pattlen[chn[c].pattnum];

                if (chnrow == p) color = colscheme.playing_inv;
            }

            if (chn[c].mute) color = colscheme.mute;
            if (p == eppos) color = colscheme.edit;
            //
            if ((p == eppos) && (followplay)) color = colscheme.playing_inv;
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

                    if (color == colscheme.normal) color = colscheme.table_end;
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
                    printtext(POSMONOPATTERNSX+c*13, POSMONOPATTERNSY+1+d, colscheme.indexes_inv, &textbuffer[1]);
                }
                else
                {
                    printtext(POSMONOPATTERNSX-1+c*13, POSMONOPATTERNSY+1+d, colscheme.indexes_inv, textbuffer);
                }
            }
            else
            {
                printtext(POSMONOPATTERNSX-1+c*13, POSMONOPATTERNSY+1+d, colscheme.indexes, textbuffer);
            }
            int tmp, tmpc;
            tmp = textbuffer[8];
            textbuffer[8] = 0;
            printtext(POSMONOPATTERNSX+3+c*13, POSMONOPATTERNSY+1+d, color, &textbuffer[4]);
            textbuffer[8] = tmp;
            tmp = textbuffer[10];
            textbuffer[10] = 0;
            tmpc = ( color == colscheme.mute ) || ( color == colscheme.edit ) || ( color == colscheme.table_end ) || ( color == colscheme.playing_inv ) ? color : colscheme.instrnum;
            printtext(POSMONOPATTERNSX+7+c*13, POSMONOPATTERNSY+1+d, tmpc, &textbuffer[8]);
            textbuffer[10] = tmp;
            tmp = textbuffer[11];
            textbuffer[11] = 0;
            tmpc = ( color == colscheme.mute ) || ( color == colscheme.edit ) || ( color == colscheme.table_end ) || ( color == colscheme.playing_inv ) ? color : colscheme.command;
            printtext(POSMONOPATTERNSX+9+c*13, POSMONOPATTERNSY+1+d, tmpc, &textbuffer[10]);
            textbuffer[11] = tmp;
            tmpc = ( color == colscheme.mute ) || ( color == colscheme.edit ) || ( color == colscheme.table_end ) || ( color == colscheme.playing_inv ) ? color : colscheme.command_value;
            printtext(POSMONOPATTERNSX+10+c*13, POSMONOPATTERNSY+1+d, tmpc, &textbuffer[11]);

            /* ====================================== */

            if (c == epmarkchn)
            {
                if (epmarkstart <= epmarkend)
                {
                    if ((p >= epmarkstart) && (p <= epmarkend))
                        printbg(POSMONOPATTERNSX+c*13+3, POSMONOPATTERNSY+1+d, colscheme.mark_bg, 9);
                }
                else
                {
                    if ((p <= epmarkstart) && (p >= epmarkend))
                        printbg(POSMONOPATTERNSX+c*13+3, POSMONOPATTERNSY+1+d, colscheme.mark_bg, 9);
                }
            }
            if ((color == colscheme.edit) && (editmode == EDIT_PATTERN) && (epchn == c))
            {
                switch(epcolumn)
                {
                case 0:
                    if (!eamode) printbg(POSMONOPATTERNSX+c*13+3, POSMONOPATTERNSY+1+d, cc, 3);
                    break;

                default:
                    if (!eamode) printbg(POSMONOPATTERNSX+c*13+6+epcolumn, POSMONOPATTERNSY+1+d, cc, 1);
                    break;
                }
            }
        }
    }

    sprintf(textbuffer, "CHN ORDERLIST (SUBTUNE %02X, POS %02X)", esnum, eseditpos);
    printtext(POSMONOORDERLISTX, POSMONOORDERLISTY, colscheme.title, textbuffer);
    for (c = 0; c < MAX_CHN; c++)
    {
        sprintf(textbuffer, " %d ", c+1);
        printtext(POSMONOORDERLISTX, POSMONOORDERLISTY+1+c, colscheme.title, textbuffer);
        for ( d = 0; d < VISIBLEORDERLISTMONO; d++ )
        {
            int p = esview+d;
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
                        if ((p >= songlen[esnum][c]) && (color == colscheme.normal)) color = colscheme.command;
                    }
                    else
                    {
                        if (songorder[esnum][c][p] >= TRANSUP)
                        {
                            sprintf(textbuffer, "+%01X ", songorder[esnum][c][p]&0xf);
                            if (color == colscheme.normal) color = colscheme.command;
                        }
                        else
                        {
                            if (songorder[esnum][c][p] >= TRANSDOWN)
                            {
                                sprintf(textbuffer, "-%01X ", 16-(songorder[esnum][c][p] & 0x0f));
                                if (color == colscheme.normal) color = colscheme.command;
                            }
                            else
                            {
                                sprintf(textbuffer, "R%01X ", (songorder[esnum][c][p]+1) & 0x0f);
                                if (color == colscheme.normal) color = colscheme.command;
                            }
                        }
                    }
                }
                if (songorder[esnum][c][p] == LOOPSONG)
                {
                    sprintf(textbuffer, "RST");
                    if (color == colscheme.normal) color = colscheme.command;
                }
            }
            printtext(POSMONOORDERLISTX+4+d*3, POSMONOORDERLISTY+1+c, color, textbuffer);
            if (c == esmarkchn)
            {
                if (esmarkstart <= esmarkend)
                {
                    if ((p >= esmarkstart) && (p <= esmarkend))
                    {
                        if (p != esmarkend)
                            printbg(POSMONOORDERLISTX+4+d*3, POSMONOORDERLISTY+1+c, colscheme.mark_bg, 3);
                        else
                            printbg(POSMONOORDERLISTX+4+d*3, POSMONOORDERLISTY+1+c, colscheme.mark_bg, 2);
                    }
                }
                else
                {
                    if ((p <= esmarkstart) && (p >= esmarkend))
                    {
                        if (p != esmarkstart)
                            printbg(POSMONOORDERLISTX+4+d*3, POSMONOORDERLISTY+1+c, colscheme.mark_bg, 3);
                        else
                            printbg(POSMONOORDERLISTX+4+d*3, POSMONOORDERLISTY+1+c, colscheme.mark_bg, 2);
                    }
                }
            }
            if ((p == eseditpos) && (editmode == EDIT_ORDERLIST) && (eschn == c))
            {
                if (!eamode) printbg(POSMONOORDERLISTX+4+d*3+escolumn, POSMONOORDERLISTY+1+c, cc, 1);
            }
        }
    }

    sprintf(textbuffer, "INSTRUMENT NUM. %02X  %-16s", einum, instr[einum].name);
    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY, colscheme.title, textbuffer);

    sprintf(textbuffer, "Attack/Decay");
    if (eipos == 0) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+1, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ad);
    if (eipos == 0) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16, POSMONOINSTRTABY+1, color, textbuffer);

    sprintf(textbuffer, "Sustain/Release");
    if (eipos == 1) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+2, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].sr);
    if (eipos == 1) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16, POSMONOINSTRTABY+2, color, textbuffer);

    sprintf(textbuffer, "Wavetable Pos");
    if (eipos == 2) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+3, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[WTBL]);
    if (eipos == 2) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16, POSMONOINSTRTABY+3, color, textbuffer);

    sprintf(textbuffer, "Pulsetable Pos");
    if (eipos == 3) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+4, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[PTBL]);
    if (eipos == 3) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16, POSMONOINSTRTABY+4, color, textbuffer);

    sprintf(textbuffer, "Filtertable Pos");
    if (eipos == 4) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+5, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[FTBL]);
    if (eipos == 4) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16, POSMONOINSTRTABY+5, color, textbuffer);

    sprintf(textbuffer, "Vibrato Param");
    if (eipos == 5) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX+20, POSMONOINSTRTABY+1, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].ptr[STBL]);
    if (eipos == 5) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16+20, POSMONOINSTRTABY+1, color, textbuffer);

    sprintf(textbuffer, "Vibrato Delay");
    if (eipos == 6) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX+20, POSMONOINSTRTABY+2, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].vibdelay);
    if (eipos == 6) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16+20, POSMONOINSTRTABY+2, color, textbuffer);

    sprintf(textbuffer, "HR/Gate Timer");
    if (eipos == 7) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX+20, POSMONOINSTRTABY+3, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].gatetimer);
    if (eipos == 7) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16+20, POSMONOINSTRTABY+3, color, textbuffer);

    sprintf(textbuffer, "1stFrame Wave");
    if (eipos == 8) color = colscheme.edit;
    else color = colscheme.normal & 0xF7;
    printtext(POSMONOINSTRTABX+20, POSMONOINSTRTABY+4, color, textbuffer);

    sprintf(textbuffer, "%02X", instr[einum].firstwave);
    if (eipos == 8) color = colscheme.normal;
    else color = colscheme.playing;
    printtext(POSMONOINSTRTABX+16+20, POSMONOINSTRTABY+4, color, textbuffer);

    if (editmode == EDIT_INSTRUMENT)
    {
        if (eipos < 9)
        {
            if (!eamode) printbg(POSMONOINSTRTABX+16+eicolumn+20*(eipos/5), POSMONOINSTRTABY+1+(eipos%5), cc, 1);
        }
        else
        {
            if (!eamode) printbg(POSMONOINSTRTABX+20+strlen(instr[einum].name), POSMONOINSTRTABY, cc, 1);
        }
    }

    sprintf(textbuffer, "WAVE TBL  PULSETBL  FILT.TBL  SPEEDTBL");
    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+7, colscheme.title, textbuffer);

    for (c = 0; c < MAX_TABLES; c++)
    {
        for (d = 0; d < VISIBLETABLEROWS; d++)
        {
            int p = etview[c]+d;

            color = colscheme.normal;
            switch (c)
            {
            case WTBL:
                if (ltable[c][p] >= WAVECMD) color = colscheme.table_modval;
                if (ltable[c][p] <= 0x10) color = colscheme.table_modval;
                break;

            case PTBL:
                if (ltable[c][p] >= 0x80) color = colscheme.normal;
                else color = colscheme.table_modval;
                break;

            case FTBL:
                if ((ltable[c][p] >= 0x80) || ((!ltable[c][p]) && (rtable[c][p]))) color = colscheme.normal;
                else color = colscheme.table_modval;
                break;
            }

            if (ltable[c][p] == 0xFF) color = colscheme.table_end;

            if ((p == etpos) && (etnum == c)) color = colscheme.edit;

            sprintf(textbuffer, "%02X:", p+1);

            printtext(POSMONOINSTRTABX+10*c, POSMONOINSTRTABY+8+d, colscheme.indexes, textbuffer);

            // allow single 00 00 columns:
            if ( ltable[c][p] || rtable[c][p] || ltable[c][p+1] || rtable[c][p+1] )
            {
                sprintf(textbuffer, "%02X %02X", ltable[c][p], rtable[c][p]);
            }
            else
            {
                sprintf(textbuffer, ".. ..");
            }
            printtext(POSMONOINSTRTABX+10*c+3, POSMONOINSTRTABY+8+d, color, textbuffer);

            if (etmarknum == c)
            {
                if (etmarkstart <= etmarkend)
                {
                    if ((p >= etmarkstart) && (p <= etmarkend))
                        printbg(POSMONOINSTRTABX+10*c+3, POSMONOINSTRTABY+8+d, colscheme.mark_bg, 5);
                }
                else
                {
                    if ((p <= etmarkstart) && (p >= etmarkend))
                        printbg(POSMONOINSTRTABX+10*c+3, POSMONOINSTRTABY+8+d, colscheme.mark_bg, 5);
                }
            }
        }
    }

    if (editmode == EDIT_TABLES)
    {
        if (!eamode) printbg(POSMONOINSTRTABX+3+etnum*10+(etcolumn & 1)+(etcolumn/2)*3, POSMONOINSTRTABY+8+etpos-etview[etnum], cc, 1);
    }

    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+8+VISIBLETABLEROWS+1, colscheme.title, "NAME   ");
    sprintf(textbuffer, "%-32s", songname);
    printtext(POSMONOINSTRTABX+7, POSMONOINSTRTABY+8+VISIBLETABLEROWS+1, colscheme.edit, textbuffer);

    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+8+VISIBLETABLEROWS+2, colscheme.title, "AUTHOR ");
    sprintf(textbuffer, "%-32s", authorname);
    printtext(POSMONOINSTRTABX+7, POSMONOINSTRTABY+8+VISIBLETABLEROWS+2, colscheme.edit, textbuffer);

    printtext(POSMONOINSTRTABX, POSMONOINSTRTABY+8+VISIBLETABLEROWS+3, colscheme.title, "COPYR. ");
    sprintf(textbuffer, "%-32s", copyrightname);
    printtext(POSMONOINSTRTABX+7, POSMONOINSTRTABY+8+VISIBLETABLEROWS+3, colscheme.edit, textbuffer);

    if ((editmode == EDIT_NAMES) && (!eamode))
    {
        switch(enpos)
        {
        case 0:
            printbg(POSMONOINSTRTABX+7+strlen(songname), POSMONOINSTRTABY+8+VISIBLETABLEROWS+1, cc, 1);
            break;
        case 1:
            printbg(POSMONOINSTRTABX+7+strlen(authorname), POSMONOINSTRTABY+8+VISIBLETABLEROWS+2, cc, 1);
            break;
        case 2:
            printbg(POSMONOINSTRTABX+7+strlen(copyrightname), POSMONOINSTRTABY+8+VISIBLETABLEROWS+3, cc, 1);
            break;
        }
    }

    sprintf(textbuffer, "OCTAVE");
    printtext(POSMONOOCTAVEX, POSMONOOCTAVEY, colscheme.title, textbuffer);

    sprintf(textbuffer, "%d", epoctave);
    printtext(POSMONOOCTAVEX+7, POSMONOOCTAVEY, colscheme.playing, textbuffer);

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

    if (recordmode) printtext(POSMONOOCTAVEX, POSMONOOCTAVEY+1, color, "EDITMODE");
    else printtext(POSMONOOCTAVEX, POSMONOOCTAVEY+1, colscheme.playing, "JAM MODE");

    if (isplaying()) printtext(POSMONOOCTAVEX+10, POSMONOOCTAVEY, colscheme.title, "PLAYING");
    else printtext(POSMONOOCTAVEX+10, POSMONOOCTAVEY, colscheme.normal, "STOPPED");

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

    printtext(POSMONOOCTAVEX+10, POSMONOOCTAVEY+1, colscheme.edit, textbuffer);

    printtext(POSMONOCHNSX, POSMONOCHNSY, colscheme.title, " CHN1   CHN2   CHN3 ");
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
        printtext(POSMONOCHNSX+7*c, POSMONOCHNSY+1, colscheme.edit, textbuffer);
    }

    if (etlock) printtext(POSMONOCHNSX-2, POSMONOCHNSY+1, colscheme.status_bottom, " ");
    else printtext(POSMONOCHNSX-2, POSMONOCHNSY+1, colscheme.status_bottom, "U");
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

