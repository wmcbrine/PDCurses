/* PDCurses */

#include "pdcdos.h"

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
   ERR otherwise. On DOS, this function also adjusts the value of COLORS
   -- 16 for FALSE, and 8 for TRUE.

   PDC_set_bold() toggles whether the A_BOLD attribute selects an actual
   bold font (TRUE), or sets the foreground color to high intensity
   (FALSE). It returns OK if it could set the state to match the given
   parameter, ERR otherwise.

   PDC_set_title() sets the title of the window in which the curses
   program is running. This function may not do anything on some
   platforms.

### Portability
                             X/Open  ncurses  NetBSD
    PDC_set_blink               -       -       -
    PDC_set_title               -       -       -

**man-end****************************************************************/

int PDC_curs_set(int visibility)
{
    int ret_vis, start, end;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    ret_vis = SP->visibility;
    SP->visibility = visibility;

    switch (visibility)
    {
        case 0:  /* invisible */
            start = 1;
            end = 0;
            break;
        case 2:  /* highly visible */
            start = 0;   /* full-height block */
            end = PDC_state.font_height - 1;
            break;
        default:  /* normal visibility */
            start = SP->orig_cursor >> 8;
            end =   SP->orig_cursor & 0xFF;
    }

    PDC_private_cursor_off();
    PDC_state.cursor_start = start;
    PDC_state.cursor_end = end;
    if (visibility != 0)
        PDC_private_cursor_on(PDC_state.cursor_row, PDC_state.cursor_col);

    return ret_vis;
}

void PDC_set_title(const char *title)
{
    PDC_LOG(("PDC_set_title() - called: <%s>\n", title));
}

int PDC_set_blink(bool blinkon)
{
    return blinkon ? ERR : OK;
}

int PDC_set_bold(bool boldon)
{
    /* TODO: support this */
    return boldon ? ERR : OK;
}
