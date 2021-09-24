/**
 * =============================================================================
 * user input routines
 * =============================================================================
 */

#define GT2F_INPUT_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtmidi/rtmidi_c.h"
#include "gt2fork.h"

int key = 0;
int rawkey = 0;
int shiftpressed = 0;
int altpressed = 0;
int cursorflashdelay = 0;
int mouseb = 0;
int prevmouseb = 0;
unsigned mousex = 0;
unsigned mousey = 0;
unsigned mousepixelx = 0;
unsigned mousepixely = 0;
unsigned oldmousepixelx = 0xffffffff;
unsigned oldmousepixely = 0xffffffff;
int mouseheld = 0;
int mousesizex = 11;
int mousesizey = 20;
int midiDevicePort = 0;

RtMidiInPtr midi_device = NULL;

void closeMidiInput()
{
    if (midi_device != NULL)
    {
        rtmidi_in_free(midi_device);
    }
}

int initMidiInput()
{
    int port = midiDevicePort;

    midi_device = rtmidi_in_create(RTMIDI_API_UNSPECIFIED, "gt2fork", 100);
    if (!midi_device->ok)
    {
        fprintf(
            stderr,
            "[ERROR] MIDI not available: %s\n",
            midi_device->msg
        );
        return 0;
    }

    rtmidi_open_port(midi_device, port, "midi_in");
    if (!midi_device->ok)
    {
        fprintf(
            stderr,
            "[ERROR] Can't open MIDI port #%i: %s\n",
            port, midi_device->msg
        );
        return 0;
    }

    rtmidi_in_set_callback(midi_device, getMidiInput, NULL);
    if (!midi_device->ok)
    {
        fprintf(
            stderr,
            "[ERROR] MIDI callback not available: %s\n",
            midi_device->msg
        );
        return 0;
    }

    return 1;
}

int selectMidiInput()
{
    int exit = 0;
    int i;
    int port = midiDevicePort;
    int numPorts = rtmidi_get_port_count(midi_device);
    char portNames[MAX_MIDI_DEVICES][128];
    int once = 1;

    if (midi_device == NULL)
    {
        return 0;
    }

    if (numPorts > 0)
    {
        for (i = 0; i < numPorts; i++)
        {
            sprintf(portNames[i], rtmidi_get_port_name(midi_device, i), "%s");
        }
    }
    else
    {
        port = 0;
        exit = 1;
    }

    rtmidi_close_port(midi_device);

    int cursorPosY = dpos.loadboxY+3+port;

    while (!exit)
    {
        int cursorColor = cursorcolortable[cursorflash];

        if (cursorflashdelay >= 6)
        {
            cursorflashdelay %= 6;
            cursorflash++;
            cursorflash &= 3;
        }
        fliptoscreen();
        getkey();

        switch(rawkey)
        {
        case KEY_ESC:
            exit = 1;
            break;
        case KEY_UP:
            if (cursorPosY - dpos.loadboxY - 3 > 0)
            {
                printbg(
                    dpos.loadboxX-62/2+2,
                    cursorPosY,
                    colscheme.bgColor,
                    1
                );
                cursorPosY--;
            }
            break;
        case KEY_DOWN:
            if (cursorPosY - dpos.loadboxY - 3 < numPorts)
            {
                printbg(
                    dpos.loadboxX-62/2+2,
                    cursorPosY,
                    colscheme.bgColor,
                    1
                );
                cursorPosY++;
            }
            break;
        case KEY_ENTER:
            exit = 1;
        case KEY_SPACE:
            port = cursorPosY - dpos.loadboxY - 3;
            break;
        }

        if (once)
        {
            for (i = 0; i < 33; i++)
            {
                printblankc(
                    dpos.loadboxX-62/2+1,
                    dpos.loadboxY+1+i,
                    colscheme.title,
                    60
                );
            }
            drawbox(
                dpos.loadboxX-62/2,
                dpos.loadboxY,
                colscheme.boxColor,
                62, 35
            );
            printtext(
                dpos.loadboxX-62/2+1,
                dpos.loadboxY+1,
                colscheme.title,
                "Select MIDI device:"
            );
            for (i = 0; i < numPorts; i++)
            {
                printtext(
                    dpos.loadboxX-62/2+1,
                    dpos.loadboxY+3+i,
                    colscheme.title,
                    "[ ]"
                );

                printtext(
                    dpos.loadboxX-62/2+5,
                    dpos.loadboxY+3+i,
                    colscheme.normal,
                    portNames[i]
                );

            }
            printtext(
                dpos.loadboxX-62/2+1,
                dpos.loadboxY+33,
                colscheme.title,
                "ESC: exit    -    SPACE: select    -    ENTER: select & exit"
            );
            once = 0;
        }

        for (i = 0; i < numPorts; i++)
        {
            if (i == port)
            {
                printtext(
                    dpos.loadboxX-62/2+2,
                    dpos.loadboxY+3+i,
                    colscheme.title,
                    "*"
                );
            }
            else
            {
                printtext(
                    dpos.loadboxX-62/2+2,
                    dpos.loadboxY+3+i,
                    colscheme.title,
                    " "
                );
            }
        }

        printbg(
            dpos.loadboxX-62/2+2,
            cursorPosY,
            cursorColor,
            1
        );
    }

    rtmidi_open_port(midi_device, port, "midi_in");
    if (!midi_device->ok)
    {
        fprintf(stderr, "failed to open port %i: %s\n", port, midi_device->msg);
        return 0;
    }

    midiDevicePort = port;
    printmainscreen();
    return 1;
}

