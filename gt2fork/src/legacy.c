#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef __WIN32__
#include <SDL2/SDL_types.h>
#else
#include <SDL_types.h>
#endif

#include "legacy.h"
#include "palette.h"

/**
 * =============================================================================
 * FILE bme.c:
 * =============================================================================
 */
//
// BME (Blasphemous Multimedia Engine) main module
//

int bme_error = BME_OK;
/**
 * =============================================================================
 * FILE bme_end.c:
 * =============================================================================
 */

void fwrite8(FILE *file, unsigned data)
{
    Uint8 bytes[1];

    bytes[0] = data;
    fwrite(bytes, 1, 1, file);
}

void fwritele16(FILE *file, unsigned data)
{
    Uint8 bytes[2];

    bytes[0] = data;
    bytes[1] = data >> 8;
    fwrite(bytes, 2, 1, file);
}

void fwritele32(FILE *file, unsigned data)
{
    Uint8 bytes[4];

    bytes[0] = data;
    bytes[1] = data >> 8;
    bytes[2] = data >> 16;
    bytes[3] = data >> 24;
    fwrite(bytes, 4, 1, file);
}

unsigned fread8(FILE *file)
{
    Uint8 bytes[1];

    fread(bytes, 1, 1, file);
    return bytes[0];
}

unsigned freadle16(FILE *file)
{
    Uint8 bytes[2];

    fread(bytes, 2, 1, file);
    return (bytes[0]) | (bytes[1] << 8);
}

