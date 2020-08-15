/*
 * GOATTRACKER "console" output routines
 */

#define GCONSOLE_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gt2fork.h"

int gfxinitted = 0;
unsigned *scrbuffer = NULL;
unsigned *prevscrbuffer = NULL;
unsigned char *chardata = NULL;
int key = 0;
int rawkey = 0;
int shiftpressed = 0;
int cursorflashdelay = 0;
int mouseb = 0;
int prevmouseb = 0;
unsigned mousex = 0;
unsigned mousey = 0;
unsigned mousepixelx = 0;
unsigned mousepixely = 0;
unsigned oldmousepixelx = 0xffffffff;
unsigned oldmousepixely = 0xffffffff;
int mouseheld = 0;
int region[MAX_ROWS];
int fontwidth = 8;
int fontheight = 14;
int mousesizex = 11;
int mousesizey = 20;

void loadexternalpalette(void);
void initicon(void);

inline void setcharcolor(unsigned *dptr, short ch, short color)
{
    *dptr = (ch & 0xff) | (color << 16);
}

inline void setcolor(unsigned *dptr, short color)
{
    *dptr = (*dptr & 0xffff) | (color << 16);
}

int initscreen(void)
{
    int handle;
    unsigned xsize = MAX_COLUMNS * 8;
    unsigned ysize = MAX_ROWS * 16;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
    {
        return 0;
    }
    win_openwindow(xsize, ysize, "GoatTracker", NULL);
    // win_setmousemode(MOUSE_ALWAYS_HIDDEN);
    initicon();

    if (!gfx_init(MAX_COLUMNS * fontwidth, MAX_ROWS * fontheight, 60, 0))
    {
        win_fullscreen = 0;
        if (!gfx_init(MAX_COLUMNS * fontwidth, MAX_ROWS * fontheight, 60, 0))
        {
            return 0;
        }
    }

    scrbuffer = (unsigned*)malloc(MAX_COLUMNS * MAX_ROWS * sizeof(unsigned));
    prevscrbuffer = (unsigned*)malloc(MAX_COLUMNS * MAX_ROWS * sizeof(unsigned));
    if ((!scrbuffer) || (!prevscrbuffer)) return 0;

    memset(region, 0, sizeof region);

    chardata = (unsigned char*)malloc(4096);
    if (!chardata) return 0;
    handle = io_open("chargen.bin");
    if (handle == -1) return 0;
    io_read(handle, &chardata[0], 4096);
    io_close(handle);

    gfx_loadpalette("palette.bin");
    loadexternalpalette();
    gfx_setpalette();

    gfxinitted = 1;
    clearscreen();
    atexit(closescreen);
    return 1;
}

void loadexternalpalette(void)
{
    FILE *ext_f;
    if ((ext_f = fopen("custom.pal", "rt")))
    {
        int p = 0;
        char ln[100];
        strcpy(ln, "");
        fgets(ln, sizeof(ln), ext_f);

        if (strncmp("JASC-PAL", ln, 8) == 0)
        {
            int colors;
            fgets(ln, sizeof(ln), ext_f);
            fgets(ln, sizeof(ln), ext_f);
            if (sscanf(ln, "%d", &colors) == 1 && colors == 256)
            {
                while (!feof(ext_f))
                {
                    int r, g, b;
                    if (!fgets(ln, sizeof(ln), ext_f)) break;
                    if (sscanf(ln, "%d %d %d", &r, &g, &b) == 3)
                    {
                        // JASC palette is 8-bit and goat palette is 6-bit
                        gfx_palette[p++] = r / 4;
                        gfx_palette[p++] = g / 4;
                        gfx_palette[p++] = b / 4;
                    }

                    if (p >= 768) break;
                }
                gfx_calcpalette(64, 0, 0, 0);
            }
        }

        fclose(ext_f);
    }
}

void initicon(void)
{
    int handle = io_open("goattrk2.bmp");
    if (handle != -1)
    {
        SDL_RWops *rw;
        SDL_Surface *icon;
        char *iconbuffer;
        int size;

        size = io_lseek(handle, 0, SEEK_END);
        io_lseek(handle, 0, SEEK_SET);
        iconbuffer = (char*)malloc(size);
        if (iconbuffer)
        {
            io_read(handle, iconbuffer, size);
            io_close(handle);
            rw = SDL_RWFromMem(iconbuffer, size);
            icon = SDL_LoadBMP_RW(rw, 0);
            SDL_SetWindowIcon(win_window, icon);
            free(iconbuffer);
        }
    }
}

