#ifndef GT2F_LEGACY_H
#define GT2F_LEGACY_H

#ifdef __WIN32__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

/**
 * =============================================================================
 * FILE bme_cfg.h:
 * =============================================================================
 */
// BME configuration (some static things)

#define MAX_JOYSTICKS 16        // Maximum joysticks
#define MAX_HANDLES 16          // Up to 16 simultaneus files open from the
// datafile

#define MAX_COLORS 256          // 8bit oldskool mode

#define DEFAULT_MAX_SPRFILES 256 // Default maximum of 256 spritefiles, though
// this can be set during running of program

/**
 * =============================================================================
 * FILE bme_end.h:
 * =============================================================================
 */
void fwrite8(FILE *file, unsigned data);
void fwritele16(FILE *file, unsigned data);
void fwritele32(FILE *file, unsigned data);
unsigned fread8(FILE *file);
unsigned freadle16(FILE *file);
unsigned freadle32(FILE *file);
unsigned freadhe16(FILE *file);
unsigned freadhe32(FILE *file);
/**
 * =============================================================================
 * FILE bme_err.h:
 * =============================================================================
 */
// BME error code header file

// BME function return codes

#define BME_OK 1
#define BME_ERROR 0

// BME detailed error types (in bme_error)

#define BME_OPEN_ERROR -1
#define BME_READ_ERROR -2
#define BME_WRONG_FORMAT -3
#define BME_OUT_OF_MEMORY -4
#define BME_THREAD_ERROR -5
#define BME_ILLEGAL_CONFIG -6
#define BME_OUT_OF_CHANNELS -7
#define BME_GRAPHICS_ERROR -8
#define BME_SOUND_ERROR -9

/**
 * =============================================================================
 * FILE bme_gfx.h:
 * =============================================================================
 */
// BME graphics module header file

int gfx_init(unsigned xsize, unsigned ysize, unsigned framerate, unsigned flags);
void gfx_reinit(void);
void gfx_uninit(void);
int gfx_lock(void);
void gfx_unlock(void);
void gfx_flip(void);
void gfxSetClipRegion(unsigned left, unsigned top, unsigned right, unsigned bottom);
void gfx_setmaxspritefiles(int num);
void gfx_setmaxcolors(int num);
int gfx_loadpalette(char *name);
void gfx_calcpalette(int fade, int radd, int gadd, int badd);
void gfxSetPalette(void);
int gfx_loadblocks(char *name);
int gfx_loadsprites(int num, char *name);
void gfx_freesprites(int num);

extern int gfx_initted;
extern int gfx_scanlinemode;
extern int gfx_preventswitch;
extern int gfx_fullscreen;
extern int gfx_redraw;
extern unsigned gfx_windowxsize;
extern unsigned gfx_windowysize;
extern unsigned gfx_virtualxsize;
extern unsigned gfx_virtualysize;
extern unsigned gfx_nblocks;
extern int gfx_blockxsize;
extern int gfx_blockysize;
extern int spr_xsize;
extern int spr_ysize;
extern int spr_xhotspot;
extern int spr_yhotspot;
extern Uint8 *gfx_vscreen;
extern Uint8 *gfx_blocks;
extern unsigned char gfx_palette[];
extern SDL_Surface *gfx_screen;
extern SDL_Renderer *gfx_renderer;
/**
 * =============================================================================
 * FILE bme.h:
 * =============================================================================
 */
// BME header file for user applications

/**
 * =============================================================================
 * FILE bme_io.h:
 * =============================================================================
 */
// BME IO module header file

int io_open(char *name);
int io_lseek(int handle, int bytes, int whence);
int io_read(int handle, void *buffer, int size);
void io_close(int handle);
int io_opendatafile(char *name);
int io_openlinkeddatafile(unsigned char *ptr);
void io_setfilemode(int usedf);
unsigned io_read8(int handle);
unsigned io_readle16(int handle);
unsigned io_readle32(int handle);
unsigned io_readhe16(int handle);
unsigned io_readhe32(int handle);
/**
 * =============================================================================
 * FILE bme_main.h:
 * =============================================================================
 */
