/* PDCurses */

#include "pdcx11.h"

#include <errno.h>
#include <stdlib.h>

/*** Functions that are called by both processes ***/

unsigned char *Xcurscr;

int XCursesLINES = 24;
int XCursesCOLS = 80;

#ifdef PDCDEBUG
void XC_say(const char *msg)
{
    PDC_LOG(("%s:%s", XCLOGMSG, msg));
}
#endif

/*** Functions that are called by the "curses" process ***/

static int _setup_curses(void)
{
    int wait_value;

    XC_LOG(("_setup_curses called\n"));

    /* Set LINES and COLS now so that the size of the shared memory
       segment can be allocated */

    XCursesLINES = SP->lines;
    LINES = XCursesLINES - SP->linesrippedoff - SP->slklines;
    XCursesCOLS = COLS = SP->cols;

    atexit(XCursesExit);

    return OK;
}

int XCursesInitscr(int argc, char *argv[])
{
    int pid, rc;

    XC_LOG(("XCursesInitscr() - called\n"));

    rc = XCursesSetupX(argc, argv);
    rc = _setup_curses();

    return rc;
}

void XCursesExit(void)
{
    XC_LOG(("XCursesExit() - called\n"));

    XC_exit_process(0, 0, "XCursesProcess requested to exit");
}
