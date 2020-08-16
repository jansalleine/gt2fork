#ifndef GCONSOLE_H
#define GCONSOLE_H

#define MAX_COLUMNS 128
#define MAX_ROWS 40
#define HOLDDELAY 24
#define DOUBLECLICKDELAY 15

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
