#ifndef GCOLORS_H
#define GCOLORS_H

#define CBLACK 0x0
#define CDBLUE 0x1
#define CDGREEN 0x2
#define CDGREY 0x3
#define CDRED 0x4
#define CDBROWN 0x5
#define CLBROWN 0x6
#define CLGREY 0x7
#define CGREY 0x8
#define CLBLUE 0x9
#define CLGREEN 0xA
#define CCYAN 0xB
#define CLRED 0xC
#define CPURPLE 0xD
#define CYELLOW 0xE
#define CWHITE 0xF

#define CBG CBLACK

#define CNORMAL ((CBG << 4) | CLGREEN)
#define CMUTE ((CBG << 4) | CDGREY)
#define CEDIT ((CBG << 4) | CYELLOW)
#define CPLAYING ((CBG << 4) | CLRED)
#define CPLAYINGINV ((CDRED << 4) | CWHITE)
#define CCOMMAND ((CBG << 4) | CDGREEN)
#define CCOMMANDVAL ((CBG << 4) | CCYAN)
#define CTITLE ((CBG << 4) | CWHITE)
#define CPATTCOL ((CBG << 4) | CWHITE)
#define CINDEXES ((CBG << 4) | CGREY)
#define CINDEXESINV ((CGREY << 4) | CBG)
#define CSTATUSBAR ((CGREY << 4) | CBLACK)
#define CTBLMODVAL ((CBG << 4) | CCYAN)
#define CTBLEND ((CBG << 4) | CWHITE)
#define CINSTRNUM ((CBG << 4) | CWHITE)
#define CMARKBG ((CBG << 4) | CDRED)
#define CHELP_HEADER ((CBG << 4) | CYELLOW)
#define CHELP_NORMAL ((CBG << 4) | CGREY)
#define CBOXCOLOR ((CBG << 4) | CPURPLE)
#define CBOTTOMSTATUS ((CBG << 4) | CLRED)

typedef struct
{
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

extern COLORSCHEME colscheme;

void colscheme_init(unsigned theme);

#endif
