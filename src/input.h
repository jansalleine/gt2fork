#ifndef GT2F_INPUT_H
#define GT2F_INPUT_H

#define MAX_MIDI_DEVICES 10

void closeMidiInput();
int initMidiInput();
void getkey(void);
void getMidiInput(double timeStamp, const unsigned char *message, size_t messageSize, void *userData);
int selectMidiInput();

#ifndef GT2F_INPUT_C
extern int key, rawkey, shiftpressed, altpressed, cursorflashdelay;
extern int mouseb, prevmouseb;
extern int mouseheld;
extern unsigned mousex, mousey;
extern int midiDevicePort;
#endif

#endif
