#ifndef GT2F_COLORS_H
#define GT2F_COLORS_H

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
    int bgColor;
    int boxColor;
    int command;
    int commandValue;
    int edit;
    int helpHeader;
    int helpNormal;
    int indexes;
    int indexesInv;
    int instrnum;
    int markBg;
    int mute;
    int normal;
    int pattcol;
    int playing;
    int playingInv;
    int statusBottom;
    int statusTop;
    int tableEnd;
    int tableModVal;
    int title;
} COLORSCHEME;

void colscheme_init(unsigned theme);

#ifndef GCOLORS_C
extern COLORSCHEME colscheme;
#endif

#endif
