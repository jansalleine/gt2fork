#ifndef GT2F_ORDER_H
#define GT2F_ORDER_H

#ifndef GT2F_ORDER_C
extern int espos[MAX_CHN];
extern int esend[MAX_CHN];
extern int eseditpos;
extern int esview;
extern int escolumn;
extern int eschn;
extern int esnum;
extern int esmarkchn;
extern int esmarkstart;
extern int esmarkend;
extern int enpos;
#endif

void updateviewtopos(void);
void orderlistcommands(void);
void orderlistcommands_stereo(void);
void namecommands(void);
void nextsong(void);
void prevsong(void);
void songchange(void);
void orderleft(void);
void orderright(void);
void deleteorder(void);
void insertorder(unsigned char byte);
void orderleft_stereo(void);
void orderright_stereo(void);
void deleteorder_stereo(void);
void insertorder_stereo(unsigned char byte);

#endif