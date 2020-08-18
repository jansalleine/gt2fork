//
// GOATTRACKER v2 orderlist & songname editor
//

#define GORDER_C

#include "gt2fork.h"

unsigned char trackcopybuffer[MAX_SONGLEN+2];
int trackcopyrows = 0;
int trackcopywhole;
int trackcopyrpos;

int espos[MAX_CHN];
int esend[MAX_CHN];
int eseditpos;
int esview;
int escolumn;
int eschn;
int esnum;
int esmarkchn = -1;
int esmarkstart;
int esmarkend;
int enpos;

void orderlistcommands(void);
void orderlistcommands_stereo(void);
void namecommands(void);

void orderlistcommands(void)
{
    int c, scrrep;
    int maxChns = 3;
    // if (numsids == 1) maxChns = 3;

    if (hexnybble >= 0)
    {
        if (eseditpos != songlen[esnum][eschn])
        {
            switch(escolumn)
            {
            case 0:
                songorder[esnum][eschn][eseditpos] &= 0x0f;
                songorder[esnum][eschn][eseditpos] |= hexnybble << 4;
                if (eseditpos < songlen[esnum][eschn])
                {
                    if (songorder[esnum][eschn][eseditpos] >= MAX_PATT)
                        songorder[esnum][eschn][eseditpos] = MAX_PATT - 1;
                }
                else
                {
                    if (songorder[esnum][eschn][eseditpos] >= MAX_SONGLEN)
                        songorder[esnum][eschn][eseditpos] = MAX_SONGLEN - 1;
                }
                break;

            case 1:
                songorder[esnum][eschn][eseditpos] &= 0xf0;
                if ((songorder[esnum][eschn][eseditpos] & 0xf0) == 0xd0)
                {
                    hexnybble--;
                    if (hexnybble < 0) hexnybble = 0xf;
                }
                if ((songorder[esnum][eschn][eseditpos] & 0xf0) == 0xe0)
                {
                    hexnybble = 16 - hexnybble;
                    hexnybble &= 0xf;
                }
                songorder[esnum][eschn][eseditpos] |= hexnybble;

                if (eseditpos < songlen[esnum][eschn])
                {
                    if (songorder[esnum][eschn][eseditpos] == LOOPSONG)
                        songorder[esnum][eschn][eseditpos] = LOOPSONG-1;
                    if (songorder[esnum][eschn][eseditpos] == TRANSDOWN)
                        songorder[esnum][eschn][eseditpos] = TRANSDOWN+0x0f;
                }
                else
                {
                    if (songorder[esnum][eschn][eseditpos] >= MAX_SONGLEN)
                        songorder[esnum][eschn][eseditpos] = MAX_SONGLEN - 1;
                }
                break;
            }
            escolumn++;
            if (escolumn > 1)
            {
                escolumn = 0;
                if (eseditpos < (songlen[esnum][eschn]+1))
                {
                    eseditpos++;
                    if (eseditpos == songlen[esnum][eschn]) eseditpos++;
                }
            }
        }
    }

    switch(key)
    {
    case 'R':
        if (eseditpos < songlen[esnum][eschn])
        {
            songorder[esnum][eschn][eseditpos] = REPEAT + 0x01;
            escolumn = 1;
        }
        break;

    case '+':
        if (eseditpos < songlen[esnum][eschn])
        {
            songorder[esnum][eschn][eseditpos] = TRANSUP;
            escolumn = 1;
        }
        break;

    case '-':
        if (eseditpos < songlen[esnum][eschn])
        {
            songorder[esnum][eschn][eseditpos] = TRANSDOWN + 0x0F;
            escolumn = 1;
        }
        break;

    case '>':
    case ')':
    case ']':
        nextsong();
        break;

    case '<':
    case '(':
    case '[':
        prevsong();
        break;
    }
    switch(rawkey)
    {
    case KEY_1:
    case KEY_2:
    case KEY_3:
        if (shiftpressed)
        {
            int schn = eschn;
            int tchn = 0;

            esmarkchn = -1;
            if (rawkey == KEY_1) tchn = 0;
            if (rawkey == KEY_2) tchn = 1;
            if (rawkey == KEY_3) tchn = 2;
            if (schn != tchn)
            {
                int lentemp = songlen[esnum][schn];
                songlen[esnum][schn] = songlen[esnum][tchn];
                songlen[esnum][tchn] = lentemp;

                for (c = 0; c < MAX_SONGLEN+2; c++)
                {
                    unsigned char temp = songorder[esnum][schn][c];
                    songorder[esnum][schn][c] = songorder[esnum][tchn][c];
                    songorder[esnum][tchn][c] = temp;
                }
            }
        }
        break;

    case KEY_X:
        if (shiftpressed)
        {
            if (esmarkchn != -1)
            {
                int d = 0;

                eschn = esmarkchn;
                if (esmarkstart <= esmarkend)
                {
                    eseditpos = esmarkstart;
                    for (c = esmarkstart; c <= esmarkend; c++)
                        trackcopybuffer[d++] = songorder[esnum][eschn][c];
                    trackcopyrows = d;
                }
                else
                {
                    eseditpos = esmarkend;
                    for (c = esmarkend; c <= esmarkstart; c++)
                        trackcopybuffer[d++] = songorder[esnum][eschn][c];
                    trackcopyrows = d;
                }
                if (trackcopyrows == songlen[esnum][eschn])
                {
                    trackcopywhole = 1;
                    trackcopyrpos = songorder[esnum][eschn][songlen[esnum][eschn]+1];
                }
                else trackcopywhole = 0;
                for (c = 0; c < trackcopyrows; c++) deleteorder();
                esmarkchn = -1;
            }
        }
        break;

    case KEY_C:
        if (shiftpressed)
        {
            if (esmarkchn != -1)
            {
                int d = 0;
                if (esmarkstart <= esmarkend)
                {
                    for (c = esmarkstart; c <= esmarkend; c++)
                        trackcopybuffer[d++] = songorder[esnum][eschn][c];
                    trackcopyrows = d;
                }
                else
                {
                    for (c = esmarkend; c <= esmarkstart; c++)
                        trackcopybuffer[d++] = songorder[esnum][eschn][c];
                    trackcopyrows = d;
                }
                if (trackcopyrows == songlen[esnum][eschn])
                {
                    trackcopywhole = 1;
                    trackcopyrpos = songorder[esnum][eschn][songlen[esnum][eschn]+1];
                }
                else trackcopywhole = 0;
                esmarkchn = -1;
            }
        }
        break;

    case KEY_V:
        if (shiftpressed)
        {
            int oldlen = songlen[esnum][eschn];

            if (eseditpos < songlen[esnum][eschn])
            {
                for (c = trackcopyrows-1; c >= 0; c--)
                    insertorder(trackcopybuffer[c]);
            }
            else
            {
                for (c = 0; c < trackcopyrows; c++)
                    insertorder(trackcopybuffer[c]);
            }
            if ((trackcopywhole) && (!oldlen))
                songorder[esnum][eschn][songlen[esnum][eschn]+1] = trackcopyrpos;
        }
        break;

    case KEY_L:
        if (shiftpressed)
        {
            if (esmarkchn == -1)
            {
                esmarkchn = eschn;
                esmarkstart = 0;
                esmarkend = songlen[esnum][eschn]-1;
            }
            else esmarkchn = -1;
        }
        break;


    case KEY_SPACE:
        if (!shiftpressed)
        {
            if (eseditpos < songlen[esnum][eschn]) espos[eschn] = eseditpos;
            if (esend[eschn] < espos[eschn]) esend[eschn] = 0;
        }
        else
        {
            for (c = 0; c < maxChns; c++)
            {
                if (eseditpos < songlen[esnum][c]) espos[c] = eseditpos;
                if (esend[c] < espos[c]) esend[c] = 0;
            }
        }
        break;

    case KEY_BACKSPACE:
        if (!shiftpressed)
        {
            if ((esend[eschn] != eseditpos) && (eseditpos > espos[eschn]))
            {
                if (eseditpos < songlen[esnum][eschn]) esend[eschn] = eseditpos;
            }
            else esend[eschn] = 0;
        }
        else
        {
            if ((esend[eschn] != eseditpos) && (eseditpos > espos[eschn]))
            {
                for (c = 0; c < maxChns; c++)
                {
                    if (eseditpos < songlen[esnum][c]) esend[c] = eseditpos;
                }
            }
            else
            {
                for (c = 0; c < maxChns; c++) esend[c] = 0;
            }
        }
        break;

    case KEY_ENTER:
        if (eseditpos < songlen[esnum][eschn])
        {
            if (!shiftpressed)
            {
                if (songorder[esnum][eschn][eseditpos] < MAX_PATT)
                    epnum[eschn] = songorder[esnum][eschn][eseditpos];
            }
            else
            {
                int c, d;

                for (c = 0; c < maxChns; c++)
                {
                    int start;

                    if (eseditpos != espos[eschn]) start = eseditpos;
                    else start = espos[c];

                    for (d = start; d < songlen[esnum][c]; d++)
                    {
                        if (songorder[esnum][c][d] < MAX_PATT)
                        {
                            epnum[c] = songorder[esnum][c][d];
                            break;
                        }
                    }
                }
            }
            epmarkchn = -1;
        }
        epchn = eschn;
        epcolumn = 0;
        eppos = 0;
        epview = - VISIBLEPATTROWS/2;
        editmode = EDIT_PATTERN;
        if (epchn == epmarkchn) epmarkchn = -1;
        break;

    case KEY_DEL:
        esmarkchn = -1;
        deleteorder();
        break;

    case KEY_INS:
        esmarkchn = -1;
        insertorder(0);
        break;

    case KEY_HOME:
        if (songlen[esnum][eschn])
        {
            while ((eseditpos != 0) || (escolumn != 0)) orderleft();
        }
        break;

    case KEY_END:
        while (eseditpos != songlen[esnum][eschn]+1) orderright();
        break;

    case KEY_PGUP:
        for (scrrep = PGUPDNREPEAT * 2; scrrep; scrrep--)
            orderleft();
        break;

    case KEY_PGDN:
        for (scrrep = PGUPDNREPEAT * 2; scrrep; scrrep--)
            orderright();
        break;

    case KEY_LEFT:
        orderleft();
        break;

    case KEY_RIGHT:
        orderright();
        break;

    case KEY_UP:
        eschn--;
        if (eschn < 0) eschn = maxChns - 1;
        if ((eseditpos == songlen[esnum][eschn]) || (eseditpos > songlen[esnum][eschn]+1))
        {
            eseditpos = songlen[esnum][eschn]+1;
            escolumn = 0;
        }
        if (shiftpressed) esmarkchn = -1;
        break;

    case KEY_DOWN:
        eschn++;
        if (eschn >= maxChns) eschn = 0;
        if ((eseditpos == songlen[esnum][eschn]) || (eseditpos > songlen[esnum][eschn]+1))
        {
            eseditpos = songlen[esnum][eschn]+1;
            escolumn = 0;
        }
        if (shiftpressed) esmarkchn = -1;
        break;
    }
    if (eseditpos - esview < 0)
    {
        esview = eseditpos;
    }
    if (eseditpos - esview >= VISIBLEORDERLIST)
    {
        esview = eseditpos - VISIBLEORDERLIST + 1;
    }
}