// BME main definitions header file

#define GFX_SCANLINES 1
#define GFX_DOUBLESIZE 2
#define GFX_USE1PAGE 0
#define GFX_USE2PAGES 4
#define GFX_USE3PAGES 8
#define GFX_WAITVBLANK 16
#define GFX_FULLSCREEN 32
#define GFX_WINDOW 64
#define GFX_NOSWITCHING 128
#define GFX_USEDIBSECTION 256

#define MOUSEB_LEFT 1
#define MOUSEB_RIGHT 2
#define MOUSEB_MIDDLE 4

#define JOY_LEFT 1
#define JOY_RIGHT 2
#define JOY_UP 4
#define JOY_DOWN 8
#define JOY_FIRE1 16
#define JOY_FIRE2 32
#define JOY_FIRE3 64
#define JOY_FIRE4 128

#define LEFT 0
#define MIDDLE 128
#define RIGHT 255

#define B_OFF 0
#define B_SOLID 1
#define B_NOTSOLID 2

#define MONO 0
#define STEREO 1
#define EIGHTBIT 0
#define SIXTEENBIT 2

#define VM_OFF 0
#define VM_ON 1
#define VM_ONESHOT 0
#define VM_LOOP 2
#define VM_16BIT 4

#define KEY_BACKSPACE    SDL_SCANCODE_BACKSPACE
#define KEY_CAPSLOCK     SDL_SCANCODE_CAPSLOCK
#define KEY_ENTER        SDL_SCANCODE_RETURN
#define KEY_ESC          SDL_SCANCODE_ESCAPE
#define KEY_ALT          SDL_SCANCODE_LALT
#define KEY_CTRL         SDL_SCANCODE_LCTRL
#define KEY_LEFTCTRL     SDL_SCANCODE_LCTRL
#define KEY_RIGHTALT     SDL_SCANCODE_RALT
#define KEY_RIGHTCTRL    SDL_SCANCODE_RCTRL
#define KEY_LEFTSHIFT    SDL_SCANCODE_LSHIFT
#define KEY_RIGHTSHIFT   SDL_SCANCODE_RSHIFT
#define KEY_NUMLOCK      SDL_SCANCODE_NUMLOCKCLEAR
#define KEY_SCROLLLOCK   SDL_SCANCODE_SCROLLLOCK
#define KEY_SPACE        SDL_SCANCODE_SPACE
#define KEY_TAB          SDL_SCANCODE_TAB
#define KEY_F1           SDL_SCANCODE_F1
#define KEY_F2           SDL_SCANCODE_F2
#define KEY_F3           SDL_SCANCODE_F3
#define KEY_F4           SDL_SCANCODE_F4
#define KEY_F5           SDL_SCANCODE_F5
#define KEY_F6           SDL_SCANCODE_F6
#define KEY_F7           SDL_SCANCODE_F7
#define KEY_F8           SDL_SCANCODE_F8
#define KEY_F9           SDL_SCANCODE_F9
#define KEY_F10          SDL_SCANCODE_F10
#define KEY_F11          SDL_SCANCODE_F11
#define KEY_F12          SDL_SCANCODE_F12
#define KEY_A            SDL_SCANCODE_A
#define KEY_N            SDL_SCANCODE_N
#define KEY_B            SDL_SCANCODE_B
#define KEY_O            SDL_SCANCODE_O
#define KEY_C            SDL_SCANCODE_C
#define KEY_P            SDL_SCANCODE_P
#define KEY_D            SDL_SCANCODE_D
#define KEY_Q            SDL_SCANCODE_Q
#define KEY_E            SDL_SCANCODE_E
#define KEY_R            SDL_SCANCODE_R
#define KEY_F            SDL_SCANCODE_F
#define KEY_S            SDL_SCANCODE_S
#define KEY_G            SDL_SCANCODE_G
#define KEY_T            SDL_SCANCODE_T
#define KEY_H            SDL_SCANCODE_H
#define KEY_U            SDL_SCANCODE_U
#define KEY_I            SDL_SCANCODE_I
#define KEY_V            SDL_SCANCODE_V
#define KEY_J            SDL_SCANCODE_J
#define KEY_W            SDL_SCANCODE_W
#define KEY_K            SDL_SCANCODE_K
#define KEY_X            SDL_SCANCODE_X
#define KEY_L            SDL_SCANCODE_L
#define KEY_Y            SDL_SCANCODE_Y
#define KEY_M            SDL_SCANCODE_M
#define KEY_Z            SDL_SCANCODE_Z
#define KEY_1            SDL_SCANCODE_1
#define KEY_2            SDL_SCANCODE_2
#define KEY_3            SDL_SCANCODE_3
#define KEY_4            SDL_SCANCODE_4
#define KEY_5            SDL_SCANCODE_5
#define KEY_6            SDL_SCANCODE_6
#define KEY_7            SDL_SCANCODE_7
#define KEY_8            SDL_SCANCODE_8
#define KEY_9            SDL_SCANCODE_9
#define KEY_0            SDL_SCANCODE_0
#define KEY_MINUS        SDL_SCANCODE_MINUS
#define KEY_EQUAL        SDL_SCANCODE_EQUALS
#define KEY_BRACKETL     SDL_SCANCODE_LEFTBRACKET
#define KEY_BRACKETR     SDL_SCANCODE_RIGHTBRACKET
#define KEY_SEMICOLON    SDL_SCANCODE_SEMICOLON
#define KEY_APOST1       SDL_SCANCODE_APOSTROPHE
#define KEY_APOST2       SDL_SCANCODE_GRAVE
#define KEY_COMMA        SDL_SCANCODE_COMMA
#define KEY_COLON        SDL_SCANCODE_PERIOD
#define KEY_PERIOD       SDL_SCANCODE_PERIOD
#define KEY_SLASH        SDL_SCANCODE_SLASH
#define KEY_BACKSLASH    SDL_SCANCODE_BACKSLASH
#define KEY_DEL          SDL_SCANCODE_DELETE
#define KEY_DOWN         SDL_SCANCODE_DOWN
#define KEY_END          SDL_SCANCODE_END
#define KEY_HOME         SDL_SCANCODE_HOME
#define KEY_INS          SDL_SCANCODE_INSERT
#define KEY_LEFT         SDL_SCANCODE_LEFT
#define KEY_PGDN         SDL_SCANCODE_PAGEDOWN
#define KEY_PGUP         SDL_SCANCODE_PAGEUP
#define KEY_RIGHT        SDL_SCANCODE_RIGHT
#define KEY_UP           SDL_SCANCODE_UP
#define KEY_WINDOWSL     SDL_SCANCODE_LGUI
#define KEY_WINDOWSR     SDL_SCANCODE_RGUI
#define KEY_MENU         SDL_SCANCODE_MENU
#define KEY_PAUSE        SDL_SCANCODE_PAUSE
#define KEY_KPDIVIDE     SDL_SCANCODE_KP_DIVIDE
#define KEY_KPMULTIPLY   SDL_SCANCODE_KP_MULTIPLY
#define KEY_KPPLUS       SDL_SCANCODE_KP_PLUS
#define KEY_KPMINUS      SDL_SCANCODE_KP_MINUS
#define KEY_KP0          SDL_SCANCODE_KP0
#define KEY_KP1          SDL_SCANCODE_KP1
#define KEY_KP2          SDL_SCANCODE_KP2
#define KEY_KP3          SDL_SCANCODE_KP3
#define KEY_KP4          SDL_SCANCODE_KP4
#define KEY_KP5          SDL_SCANCODE_KP5
#define KEY_KP6          SDL_SCANCODE_KP6
#define KEY_KP7          SDL_SCANCODE_KP7
#define KEY_KP8          SDL_SCANCODE_KP8
#define KEY_KP9          SDL_SCANCODE_KP9
#define KEY_KPUP         SDL_SCANCODE_KP8
#define KEY_KPDOWN       SDL_SCANCODE_KP2
#define KEY_KPLEFT       SDL_SCANCODE_KP4
#define KEY_KPRIGHT      SDL_SCANCODE_KP6
#define KEY_KPENTER      SDL_SCANCODE_KP_ENTER
#define KEY_KPEQUALS     SDL_SCANCODE_KP_EQUALS
#define KEY_KPPERIOD     SDL_SCANCODE_KP_PERIOD


