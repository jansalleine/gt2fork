#ifndef GT2F_PATTERN_H
#define GT2F_PATTERN_H

#ifndef GT2F_PATTERN_C
extern int epnum[MAX_CHN];
extern int eppos;
extern int epview;
extern int epcolumn;
extern int epchn;
extern int epoctave;
extern int epmarkchn;
extern int epmarkstart;
extern int epmarkend;
#endif

void patterncommands(void);
void nextpattern(void);
void prevpattern(void);
void patternup(void);
void patterndown(void);
void shrinkpattern(void);
void expandpattern(void);
void splitpattern(void);
void joinpattern(void);

#endif