void orderlistcommands_stereo(void)
{
    int c, scrrep;
    int maxChns = MAX_CHN;
    // if (numsids == 1) maxChns = 3;

    if (hexnybble >= 0)
    {
        if (eseditpos != songlen_stereo[esnum][eschn])
        {
            switch(escolumn)
            {
            case 0:
                songorder_stereo[esnum][eschn][eseditpos] &= 0x0f;
                songorder_stereo[esnum][eschn][eseditpos] |= hexnybble << 4;
                if (eseditpos < songlen_stereo[esnum][eschn])
                {
                    if (songorder_stereo[esnum][eschn][eseditpos] >= MAX_PATT)
                        songorder_stereo[esnum][eschn][eseditpos] = MAX_PATT - 1;
                }
                else
                {
                    if (songorder_stereo[esnum][eschn][eseditpos] >= MAX_SONGLEN)
                        songorder_stereo[esnum][eschn][eseditpos] = MAX_SONGLEN - 1;
                }
                break;

            case 1:
                songorder_stereo[esnum][eschn][eseditpos] &= 0xf0;
                if ((songorder_stereo[esnum][eschn][eseditpos] & 0xf0) == 0xd0)
                {
                    hexnybble--;
                    if (hexnybble < 0) hexnybble = 0xf;
                }
                if ((songorder_stereo[esnum][eschn][eseditpos] & 0xf0) == 0xe0)
                {
                    hexnybble = 16 - hexnybble;
                    hexnybble &= 0xf;
                }
                songorder_stereo[esnum][eschn][eseditpos] |= hexnybble;

                if (eseditpos < songlen_stereo[esnum][eschn])
                {
                    if (songorder_stereo[esnum][eschn][eseditpos] == LOOPSONG)
                        songorder_stereo[esnum][eschn][eseditpos] = LOOPSONG-1;
                    if (songorder_stereo[esnum][eschn][eseditpos] == TRANSDOWN)
                        songorder_stereo[esnum][eschn][eseditpos] = TRANSDOWN+0x0f;
                }
                else
                {
                    if (songorder_stereo[esnum][eschn][eseditpos] >= MAX_SONGLEN)
                        songorder_stereo[esnum][eschn][eseditpos] = MAX_SONGLEN - 1;
                }
                break;
            }
            escolumn++;
            if (escolumn > 1)
            {
                escolumn = 0;
                if (eseditpos < (songlen_stereo[esnum][eschn]+1))
                {
                    eseditpos++;
                    if (eseditpos == songlen_stereo[esnum][eschn]) eseditpos++;
                }
            }
        }
    }

    switch(key)
    {
    case 'R':
        if (eseditpos < songlen_stereo[esnum][eschn])
        {
            songorder_stereo[esnum][eschn][eseditpos] = REPEAT + 0x01;
            escolumn = 1;
        }
        break;

    case '+':
        if (eseditpos < songlen_stereo[esnum][eschn])
        {
            songorder_stereo[esnum][eschn][eseditpos] = TRANSUP;
            escolumn = 1;
        }
        break;

    case '-':
        if (eseditpos < songlen_stereo[esnum][eschn])
        {
            songorder_stereo[esnum][eschn][eseditpos] = TRANSDOWN + 0x0F;
            escolumn = 1;
        }
        break;

    case '>':
    case ')':
    case ']':
        nextsong();
        break;

    case '<':
    case '(':
    case '[':
        prevsong();
        break;
    }
    switch(rawkey)
    {
    case KEY_1:
    case KEY_2:
    case KEY_3:
        if (shiftpressed)
        {
            int schn = eschn;
            int tchn = 0;

            esmarkchn = -1;
            if (rawkey == KEY_1) tchn = 0;
            if (rawkey == KEY_2) tchn = 1;
            if (rawkey == KEY_3) tchn = 2;
            if (schn != tchn)
            {
                int lentemp = songlen_stereo[esnum][schn];
                songlen_stereo[esnum][schn] = songlen_stereo[esnum][tchn];
                songlen_stereo[esnum][tchn] = lentemp;

                for (c = 0; c < MAX_SONGLEN+2; c++)
                {
                    unsigned char temp = songorder_stereo[esnum][schn][c];
                    songorder_stereo[esnum][schn][c] = songorder_stereo[esnum][tchn][c];
                    songorder_stereo[esnum][tchn][c] = temp;
                }
            }
        }
        break;

    case KEY_X:
        if (shiftpressed)
        {
            if (esmarkchn != -1)
            {
                int d = 0;

                eschn = esmarkchn;
                if (esmarkstart <= esmarkend)
                {
                    eseditpos = esmarkstart;
                    for (c = esmarkstart; c <= esmarkend; c++)
                        trackcopybuffer[d++] = songorder_stereo[esnum][eschn][c];
                    trackcopyrows = d;
                }
                else
                {
                    eseditpos = esmarkend;
                    for (c = esmarkend; c <= esmarkstart; c++)
                        trackcopybuffer[d++] = songorder_stereo[esnum][eschn][c];
                    trackcopyrows = d;
                }
                if (trackcopyrows == songlen_stereo[esnum][eschn])
                {
                    trackcopywhole = 1;
                    trackcopyrpos = songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]+1];
                }
                else trackcopywhole = 0;
                for (c = 0; c < trackcopyrows; c++) deleteorder_stereo();
                esmarkchn = -1;
            }
        }
        break;

    case KEY_C:
        if (shiftpressed)
        {
            if (esmarkchn != -1)
            {
                int d = 0;
                if (esmarkstart <= esmarkend)
                {
                    for (c = esmarkstart; c <= esmarkend; c++)
                        trackcopybuffer[d++] = songorder_stereo[esnum][eschn][c];
                    trackcopyrows = d;
                }
                else
                {
                    for (c = esmarkend; c <= esmarkstart; c++)
                        trackcopybuffer[d++] = songorder_stereo[esnum][eschn][c];
                    trackcopyrows = d;
                }
                if (trackcopyrows == songlen_stereo[esnum][eschn])
                {
                    trackcopywhole = 1;
                    trackcopyrpos = songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]+1];
                }
                else trackcopywhole = 0;
                esmarkchn = -1;
            }
        }
        break;

    case KEY_V:
        if (shiftpressed)
        {
            int oldlen = songlen_stereo[esnum][eschn];

            if (eseditpos < songlen_stereo[esnum][eschn])
            {
                for (c = trackcopyrows-1; c >= 0; c--)
                    insertorder_stereo(trackcopybuffer[c]);
            }
            else
            {
                for (c = 0; c < trackcopyrows; c++)
                    insertorder_stereo(trackcopybuffer[c]);
            }
            if ((trackcopywhole) && (!oldlen))
                songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]+1] = trackcopyrpos;
        }
        break;

    case KEY_L:
        if (shiftpressed)
        {
            if (esmarkchn == -1)
            {
                esmarkchn = eschn;
                esmarkstart = 0;
                esmarkend = songlen_stereo[esnum][eschn]-1;
            }
            else esmarkchn = -1;
        }
        break;


    case KEY_SPACE:
        if (!shiftpressed)
        {
            if (eseditpos < songlen_stereo[esnum][eschn]) espos[eschn] = eseditpos;
            if (esend[eschn] < espos[eschn]) esend[eschn] = 0;
        }
        else
        {
            for (c = 0; c < maxChns; c++)
            {
                if (eseditpos < songlen_stereo[esnum][c]) espos[c] = eseditpos;
                if (esend[c] < espos[c]) esend[c] = 0;
            }
        }
        break;

    case KEY_BACKSPACE:
        if (!shiftpressed)
        {
            if ((esend[eschn] != eseditpos) && (eseditpos > espos[eschn]))
            {
                if (eseditpos < songlen_stereo[esnum][eschn]) esend[eschn] = eseditpos;
            }
            else esend[eschn] = 0;
        }
        else
        {
            if ((esend[eschn] != eseditpos) && (eseditpos > espos[eschn]))
            {
                for (c = 0; c < maxChns; c++)
                {
                    if (eseditpos < songlen_stereo[esnum][c]) esend[c] = eseditpos;
                }
            }
            else
            {
                for (c = 0; c < maxChns; c++) esend[c] = 0;
            }
        }
        break;

    case KEY_ENTER:
        if (eseditpos < songlen_stereo[esnum][eschn])
        {
            if (!shiftpressed)
            {
                if (songorder_stereo[esnum][eschn][eseditpos] < MAX_PATT)
                    epnum[eschn] = songorder_stereo[esnum][eschn][eseditpos];
            }
            else
            {
                int c, d;

                for (c = 0; c < maxChns; c++)
                {
                    int start;

                    if (eseditpos != espos[eschn]) start = eseditpos;
                    else start = espos[c];

                    for (d = start; d < songlen_stereo[esnum][c]; d++)
                    {
                        if (songorder_stereo[esnum][c][d] < MAX_PATT)
                        {
                            epnum[c] = songorder_stereo[esnum][c][d];
                            break;
                        }
                    }
                }
            }
            epmarkchn = -1;
        }
        epchn = eschn;
        epcolumn = 0;
        eppos = 0;
        epview = - VISIBLEPATTROWS/2;
        editmode = EDIT_PATTERN;
        if (epchn == epmarkchn) epmarkchn = -1;
        break;

    case KEY_DEL:
        esmarkchn = -1;
        deleteorder_stereo();
        break;

    case KEY_INS:
        esmarkchn = -1;
        insertorder_stereo(0);
        break;

    case KEY_HOME:
        if (songlen_stereo[esnum][eschn])
        {
            while ((eseditpos != 0) || (escolumn != 0)) orderleft_stereo();
        }
        break;

    case KEY_END:
        while (eseditpos != songlen_stereo[esnum][eschn]+1) orderright_stereo();
        break;

    case KEY_PGUP:
        for (scrrep = PGUPDNREPEAT * 2; scrrep; scrrep--)
            orderleft_stereo();
        break;

    case KEY_PGDN:
        for (scrrep = PGUPDNREPEAT * 2; scrrep; scrrep--)
            orderright_stereo();
        break;

    case KEY_LEFT:
        orderleft_stereo();
        break;

    case KEY_RIGHT:
        orderright_stereo();
        break;

    case KEY_UP:
        eschn--;
        if (eschn < 0) eschn = maxChns - 1;
        if ((eseditpos == songlen_stereo[esnum][eschn]) || (eseditpos > songlen_stereo[esnum][eschn]+1))
        {
            eseditpos = songlen_stereo[esnum][eschn]+1;
            escolumn = 0;
        }
        if (shiftpressed) esmarkchn = -1;
        break;

    case KEY_DOWN:
        eschn++;
        if (eschn >= maxChns) eschn = 0;
        if ((eseditpos == songlen_stereo[esnum][eschn]) || (eseditpos > songlen_stereo[esnum][eschn]+1))
        {
            eseditpos = songlen_stereo[esnum][eschn]+1;
            escolumn = 0;
        }
        if (shiftpressed) esmarkchn = -1;
        break;
    }
    if (eseditpos - esview < 0)
    {
        esview = eseditpos;
    }
    if (eseditpos - esview >= VISIBLEORDERLIST)
    {
        esview = eseditpos - VISIBLEORDERLIST + 1;
    }
}