typedef struct
{
    Sint8 *start;
    Sint8 *repeat;
    Sint8 *end;
    unsigned char voicemode;
} SAMPLE;

typedef struct
{
    volatile Sint8 *pos;
    Sint8 *repeat;
    Sint8 *end;
    SAMPLE *smp;
    unsigned freq;
    volatile unsigned fractpos;
    int vol;
    int mastervol;
    unsigned panning;
    volatile unsigned voicemode;
} CHANNEL;

typedef struct
{
    unsigned rawcode;
    char *name;
} KEY;

typedef struct
{
    Sint16 xsize;
    Sint16 ysize;
    Sint16 xhot;
    Sint16 yhot;
    Uint32 offset;
} SPRITEHEADER;

typedef struct
{
    Uint32 type;
    Uint32 offset;
} BLOCKHEADER;

typedef struct
{
    Uint8 blocksname[13];
    Uint8 palettename[13];
} MAPHEADER;

typedef struct
{
    Sint32 xsize;
    Sint32 ysize;
    Uint8 xdivisor;
    Uint8 ydivisor;
    Uint8 xwrap;
    Uint8 ywrap;
} LAYERHEADER;

extern int bme_error;
/**
 * =============================================================================
 * FILE bme_mou.h:
 * =============================================================================
 */
