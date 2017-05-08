/* Public Domain Curses */

#include "pdcx11.h"

/* special purpose function keys */

static int PDC_shutdown_key[PDC_MAX_FUNCTION_KEYS] = { 0, 0, 0, 0, 0 };

/* COLOR_PAIR to attribute encoding table. */

short *xc_atrtab = (short *)NULL;

/* close the physical screen */

void PDC_scr_close(void)
{
    PDC_LOG(("PDC_scr_close() - called\n"));
}

void PDC_scr_free(void)
{
    XCursesExit();

    xc_atrtab = (short *)NULL;
}

/* open the physical screen -- allocate SP, miscellaneous intialization */

int PDC_scr_open(int argc, char **argv)
{
    extern bool sb_started;

    PDC_LOG(("PDC_scr_open() - called\n"));

    if ((XCursesInitscr(argc, argv) == ERR) || !SP)
        return ERR;

    SP->cursrow = SP->curscol = 0;
    SP->orig_attr = FALSE;
    SP->sb_on = sb_started;
    SP->sb_total_y = 0;
    SP->sb_viewport_y = 0;
    SP->sb_cur_y = 0;
    SP->sb_total_x = 0;
    SP->sb_viewport_x = 0;
    SP->sb_cur_x = 0;

    return OK;
}

/* the core of resize_term() */

int PDC_resize_screen(int nlines, int ncols)
{
    PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
             nlines, ncols));

    if( !stdscr)      /* window hasn't been created yet;  we're */
    {                 /* specifying its size before doing so    */
        XCursesLINES = nlines;
        XCursesCOLS = ncols;
        return OK;
    }

    if (nlines || ncols || !SP->resized)
        return ERR;

    shmdt((char *)Xcurscr);
    XCursesInstructAndWait(CURSES_RESIZE);

    if ((shmid_Xcurscr = shmget(shmkey_Xcurscr,
        SP->XcurscrSize + XCURSESSHMMIN, 0700)) < 0)
    {
        perror("Cannot allocate shared memory for curscr");
        kill(xc_otherpid, SIGKILL);
        return ERR;
    }

    XCursesLINES = SP->lines;
    XCursesCOLS = SP->cols;

    PDC_LOG(("%s:shmid_Xcurscr %d shmkey_Xcurscr %d SP->lines %d "
             "SP->cols %d\n", XCLOGMSG, shmid_Xcurscr,
             shmkey_Xcurscr, SP->lines, SP->cols));

    Xcurscr = (unsigned char*)shmat(shmid_Xcurscr, 0, 0);
    xc_atrtab = (short *)(Xcurscr + XCURSCR_ATRTAB_OFF);

    SP->resized = FALSE;

    return OK;
}

void PDC_reset_prog_mode(void)
{
    PDC_LOG(("PDC_reset_prog_mode() - called.\n"));
}

void PDC_reset_shell_mode(void)
{
    PDC_LOG(("PDC_reset_shell_mode() - called.\n"));
}

void PDC_restore_screen_mode(int i)
{
}

void PDC_save_screen_mode(int i)
{
}

void PDC_init_pair(short pair, short fg, short bg)
{
    xc_atrtab[pair * 2] = fg;
    xc_atrtab[pair * 2 + 1] = bg;
}

int PDC_pair_content(short pair, short *fg, short *bg)
{
    *fg = xc_atrtab[pair * 2];
    *bg = xc_atrtab[pair * 2 + 1];

    return OK;
}

bool PDC_can_change_color(void)
{
    return TRUE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
    XColor *tmp = (XColor *)(Xcurscr + XCURSCR_XCOLOR_OFF);

    tmp->pixel = color;

    XCursesInstructAndWait(CURSES_GET_COLOR);

    *red = ((double)(tmp->red) * 1000 / 65535) + 0.5;
    *green = ((double)(tmp->green) * 1000 / 65535) + 0.5;
    *blue = ((double)(tmp->blue) * 1000 / 65535) + 0.5;

    return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
    XColor *tmp = (XColor *)(Xcurscr + XCURSCR_XCOLOR_OFF);

    tmp->pixel = color;

    tmp->red = ((double)red * 65535 / 1000) + 0.5;
    tmp->green = ((double)green * 65535 / 1000) + 0.5;
    tmp->blue = ((double)blue * 65535 / 1000) + 0.5;

    XCursesInstructAndWait(CURSES_SET_COLOR);

    return OK;
}

/*man-start**************************************************************

Function keys
-------------

### Synopsis

   int PDC_set_function_key( const unsigned function, const int new_key);

### Description

   Allows one to set a 'shut down' key,  and reassign hotkeys used for
   pasting from the clipboard and enlarging and decreasing the font size,
   and for using the font selection dialog (on platforms where these
   things are possible and implemented).  For example, calling

   PDC_set_function_key( FUNCTION_KEY_SHUT_DOWN, ALT_Q);

   would reset PDCurses such that,  if the user clicks on the 'close' box,
   Alt-Q would be added to the key queue.  This would give the app the
   opportunity to shut things down gracefully,  perhaps asking "are you
   sure",  and/or "save changes or discard or cancel",  rather than just
   having the window close (the default behavior).

   Similarly,  one can set FUNCTION_KEY_ABORT to a key which,  when pressed,
   will cause the program to abort gracelessly (no key returned to the
   application).  One would normally use this to enable/disable Ctrl-C or
   Ctrl-Break.

### Return Value

   Returns key code previously set for that function,  or -1 if the
   function does not actually exist.

### Portability

   PDCurses-only function.

**man-end****************************************************************/

int PDC_set_function_key( const unsigned function, const int new_key)
{
    int old_key = -1;

    if (function < PDC_MAX_FUNCTION_KEYS)
    {
         old_key = PDC_shutdown_key[function];
         PDC_shutdown_key[function] = new_key;
    }
    if (function == FUNCTION_KEY_SHUT_DOWN)
    {
        SP->exit_key = new_key;
    }
    return(old_key);
}


/*man-start**************************************************************

Resize limits
-------------

### Synopsis

    void PDC_set_resize_limits( const int new_min_lines,
                                const int new_max_lines,
                                const int new_min_cols,
                                const int new_max_cols);

### Description

   For platforms supporting resizable windows (SDLx, Win32a, X11).  Some
   programs may be unprepared for a resize event;  for these,  calling
   this function with the max and min limits equal ensures that no
   user resizing can be done.  Other programs may require at least a
   certain number,  and/or no more than a certain number,  of columns
   and/or lines.

### Portability

   PDCurses-only function.

**man-end****************************************************************/

/* Note that at least at present,  only Win32a pays any attention to
resize limits. */

int PDC_min_lines = 25, PDC_min_cols = 80;
int PDC_max_lines = 25, PDC_max_cols = 80;

void PDC_set_resize_limits( const int new_min_lines, const int new_max_lines,
                  const int new_min_cols, const int new_max_cols)
{
    PDC_min_lines = max( new_min_lines, 2);
    PDC_max_lines = max( new_max_lines, PDC_min_lines);
    PDC_min_cols = max( new_min_cols, 2);
    PDC_max_cols = max( new_max_cols, PDC_min_cols);
}