void namecommands(void)
{
    switch(rawkey)
    {
    case KEY_DOWN:
    case KEY_ENTER:
        enpos++;
        if (enpos > 2) enpos = 0;
        break;

    case KEY_UP:
        enpos--;
        if (enpos < 0) enpos = 2;
        break;
    }
    switch(enpos)
    {
    case 0:
        editstring(songname, MAX_STR);
        break;

    case 1:
        editstring(authorname, MAX_STR);
        break;

    case 2:
        editstring(copyrightname, MAX_STR);
        break;
    }
}

void insertorder(unsigned char byte)
{
    if ((songlen[esnum][eschn] - eseditpos)-1 >= 0)
    {
        int len;
        if (songlen[esnum][eschn] < MAX_SONGLEN)
        {
            len = songlen[esnum][eschn]+1;
            songorder[esnum][eschn][len+1] =
                songorder[esnum][eschn][len];
            songorder[esnum][eschn][len] = LOOPSONG;
            if (len) songorder[esnum][eschn][len-1] = byte;
            countthispattern();
        }
        memmove(&songorder[esnum][eschn][eseditpos+1],
                &songorder[esnum][eschn][eseditpos],
                (songlen[esnum][eschn] - eseditpos)-1);
        songorder[esnum][eschn][eseditpos] = byte;
        len = songlen[esnum][eschn]+1;
        if ((songorder[esnum][eschn][len] > eseditpos) &&
                (songorder[esnum][eschn][len] < (len-2)))
            songorder[esnum][eschn][len]++;
    }
    else
    {
        if (eseditpos > songlen[esnum][eschn])
        {
            if (songlen[esnum][eschn] < MAX_SONGLEN)
            {
                songorder[esnum][eschn][eseditpos+1] =
                    songorder[esnum][eschn][eseditpos];
                songorder[esnum][eschn][eseditpos] = LOOPSONG;
                if (eseditpos) songorder[esnum][eschn][eseditpos-1] = byte;
                countthispattern();
                eseditpos = songlen[esnum][eschn]+1;
            }
        }
    }
}