void closescreen(void)
{
    if (scrbuffer)
    {
        free(scrbuffer);
        scrbuffer = NULL;
    }
    if (prevscrbuffer)
    {
        free(prevscrbuffer);
        prevscrbuffer = NULL;
    }
    if (chardata)
    {
        free(chardata);
        chardata = NULL;
    }

    gfxinitted = 0;
}

void clearscreen(void)
{
    int c;
    unsigned *dptr = scrbuffer;

    if (!gfxinitted) return;

    for (c = 0; c < MAX_ROWS * MAX_COLUMNS; c++)
    {
        setcharcolor(dptr, 0x20, (colscheme.bgcolor << 4));
        dptr++;
    }
}

void printtext(int x, int y, int color, const char *text)
{
    unsigned *dptr = scrbuffer + (x + y * MAX_COLUMNS);

    if (!gfxinitted) return;
    if (y < 0) return;
    if (y >= MAX_ROWS) return;
    while (*text)
    {
        setcharcolor(dptr, *text, color);
        dptr++;
        text++;
    }
}

void printtextc(int y, int color, const char *text)
{
    int x = (MAX_COLUMNS - strlen(text)) / 2;

    printtext(x, y, color, text);
}

void printtextcp(int cp, int y, int color, const char *text)
{
    int x = cp - (strlen(text) / 2);

    printtext(x, y, color, text);
}


void printblank(int x, int y, int length)
{
    unsigned *dptr = scrbuffer + (x + y * MAX_COLUMNS);

    if (!gfxinitted) return;
    if (y < 0) return;
    if (y >= MAX_ROWS) return;
    while (length--)
    {
        setcharcolor(dptr, 0x20, colscheme.bgcolor);
        dptr++;
    }
}

void printblankc(int x, int y, int color, int length)
{
    unsigned *dptr = scrbuffer + (x + y * MAX_COLUMNS);

    if (!gfxinitted) return;
    if (y < 0) return;
    if (y >= MAX_ROWS) return;
    while (length--)
    {
        setcharcolor(dptr, 0x20, color);
        dptr++;
    }
}

void drawbox(int x, int y, int color, int sx, int sy)
{
    unsigned *dptr;
    unsigned *dptr2;
    int counter;

    if (!gfxinitted) return;
    if (y < 0) return;
    if (y >= MAX_ROWS) return;
    if (y+sy > MAX_ROWS) return;
    if ((!sx) || (!sy)) return;

    dptr = scrbuffer + (x + y * MAX_COLUMNS);
    dptr2 = scrbuffer + ((x+sx-1) + y * MAX_COLUMNS);
    counter = sy;

    while (counter--)
    {
        setcharcolor(dptr, '|', color);
        setcharcolor(dptr2, '|', color);
        dptr += MAX_COLUMNS;
        dptr2 += MAX_COLUMNS;
    }

    dptr = scrbuffer + (x + y * MAX_COLUMNS);
    dptr2 = scrbuffer + (x + (y+sy-1) * MAX_COLUMNS);
    counter = sx;

    while (counter--)
    {
        setcharcolor(dptr, '-', color);
        setcharcolor(dptr2, '-', color);
        dptr++;
        dptr2++;
    }

    dptr = scrbuffer + (x + y * MAX_COLUMNS);
    setcharcolor(dptr, '+', color);

    dptr = scrbuffer + ((x+sx-1) + y * MAX_COLUMNS);
    setcharcolor(dptr, '+', color);

    dptr = scrbuffer + (x + (y+sy-1) * MAX_COLUMNS);
    setcharcolor(dptr, '+', color);

    dptr = scrbuffer + ((x+sx-1) + (y+sy-1) * MAX_COLUMNS);
    setcharcolor(dptr, '+', color);
}

