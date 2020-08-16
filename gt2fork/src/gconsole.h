#ifndef GCONSOLE_H
#define GCONSOLE_H

#define MAX_COLUMNS 128
#define MAX_ROWS 40
#define HOLDDELAY 24
#define DOUBLECLICKDELAY 15

#define POSMONOPATTERNSX 1+13
#define POSMONOPATTERNSY 2
#define POSMONOOCTAVEX 1
#define POSMONOOCTAVEY MAX_ROWS-2
#define POSMONOCHNSX MAX_COLUMNS-21
#define POSMONOCHNSY MAX_ROWS-2
#define POSMONOORDERLISTX 54
#define POSMONOORDERLISTY 2
#define POSMONOINSTRTABX 80
#define POSMONOINSTRTABY 10
#define POSSTATUSBOTTOMX ((MAX_COLUMNS/2) - 30)
#define POSSTATUSBOTTOMY MAX_ROWS-2
#define POSSTATUSTOPX 0
#define POSSTATUSTOPY 0
#define POSSTATUSTOPFVX 80
#define POSSTATUSTOPENDX MAX_COLUMNS-1
#define POSLOADBOXX 64
#define POSLOADBOXY 2

typedef struct
{
    int channelsX;
    int channelsY;
    int instrumentsX;
    int instrumentsY;
    int loadboxX;
    int loadboxY;
    int octaveX;
    int octaveY;
    int orderlistX;
    int orderlistY;
    int patternsX;
    int patternsY;
    int statusBottomX;
    int statusBottomY;
    int statusTopEndX;
    int statusTopFvX;
    int statusTopX;
    int statusTopY;
} POSITIONS;

int initscreen(void);
void closescreen(void);
void clearscreen(void);
void fliptoscreen(void);
void printtext(int x, int y, int color, const char *text);
void printtextc(int y, int color, const char *text);
void printtextcp(int cp, int y, int color, const char *text);
void printblank(int x, int y, int length);
void printblankc(int x, int y, int color, int length);
void drawbox(int x, int y, int color, int sx, int sy);
void printbg(int x, int y, int color, int length);
void getkey(void);

#ifndef GCONSOLE_C
extern int key, rawkey, shiftpressed, cursorflashdelay;
extern int mouseb, prevmouseb;
extern int mouseheld;
extern int mousex, mousey;
extern POSITIONS dpos;
#endif

#endif