void insertorder_stereo(unsigned char byte)
{
    if ((songlen_stereo[esnum][eschn] - eseditpos)-1 >= 0)
    {
        int len;
        if (songlen_stereo[esnum][eschn] < MAX_SONGLEN)
        {
            len = songlen_stereo[esnum][eschn]+1;
            songorder_stereo[esnum][eschn][len+1] =
                songorder_stereo[esnum][eschn][len];
            songorder_stereo[esnum][eschn][len] = LOOPSONG;
            if (len) songorder_stereo[esnum][eschn][len-1] = byte;
            countthispattern();
        }
        memmove(&songorder_stereo[esnum][eschn][eseditpos+1],
                &songorder_stereo[esnum][eschn][eseditpos],
                (songlen_stereo[esnum][eschn] - eseditpos)-1);
        songorder_stereo[esnum][eschn][eseditpos] = byte;
        len = songlen_stereo[esnum][eschn]+1;
        if ((songorder_stereo[esnum][eschn][len] > eseditpos) &&
                (songorder_stereo[esnum][eschn][len] < (len-2)))
            songorder_stereo[esnum][eschn][len]++;
    }
    else
    {
        if (eseditpos > songlen_stereo[esnum][eschn])
        {
            if (songlen_stereo[esnum][eschn] < MAX_SONGLEN)
            {
                songorder_stereo[esnum][eschn][eseditpos+1] =
                    songorder_stereo[esnum][eschn][eseditpos];
                songorder_stereo[esnum][eschn][eseditpos] = LOOPSONG;
                if (eseditpos) songorder_stereo[esnum][eschn][eseditpos-1] = byte;
                countthispattern();
                eseditpos = songlen_stereo[esnum][eschn]+1;
            }
        }
    }
}