// BME mouse functions header file

void mou_init(void);
void mou_uninit(void);
void mou_getpos(unsigned *x, unsigned *y);
void mou_getmove(int *dx, int *dy);
unsigned mou_getbuttons(void);
/**
 * =============================================================================
 * FILE bme_snd.h:
 * =============================================================================
 */
// Sound functions header file

int snd_init(unsigned mixrate, unsigned mixmode, unsigned bufferlength, unsigned channels, int usedirectsound);
void snd_uninit(void);
void snd_setcustommixer(void (*custommixer)(Sint32 *dest, unsigned samples));
void snd_preventdistortion(unsigned channels);
void snd_setmastervolume(unsigned chnum, unsigned char mastervol);
void snd_setmusicmastervolume(unsigned musicchannels, unsigned char mastervol);
void snd_setsfxmastervolume(unsigned musicchannels, unsigned char mastervol);

extern void (*snd_player)(void);
extern CHANNEL *snd_channel;
extern int snd_sndinitted;
extern int snd_bpmtempo;
extern int snd_bpmcount;
extern int snd_channels;
extern int snd_buffers;
extern unsigned snd_mixmode;
extern unsigned snd_mixrate;

extern SDL_AudioDeviceID audioDevice;
/**
 * =============================================================================
 * FILE bme_win.h:
 * =============================================================================
 */
// BME windows, input & timing module header file

int win_openwindow(unsigned xsize, unsigned ysize, char *appname, char *icon);
void win_closewindow(void);
void win_checkmessages(void);
int win_getspeed(int framerate);
void win_setmousemode(int mode);

extern int win_windowinitted;
extern int win_quitted;
extern int win_fullscreen;
extern unsigned char win_keytable[SDL_NUM_SCANCODES];
extern unsigned char win_keystate[SDL_NUM_SCANCODES];
extern unsigned char win_asciikey;
extern unsigned win_virtualkey;
extern unsigned win_mousexpos;
extern unsigned win_mouseypos;
extern unsigned win_mousexrel;
extern unsigned win_mouseyrel;
extern unsigned win_mousebuttons;
extern int win_mousemode;
extern SDL_Joystick *joy[MAX_JOYSTICKS];
extern Sint16 joyx[MAX_JOYSTICKS];
extern Sint16 joyy[MAX_JOYSTICKS];
extern Uint32 joybuttons[MAX_JOYSTICKS];
extern SDL_Window *win_window;

#endif
