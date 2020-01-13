/* Public Domain Curses */

#include "pdcx11.h"

#include <string.h>

/*man-start**************************************************************

pdcsetsc
--------

### Synopsis

    int PDC_set_blink(bool blinkon);
    int PDC_set_bold(bool boldon);
    void PDC_set_title(const char *title);

### Description

   PDC_set_blink() toggles whether the A_BLINK attribute sets an actual
   blink mode (TRUE), or sets the background color to high intensity
   (FALSE). The default is platform-dependent (FALSE in most cases). It
   returns OK if it could set the state to match the given parameter,
   ERR otherwise.

   PDC_set_bold() toggles whether the A_BOLD attribute selects an actual
   bold font (TRUE), or sets the foreground color to high intensity
   (FALSE). It returns OK if it could set the state to match the given
   parameter, ERR otherwise.

   PDC_set_title() sets the title of the window in which the curses
   program is running. This function may not do anything on some
   platforms. (Currently it only works in Win32 and X11.)

### Portability
                             X/Open    BSD    SYS V
    PDC_set_blink               -       -       -
    PDC_set_bold                -       -       -
    PDC_set_title               -       -       -

**man-end****************************************************************/

int PDC_curs_set(int visibility)
{
    int ret_vis = SP->visibility;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    if (visibility != -1)
        SP->visibility = visibility;

    PDC_display_cursor(SP->cursrow, SP->curscol, SP->cursrow,
                       SP->curscol, visibility);

    return ret_vis;
}

void PDC_set_title(const char *title)
{
    int len;

    PDC_LOG(("PDC_set_title() - called:<%s>\n", title));

    len = strlen(title) + 1;        /* write nul character */

    XCursesInstruct(CURSES_TITLE);

    if (XC_write_display_socket_int(len) >= 0)
        if (XC_write_socket(xc_display_sock, title, len) >= 0)
            return;

    XCursesExitCursesProcess(1, "exiting from PDC_set_title");
}

        /* If SP->termattrs & A_BLINK != 0, then text with the A_BLINK   */
        /* attribute will actually blink.  Otherwise,  such text will    */
        /* be shown with higher color intensity (the R, G, and B values  */
        /* are averaged with pure white).  See pdcdisp.c for details of  */
        /* how this is done.                                             */
        /*     Unlike on other PDCurses platforms,  this doesn't require */
        /* decreasing the number of colors by half.  Also,  the choice   */
        /* indicated by 'blinkon' will actually be respected,  so OK is  */
        /* always returned (most platforms don't actually support        */
        /* blinking).                                                    */
        /*      The default behavior is to not show A_BLINK text as      */
        /* blinking,  i.e.,  SP->termattrs & A_BLINK = 0.  Blinking text */
        /* can be pretty annoying to some people.  You should probably   */
        /* call PDC_set_blink( TRUE) only if there is something to which */
        /* the user _must_ pay attention;  say,  "the nuclear reactor    */
        /* is about to melt down".  Otherwise,  the bolder,  brighter    */
        /* text should be attention-getting enough.                      */

int PDC_set_blink(bool blinkon)
{
    if (!SP)
        return ERR;

    if (SP->color_started)
        COLORS = 256;

    if (blinkon)
        SP->termattrs |= A_BLINK;
    else
        SP->termattrs &= ~A_BLINK;

    XCursesInstruct( blinkon ? CURSES_BLINK_ON : CURSES_BLINK_OFF);
    return OK;
}

int PDC_set_bold(bool boldon)
{
    if (!SP)
        return ERR;

    if (boldon)
        SP->termattrs |= A_BOLD;
    else
        SP->termattrs &= ~A_BOLD;
    return OK;
}