void deleteorder(void)
{
    if ((songlen[esnum][eschn] - eseditpos)-1 >= 0)
    {
        int len;
        memmove(&songorder[esnum][eschn][eseditpos],
                &songorder[esnum][eschn][eseditpos+1],
                (songlen[esnum][eschn] - eseditpos)-1);
        songorder[esnum][eschn][songlen[esnum][eschn]-1] = 0x00;
        if (songlen[esnum][eschn] > 0)
        {
            songorder[esnum][eschn][songlen[esnum][eschn]-1] =
                songorder[esnum][eschn][songlen[esnum][eschn]];
            songorder[esnum][eschn][songlen[esnum][eschn]] =
                songorder[esnum][eschn][songlen[esnum][eschn]+1];
            countthispattern();
        }
        if (eseditpos == songlen[esnum][eschn]) eseditpos++;
        len = songlen[esnum][eschn]+1;
        if ((songorder[esnum][eschn][len] > eseditpos) &&
                (songorder[esnum][eschn][len] > 0))
            songorder[esnum][eschn][len]--;
    }
    else
    {
        if (eseditpos > songlen[esnum][eschn])
        {
            if (songlen[esnum][eschn] > 0)
            {
                songorder[esnum][eschn][songlen[esnum][eschn]-1] =
                    songorder[esnum][eschn][songlen[esnum][eschn]];
                songorder[esnum][eschn][songlen[esnum][eschn]] =
                    songorder[esnum][eschn][songlen[esnum][eschn]+1];
                countthispattern();
                eseditpos = songlen[esnum][eschn]+1;
            }
        }
    }
}

