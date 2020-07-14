#ifndef GDISPLAY_H
#define GDISPLAY_H

#define CNORMAL 0x0A
#define CMUTE 0x03
#define CEDIT 0x0E
#define CPLAYING 0x0C
#define CPLAYINGINV 0x4F
#define CCOMMAND 0x02
#define CTITLE 0x0F
#define CPATTCOL 0x0F
#define CINDEXES 0x08
#define CINDEXESINV 0x80
#define CSTATUS 0x80

void printmainscreen(void);
void displayupdate(void);
void printstatus(void);
void resettime(void);
void incrementtime(void);

#endif
