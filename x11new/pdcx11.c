/* PDCurses */

#include "pdcx11.h"

#include <errno.h>
#include <stdlib.h>

unsigned char *Xcurscr;

int XCursesLINES = 24;
int XCursesCOLS = 80;

#ifdef PDCDEBUG
void XC_say(const char *msg)
{
    PDC_LOG(("%s:%s", XCLOGMSG, msg));
}
#endif

int XCursesInitscr(int argc, char *argv[])
{
    XC_LOG(("XCursesInitscr() - called\n"));

    if (ERR == XCursesSetupX(argc, argv))
        return ERR;

    XCursesLINES = SP->lines;
    LINES = XCursesLINES - SP->linesrippedoff - SP->slklines;
    XCursesCOLS = COLS = SP->cols;

    atexit(XCursesExit);

    return OK;
}