unsigned freadle32(FILE *file)
{
    Uint8 bytes[4];

    fread(bytes, 4, 1, file);
    return (bytes[0]) | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

unsigned freadhe16(FILE *file)
{
    Uint8 bytes[2];

    fread(bytes, 2, 1, file);
    return (bytes[1]) | (bytes[0] << 8);
}

unsigned freadhe32(FILE *file)
{
    Uint8 bytes[4];

    fread(bytes, 4, 1, file);
    return (bytes[3]) | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
}


/**
 * =============================================================================
 * FILE bme_gfx.c:
 * =============================================================================
 */
//
// BME (Blasphemous Multimedia Engine) graphics main module
//

// Prototypes

int gfx_init(unsigned xsize, unsigned ysize, unsigned framerate, unsigned flags);
void gfx_reinit(void);
void gfx_uninit(void);
int gfx_lock(void);
void gfx_unlock(void);
void gfx_flip(void);
void gfx_setclipregion(unsigned left, unsigned top, unsigned right, unsigned bottom);
void gfx_setmaxspritefiles(int num);
void gfx_setmaxcolors(int num);
int gfx_loadpalette(char *name);
void gfx_calcpalette(int fade, int radd, int gadd, int badd);
void gfx_setpalette(void);
int gfx_loadblocks(char *name);
int gfx_loadsprites(int num, char *name);
void gfx_freesprites(int num);

int gfx_initted = 0;
int gfx_redraw = 0;
int gfx_fullscreen = 0;
int gfx_scanlinemode = 0;
int gfx_preventswitch = 0;
unsigned gfx_virtualxsize;
unsigned gfx_virtualysize;
unsigned gfx_windowxsize;
unsigned gfx_windowysize;
int gfx_blockxsize = 16;
int gfx_blockysize = 16;
int spr_xsize = 0;
int spr_ysize = 0;
int spr_xhotspot = 0;
int spr_yhotspot = 0;
unsigned gfx_nblocks = 0;
SDL_Surface *gfx_screen = NULL;
SDL_Renderer *gfx_renderer = NULL;

// Static variables

static int gfx_initexec = 0;
static int gfx_last_xsize;
static int gfx_last_ysize;
static int gfx_last_x;
static int gfx_last_y;
static unsigned gfx_last_framerate;
static int gfx_cliptop;
static int gfx_clipbottom;
static int gfx_clipleft;
static int gfx_clipright;
static int gfx_maxcolors = MAX_COLORS;
static int gfx_maxspritefiles = 0;
static SPRITEHEADER **gfx_spriteheaders = NULL;
static Uint8 **gfx_spritedata = NULL;
static unsigned *gfx_spriteamount = NULL;
static SDL_Color gfx_sdlpalette[MAX_COLORS];
static int gfx_locked = 0;
static SDL_Texture *sdlTexture = NULL;

int gfx_init(unsigned xsize, unsigned ysize, unsigned framerate, unsigned flags)
{
    int sdlflags = SDL_RENDERER_ACCELERATED;

    // Prevent re-entry (by window procedure)
    if (gfx_initexec) return BME_OK;
    gfx_initexec = 1;

    gfx_last_xsize = xsize;
    gfx_last_ysize = ysize;
    gfx_last_framerate = framerate;

    if (win_fullscreen) gfx_fullscreen = 1;
    else gfx_fullscreen = 0;

    // Calculate virtual window size

    gfx_virtualxsize = xsize;
    gfx_virtualysize = ysize;

    if ((!gfx_virtualxsize) || (!gfx_virtualysize))
    {
        gfx_initexec = 0;
        gfx_uninit();
        bme_error = BME_ILLEGAL_CONFIG;
        return BME_ERROR;
    }

    // Calculate actual window size (for scanline mode & doublesize mode
    // this is double the virtual)

    gfx_windowxsize = gfx_virtualxsize;
    gfx_windowysize = gfx_virtualysize;

    gfx_setclipregion(0, 0, gfx_virtualxsize, gfx_virtualysize);

    // Colors 0 & 255 are always black & white
    gfx_sdlpalette[0].r = 0;
    gfx_sdlpalette[0].g = 0;
    gfx_sdlpalette[0].b = 0;
    gfx_sdlpalette[0].a = 255;
    gfx_sdlpalette[255].r = 255;
    gfx_sdlpalette[255].g = 255;
    gfx_sdlpalette[255].b = 255;
    gfx_sdlpalette[255].a = 255;

    gfx_renderer = SDL_CreateRenderer(win_window, -1, sdlflags);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    // Set Logical Size
    SDL_RenderSetLogicalSize(gfx_renderer, xsize, ysize);
    gfx_screen = SDL_CreateRGBSurfaceWithFormat(0, xsize, ysize, 8, SDL_PIXELFORMAT_INDEX8);
    sdlTexture = SDL_CreateTexture(gfx_renderer,
                                   SDL_PIXELFORMAT_RGBA32,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   xsize, ysize);

    SDL_GetWindowPosition( win_window, &gfx_last_x, &gfx_last_y );
    gfx_reinit();

    gfx_initexec = 0;
    if (gfx_screen)
    {
        gfx_initted = 1;
        gfx_redraw = 1;
        gfx_setpalette();
        win_setmousemode(win_mousemode);
        return BME_OK;
    }
    else return BME_ERROR;
}

void gfx_reinit(void)
{
    if (win_fullscreen)
    {
        SDL_GetWindowSize( win_window, &gfx_last_xsize, &gfx_last_ysize );
        SDL_GetWindowPosition( win_window, &gfx_last_x, &gfx_last_y );
        SDL_SetWindowFullscreen( win_window, SDL_WINDOW_FULLSCREEN_DESKTOP );
    }
    else
    {
        SDL_SetWindowFullscreen( win_window, 0 );
        SDL_SetWindowSize( win_window, gfx_last_xsize, gfx_last_ysize );
        SDL_SetWindowPosition( win_window, gfx_last_x, gfx_last_y );
    }
}

void gfx_uninit(void)
{
    SDL_DestroyTexture(sdlTexture);
    SDL_FreeSurface(gfx_screen);
    SDL_DestroyRenderer(gfx_renderer);
    gfx_initted = 0;
    return;
}

int gfx_lock(void)
{
    if (gfx_locked) return 1;
    if (!gfx_initted) return 0;
    if (!SDL_LockSurface(gfx_screen))
    {
        gfx_locked = 1;
        return 1;
    }
    else return 0;
}

void gfx_unlock(void)
{
    if (gfx_locked)
    {
        SDL_UnlockSurface(gfx_screen);
        gfx_locked = 0;
    }
}

void gfx_flip()
{
    SDL_Surface* surf = SDL_ConvertSurfaceFormat(gfx_screen, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_UpdateTexture(sdlTexture, NULL, surf->pixels, surf->pitch);
    SDL_FreeSurface(surf);
    SDL_RenderClear(gfx_renderer);
    SDL_RenderCopy(gfx_renderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(gfx_renderer);
    gfx_redraw = 0;
}

void gfx_setmaxcolors(int num)
{
    gfx_maxcolors = num;
}

int gfx_loadpalette(char *name)
{
    //~ int handle;

    //~ handle = io_open(name);
    //~ if (handle == -1)
    //~ {
        //~ bme_error = BME_OPEN_ERROR;
        //~ return BME_ERROR;
    //~ }
    //~ if (io_read(handle, gfx_palette, gfx_paletteLength) != gfx_paletteLength)
    //~ {
        //~ printf("BME_ERROR read palette\n");
        //~ bme_error = BME_READ_ERROR;
        //~ io_close(handle);
        //~ return BME_ERROR;
    //~ }
    //~ else
    //~ {
        //~ printf("BME_OK read palette\n");
    //~ }

    //~ io_close(handle);
    gfx_calcpalette(64, 0, 0, 0);
    bme_error = BME_OK;
    return BME_OK;
}

void gfx_calcpalette(int fade, int radd, int gadd, int badd)
{
    // Uint8  *sptr = &gfx_palette[3];
    unsigned char *sptr = &gfx_palette[3];
    int c, cl;
    if (radd < 0) radd = 0;
    if (gadd < 0) gadd = 0;
    if (badd < 0) badd = 0;

    for (c = 1; c < 255; c++)
    {
        cl = *sptr;
        cl *= fade;
        cl >>= 6;
        cl += radd;
        if (cl > 63) cl = 63;
        if (cl < 0) cl = 0;
        gfx_sdlpalette[c].r = (cl << 2) | (cl & 3);
        sptr++;

        cl = *sptr;
        cl *= fade;
        cl >>= 6;
        cl += gadd;
        if (cl > 63) cl = 63;
        if (cl < 0) cl = 0;
        gfx_sdlpalette[c].g = (cl << 2) | (cl & 3);
        sptr++;

        cl = *sptr;
        cl *= fade;
        cl >>= 6;
        cl += badd;
        if (cl > 63) cl = 63;
        if (cl < 0) cl = 0;
        gfx_sdlpalette[c].b = (cl << 2) | (cl & 3);
        sptr++;

        gfx_sdlpalette[c].a = 255;
    }
}

void gfx_setpalette(void)
{
    if (!gfx_initted) return;

    SDL_SetPaletteColors(gfx_screen->format->palette, &gfx_sdlpalette[0], 0, gfx_maxcolors);
}

void gfx_setclipregion(unsigned left, unsigned top, unsigned right, unsigned bottom)
{
    if (left >= right) return;
    if (top >= bottom) return;
    if (left >= gfx_virtualxsize) return;
    if (top >= gfx_virtualysize) return;
    if (right > gfx_virtualxsize) return;
    if (bottom > gfx_virtualysize) return;

    gfx_clipleft = left;
    gfx_clipright = right;
    gfx_cliptop = top;
    gfx_clipbottom = bottom;
}

void gfx_setmaxspritefiles(int num)
{
    if (num <= 0) return;

    if (gfx_spriteheaders) return;

    gfx_spriteheaders = malloc(num * sizeof(Uint8 *));
    gfx_spritedata = malloc(num * sizeof(Uint8 *));
    gfx_spriteamount = malloc(num * sizeof(unsigned));
    if ((gfx_spriteheaders) && (gfx_spritedata) && (gfx_spriteamount))
    {
        int c;

        gfx_maxspritefiles = num;
        for (c = 0; c < num; c++)
        {
            gfx_spriteamount[c] = 0;
            gfx_spritedata[c] = NULL;
            gfx_spriteheaders[c] = NULL;
        }
    }
    else gfx_maxspritefiles = 0;
}

int gfx_loadsprites(int num, char *name)
{
    int handle, size, c;
    int datastart;

    if (!gfx_spriteheaders)
    {
        gfx_setmaxspritefiles(DEFAULT_MAX_SPRFILES);
    }

    bme_error = BME_OPEN_ERROR;
    if (num >= gfx_maxspritefiles) return BME_ERROR;

    gfx_freesprites(num);

    handle = io_open(name);
    if (handle == -1) return BME_ERROR;

    size = io_lseek(handle, 0, SEEK_END);
    io_lseek(handle, 0, SEEK_SET);

    gfx_spriteamount[num] = io_readle32(handle);

    gfx_spriteheaders[num] = malloc(gfx_spriteamount[num] * sizeof(SPRITEHEADER));

    if (!gfx_spriteheaders[num])
    {
        bme_error = BME_OUT_OF_MEMORY;
        io_close(handle);
        return BME_ERROR;
    }

    for (c = 0; c < gfx_spriteamount[num]; c++)
    {
        SPRITEHEADER *hptr = gfx_spriteheaders[num] + c;

        hptr->xsize = io_readle16(handle);
        hptr->ysize = io_readle16(handle);
        hptr->xhot = io_readle16(handle);
        hptr->yhot = io_readle16(handle);
        hptr->offset = io_readle32(handle);
    }

    datastart = io_lseek(handle, 0, SEEK_CUR);
    gfx_spritedata[num] = malloc(size - datastart);
    if (!gfx_spritedata[num])
    {
        bme_error = BME_OUT_OF_MEMORY;
        io_close(handle);
        return BME_ERROR;
    }
    io_read(handle, gfx_spritedata[num], size - datastart);
    io_close(handle);
    bme_error = BME_OK;
    return BME_OK;
}

void gfx_freesprites(int num)
{
    if (num >= gfx_maxspritefiles) return;

    if (gfx_spritedata[num])
    {
        free(gfx_spritedata[num]);
        gfx_spritedata[num] = NULL;
    }
    if (gfx_spriteheaders[num])
    {
        free(gfx_spriteheaders[num]);
        gfx_spriteheaders[num] = NULL;
    }
}

void gfx_copyscreen8(Uint8  *destaddress, Uint8  *srcaddress, unsigned pitch)
{
    int c;

    for (c = 0; c < gfx_virtualysize; c++)
    {
        memcpy(destaddress, srcaddress, gfx_virtualxsize);
        destaddress += pitch;
        srcaddress += gfx_virtualxsize;
    }
}
/**
 * =============================================================================
 * FILE bme_io.c:
 * =============================================================================
 */
//
// BME (Blasphemous Multimedia Engine) datafile IO main module
//

typedef struct
{
    Uint32 offset;
    Sint32 length;
    char name[13];
} HEADER;

typedef struct
{
    HEADER *currentheader;
    int filepos;
    int open;
} HANDLE;

static int io_usedatafile = 0;
static HEADER *fileheaders;
static unsigned files;
static char ident[4];
static char *idstring = "DAT!";
static HANDLE handle[MAX_HANDLES];
static FILE *fileptr[MAX_HANDLES] = {NULL};
static FILE *datafilehandle = NULL;
static unsigned char *datafileptr;
static unsigned char *datafilestart;

// static unsigned freadle32(FILE *index);
static void linkedseek(unsigned pos);
static void linkedread(void *buffer, int length);
static unsigned linkedreadle32(void);

void io_setfilemode(int usedf)
{
    io_usedatafile = usedf;
}

int io_openlinkeddatafile(unsigned char *ptr)
{
    int index;

    if (datafilehandle) fclose(datafilehandle);
    datafilehandle = NULL;

    datafilestart = ptr;
    linkedseek(0);

    linkedread(ident, 4);
    if (memcmp(ident, idstring, 4))
    {
        bme_error = BME_WRONG_FORMAT;
        return BME_ERROR;
    }

    files = linkedreadle32();
    fileheaders = malloc(files * sizeof(HEADER));
    if (!fileheaders)
    {
        bme_error = BME_OUT_OF_MEMORY;
        return BME_ERROR;
    }
    for (index = 0; index < files; index++)
    {
        fileheaders[index].offset = linkedreadle32();
        fileheaders[index].length = linkedreadle32();
        linkedread(&fileheaders[index].name, 13);
    }

    for (index = 0; index < MAX_HANDLES; index++) handle[index].open = 0;
    io_usedatafile = 1;
    bme_error = BME_OK;
    return BME_OK;
}

int io_opendatafile(char *name)
{
    int index;

    if (name)
    {
        datafilehandle = fopen(name, "rb");
        if (!datafilehandle)
        {
            bme_error = BME_OPEN_ERROR;
            return BME_ERROR;
        }
    }

    fread(ident, 4, 1, datafilehandle);
    if (memcmp(ident, idstring, 4))
    {
        bme_error = BME_WRONG_FORMAT;
        return BME_ERROR;
    }

    files = freadle32(datafilehandle);
    fileheaders = malloc(files * sizeof(HEADER));
    if (!fileheaders)
    {
        bme_error = BME_OUT_OF_MEMORY;
        return BME_ERROR;
    }
    for (index = 0; index < files; index++)
    {
        fileheaders[index].offset = freadle32(datafilehandle);
        fileheaders[index].length = freadle32(datafilehandle);
        fread(&fileheaders[index].name, 13, 1, datafilehandle);
    }

    for (index = 0; index < MAX_HANDLES; index++) handle[index].open = 0;
    io_usedatafile = 1;
    bme_error = BME_OK;
    return BME_OK;
}

// Returns nonnegative file handle if successful, -1 on error

int io_open(char *name)
{
    if (!name) return -1;

    if (!io_usedatafile)
    {
        int index;
        for (index = 0; index < MAX_HANDLES; index++)
        {
            if (!fileptr[index]) break;
        }
        if (index == MAX_HANDLES) return -1;
        else
        {
            FILE *file = fopen(name, "rb");
            if (!file)
            {
                return -1;
            }
            else
            {
                fileptr[index] = file;
                return index;
            }
        }
    }
    else
    {
        int index;
        int namelength;
        char namecopy[13];

        namelength = strlen(name);
        if (namelength > 12) namelength = 12;
        memcpy(namecopy, name, namelength + 1);
        for (index = 0; index < strlen(namecopy); index++)
        {
            namecopy[index] = toupper(namecopy[index]);
        }

        for (index = 0; index < MAX_HANDLES; index++)
        {
            if (!handle[index].open)
            {
                int count = files;
                handle[index].currentheader = fileheaders;

                while (count)
                {
                    if (!strcmp(namecopy, handle[index].currentheader->name))
                    {
                        handle[index].open = 1;
                        handle[index].filepos = 0;
                        return index;
                    }
                    count--;
                    handle[index].currentheader++;
                }
                return -1;
            }
        }
        return -1;
    }
}

// Returns file position after seek or -1 on error

int io_lseek(int index, int offset, int whence)
{
    if (!io_usedatafile)
    {
        fseek(fileptr[index], offset, whence);
        return ftell(fileptr[index]);
    }
    else
    {
        int newpos;

        if ((index < 0) || (index >= MAX_HANDLES)) return -1;

        if (!handle[index].open) return -1;
        switch(whence)
        {
        default:
        case SEEK_SET:
            newpos = offset;
            break;

        case SEEK_CUR:
            newpos = offset + handle[index].filepos;
            break;

        case SEEK_END:
            newpos = offset + handle[index].currentheader->length;
            break;
        }
        if (newpos < 0) newpos = 0;
        if (newpos > handle[index].currentheader->length) newpos = handle[index].currentheader->length;
        handle[index].filepos = newpos;
        return newpos;
    }
}

// Returns number of bytes actually read, -1 on error

int io_read(int index, void *buffer, int length)
{
    if (!io_usedatafile)
    {
        return fread(buffer, 1, length, fileptr[index]);
    }
    else
    {
        int readbytes;

        if ((index < 0) || (index >= MAX_HANDLES)) return -1;

        if (!handle[index].open) return -1;
        if (length + handle[index].filepos > handle[index].currentheader->length)
            length = handle[index].currentheader->length - handle[index].filepos;

        if (datafilehandle)
        {
            fseek(datafilehandle, handle[index].currentheader->offset + handle[index].filepos, SEEK_SET);
            readbytes = fread(buffer, 1, length, datafilehandle);
        }
        else
        {
            linkedseek(handle[index].currentheader->offset + handle[index].filepos);
            linkedread(buffer, length);
            readbytes = length;
        }
        handle[index].filepos += readbytes;
        return readbytes;
    }
}

// Returns nothing

void io_close(int index)
{
    if (!io_usedatafile)
    {
        fclose(fileptr[index]);
        fileptr[index] = NULL;
    }
    else
    {
        if ((index < 0) || (index >= MAX_HANDLES)) return;

        handle[index].open = 0;
    }
}

unsigned io_read8(int index)
{
    unsigned char byte;

    io_read(index, &byte, 1);
    return byte;
}

unsigned io_readle16(int index)
{
    unsigned char bytes[2];

    io_read(index, bytes, 2);
    return (bytes[1] << 8) | bytes[0];
}

unsigned io_readhe16(int index)
{
    unsigned char bytes[2];

    io_read(index, bytes, 2);
    return (bytes[0] << 8) | bytes[1];
}

unsigned io_readle32(int index)
{
    unsigned char bytes[4];

    io_read(index, bytes, 4);
    return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
}

unsigned io_readhe32(int index)
{
    unsigned char bytes[4];

    io_read(index, bytes, 4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

//~ static unsigned freadle32(FILE *file)
//~ {
    //~ unsigned char bytes[4];

    //~ fread(&bytes, 4, 1, file);
    //~ return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
//~ }

static void linkedseek(unsigned pos)
{
    datafileptr = &datafilestart[pos];
}

static void linkedread(void *buffer, int length)
{
    unsigned char *dest = (unsigned char *)buffer;
    while (length--)
    {
        *dest++ = *datafileptr++;
    }
}

static unsigned linkedreadle32(void)
{
    unsigned char bytes[4];

    linkedread(&bytes, 4);
    return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
}

/**
 * =============================================================================
 * FILE bme_mou.c:
 * =============================================================================
 */
//
// BME (Blasphemous Multimedia Engine) mouse module
//

void mou_init(void);
void mou_uninit(void);
void mou_getpos(unsigned *x, unsigned *y);
void mou_getmove(int *dx, int *dy);
unsigned mou_getbuttons(void);

void mou_init(void)
{
    win_mousebuttons = 0;
}

void mou_uninit(void)
{
}

void mou_getpos(unsigned *x, unsigned *y)
{
    if (!gfx_initted)
    {
        *x = win_mousexpos;
        *y = win_mouseypos;
    }
    else
    {
        *x = win_mousexpos * gfx_virtualxsize / gfx_windowxsize;
        *y = win_mouseypos * gfx_virtualysize / gfx_windowysize;
    }
}

void mou_getmove(int *dx, int *dy)
{
    *dx = win_mousexrel;
    *dy = win_mouseyrel;
    win_mousexrel = 0;
    win_mouseyrel = 0;

}

unsigned mou_getbuttons(void)
{
    return win_mousebuttons;
}

/**
 * =============================================================================
 * FILE bme_snd.c:
 * =============================================================================
 */
//
// BME (Blasphemous Multimedia Engine) sound main module
//

// Prototypes
int snd_init(unsigned mixrate, unsigned mixmode, unsigned bufferlength, unsigned channels, int usedirectsound);
void snd_uninit(void);
void snd_setcustommixer(void (*custommixer)(Sint32 *dest, unsigned samples));

static int snd_initchannels(unsigned channels);
static int snd_initmixer(void);
static void snd_uninitmixer(void);
static void snd_mixdata(Uint8 *dest, unsigned bytes);
static void snd_mixchannels(Sint32 *dest, unsigned samples);
static void snd_mixer(void *userdata, Uint8 *stream, int len);

// Lowlevel mixing functions
static void snd_clearclipbuffer(Sint32 *clipbuffer, unsigned clipsamples);
static void snd_mixchannel(CHANNEL *chptr, Sint32 *dest, unsigned samples);
static void snd_16bit_postprocess(Sint32 *src, Sint16 *dest, unsigned samples);
static void snd_8bit_postprocess(Sint32 *src, Uint8 *dest, unsigned samples);

void (*snd_player)(void) = NULL;
CHANNEL *snd_channel = NULL;
int snd_channels = 0;
int snd_sndinitted = 0;
int snd_bpmcount;
int snd_bpmtempo = 125;
unsigned snd_mixmode;
unsigned snd_mixrate;

static void (*snd_custommixer)(Sint32 *dest, unsigned samples) = NULL;
static unsigned snd_buffersize;
static unsigned snd_samplesize;
static unsigned snd_previouschannels = 0xffffffff;
static int snd_atexit_registered = 0;
static Sint32 *snd_clipbuffer = NULL;
static SDL_AudioSpec desired;
static SDL_AudioSpec obtained;

SDL_AudioDeviceID audioDevice;

int snd_init(
        unsigned mixrate,
        unsigned mixmode,
        unsigned bufferlength,
        unsigned channels,
        int usedirectsound
    )
{
    // Register snd_uninit as an atexit function

    if (!snd_atexit_registered)
    {
        atexit(snd_uninit);
        snd_atexit_registered = 1;
    }

    // If user wants to re-initialize, shutdown first

    snd_uninit();

    // Check for illegal config

    if ((channels < 1) || (!mixrate) || (!bufferlength))
    {
        bme_error = BME_ILLEGAL_CONFIG;
        snd_uninit();
        return BME_ERROR;
    }

    desired.freq = mixrate;
    desired.format = AUDIO_U8;
    if (mixmode & SIXTEENBIT)
    {
        desired.format = AUDIO_S16SYS;
    }
    desired.channels = 1;
    if (mixmode & STEREO) desired.channels = 2;
    desired.samples = bufferlength * mixrate / 1000;
    {
        int bits = 0;

        for (;;)
        {
            desired.samples >>= 1;
            if (!desired.samples) break;
            bits++;
        }
        desired.samples = 1 << bits;
    }

    desired.callback = snd_mixer;
    desired.userdata = NULL;

    // Init tempo count

    snd_bpmcount = 0;

    // (Re)allocate channels if necessary
    if (snd_initchannels(channels) != BME_OK)
    {
        return BME_ERROR;
    }

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);

    SDL_PauseAudioDevice(audioDevice, 1);


    if (audioDevice==0)
    {
        bme_error = BME_OPEN_ERROR;
        snd_uninit();
        return BME_ERROR;
    }
    snd_sndinitted = 1;

    snd_mixmode = 0;
    snd_samplesize = 1;
    if (obtained.channels == 2)
    {
        snd_mixmode |= STEREO;
        snd_samplesize <<= 1;
    }

    if ((obtained.format == AUDIO_S16SYS) ||
            (obtained.format == AUDIO_S16LSB) ||
            (obtained.format == AUDIO_S16MSB))
    {
        snd_mixmode |= SIXTEENBIT;
        snd_samplesize <<= 1;
    }

    snd_buffersize = obtained.size;
    snd_mixrate = obtained.freq;

    // Allocate mixer tables

    if (!snd_initmixer())
    {
        bme_error = BME_OUT_OF_MEMORY;
        snd_uninit();
        return BME_ERROR;
    }

    SDL_PauseAudioDevice(audioDevice, 0);

    bme_error = BME_OK;
    return BME_OK;
}

int snd_initchannels(unsigned channels)
{
    int c;

    if (snd_previouschannels != channels)
    {
        CHANNEL *chptr;
        if (snd_channel)
        {
            free(snd_channel);
            snd_channel = NULL;
            snd_channels = 0;
        }

        snd_channel = malloc(channels * sizeof(CHANNEL));
        if (!snd_channel)
        {
            bme_error = BME_OUT_OF_MEMORY;
            snd_uninit();
            return BME_ERROR;
        }
        chptr = &snd_channel[0];
        snd_channels = channels;
        snd_previouschannels = channels;

        // Init all channels (no sound played, no sample, mastervolume 64)
        for (c = snd_channels; c > 0; c--)
        {
            chptr->voicemode = VM_OFF;
            chptr->smp = NULL;
            chptr->mastervol = 64;
            chptr++;
        }
    }

    return BME_OK;
}

void snd_uninit(void)
{
    if (snd_sndinitted)
    {
        SDL_CloseAudioDevice(audioDevice);
        snd_sndinitted = 0;
    }
    snd_uninitmixer();
}

void snd_setcustommixer(void (*custommixer)(Sint32 *dest, unsigned samples))
{
    snd_custommixer = custommixer;
}

static int snd_initmixer(void)
{
    snd_uninitmixer();

    if (snd_mixmode & STEREO)
    {
        snd_clipbuffer = malloc((snd_buffersize / snd_samplesize) * sizeof(int) * 2);
    }
    else
    {
        snd_clipbuffer = malloc((snd_buffersize / snd_samplesize) * sizeof(int));
    }
    if (!snd_clipbuffer) return 0;

    return 1;
}

static void snd_uninitmixer(void)
{
    if (snd_clipbuffer)
    {
        free(snd_clipbuffer);
        snd_clipbuffer = NULL;
    }
}

static void snd_mixer(void *userdata, Uint8 *stream, int len)
{
    snd_mixdata(stream, len);
}

static void snd_mixdata(Uint8 *dest, unsigned bytes)
{
    unsigned mixsamples = bytes;
    unsigned clipsamples = bytes;
    Sint32 *clipptr = (Sint32 *)snd_clipbuffer;
    if (snd_mixmode & STEREO) mixsamples >>= 1;
    if (snd_mixmode & SIXTEENBIT)
    {
        clipsamples >>= 1;
        mixsamples >>= 1;
    }

    snd_clearclipbuffer(snd_clipbuffer, clipsamples);
    if (snd_player) // Must the player be called?
    {
        int musicsamples;

        while(mixsamples)
        {
            if ((!snd_bpmcount) && (snd_player)) // Player still active?
            {
                // Call player
                snd_player();
                // Reset tempocounter
                snd_bpmcount = ((snd_mixrate * 5) >> 1) / snd_bpmtempo;
            }

            musicsamples = mixsamples;
            if (musicsamples > snd_bpmcount) musicsamples = snd_bpmcount;
            snd_bpmcount -= musicsamples;
            if (!snd_custommixer)
            {
                snd_mixchannels(clipptr, musicsamples);
            }
            else
            {
                snd_custommixer(clipptr, musicsamples);
            }
            if (snd_mixmode & STEREO) clipptr += musicsamples * 2;
            else clipptr += musicsamples;
            mixsamples -= musicsamples;
        }
    }
    else
    {
        if (!snd_custommixer)
        {
            snd_mixchannels(clipptr, mixsamples);
        }
        else
        {
            snd_custommixer(clipptr, mixsamples);
        }
    }

    clipptr = (Sint32 *)snd_clipbuffer;

    if (snd_mixmode & SIXTEENBIT)
    {
        snd_16bit_postprocess(clipptr, (Sint16 *)dest, clipsamples);
    }
    else
    {
        snd_8bit_postprocess(clipptr, dest, clipsamples);
    }
}

static void snd_mixchannels(Sint32 *dest, unsigned samples)
{
    CHANNEL *chptr = &snd_channel[0];
    int c;

    for (c = snd_channels; c; c--)
    {
        snd_mixchannel(chptr, dest, samples);
        chptr++;
    }
}

static void snd_clearclipbuffer(Sint32 *clipbuffer, unsigned clipsamples)
{
    memset(clipbuffer, 0, clipsamples*sizeof(int));
}

static void snd_16bit_postprocess(Sint32 *src, Sint16 *dest, unsigned samples)
{
    while (samples--)
    {
        int sample = *src++;
        if (sample > 32767) sample = 32767;
        if (sample < -32768) sample = -32768;
        *dest++ = sample;
    }
}

static void snd_8bit_postprocess(Sint32 *src, Uint8 *dest, unsigned samples)
{
    while (samples--)
    {
        int sample = *src++;
        if (sample > 32767) sample = 32767;
        if (sample < -32768) sample = -32768;
        *dest++ = (sample >> 8) + 128;
    }
}

static void snd_mixchannel(CHANNEL *chptr, Sint32 *dest, unsigned samples)
{
    if (chptr->voicemode & VM_ON)
    {
        unsigned freq, intadd, fractadd;

        freq = chptr->freq;
        if (freq > 535232) freq = 535232;
        intadd = freq / snd_mixrate;
        fractadd = (((freq % snd_mixrate) << 16) / snd_mixrate) & 65535;

        if (snd_mixmode & STEREO)
        {
            int leftvol = (((chptr->vol * chptr->mastervol) >> 6) * (255-chptr->panning)) >> 7;
            int rightvol = (((chptr->vol * chptr->mastervol) >> 6) * (chptr->panning)) >> 7;
            if (leftvol > 255) leftvol = 255;
            if (rightvol > 255) rightvol = 255;
            if (leftvol < 0) leftvol = 0;
            if (rightvol < 0) rightvol = 0;

            if (chptr->voicemode & VM_16BIT)
            {
                Sint16 *pos = (Sint16 *)chptr->pos;
                Sint16 *end = (Sint16 *)chptr->end;
                Sint16 *repeat = (Sint16 *)chptr->repeat;

                if (chptr->voicemode & VM_LOOP)
                {
                    while (samples--)
                    {
                        *dest = *dest + ((*pos * leftvol) >> 8);
                        dest++;
                        *dest = *dest + ((*pos * rightvol) >> 8);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        while (pos >= end) pos -= (end - repeat);
                    }
                }
                else
                {
                    while (samples--)
                    {
                        *dest = *dest + ((*pos * leftvol) >> 8);
                        dest++;
                        *dest = *dest + ((*pos * rightvol) >> 8);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        if (pos >= end)
                        {
                            chptr->voicemode &= ~VM_ON;
                            break;
                        }
                    }
                }
                chptr->pos = (Sint8 *)pos;
            }
            else
            {
                Sint8 *pos = (Sint8 *)chptr->pos;
                Sint8 *end = chptr->end;
                Sint8 *repeat = chptr->repeat;

                if (chptr->voicemode & VM_LOOP)
                {
                    while (samples--)
                    {
                        *dest = *dest + (*pos * leftvol);
                        dest++;
                        *dest = *dest + (*pos * rightvol);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        while (pos >= end) pos -= (end - repeat);
                    }
                }
                else
                {
                    while (samples--)
                    {
                        *dest = *dest + (*pos * leftvol);
                        dest++;
                        *dest = *dest + (*pos * rightvol);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        if (pos >= end)
                        {
                            chptr->voicemode &= ~VM_ON;
                            break;
                        }
                    }
                }
                chptr->pos = (Sint8 *)pos;
            }
        }
        else
        {
            int vol = ((chptr->vol * chptr->mastervol) >> 6);
            if (vol > 255) vol = 255;
            if (vol < 0) vol = 0;

            if (chptr->voicemode & VM_16BIT)
            {
                Sint16 *pos = (Sint16 *)chptr->pos;
                Sint16 *end = (Sint16 *)chptr->end;
                Sint16 *repeat = (Sint16 *)chptr->repeat;

                if (chptr->voicemode & VM_LOOP)
                {
                    while (samples--)
                    {
                        *dest = *dest + ((*pos * vol) >> 8);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        while (pos >= end) pos -= (end - repeat);
                    }
                }
                else
                {
                    while (samples--)
                    {
                        *dest = *dest + ((*pos * vol) >> 8);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        if (pos >= end)
                        {
                            chptr->voicemode &= ~VM_ON;
                            break;
                        }
                    }
                }
                chptr->pos = (Sint8 *)pos;
            }
            else
            {
                Sint8 *pos = (Sint8 *)chptr->pos;
                Sint8 *end = chptr->end;
                Sint8 *repeat = chptr->repeat;

                if (chptr->voicemode & VM_LOOP)
                {
                    while (samples--)
                    {
                        *dest = *dest + (*pos * vol);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        while (pos >= end) pos -= (end - repeat);
                    }
                }
                else
                {
                    while (samples--)
                    {
                        *dest = *dest + (*pos * vol);
                        dest++;
                        chptr->fractpos += fractadd;
                        if (chptr->fractpos > 65535)
                        {
                            chptr->fractpos &= 65535;
                            pos++;
                        }
                        pos += intadd;
                        if (pos >= end)
                        {
                            chptr->voicemode &= ~VM_ON;
                            break;
                        }
                    }
                }
                chptr->pos = (Sint8 *)pos;
            }
        }
    }
}
/**
 * =============================================================================
 * FILE bme_win.c:
 * =============================================================================
 */
//
// BME (Blasphemous Multimedia Engine) windows & timing module
//

SDL_Joystick *joy[MAX_JOYSTICKS] = {NULL};
Sint16 joyx[MAX_JOYSTICKS];
Sint16 joyy[MAX_JOYSTICKS];
Uint32 joybuttons[MAX_JOYSTICKS];
SDL_Window *win_window = NULL;

// Prototypes

int win_openwindow(unsigned xsize, unsigned ysize, char *appname, char *icon);
void win_closewindow(void);
void win_messagebox(char *string);
void win_checkmessages(void);
int win_getspeed(int framerate);
void win_setmousemode(int mode);

// Global variables

int win_fullscreen = 0; // By default windowed
int win_windowinitted = 0;
int win_quitted = 0;
unsigned char win_keytable[SDL_NUM_SCANCODES] = {0};
unsigned char win_asciikey = 0;
unsigned win_virtualkey = 0;
unsigned win_mousexpos = 0;
unsigned win_mouseypos = 0;
unsigned win_mousexrel = 0;
unsigned win_mouseyrel = 0;
unsigned win_mousebuttons = 0;
int win_mousemode = MOUSE_ALWAYS_VISIBLE;
unsigned char win_keystate[SDL_NUM_SCANCODES] = {0};

// Static variables

static int win_lasttime = 0;
static int win_currenttime = 0;
static int win_framecounter = 0;
static int win_activateclick = 0;

int win_openwindow(unsigned xsize, unsigned ysize, char *appname, char *icon)
{
    Uint32 flags = SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE;

    if (win_fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    if (!win_windowinitted)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
        {
            return BME_ERROR;
        }
        atexit(SDL_Quit);
        win_windowinitted = 1;
    }

    win_window = SDL_CreateWindow(appname,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  xsize, ysize,
                                  flags);
    return BME_OK;
}

void win_closewindow(void)
{
    SDL_DestroyWindow(win_window);
}

void win_messagebox(char *string)
{
    return;
}

int win_getspeed(int framerate)
{
    // Note: here 1/10000th of a second accuracy is used (although
    // timer resolution is only millisecond) for minimizing
    // inaccuracy in frame duration calculation -> smoother screen
    // update

    int frametime = 10000 / framerate;
    int frames = 0;

    while (!frames)
    {
        win_checkmessages();

        win_lasttime = win_currenttime;
        win_currenttime = SDL_GetTicks();

        win_framecounter += (win_currenttime - win_lasttime)*10;
        frames = win_framecounter / frametime;
        win_framecounter -= frames * frametime;

        if (!frames) SDL_Delay((frametime - win_framecounter)/10);
    }

    return frames;
}

// This is the "message pump". Called by following functions:
// win_getspeed();
// kbd_waitkey();
//
// It is recommended to be called in any long loop where those two functions
// are not called.

void win_checkmessages(void)
{
    SDL_Event event;
    unsigned keynum;

    win_activateclick = 0;

    SDL_PumpEvents();

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_JOYBUTTONDOWN:
            joybuttons[event.jbutton.which] |= 1 << event.jbutton.button;
            break;

        case SDL_JOYBUTTONUP:
            joybuttons[event.jbutton.which] &= ~(1 << event.jbutton.button);
            break;

        case SDL_JOYAXISMOTION:
            switch (event.jaxis.axis)
            {
            case 0:
                joyx[event.jaxis.which] = event.jaxis.value;
                break;

            case 1:
                joyy[event.jaxis.which] = event.jaxis.value;
                break;
            }
            break;

        case SDL_MOUSEMOTION:
            win_mousexpos = event.motion.x;
            win_mouseypos = event.motion.y;
            win_mousexrel += event.motion.xrel;
            win_mouseyrel += event.motion.yrel;
            break;

        case SDL_MOUSEBUTTONDOWN:
            switch(event.button.button)
            {
            case SDL_BUTTON_LEFT:
                win_mousebuttons |= MOUSEB_LEFT;
                break;

            case SDL_BUTTON_MIDDLE:
                win_mousebuttons |= MOUSEB_MIDDLE;
                break;

            case SDL_BUTTON_RIGHT:
                win_mousebuttons |= MOUSEB_RIGHT;
                break;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            switch(event.button.button)
            {
            case SDL_BUTTON_LEFT:
                win_mousebuttons &= ~MOUSEB_LEFT;
                break;

            case SDL_BUTTON_MIDDLE:
                win_mousebuttons &= ~MOUSEB_MIDDLE;
                break;

            case SDL_BUTTON_RIGHT:
                win_mousebuttons &= ~MOUSEB_RIGHT;
                break;
            }
            break;

        case SDL_QUIT:
            win_quitted = 1;
            break;

        case SDL_TEXTINPUT:
            win_asciikey = event.text.text[0];
            // printf("bme_win.c: win_checkmessages() case SDL_TEXTINPUT, win_asciikey: %i \n", win_asciikey);
            break;

        case SDL_KEYDOWN:
            win_virtualkey = event.key.keysym.sym;
            keynum = event.key.keysym.scancode;
            /*
            printf("win_virtualkey: %i\n", win_virtualkey);
            printf("keynum:         %i\n", keynum);
            */
            if (keynum < SDL_NUM_SCANCODES)
            {
                if ((keynum == SDL_SCANCODE_RETURN) &&
                        ((win_keystate[SDL_SCANCODE_LALT])
                         || (win_keystate[SDL_SCANCODE_RALT])))
                {
                    win_fullscreen ^= 1;
                    gfx_reinit();
                }
                else
                {
                    win_keytable[keynum] = 1;
                    win_keystate[keynum] = 1;
                }
            }
            break;

        case SDL_KEYUP:
            keynum = event.key.keysym.scancode;
            if (keynum < SDL_NUM_SCANCODES)
            {
                win_keytable[keynum] = 0;
                win_keystate[keynum] = 0;
            }
            break;

        case SDL_WINDOWEVENT_RESIZED:
            gfx_redraw = 1;
            break;
        }
    }
}

void win_setmousemode(int mode)
{
    win_mousemode = mode;

    switch(mode)
    {
    case MOUSE_ALWAYS_VISIBLE:
        SDL_ShowCursor(SDL_ENABLE);
        break;

    case MOUSE_FULLSCREEN_HIDDEN:
        if (gfx_fullscreen)
        {
            SDL_ShowCursor(SDL_DISABLE);
        }
        else
        {
            SDL_ShowCursor(SDL_ENABLE);
        }
        break;

    case MOUSE_ALWAYS_HIDDEN:
        SDL_ShowCursor(SDL_DISABLE);
        break;
    }
}