void deleteorder_stereo(void)
{
    if ((songlen_stereo[esnum][eschn] - eseditpos)-1 >= 0)
    {
        int len;
        memmove(&songorder_stereo[esnum][eschn][eseditpos],
                &songorder_stereo[esnum][eschn][eseditpos+1],
                (songlen_stereo[esnum][eschn] - eseditpos)-1);
        songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]-1] = 0x00;
        if (songlen_stereo[esnum][eschn] > 0)
        {
            songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]-1] =
                songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]];
            songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]] =
                songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]+1];
            countthispattern();
        }
        if (eseditpos == songlen_stereo[esnum][eschn]) eseditpos++;
        len = songlen_stereo[esnum][eschn]+1;
        if ((songorder_stereo[esnum][eschn][len] > eseditpos) &&
                (songorder_stereo[esnum][eschn][len] > 0))
            songorder_stereo[esnum][eschn][len]--;
    }
    else
    {
        if (eseditpos > songlen_stereo[esnum][eschn])
        {
            if (songlen_stereo[esnum][eschn] > 0)
            {
                songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]-1] =
                    songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]];
                songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]] =
                    songorder_stereo[esnum][eschn][songlen_stereo[esnum][eschn]+1];
                countthispattern();
                eseditpos = songlen_stereo[esnum][eschn]+1;
            }
        }
    }
}