/*
 * Midi Commands
 * https://computermusicresource.com/MIDI.Commands.html
 *
 * COMMAND   | DATA BYTE 1       | DATA BYTE 2           | COMMENT
 * ----------+-------------------+-----------------------+----------------------
 * $80-$8F   | Key # (0-127)     | Off Velocity (0-127)  | Note OFF
 * [128-143] |                   |                       |
 * ----------+-------------------+-----------------------+----------------------
 * $90-$9F   | Key # (0-127)     | On Velocity (0-127)   | Note ON
 * [144-159] |                   |                       |
 * ----------+-------------------+-----------------------+----------------------
 * $A0-$AF   | Key # (0-127)     | Pressure (0-127)      | Poly Key Pressure
 * ----------+-------------------+-----------------------+----------------------
 * $B0-$BF   | Control # (0-127) | Control Value (0-127) | Control Change
 * [176-191] |                   |                       |
 * ----------+-------------------+-----------------------+----------------------
 * $C0-$CF   | Program # (0-127) | -- Not Used --        | Program Change
 * [192-207] |                   |                       |
 * ----------+-------------------+-----------------------+----------------------
 * $D0-$DF   | Pressure Value    | -- Not Used --        | Mono Key Pressure
 *           | (0-127)           |                       | (Channel Pressure)
 * ----------+-------------------+-----------------------+----------------------
 * $E0-$EF   | Range (LSB)       | Range (MSB)           | Pitch Bend
 * ----------+-------------------+-----------------------+----------------------
 * $F0-$FF   | Manufacturer's ID | Model ID              | System
 * ----------+-------------------+-----------------------+----------------------
 */

void getMidiInput(double timeStamp, const unsigned char *message,
                  size_t messageSize, void *userData)
{
    int i;
    unsigned char midiCommand;
    unsigned char note;
    unsigned char velocity;
    unsigned char instrument;

    for (i = 0; i < messageSize; i++)
    {
        printf("size: %lu: %02X %u %u\n", messageSize,
            *message, *(message+1), *(message+2));

        midiCommand = message[i++];

        if ((midiCommand & 0xf0) == 0x90)
        {
            note = message[i++];
            velocity = message[i];

            if (editmode == EDIT_PATTERN)
            {
                if (velocity != 0)
                {
                    note += MIDINOTEOFFSET;

                    if (note < FIRSTNOTE)
                    {
                        note = FIRSTNOTE;
                    }
                    else if (note > LASTNOTE)
                    {
                        note = LASTNOTE;
                    }
                    insertNote(note);
                    updatePatternView();
                }
                else
                {
                    /*
                     *  unsure what to do in this case
                     *  KEYOFF? REST? do nothing ?!?
                     */
                    note = KEYOFF;
                    insertNote(note);
                    updatePatternView();
                }
            }
        }
        else if ((midiCommand & 0xf0) == 0x80)
        {
            if (editmode == EDIT_PATTERN)
            {
                note = KEYOFF;
                insertNote(note);
                updatePatternView();
            }
        }
        else if ((midiCommand & 0xf0) == 0xC0)
        {
            instrument = message[i++];
            setinstr(instrument);
        }
    }
}

void getkey(void)
{
    int c;
    win_asciikey = 0;
    cursorflashdelay += win_getspeed(50);

    prevmouseb = mouseb;

    mou_getpos(&mousepixelx, &mousepixely);
    mouseb = mou_getbuttons();
    mousex = mousepixelx / fontwidth;
    mousey = mousepixely / fontheight;

    if (mouseb)
    {
        mouseheld++;
    }
    else
    {
        mouseheld = 0;
    }

    key = win_asciikey;
    rawkey = 0;
    for (c = 0; c < SDL_NUM_SCANCODES; c++)
    {
        if (win_keytable[c])
        {
            if ((c != SDL_SCANCODE_LSHIFT) && (c != SDL_SCANCODE_RSHIFT) &&
                    (c != SDL_SCANCODE_LCTRL) && (c != SDL_SCANCODE_RCTRL))
            {
                rawkey = c;
                win_keytable[c] = 0;
                break;
            }
        }
    }

    shiftpressed = 0;
    if ((win_keystate[SDL_SCANCODE_LSHIFT])||
            (win_keystate[SDL_SCANCODE_RSHIFT])||
            (win_keystate[SDL_SCANCODE_LCTRL])||
            (win_keystate[SDL_SCANCODE_RCTRL]))
    {
        shiftpressed = 1;
    }

    altpressed = 0;
    if ((win_keystate[SDL_SCANCODE_LALT])||
        (win_keystate[SDL_SCANCODE_RALT]))
    {
        altpressed = 1;
    }

    if (rawkey == SDL_SCANCODE_KP_ENTER)
    {
        key = KEY_ENTER;
        rawkey = SDL_SCANCODE_RETURN;
    }

    if (rawkey == SDL_SCANCODE_KP_0) key = '0';
    if (rawkey == SDL_SCANCODE_KP_1) key = '1';
    if (rawkey == SDL_SCANCODE_KP_2) key = '2';
    if (rawkey == SDL_SCANCODE_KP_3) key = '3';
    if (rawkey == SDL_SCANCODE_KP_4) key = '4';
    if (rawkey == SDL_SCANCODE_KP_5) key = '5';
    if (rawkey == SDL_SCANCODE_KP_6) key = '6';
    if (rawkey == SDL_SCANCODE_KP_7) key = '7';
    if (rawkey == SDL_SCANCODE_KP_8) key = '8';
    if (rawkey == SDL_SCANCODE_KP_9) key = '9';
}