void printbg(int x, int y, int color, int length)
{
    unsigned *dptr = scrbuffer + (x + y * MAX_COLUMNS);

    if (!gfxinitted) return;
    if (y < 0) return;
    if (y >= MAX_ROWS) return;
    while (length--)
    {
        setcolor(dptr, 15 | (color << 4));
        dptr++;
    }
}

void fliptoscreen(void)
{
    unsigned *sptr = scrbuffer;
    unsigned *cmpptr = prevscrbuffer;
    int x,y;
    int regionschanged = 0;

    if (!gfxinitted) return;

    // If redraw requested, mark whole screen changed
    if (gfx_redraw)
    {
        gfx_redraw = 0;
        memset(prevscrbuffer, 0xff, MAX_COLUMNS*MAX_ROWS*sizeof(unsigned));
    }

    if (!gfx_lock()) return;

    // Now redraw text on changed areas
    for (y = 0; y < MAX_ROWS; y++)
    {
        for (x = 0; x < MAX_COLUMNS; x++)
        {
            // Check if char changed
            if (*sptr != *cmpptr)
            {
                *cmpptr = *sptr;
                region[y] = 1;
                regionschanged = 1;

                unsigned char *chptr = &chardata[(*sptr & 0xffff)*16];
                unsigned char *dptr = (unsigned char*)gfx_screen->pixels + y*fontheight * gfx_screen->pitch + x*fontwidth;
                unsigned char bgcolor = (*sptr) >> 20;
                unsigned char fgcolor = ((*sptr) >> 16) & 0xf;
                int c;
                unsigned char e = *chptr++;

                for (c = 0; c < 14; c++)
                {
                    e = *chptr++;

                    if (e & 128) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    if (e & 64) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    if (e & 32) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    if (e & 16) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    if (e & 8) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    if (e & 4) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    if (e & 2) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    if (e & 1) *dptr++ = fgcolor;
                    else *dptr++ = bgcolor;
                    dptr += gfx_screen->pitch - fontwidth;
                }

            }
            sptr++;
            cmpptr++;
        }
    }

    // Redraw changed screen regions
    gfx_unlock();
    gfx_flip();
}

void getkey(void)
{
    int c;
    win_asciikey = 0;
    cursorflashdelay += win_getspeed(50);

    prevmouseb = mouseb;

    mou_getpos(&mousepixelx, &mousepixely);
    mouseb = mou_getbuttons();
    mousex = mousepixelx / fontwidth;
    mousey = mousepixely / fontheight;

    if (mouseb) mouseheld++;
    else mouseheld = 0;

    key = win_asciikey;
    rawkey = 0;
    for (c = 0; c < SDL_NUM_SCANCODES; c++)
    {
        if (win_keytable[c])
        {
            if ((c != SDL_SCANCODE_LSHIFT) && (c != SDL_SCANCODE_RSHIFT) &&
                    (c != SDL_SCANCODE_LCTRL) && (c != SDL_SCANCODE_RCTRL))
            {
                rawkey = c;
                win_keytable[c] = 0;
                break;
            }
        }
    }

    shiftpressed = 0;
    if ((win_keystate[SDL_SCANCODE_LSHIFT])||
            (win_keystate[SDL_SCANCODE_RSHIFT])||
            (win_keystate[SDL_SCANCODE_LCTRL])||
            (win_keystate[SDL_SCANCODE_RCTRL]))
    {
        shiftpressed = 1;
    }

    if (rawkey == SDL_SCANCODE_KP_ENTER)
    {
        key = KEY_ENTER;
        rawkey = SDL_SCANCODE_RETURN;
    }

    if (rawkey == SDL_SCANCODE_KP_0) key = '0';
    if (rawkey == SDL_SCANCODE_KP_1) key = '1';
    if (rawkey == SDL_SCANCODE_KP_2) key = '2';
    if (rawkey == SDL_SCANCODE_KP_3) key = '3';
    if (rawkey == SDL_SCANCODE_KP_4) key = '4';
    if (rawkey == SDL_SCANCODE_KP_5) key = '5';
    if (rawkey == SDL_SCANCODE_KP_6) key = '6';
    if (rawkey == SDL_SCANCODE_KP_7) key = '7';
    if (rawkey == SDL_SCANCODE_KP_8) key = '8';
    if (rawkey == SDL_SCANCODE_KP_9) key = '9';
}
