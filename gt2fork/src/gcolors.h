#ifndef GCOLORS_H
#define GCOLORS_H

#define CBLACK  0x0
#define CDBLUE  0x1
#define CDGREEN 0x2
#define CDGREY  0x3
#define CDRED   0x4
#define CDBROWN 0x5
#define CLBROWN 0x6
#define CLGREY  0x7
#define CGREY   0x8
#define CLBLUE  0x9
#define CLGREEN 0xA
#define CCYAN   0xB
#define CLRED   0xC
#define CPURPLE 0xD
#define CYELLOW 0xE
#define CWHITE  0xF

typedef struct
{
    int bgcolor;
    int boxcolor;
    int command;
    int command_value;
    int edit;
    int help_header;
    int help_normal;
    int indexes;
    int indexes_inv;
    int instrnum;
    int mark_bg;
    int mute;
    int normal;
    int pattcol;
    int playing;
    int playing_inv;
    int status_bottom;
    int status_top;
    int table_end;
    int table_modval;
    int title;
} COLORSCHEME;

void colscheme_init(unsigned theme);

#ifndef GCOLORS_C
extern COLORSCHEME colscheme;
#endif

#endif
