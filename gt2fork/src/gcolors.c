#define GCOLORS_C

#include "gt2fork.h"

COLORSCHEME colscheme = {};

void colscheme_init(unsigned theme)
{
    switch (theme)
    {
    case 1:
        colscheme.bgcolor = CDBLUE;
        colscheme.boxcolor = ((CDBLUE << 4) | CPURPLE);
        colscheme.command = ((CDBLUE << 4) | CDGREEN);
        colscheme.command_value = ((CDBLUE << 4) | CCYAN);
        colscheme.edit = ((CDBLUE << 4) | CYELLOW);
        colscheme.help_header = ((CDBLUE << 4) | CYELLOW);
        colscheme.help_normal = ((CDBLUE << 4) | CLBLUE);
        colscheme.indexes = ((CDBLUE << 4) | CLBLUE);
        colscheme.indexes_inv = ((CLBLUE << 4) | CDBLUE);
        colscheme.instrnum = ((CDBLUE << 4) | CWHITE);
        colscheme.mark_bg = CDRED;
        colscheme.mute = ((CDBLUE << 4) | CDGREY);
        colscheme.normal = ((CDBLUE << 4) | CLGREEN);
        colscheme.pattcol = ((CDBLUE << 4) | CWHITE);
        colscheme.playing = ((CDBLUE << 4) | CLRED);
        colscheme.playing_inv = ((CDRED << 4) | CWHITE);
        colscheme.status_bottom = ((CDBLUE << 4) | CLRED);
        colscheme.status_top = ((CLBLUE << 4) | CDBLUE);
        colscheme.table_end = ((CDBLUE << 4) | CWHITE);
        colscheme.table_modval = ((CDBLUE << 4) | CCYAN);
        colscheme.title = ((CDBLUE << 4) | CWHITE);
        break;
    default:
        colscheme.bgcolor = CBLACK;
        colscheme.boxcolor = ((CBLACK << 4) | CPURPLE);
        colscheme.command = ((CBLACK << 4) | CDGREEN);
        colscheme.command_value = ((CBLACK << 4) | CCYAN);
        colscheme.edit = ((CBLACK << 4) | CYELLOW);
        colscheme.help_header = ((CBLACK << 4) | CYELLOW);
        colscheme.help_normal = ((CBLACK << 4) | CGREY);
        colscheme.indexes = ((CBLACK << 4) | CGREY);
        colscheme.indexes_inv = ((CGREY << 4) | CBLACK);
        colscheme.instrnum = ((CBLACK << 4) | CWHITE);
        colscheme.mark_bg = ((CBLACK << 4) | CDRED);
        colscheme.mute = ((CBLACK << 4) | CDGREY);
        colscheme.normal = ((CBLACK << 4) | CLGREEN);
        colscheme.pattcol = ((CBLACK << 4) | CWHITE);
        colscheme.playing = ((CBLACK << 4) | CLRED);
        colscheme.playing_inv = ((CDRED << 4) | CWHITE);
        colscheme.status_bottom = ((CBLACK << 4) | CLRED);
        colscheme.status_top = ((CGREY << 4) | CBLACK);
        colscheme.table_end = ((CBLACK << 4) | CWHITE);
        colscheme.table_modval = ((CBLACK << 4) | CCYAN);
        colscheme.title = ((CBLACK << 4) | CWHITE);
        break;
    }
}