void orderleft(void)
{
    if ((shiftpressed) && (eseditpos < songlen[esnum][eschn]))
    {
        if ((esmarkchn != eschn) || (eseditpos != esmarkend))
        {
            esmarkchn = eschn;
            esmarkstart = esmarkend = eseditpos;
        }
    }
    escolumn--;
    if (escolumn < 0)
    {
        if (eseditpos > 0)
        {
            eseditpos--;
            if (eseditpos == songlen[esnum][eschn]) eseditpos--;
            escolumn = 1;
            if (eseditpos < 0)
            {
                eseditpos = 1;
                escolumn = 0;
            }
        }
        else escolumn = 0;
    }
    if ((shiftpressed) && (eseditpos < songlen[esnum][eschn])) esmarkend = eseditpos;
}

void orderleft_stereo(void)
{
    if ((shiftpressed) && (eseditpos < songlen_stereo[esnum][eschn]))
    {
        if ((esmarkchn != eschn) || (eseditpos != esmarkend))
        {
            esmarkchn = eschn;
            esmarkstart = esmarkend = eseditpos;
        }
    }
    escolumn--;
    if (escolumn < 0)
    {
        if (eseditpos > 0)
        {
            eseditpos--;
            if (eseditpos == songlen_stereo[esnum][eschn]) eseditpos--;
            escolumn = 1;
            if (eseditpos < 0)
            {
                eseditpos = 1;
                escolumn = 0;
            }
        }
        else escolumn = 0;
    }
    if ((shiftpressed) && (eseditpos < songlen_stereo[esnum][eschn])) esmarkend = eseditpos;
}

