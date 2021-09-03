/* =============================================================================
 * Colors:
 *
 * 0: Black         CBLACK
 * 1: Dark Blue     CDBLUE
 * 2: Dark Green    CDGREEN
 * 3: Dark Grey     CDGREY
 * 4: Dark Red      CDRED
 * 5: Dark Brown    CDBROWN
 * 6: Light Brown   CLBROWN
 * 7: Light Grey    CLGREY
 * 8: Grey          CGREY
 * 9: Light Blue    CLBLUE
 * A: Light Green   CLGREEN
 * B: Cyan          CCYAN
 * C: Light Red     CLRED
 * D: Purple        CPURPLE
 * E: Yellow        CYELLOW
 * F: White         CWHITE
 * =============================================================================
 */

#define GT2F_COLORS_C

#include "gt2fork.h"

COLORSCHEME colscheme = {};

void colscheme_init(unsigned theme)
{
    switch (theme)
    {
        case 1:
            colscheme.bgColor = CDBLUE;
            colscheme.boxColor = ((CDBLUE << 4) | CPURPLE);
            colscheme.command = ((CDBLUE << 4) | CDGREEN);
            colscheme.commandValue = ((CDBLUE << 4) | CCYAN);
            colscheme.edit = ((CDBLUE << 4) | CYELLOW);
            colscheme.helpHeader = ((CDBLUE << 4) | CYELLOW);
            colscheme.helpNormal = ((CDBLUE << 4) | CLBLUE);
            colscheme.indexes = ((CDBLUE << 4) | CLBLUE);
            colscheme.indexesInv = ((CLBLUE << 4) | CDBLUE);
            colscheme.instrnum = ((CDBLUE << 4) | CWHITE);
            colscheme.markBg = CDRED;
            colscheme.mute = ((CDBLUE << 4) | CDGREY);
            colscheme.normal = ((CDBLUE << 4) | CLGREEN);
            colscheme.pattcol = ((CDBLUE << 4) | CWHITE);
            colscheme.playing = ((CDBLUE << 4) | CLRED);
            colscheme.playingInv = ((CDRED << 4) | CWHITE);
            colscheme.statusBottom = ((CDBLUE << 4) | CLRED);
            colscheme.statusTop = ((CLBLUE << 4) | CDBLUE);
            colscheme.tableEnd = ((CDBLUE << 4) | CWHITE);
            colscheme.tableModVal = ((CDBLUE << 4) | CCYAN);
            colscheme.title = ((CDBLUE << 4) | CWHITE);
            break;
        default:
            colscheme.bgColor = CBLACK;
            colscheme.boxColor = ((CBLACK << 4) | CPURPLE);
            colscheme.command = ((CBLACK << 4) | CDGREEN);
            colscheme.commandValue = ((CBLACK << 4) | CCYAN);
            colscheme.edit = ((CBLACK << 4) | CYELLOW);
            colscheme.helpHeader = ((CBLACK << 4) | CYELLOW);
            colscheme.helpNormal = ((CBLACK << 4) | CGREY);
            colscheme.indexes = ((CBLACK << 4) | CGREY);
            colscheme.indexesInv = ((CGREY << 4) | CBLACK);
            colscheme.instrnum = ((CBLACK << 4) | CWHITE);
            colscheme.markBg = ((CBLACK << 4) | CDRED);
            colscheme.mute = ((CBLACK << 4) | CDGREY);
            colscheme.normal = ((CBLACK << 4) | CLGREEN);
            colscheme.pattcol = ((CBLACK << 4) | CWHITE);
            colscheme.playing = ((CBLACK << 4) | CLRED);
            colscheme.playingInv = ((CDRED << 4) | CWHITE);
            colscheme.statusBottom = ((CBLACK << 4) | CLRED);
            colscheme.statusTop = ((CGREY << 4) | CBLACK);
            colscheme.tableEnd = ((CBLACK << 4) | CWHITE);
            colscheme.tableModVal = ((CBLACK << 4) | CCYAN);
            colscheme.title = ((CBLACK << 4) | CWHITE);
            break;
    }
}
