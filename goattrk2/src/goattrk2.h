#ifndef GOATTRK2_H
#define GOATTRK2_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include "bme.h"

#include "gcommon.h"
#include "gconsole.h"
#include "gsound.h"
#include "gsid.h"
#include "gsong.h"
#include "gplay.h"
#include "gdisplay.h"
#include "greloc.h"
#include "gfile.h"
#include "gpattern.h"
#include "gorder.h"
#include "ginstr.h"
#include "gtable.h"

#define EDIT_PATTERN 0
#define EDIT_ORDERLIST 1
#define EDIT_INSTRUMENT 2
#define EDIT_TABLES 3
#define EDIT_NAMES 4

#define KEY_TRACKER 0
#define KEY_DMC 1
#define KEY_JANKO 2

#define VISIBLEPATTROWS 31
#define VISIBLEORDERLIST 15
#define VISIBLETABLEROWS 15
#define VISIBLEFILES 24

#define PGUPDNREPEAT 8

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

#ifndef GOATTRK2_C
extern int menu;
extern int editmode;
extern int recordmode;
extern int followplay;
extern int hexnybble;
extern int stepsize;
extern int autoadvance;
extern int defaultpatternlength;
extern int cursorflash;
extern int cursorcolortable[];
extern int exitprogram;
extern int eacolumn;
extern int eamode;
extern unsigned keypreset;
extern unsigned playerversion;
extern int fileformat;
extern int zeropageadr;
extern int playeradr;
extern unsigned sidmodel;
extern unsigned multiplier;
extern unsigned adparam;
extern unsigned ntsc;
extern unsigned patterndispmode;
extern unsigned sidaddress;
extern unsigned finevibrato;
extern unsigned optimizepulse;
extern unsigned optimizerealtime;
extern unsigned usefinevib;
extern unsigned b;
extern unsigned mr;
extern unsigned writer;
extern unsigned hardsid;
extern unsigned catweasel;
extern unsigned interpolate;
extern unsigned hardsidbufinteractive;
extern unsigned hardsidbufplayback;
extern float basepitch;
extern char configbuf[MAX_PATHNAME];
extern char loadedsongfilename[MAX_FILENAME];
extern char songfilename[MAX_FILENAME];
extern char songfilter[MAX_FILENAME];
extern char songpath[MAX_PATHNAME];
extern char instrfilename[MAX_FILENAME];
extern char instrfilter[MAX_FILENAME];
extern char instrpath[MAX_PATHNAME];
extern char packedpath[MAX_PATHNAME];
extern char *programname;
extern char *notename[];
extern char textbuffer[MAX_PATHNAME];
extern unsigned char hexkeytbl[16];
extern unsigned char datafile[];
#endif

void getparam(FILE *handle, unsigned *value);
void getfloatparam(FILE *handle, float *value);
void getstringparam(FILE *handle, char *value);
void waitkey(void);
void waitkeymouse(void);
void waitkeynoupdate(void);
void waitkeymousenoupdate(void);
void converthex(void);
void docommand(void);
void onlinehelp(int standalone, int context);
void mousecommands(void);
void generalcommands(void);
void load(void);
void save(void);
void quit(void);
void clear(void);
void prevmultiplier(void);
void nextmultiplier(void);
void editadsr(void);
void calculatefreqtable(void);
void setspecialnotenames(void);
void readscalatuningfile(void);

#endif