void orderright(void)
{
    if ((shiftpressed) && (eseditpos < songlen[esnum][eschn]))
    {
        if ((esmarkchn != eschn) || (eseditpos != esmarkend))
        {
            esmarkchn = eschn;
            esmarkstart = esmarkend = eseditpos;
        }
    }
    escolumn++;
    if (escolumn > 1)
    {
        escolumn = 0;
        if (eseditpos < (songlen[esnum][eschn]+1))
        {
            eseditpos++;
            if (eseditpos == songlen[esnum][eschn]) eseditpos++;
        }
        else escolumn = 1;
    }
    if ((shiftpressed) && (eseditpos < songlen[esnum][eschn])) esmarkend = eseditpos;
}

void orderright_stereo(void)
{
    if ((shiftpressed) && (eseditpos < songlen_stereo[esnum][eschn]))
    {
        if ((esmarkchn != eschn) || (eseditpos != esmarkend))
        {
            esmarkchn = eschn;
            esmarkstart = esmarkend = eseditpos;
        }
    }
    escolumn++;
    if (escolumn > 1)
    {
        escolumn = 0;
        if (eseditpos < (songlen_stereo[esnum][eschn]+1))
        {
            eseditpos++;
            if (eseditpos == songlen_stereo[esnum][eschn]) eseditpos++;
        }
        else escolumn = 1;
    }
    if ((shiftpressed) && (eseditpos < songlen_stereo[esnum][eschn])) esmarkend = eseditpos;
}

void nextsong(void)
{
    esnum++;
    if (esnum >= MAX_SONGS) esnum = MAX_SONGS - 1;
    songchange();
}

void prevsong(void)
{
    esnum--;
    if (esnum < 0) esnum = 0;
    songchange();
}

void songchange(void)
{
    int c;
    int maxChns = MAX_CHN;
    if (numsids == 1) maxChns = 3;
    int currentSonglen = 0;
    if (numsids == 1)
    {
        currentSonglen = songlen[esnum][eschn];
    }
    else if (numsids == 2)
    {
        currentSonglen = songlen_stereo[esnum][eschn];
    }

    for (c = 0; c < maxChns; c++)
    {
        espos[c] = 0;
        esend[c] = 0;
        epnum[c] = c;
    }
    updateviewtopos();

    eppos = 0;
    epview = - VISIBLEPATTROWS/2;
    eseditpos = 0;
    if (eseditpos == currentSonglen) eseditpos++;
    esview = 0;
    epmarkchn = -1;
    esmarkchn = -1;
    stopsong();
}

void updateviewtopos(void)
{
    int c, d;
    int maxChns = MAX_CHN;
    if (numsids == 1) maxChns = 3;
    int currentSonglen = 0;
    int currentSongorder = 0;

    for (c = 0; c < maxChns; c++)
    {
        if (numsids == 1)
        {
            currentSonglen = songlen[esnum][c];
        }
        else if (numsids == 2)
        {
            currentSonglen = songlen_stereo[esnum][c];
        }
        for (d = espos[c]; d < currentSonglen; d++)
        {
            if (numsids == 1)
            {
                currentSongorder = songorder[esnum][c][d];
            }
            else if (numsids == 2)
            {
                currentSongorder = songorder_stereo[esnum][c][d];
            }
            if (currentSongorder < MAX_PATT)
            {
                epnum[c] = currentSongorder;
                break;
            }
        }
    }
}
