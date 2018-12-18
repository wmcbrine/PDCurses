PDCurses User's Guide
=====================

Curses Overview
---------------

The X/Open Curses Interface Definition describes a set of C-Language
functions that provide screen-handling and updating, which are
collectively known as the curses library.

The curses library permits manipulation of data structures called
windows which may be thought of as two-dimensional arrays of
characters representing all or part of a terminal's screen.  The
windows are manipulated using a procedural interface described
elsewhere.  The curses package maintains a record of what characters
are on the screen.  At the most basic level, manipulation is done with
the routines move() and addch() which are used to "move" the curses
around and add characters to the default window, stdscr, which
represents the whole screen.

An application may use these routines to add data to the window in any
convenient order.  Once all data have been added, the routine
refresh() is called.  The package then determines what changes have
been made which affect the screen.  The screen contents are then
changed to reflect those characters now in the window, using a
sequence of operations optimized for the type of terminal in use.

At a higher level routines combining the actions of move() and addch()
are defined, as are routines to add whole strings and to perform
format conversions in the manner of printf().

Interfaces are also defined to erase the entire window and to specify
the attributes of individual characters in the window.  Attributes
such as inverse video, underline and blink can be used on a
per-character basis.

New windows can be created by allowing the application to build
several images of the screen and display the appropriate one very
quickly.  New windows are created using the routine newwin().  For
each routine that manipulates the default window, stdscr, there is a
corresponding routine prefixed with w to manipulate the contents of a
specified window; for example, move() and wmove().  In fact, move(...)
is functionally equivalent to wmove( stdscr, ...).  This is similar to
the interface offered by printf(...) and fprintf(stdout, ...).

Windows do not have to correspond to the entire screen.  It is
possible to create smaller windows, and also to indicate that the
window is only partially visible on the screen.  Furthermore, large
windows or pads, which are bigger than the actual screen size, may be
created.

Interfaces are also defined to allow input character manipulation and
to disable and enable many input attributes: character echo, single
character input with or without signal processing (cbreak or raw
modes), carriage returns mapping to newlines, screen scrolling, etc.


Data Types and the \<curses.h\> Header
--------------------------------------

The data types supported by curses are described in this section.

As the library supports a procedural interface to the data types, actual
structure contents are not described.  All curses data are manipulated
using the routines provided.


### The \<curses.h\> Header

The \<curses.h\> header defines various constants and declares the data
types that are available to the application.


### Data Types

The following data types are declared:

    WINDOW *  pointer to screen representation
    SCREEN *  pointer to terminal descriptor
    bool      boolean data type
    chtype    representation of a character in a window
    cchar_t   the wide-character equivalent of chtype
    attr_t    for WA_-style attributes

The actual WINDOW and SCREEN objects used to store information are
created by the corresponding routines and a pointer to them is provided.
All manipulation is through that pointer.


### Variables

The following variables are defined:

    LINES         number of lines on terminal screen
    COLS          number of columns on terminal screen
    stdscr        pointer to the default screen window
    curscr        pointer to the current screen image
    SP            pointer to the current SCREEN struct
    Mouse_status  status of the mouse
    COLORS        number of colors available
    COLOR_PAIRS   number of color pairs available
    TABSIZE       size of one TAB block
    acs_map[]     alternate character set map
    ttytype[]     terminal name/description


### Constants

The following constants are defined:

#### General

    FALSE         boolean false value
    TRUE          boolean true value
    NULL          zero pointer value
    ERR           value returned on error condition
    OK            value returned on successful completion

#### Video Attributes

Normally, attributes are a property of the character.

For chtype:

    A_ALTCHARSET  use the alternate character set
    A_BLINK       bright background or blinking
    A_BOLD        bright foreground or bold
    A_DIM         half bright -- no effect in PDCurses
    A_INVIS       invisible -- no effect in PDCurses
    A_ITALIC      italic
    A_LEFT        line along the left edge
    A_PROTECT     protected -- no effect in PDCurses
    A_REVERSE     reverse video
    A_RIGHT       line along the right edge
    A_STANDOUT    terminal's best highlighting mode
    A_UNDERLINE   underline

    A_ATTRIBUTES  bit-mask to extract attributes
    A_CHARTEXT    bit-mask to extract a character
    A_COLOR       bit-mask to extract a color-pair

Not all attributes will work on all terminals. A_ITALIC is not standard,
but is shared with ncurses.

For attr_t:

    WA_ALTCHARSET same as A_ALTCHARSET
    WA_BLINK      same as A_BLINK
    WA_BOLD       same as A_BOLD
    WA_DIM        same as A_DIM
    WA_INVIS      same as A_INVIS
    WA_ITALIC     same as A_ITALIC
    WA_LEFT       same as A_LEFT
    WA_PROTECT    same as A_PROTECT
    WA_REVERSE    same as A_REVERSE
    WA_RIGHT      same as A_RIGHT
    WA_STANDOUT   same as A_STANDOUT
    WA_UNDERLINE  same as A_UNDERLINE

The following are also defined, for compatibility, but currently have no
effect in PDCurses: A_HORIZONTAL, A_LOW, A_TOP, A_VERTICAL and their
WA_* equivalents.

### The Alternate Character Set

For use in chtypes and with related functions. These are a portable way
to represent graphics characters on different terminals.

VT100-compatible symbols -- box characters:

    ACS_ULCORNER  upper left box corner
    ACS_LLCORNER  lower left box corner
    ACS_URCORNER  upper right box corner
    ACS_LRCORNER  lower right box corner
    ACS_RTEE      right "T"
    ACS_LTEE      left "T"
    ACS_BTEE      bottom "T"
    ACS_TTEE      top "T"
    ACS_HLINE     horizontal line
    ACS_VLINE     vertical line
    ACS_PLUS      plus sign, cross, or four-corner piece

VT100-compatible symbols -- other:

    ACS_S1        scan line 1
    ACS_S9        scan line 9
    ACS_DIAMOND   diamond
    ACS_CKBOARD   checkerboard -- 50% grey
    ACS_DEGREE    degree symbol
    ACS_PLMINUS   plus/minus sign
    ACS_BULLET    bullet

Teletype 5410v1 symbols -- these are defined in SysV curses, but are not
well-supported by most terminals. Stick to VT100 characters for optimum
portability:

    ACS_LARROW    left arrow
    ACS_RARROW    right arrow
    ACS_DARROW    down arrow
    ACS_UARROW    up arrow
    ACS_BOARD     checkerboard -- lighter (less dense) than
                  ACS_CKBOARD
    ACS_LANTERN   lantern symbol
    ACS_BLOCK     solid block

That goes double for these -- undocumented SysV symbols. Don't use them:

    ACS_S3        scan line 3
    ACS_S7        scan line 7
    ACS_LEQUAL    less than or equal
    ACS_GEQUAL    greater than or equal
    ACS_PI        pi
    ACS_NEQUAL    not equal
    ACS_STERLING  pounds sterling symbol

Box character aliases:

    ACS_BSSB      same as ACS_ULCORNER
    ACS_SSBB      same as ACS_LLCORNER
    ACS_BBSS      same as ACS_URCORNER
    ACS_SBBS      same as ACS_LRCORNER
    ACS_SBSS      same as ACS_RTEE
    ACS_SSSB      same as ACS_LTEE
    ACS_SSBS      same as ACS_BTEE
    ACS_BSSS      same as ACS_TTEE
    ACS_BSBS      same as ACS_HLINE
    ACS_SBSB      same as ACS_VLINE
    ACS_SSSS      same as ACS_PLUS

For cchar_t and wide-character functions, WACS_ equivalents are also
defined.

### Colors

For use with init_pair(), color_set(), etc.:

    COLOR_BLACK
    COLOR_BLUE
    COLOR_GREEN
    COLOR_CYAN
    COLOR_RED
    COLOR_MAGENTA
    COLOR_YELLOW
    COLOR_WHITE

Use these instead of numeric values. The definition of the colors
depends on the implementation of curses.


### Input Values

The following constants might be returned by getch() if keypad() has
been enabled.  Note that not all of these may be supported on a
particular terminal:

    KEY_BREAK     break key
    KEY_DOWN      the four arrow keys
    KEY_UP
    KEY_LEFT
    KEY_RIGHT
    KEY_HOME      home key (upward+left arrow)
    KEY_BACKSPACE backspace
    KEY_F0        function keys; space for 64 keys is reserved
    KEY_F(n)      (KEY_F0+(n))
    KEY_DL        delete line
    KEY_IL        insert line
    KEY_DC        delete character
    KEY_IC        insert character
    KEY_EIC       exit insert character mode
    KEY_CLEAR     clear screen
    KEY_EOS       clear to end of screen
    KEY_EOL       clear to end of line
    KEY_SF        scroll 1 line forwards
    KEY_SR        scroll 1 line backwards (reverse)
    KEY_NPAGE     next page
    KEY_PPAGE     previous page
    KEY_STAB      set tab
    KEY_CTAB      clear tab
    KEY_CATAB     clear all tabs
    KEY_ENTER     enter or send
    KEY_SRESET    soft (partial) reset
    KEY_RESET     reset or hard reset
    KEY_PRINT     print or copy
    KEY_LL        home down or bottom (lower left)
    KEY_A1        upper left of virtual keypad
    KEY_A3        upper right of virtual keypad
    KEY_B2        center of virtual keypad
    KEY_C1        lower left of virtual keypad
    KEY_C3        lower right of virtual keypad

    KEY_BTAB      Back tab key
    KEY_BEG       Beginning key
    KEY_CANCEL    Cancel key
    KEY_CLOSE     Close key
    KEY_COMMAND   Cmd (command) key
    KEY_COPY      Copy key
    KEY_CREATE    Create key
    KEY_END       End key
    KEY_EXIT      Exit key
    KEY_FIND      Find key
    KEY_HELP      Help key
    KEY_MARK      Mark key
    KEY_MESSAGE   Message key
    KEY_MOVE      Move key
    KEY_NEXT      Next object key
    KEY_OPEN      Open key
    KEY_OPTIONS   Options key
    KEY_PREVIOUS  Previous object key
    KEY_REDO      Redo key
    KEY_REFERENCE Reference key
    KEY_REFRESH   Refresh key
    KEY_REPLACE   Replace key
    KEY_RESTART   Restart key
    KEY_RESUME    Resume key
    KEY_SAVE      Save key
    KEY_SBEG      Shifted beginning key
    KEY_SCANCEL   Shifted cancel key
    KEY_SCOMMAND  Shifted command key
    KEY_SCOPY     Shifted copy key
    KEY_SCREATE   Shifted create key
    KEY_SDC       Shifted delete char key
    KEY_SDL       Shifted delete line key
    KEY_SELECT    Select key
    KEY_SEND      Shifted end key
    KEY_SEOL      Shifted clear line key
    KEY_SEXIT     Shifted exit key
    KEY_SFIND     Shifted find key
    KEY_SHELP     Shifted help key
    KEY_SHOME     Shifted home key
    KEY_SIC       Shifted input key
    KEY_SLEFT     Shifted left arrow key
    KEY_SMESSAGE  Shifted message key
    KEY_SMOVE     Shifted move key
    KEY_SNEXT     Shifted next key
    KEY_SOPTIONS  Shifted options key
    KEY_SPREVIOUS Shifted prev key
    KEY_SPRINT    Shifted print key
    KEY_SREDO     Shifted redo key
    KEY_SREPLACE  Shifted replace key
    KEY_SRIGHT    Shifted right arrow
    KEY_SRSUME    Shifted resume key
    KEY_SSAVE     Shifted save key
    KEY_SSUSPEND  Shifted suspend key
    KEY_SUNDO     Shifted undo key
    KEY_SUSPEND   Suspend key
    KEY_UNDO      Undo key

The virtual keypad is arranged like this:

    A1     up     A3
    left   B2  right
    C1    down    C3

This list is incomplete -- see curses.h for the full list, and use the
testcurs demo to see what values are actually returned. The above are
just the keys required by X/Open. In particular, PDCurses defines many
CTL_ and ALT_ combinations; these are not portable.

--------------------------------------------------------------------------
PDCurses Definitions and Variables
==================================

PDCurses definitions list:  (Only define those needed)

    XCURSES         True if compiling for X11.
    PDC_RGB         True if you want to use RGB color definitions
                    (Red = 1, Green = 2, Blue = 4) instead of BGR.
    PDC_WIDE        True if building wide-character support.
    PDC_DLL_BUILD   True if building a Windows DLL.
    NCURSES_MOUSE_VERSION   Use the ncurses mouse API instead
                            of PDCurses' traditional mouse API.

PDCurses portable platform definitions list:

    PDC_BUILD       Defines API build version.
    PDCURSES        Enables access to PDCurses-only routines.
    XOPEN           Always true.
    SYSVcurses      True if you are compiling for SYSV portability.
    BSDcurses       True if you are compiling for BSD portability.



--------------------------------------------------------------------------


PDCurses Text Attributes
========================

PDCurses uses a long (32 bits) for its chtype, as in System V.

The following is the structure of a win->_attrs chtype:

    +--------------------------------------------------------------------+
    |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|..| 2| 1| 0|
    +--------------------------------------------------------------------+
          color pair        |     modifiers         |   character eg 'a'

The available non-color attributes are bold, underline, right-line,
left-line, italic, reverse and blink, plus the alternate character set
indicator. 256 color pairs (8 bits), 8 bits for other attributes, and 16
bits for character data.



--------------------------------------------------------------------------

PDCurses Functions
==================

addch
-----

### Synopsis

    int addch(const chtype ch);
    int waddch(WINDOW *win, const chtype ch);
    int mvaddch(int y, int x, const chtype ch);
    int mvwaddch(WINDOW *win, int y, int x, const chtype ch);
    int echochar(const chtype ch);
    int wechochar(WINDOW *win, const chtype ch);

    int addrawch(chtype ch);
    int waddrawch(WINDOW *win, chtype ch);
    int mvaddrawch(int y, int x, chtype ch);
    int mvwaddrawch(WINDOW *win, int y, int x, chtype ch);

    int add_wch(const cchar_t *wch);
    int wadd_wch(WINDOW *win, const cchar_t *wch);
    int mvadd_wch(int y, int x, const cchar_t *wch);
    int mvwadd_wch(WINDOW *win, int y, int x, const cchar_t *wch);
    int echo_wchar(const cchar_t *wch);
    int wecho_wchar(WINDOW *win, const cchar_t *wch);

### Description

   addch() adds the chtype ch to the default window (stdscr) at the
   current cursor position, and advances the cursor. Note that
   chtypes can convey both text (a single character) and
   attributes, including a color pair. add_wch() is the wide-
   character version of this function, taking a pointer to a
   cchar_t instead of a chtype.

   waddch() is like addch(), but also lets you specify the window.
   (This is in fact the core output routine.) wadd_wch() is the
   wide version.

   mvaddch() moves the cursor to the specified (y, x) position, and
   adds ch to stdscr. mvadd_wch() is the wide version.

   mvwaddch() moves the cursor to the specified position and adds
   ch to the specified window. mvwadd_wch() is the wide version.

   echochar() adds ch to stdscr at the current cursor position and
   calls refresh(). echo_wchar() is the wide version.

   wechochar() adds ch to the specified window and calls
   wrefresh(). wecho_wchar() is the wide version.

   addrawch(), waddrawch(), mvaddrawch() and mvwaddrawch() are
   PDCurses-specific wrappers for addch() etc. that disable the
   translation of control characters.

   The following applies to all these functions:

   If the cursor moves on to the right margin, an automatic newline
   is performed.  If scrollok is enabled, and a character is added
   to the bottom right corner of the window, the scrolling region
   will be scrolled up one line.  If scrolling is not allowed, ERR
   will be returned.

   If ch is a tab, newline, or backspace, the cursor will be moved
   appropriately within the window.  If ch is a newline, the
   clrtoeol routine is called before the cursor is moved to the
   beginning of the next line.  If newline mapping is off, the
   cursor will be moved to the next line, but the x coordinate will
   be unchanged.  If ch is a tab the cursor is moved to the next
   tab position within the window.  If ch is another control
   character, it will be drawn in the ^X notation.  Calling the
   inch() routine after adding a control character returns the
   representation of the control character, not the control
   character.

   Video attributes can be combined with a character by ORing them
   into the parameter. Text, including attributes, can be copied
   from one place to another by using inch() and addch().

   Note that in PDCurses, for now, a cchar_t and a chtype are the
   same. The text field is 16 bits wide, and is treated as Unicode
   (UCS-2) when PDCurses is built with wide-character support
   (define PDC_WIDE). So, in functions that take a chtype, like
   addch(), both the wide and narrow versions will handle Unicode.
   But for portability, you should use the wide functions.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    addch                       Y       Y       Y
    waddch                      Y       Y       Y
    mvaddch                     Y       Y       Y
    mvwaddch                    Y       Y       Y
    echochar                    Y       -      3.0
    wechochar                   Y       -      3.0
    addrawch                    -       -       -
    waddrawch                   -       -       -
    mvaddrawch                  -       -       -
    mvwaddrawch                 -       -       -
    add_wch                     Y
    wadd_wch                    Y
    mvadd_wch                   Y
    mvwadd_wch                  Y
    echo_wchar                  Y
    wecho_wchar                 Y



--------------------------------------------------------------------------


addchstr
--------

### Synopsis

    int addchstr(const chtype *ch);
    int addchnstr(const chtype *ch, int n);
    int waddchstr(WINDOW *win, const chtype *ch);
    int waddchnstr(WINDOW *win, const chtype *ch, int n);
    int mvaddchstr(int y, int x, const chtype *ch);
    int mvaddchnstr(int y, int x, const chtype *ch, int n);
    int mvwaddchstr(WINDOW *, int y, int x, const chtype *ch);
    int mvwaddchnstr(WINDOW *, int y, int x, const chtype *ch, int n);

    int add_wchstr(const cchar_t *wch);
    int add_wchnstr(const cchar_t *wch, int n);
    int wadd_wchstr(WINDOW *win, const cchar_t *wch);
    int wadd_wchnstr(WINDOW *win, const cchar_t *wch, int n);
    int mvadd_wchstr(int y, int x, const cchar_t *wch);
    int mvadd_wchnstr(int y, int x, const cchar_t *wch, int n);
    int mvwadd_wchstr(WINDOW *win, int y, int x, const cchar_t *wch);
    int mvwadd_wchnstr(WINDOW *win, int y, int x, const cchar_t *wch,
                       int n);

### Description

   These routines write a chtype or cchar_t string directly into
   the window structure, starting at the current or specified
   position. The four routines with n as the last argument copy at
   most n elements, but no more than will fit on the line. If n =
   -1 then the whole string is copied, up to the maximum number
   that will fit on the line.

   The cursor position is not advanced. These routines do not check
   for newline or other special characters, nor does any line
   wrapping occur.

### Return Value

   All functions return OK or ERR.

### Portability
                             X/Open    BSD    SYS V
    addchstr                    Y       -      4.0
    waddchstr                   Y       -      4.0
    mvaddchstr                  Y       -      4.0
    mvwaddchstr                 Y       -      4.0
    addchnstr                   Y       -      4.0
    waddchnstr                  Y       -      4.0
    mvaddchnstr                 Y       -      4.0
    mvwaddchnstr                Y       -      4.0
    add_wchstr                  Y
    wadd_wchstr                 Y
    mvadd_wchstr                Y
    mvwadd_wchstr               Y
    add_wchnstr                 Y
    wadd_wchnstr                Y
    mvadd_wchnstr               Y
    mvwadd_wchnstr              Y



--------------------------------------------------------------------------


addstr
------

### Synopsis

    int addstr(const char *str);
    int addnstr(const char *str, int n);
    int waddstr(WINDOW *win, const char *str);
    int waddnstr(WINDOW *win, const char *str, int n);
    int mvaddstr(int y, int x, const char *str);
    int mvaddnstr(int y, int x, const char *str, int n);
    int mvwaddstr(WINDOW *win, int y, int x, const char *str);
    int mvwaddnstr(WINDOW *win, int y, int x, const char *str, int n);

    int addwstr(const wchar_t *wstr);
    int addnwstr(const wchar_t *wstr, int n);
    int waddwstr(WINDOW *win, const wchar_t *wstr);
    int waddnwstr(WINDOW *win, const wchar_t *wstr, int n);
    int mvaddwstr(int y, int x, const wchar_t *wstr);
    int mvaddnwstr(int y, int x, const wchar_t *wstr, int n);
    int mvwaddwstr(WINDOW *win, int y, int x, const wchar_t *wstr);
    int mvwaddnwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n);

### Description

   These routines write all the characters of the null-terminated
   string str or wide-character string wstr to the given window.
   The functionality is similar to calling waddch() once for each
   character in the string; except that, when PDCurses is built
   with wide-character support enabled, the narrow-character
   functions treat the string as a multibyte string in the current
   locale, and convert it. The routines with n as the last
   argument write at most n characters; if n is negative, then the
   entire string will be added.

### Return Value

   All functions return OK or ERR.

### Portability
                             X/Open    BSD    SYS V
    addstr                      Y       Y       Y
    waddstr                     Y       Y       Y
    mvaddstr                    Y       Y       Y
    mvwaddstr                   Y       Y       Y
    addnstr                     Y       -      4.0
    waddnstr                    Y       -      4.0
    mvaddnstr                   Y       -      4.0
    mvwaddnstr                  Y       -      4.0
    addwstr                     Y
    waddwstr                    Y
    mvaddwstr                   Y
    mvwaddwstr                  Y
    addnwstr                    Y
    waddnwstr                   Y
    mvaddnwstr                  Y
    mvwaddnwstr                 Y



--------------------------------------------------------------------------


attr
----

### Synopsis

    int attroff(chtype attrs);
    int wattroff(WINDOW *win, chtype attrs);
    int attron(chtype attrs);
    int wattron(WINDOW *win, chtype attrs);
    int attrset(chtype attrs);
    int wattrset(WINDOW *win, chtype attrs);
    int standend(void);
    int wstandend(WINDOW *win);
    int standout(void);
    int wstandout(WINDOW *win);

    int color_set(short color_pair, void *opts);
    int wcolor_set(WINDOW *win, short color_pair, void *opts);

    int attr_get(attr_t *attrs, short *color_pair, void *opts);
    int attr_off(attr_t attrs, void *opts);
    int attr_on(attr_t attrs, void *opts);
    int attr_set(attr_t attrs, short color_pair, void *opts);
    int wattr_get(WINDOW *win, attr_t *attrs, short *color_pair,
                  void *opts);
    int wattr_off(WINDOW *win, attr_t attrs, void *opts);
    int wattr_on(WINDOW *win, attr_t attrs, void *opts);
    int wattr_set(WINDOW *win, attr_t attrs, short color_pair,
                  void *opts);

    int chgat(int n, attr_t attr, short color, const void *opts);
    int mvchgat(int y, int x, int n, attr_t attr, short color,
                const void *opts);
    int mvwchgat(WINDOW *win, int y, int x, int n, attr_t attr,
                 short color, const void *opts);
    int wchgat(WINDOW *win, int n, attr_t attr, short color,
               const void *opts);

    chtype getattrs(WINDOW *win);

### Description

   These functions manipulate the current attributes and/or colors
   of the named window.  These attributes can be any combination
   of A_STANDOUT, A_REVERSE, A_BOLD, A_DIM, A_BLINK, A_UNDERLINE.

   These constants are defined in <curses.h> and can be combined
   with the bitwise-OR operator (|).

   The current attributes of a window are applied to all chtypes
   that are written into the window with waddch(). Attributes are
   a property of the chtype, and move with the character through
   any scrolling or insert/delete operations.

   attrset() sets the current attributes of the given window to
   attrs. attroff() turns off the named attributes without
   affecting any other attributes; attron() turns them on.
   color_set() sets the window color to the value of color_pair.

   standout() is the same as attron(A_STANDOUT). standend() is the
   same as attrset(A_NORMAL); that is, it turns off all attributes.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    attroff                     Y       Y       Y
    wattroff                    Y       Y       Y
    attron                      Y       Y       Y
    wattron                     Y       Y       Y
    attrset                     Y       Y       Y
    wattrset                    Y       Y       Y
    standend                    Y       Y       Y
    wstandend                   Y       Y       Y
    standout                    Y       Y       Y
    wstandout                   Y       Y       Y
    color_set                   Y
    wcolor_set                  Y
    attr_get                    Y
    wattr_get                   Y
    attr_on                     Y
    wattr_on                    Y
    attr_off                    Y
    wattr_off                   Y
    attr_set                    Y
    wattr_set                   Y
    chgat                       Y
    wchgat                      Y
    mvchgat                     Y
    mvwchgat                    Y
    getattrs                    -



--------------------------------------------------------------------------


beep
----

### Synopsis

    int beep(void);
    int flash(void);

### Description

   beep() sounds the audible bell on the terminal, if possible;
   if not, it calls flash().

   flash() "flashes" the screen, by inverting the foreground and
   background of every cell, pausing, and then restoring the
   original attributes.

### Return Value

   These functions return OK.

### Portability
                             X/Open    BSD    SYS V
    beep                        Y       Y       Y
    flash                       Y       Y       Y



--------------------------------------------------------------------------


bkgd
----

### Synopsis

    int bkgd(chtype ch);
    void bkgdset(chtype ch);
    chtype getbkgd(WINDOW *win);
    int wbkgd(WINDOW *win, chtype ch);
    void wbkgdset(WINDOW *win, chtype ch);

    int bkgrnd(const cchar_t *wch);
    void bkgrndset(const cchar_t *wch);
    int getbkgrnd(cchar_t *wch);
    int wbkgrnd(WINDOW *win, const cchar_t *wch);
    void wbkgrndset(WINDOW *win, const cchar_t *wch);
    int wgetbkgrnd(WINDOW *win, cchar_t *wch);

### Description

   bkgdset() and wbkgdset() manipulate the background of a window.
   The background is a chtype consisting of any combination of
   attributes and a character; it is combined with each chtype
   added or inserted to the window by waddch() or winsch(). Only
   the attribute part is used to set the background of non-blank
   characters, while both character and attributes are used for
   blank positions.

   bkgd() and wbkgd() not only change the background, but apply it
   immediately to every cell in the window.

   The attributes that are defined with the attrset()/attron() set
   of functions take precedence over the background attributes if
   there is a conflict (e.g., different color pairs).

### Return Value

   bkgd() and wbkgd() return OK, unless the window is NULL, in
   which case they return ERR.

### Portability
                             X/Open    BSD    SYS V
    bkgd                        Y       -      4.0
    bkgdset                     Y       -      4.0
    getbkgd                     Y
    wbkgd                       Y       -      4.0
    wbkgdset                    Y       -      4.0
    bkgrnd                      Y
    bkgrndset                   Y
    getbkgrnd                   Y
    wbkgrnd                     Y
    wbkgrndset                  Y
    wgetbkgrnd                  Y



--------------------------------------------------------------------------


border
------

### Synopsis

    int border(chtype ls, chtype rs, chtype ts, chtype bs, chtype tl,
               chtype tr, chtype bl, chtype br);
    int wborder(WINDOW *win, chtype ls, chtype rs, chtype ts,
                chtype bs, chtype tl, chtype tr, chtype bl, chtype br);
    int box(WINDOW *win, chtype verch, chtype horch);
    int hline(chtype ch, int n);
    int vline(chtype ch, int n);
    int whline(WINDOW *win, chtype ch, int n);
    int wvline(WINDOW *win, chtype ch, int n);
    int mvhline(int y, int x, chtype ch, int n);
    int mvvline(int y, int x, chtype ch, int n);
    int mvwhline(WINDOW *win, int y, int x, chtype ch, int n);
    int mvwvline(WINDOW *win, int y, int x, chtype ch, int n);

    int border_set(const cchar_t *ls, const cchar_t *rs,
                   const cchar_t *ts, const cchar_t *bs,
                   const cchar_t *tl, const cchar_t *tr,
                const cchar_t *bl, const cchar_t *br);
    int wborder_set(WINDOW *win, const cchar_t *ls, const cchar_t *rs,
                    const cchar_t *ts, const cchar_t *bs,
                    const cchar_t *tl, const cchar_t *tr,
                    const cchar_t *bl, const cchar_t *br);
    int box_set(WINDOW *win, const cchar_t *verch, const cchar_t *horch);
    int hline_set(const cchar_t *wch, int n);
    int vline_set(const cchar_t *wch, int n);
    int whline_set(WINDOW *win, const cchar_t *wch, int n);
    int wvline_set(WINDOW *win, const cchar_t *wch, int n);
    int mvhline_set(int y, int x, const cchar_t *wch, int n);
    int mvvline_set(int y, int x, const cchar_t *wch, int n);
    int mvwhline_set(WINDOW *win, int y, int x, const cchar_t *wch, int n);
    int mvwvline_set(WINDOW *win, int y, int x, const cchar_t *wch, int n);

### Description

   border(), wborder(), and box() draw a border around the edge of
   the window. If any argument is zero, an appropriate default is
   used:

   ls    left side of border             ACS_VLINE
   rs    right side of border            ACS_VLINE
   ts    top side of border              ACS_HLINE
   bs    bottom side of border           ACS_HLINE
   tl    top left corner of border       ACS_ULCORNER
   tr    top right corner of border      ACS_URCORNER
   bl    bottom left corner of border    ACS_LLCORNER
   br    bottom right corner of border   ACS_LRCORNER

   hline() and whline() draw a horizontal line, using ch, starting
   from the current cursor position. The cursor position does not
   change. The line is at most n characters long, or as many as
   will fit in the window.

   vline() and wvline() draw a vertical line, using ch, starting
   from the current cursor position. The cursor position does not
   change. The line is at most n characters long, or as many as
   will fit in the window.

### Return Value

   These functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    border                      Y       -      4.0
    wborder                     Y       -      4.0
    box                         Y       Y       Y
    hline                       Y       -      4.0
    vline                       Y       -      4.0
    whline                      Y       -      4.0
    wvline                      Y       -      4.0
    mvhline                     Y
    mvvline                     Y
    mvwhline                    Y
    mvwvline                    Y
    border_set                  Y
    wborder_set                 Y
    box_set                     Y
    hline_set                   Y
    vline_set                   Y
    whline_set                  Y
    wvline_set                  Y
    mvhline_set                 Y
    mvvline_set                 Y
    mvwhline_set                Y
    mvwvline_set                Y



--------------------------------------------------------------------------


clear
-----

### Synopsis

    int clear(void);
    int wclear(WINDOW *win);
    int erase(void);
    int werase(WINDOW *win);
    int clrtobot(void);
    int wclrtobot(WINDOW *win);
    int clrtoeol(void);
    int wclrtoeol(WINDOW *win);

### Description

   erase() and werase() copy blanks (i.e. the background chtype) to
   every cell of the window.

   clear() and wclear() are similar to erase() and werase(), but
   they also call clearok() to ensure that the the window is
   cleared on the next wrefresh().

   clrtobot() and wclrtobot() clear the window from the current
   cursor position to the end of the window.

   clrtoeol() and wclrtoeol() clear the window from the current
   cursor position to the end of the current line.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    clear                       Y       Y       Y
    wclear                      Y       Y       Y
    erase                       Y       Y       Y
    werase                      Y       Y       Y
    clrtobot                    Y       Y       Y
    wclrtobot                   Y       Y       Y
    clrtoeol                    Y       Y       Y
    wclrtoeol                   Y       Y       Y



--------------------------------------------------------------------------


color
-----

### Synopsis

    int start_color(void);
    int init_pair(short pair, short fg, short bg);
    int init_color(short color, short red, short green, short blue);
    bool has_colors(void);
    bool can_change_color(void);
    int color_content(short color, short *red, short *green, short *blue);
    int pair_content(short pair, short *fg, short *bg);

    int assume_default_colors(int f, int b);
    int use_default_colors(void);

    int PDC_set_line_color(short color);

### Description

   To use these routines, start_color() must be called, usually
   immediately after initscr(). Colors are always used in pairs,
   referred to as color-pairs. A color-pair consists of a
   foreground color and a background color. A color-pair is
   initialized via init_pair(). After initialization, COLOR_PAIR(n)
   can be used like any other video attribute.

   start_color() initializes eight basic colors (black, red, green,
   yellow, blue, magenta, cyan, and white), and two global
   variables; COLORS and COLOR_PAIRS (respectively defining the
   maximum number of colors and color-pairs the terminal is capable
   of displaying).

   init_pair() changes the definition of a color-pair. It takes
   three arguments: the number of the color-pair to be redefined,
   and the new values of the foreground and background colors. The
   pair number must be between 0 and COLOR_PAIRS - 1, inclusive.
   The foreground and background must be between 0 and COLORS - 1,
   inclusive. If the color pair was previously initialized, the
   screen is refreshed, and all occurrences of that color-pair are
   changed to the new definition.

   has_colors() indicates if the terminal supports, and can
   maniplulate color. It returns TRUE or FALSE.

   can_change_color() indicates if the terminal has the capability
   to change the definition of its colors.

   pair_content() is used to determine what the colors of a given
   color-pair consist of.

   assume_default_colors() and use_default_colors() emulate the
   ncurses extensions of the same names. assume_default_colors(f,
   b) is essentially the same as init_pair(0, f, b) (which isn't
   allowed); it redefines the default colors. use_default_colors()
   allows the use of -1 as a foreground or background color with
   init_pair(), and calls assume_default_colors(-1, -1); -1
   represents the foreground or background color that the terminal
   had at startup. If the environment variable PDC_ORIGINAL_COLORS
   is set at the time start_color() is called, that's equivalent to
   calling use_default_colors().

   PDC_set_line_color() is used to set the color, globally, for the
   color of the lines drawn for the attributes: A_UNDERLINE, A_LEFT
   and A_RIGHT. A value of -1 (the default) indicates that the
   current foreground color should be used.

   NOTE: COLOR_PAIR() and PAIR_NUMBER() are implemented as macros.

### Return Value

   All functions return OK on success and ERR on error, except for
   has_colors() and can_change_colors(), which return TRUE or FALSE.

### Portability
                             X/Open    BSD    SYS V
    start_color                 Y       -      3.2
    init_pair                   Y       -      3.2
    init_color                  Y       -      3.2
    has_colors                  Y       -      3.2
    can_change_color            Y       -      3.2
    color_content               Y       -      3.2
    pair_content                Y       -      3.2
    assume_default_colors       -       -       -
    use_default_colors          -       -       -
    PDC_set_line_color          -       -       -



--------------------------------------------------------------------------


debug
-----

### Synopsis

    void traceon(void);
    void traceoff(void);
    void PDC_debug(const char *, ...);

### Description

   traceon() and traceoff() toggle the recording of debugging
   information to the file "trace". Although not standard, similar
   functions are in some other curses implementations.

   PDC_debug() is the function that writes to the file, based on
   whether traceon() has been called. It's used from the PDC_LOG()
   macro.

   The environment variable PDC_TRACE_FLUSH controls whether the
   trace file contents are fflushed after each write.  The default
   is not. Set it to enable this (may affect performance).

### Portability
                             X/Open    BSD    SYS V
    traceon                     -       -       -
    traceoff                    -       -       -
    PDC_debug                   -       -       -



--------------------------------------------------------------------------


delch
-----

### Synopsis

    int delch(void);
    int wdelch(WINDOW *win);
    int mvdelch(int y, int x);
    int mvwdelch(WINDOW *win, int y, int x);

### Description

   The character under the cursor in the window is deleted.  All
   characters to the right on the same line are moved to the left
   one position and the last character on the line is filled with
   a blank.  The cursor position does not change (after moving to
   y, x if coordinates are specified).

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    delch                       Y       Y       Y
    wdelch                      Y       Y       Y
    mvdelch                     Y       Y       Y
    mvwdelch                    Y       Y       Y



--------------------------------------------------------------------------


deleteln
--------

### Synopsis

    int deleteln(void);
    int wdeleteln(WINDOW *win);
    int insdelln(int n);
    int winsdelln(WINDOW *win, int n);
    int insertln(void);
    int winsertln(WINDOW *win);

    int mvdeleteln(int y, int x);
    int mvwdeleteln(WINDOW *win, int y, int x);
    int mvinsertln(int y, int x);
    int mvwinsertln(WINDOW *win, int y, int x);

### Description

   With the deleteln() and wdeleteln() functions, the line under
   the cursor in the window is deleted.  All lines below the
   current line are moved up one line.  The bottom line of the
   window is cleared.  The cursor position does not change.

   With the insertln() and winsertn() functions, a blank line is
   inserted above the current line and the bottom line is lost.

   mvdeleteln(), mvwdeleteln(), mvinsertln() and mvwinsertln()
   allow moving the cursor and inserting/deleting in one call.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    deleteln                    Y       Y       Y
    wdeleteln                   Y       Y       Y
    mvdeleteln                  -       -       -
    mvwdeleteln                 -       -       -
    insdelln                    Y       -      4.0
    winsdelln                   Y       -      4.0
    insertln                    Y       Y       Y
    winsertln                   Y       Y       Y
    mvinsertln                  -       -       -
    mvwinsertln                 -       -       -



--------------------------------------------------------------------------


getch
-----

### Synopsis

    int getch(void);
    int wgetch(WINDOW *win);
    int mvgetch(int y, int x);
    int mvwgetch(WINDOW *win, int y, int x);
    int ungetch(int ch);
    int flushinp(void);

    int get_wch(wint_t *wch);
    int wget_wch(WINDOW *win, wint_t *wch);
    int mvget_wch(int y, int x, wint_t *wch);
    int mvwget_wch(WINDOW *win, int y, int x, wint_t *wch);
    int unget_wch(const wchar_t wch);

    unsigned long PDC_get_key_modifiers(void);
    int PDC_save_key_modifiers(bool flag);
    int PDC_return_key_modifiers(bool flag);

### Description

   With the getch(), wgetch(), mvgetch(), and mvwgetch() functions,
   a character is read from the terminal associated with the window.
   In nodelay mode, if there is no input waiting, the value ERR is
   returned. In delay mode, the program will hang until the system
   passes text through to the program. Depending on the setting of
   cbreak(), this will be after one character or after the first
   newline.  Unless noecho() has been set, the character will also
   be echoed into the designated window.

   If keypad() is TRUE, and a function key is pressed, the token for
   that function key will be returned instead of the raw characters.
   Possible function keys are defined in <curses.h> with integers
   beginning with 0401, whose names begin with KEY_.

   If nodelay(win, TRUE) has been called on the window and no input
   is waiting, the value ERR is returned.

   ungetch() places ch back onto the input queue to be returned by
   the next call to wgetch().

   flushinp() throws away any type-ahead that has been typed by the
   user and has not yet been read by the program.

   PDC_get_key_modifiers() returns the keyboard modifiers (shift,
   control, alt, numlock) effective at the time of the last getch()
   call, if PDC_save_key_modifiers(TRUE) has been called before the
   getch(). Use the macros PDC_KEY_MODIFIER_* to determine which
   modifier(s) were set. PDC_return_key_modifiers() tells getch()
   to return modifier keys pressed alone as keystrokes (KEY_ALT_L,
   etc.). These may not work on all platforms.

   NOTE: getch() and ungetch() are implemented as macros, to avoid
   conflict with many DOS compiler's runtime libraries.

### Return Value

   These functions return ERR or the value of the character, meta
   character or function key token.

### Portability
                             X/Open    BSD    SYS V
    getch                       Y       Y       Y
    wgetch                      Y       Y       Y
    mvgetch                     Y       Y       Y
    mvwgetch                    Y       Y       Y
    ungetch                     Y       Y       Y
    flushinp                    Y       Y       Y
    get_wch                     Y
    wget_wch                    Y
    mvget_wch                   Y
    mvwget_wch                  Y
    unget_wch                   Y
    PDC_get_key_modifiers       -       -       -



--------------------------------------------------------------------------


getstr
------

### Synopsis

    int getstr(char *str);
    int wgetstr(WINDOW *win, char *str);
    int mvgetstr(int y, int x, char *str);
    int mvwgetstr(WINDOW *win, int y, int x, char *str);
    int getnstr(char *str, int n);
    int wgetnstr(WINDOW *win, char *str, int n);
    int mvgetnstr(int y, int x, char *str, int n);
    int mvwgetnstr(WINDOW *win, int y, int x, char *str, int n);

    int get_wstr(wint_t *wstr);
    int wget_wstr(WINDOW *win, wint_t *wstr);
    int mvget_wstr(int y, int x, wint_t *wstr);
    int mvwget_wstr(WINDOW *win, int, int, wint_t *wstr);
    int getn_wstr(wint_t *wstr, int n);
    int wgetn_wstr(WINDOW *win, wint_t *wstr, int n);
    int mvgetn_wstr(int y, int x, wint_t *wstr, int n);
    int mvwgetn_wstr(WINDOW *win, int y, int x, wint_t *wstr, int n);

### Description

   These routines call wgetch() repeatedly to build a string,
   interpreting erase and kill characters along the way, until a
   newline or carriage return is received. When PDCurses is built
   with wide-character support enabled, the narrow-character
   functions convert the wgetch()'d values into a multibyte string
   in the current locale before returning it. The resulting string
   is placed in the area pointed to by *str. The routines with n as
   the last argument read at most n characters.

   Note that there's no way to know how long the buffer passed to
   wgetstr() is, so use wgetnstr() to avoid buffer overflows.

### Return Value

   These functions return ERR on failure or any other value on
   success.

### Portability
                             X/Open    BSD    SYS V
    getstr                      Y       Y       Y
    wgetstr                     Y       Y       Y
    mvgetstr                    Y       Y       Y
    mvwgetstr                   Y       Y       Y
    getnstr                     Y       -      4.0
    wgetnstr                    Y       -      4.0
    mvgetnstr                   Y       -       -
    mvwgetnstr                  Y       -       -
    get_wstr                    Y
    wget_wstr                   Y
    mvget_wstr                  Y
    mvwget_wstr                 Y
    getn_wstr                   Y
    wgetn_wstr                  Y
    mvgetn_wstr                 Y
    mvwgetn_wstr                Y



--------------------------------------------------------------------------


getyx
-----

### Synopsis

    void getyx(WINDOW *win, int y, int x);
    void getparyx(WINDOW *win, int y, int x);
    void getbegyx(WINDOW *win, int y, int x);
    void getmaxyx(WINDOW *win, int y, int x);

    void getsyx(int y, int x);
    void setsyx(int y, int x);

    int getbegy(WINDOW *win);
    int getbegx(WINDOW *win);
    int getcury(WINDOW *win);
    int getcurx(WINDOW *win);
    int getpary(WINDOW *win);
    int getparx(WINDOW *win);
    int getmaxy(WINDOW *win);
    int getmaxx(WINDOW *win);

### Description

   The getyx() macro (defined in curses.h -- the prototypes here
   are merely illustrative) puts the current cursor position of the
   specified window into y and x. getbegyx() and getmaxyx() return
   the starting coordinates and size of the specified window,
   respectively. getparyx() returns the starting coordinates of the
   parent's window, if the specified window is a subwindow;
   otherwise it sets y and x to -1. These are all macros.

   getsyx() gets the coordinates of the virtual screen cursor, and
   stores them in y and x. If leaveok() is TRUE, it returns -1, -1.
   If lines have been removed with ripoffline(), then getsyx()
   includes these lines in its count; so, the returned y and x
   values should only be used with setsyx().

   setsyx() sets the virtual screen cursor to the y, x coordinates.
   If either y or x is -1, leaveok() is set TRUE, else it's set FALSE.

   getsyx() and setsyx() are meant to be used by a library routine
   that manipulates curses windows without altering the position of
   the cursor. Note that getsyx() is defined only as a macro.

   getbegy(), getbegx(), getcurx(), getcury(), getmaxy(),
   getmaxx(), getpary(), and getparx() return the appropriate
   coordinate or size values, or ERR in the case of a NULL window.

### Portability
                             X/Open    BSD    SYS V
    getyx                       Y       Y       Y
    getparyx                    -       -      4.0
    getbegyx                    -       -      3.0
    getmaxyx                    -       -      3.0
    getsyx                      -       -      3.0
    setsyx                      -       -      3.0
    getbegy                     -       -       -
    getbegx                     -       -       -
    getcury                     -       -       -
    getcurx                     -       -       -
    getpary                     -       -       -
    getparx                     -       -       -
    getmaxy                     -       -       -
    getmaxx                     -       -       -



--------------------------------------------------------------------------


inch
----

### Synopsis

    chtype inch(void);
    chtype winch(WINDOW *win);
    chtype mvinch(int y, int x);
    chtype mvwinch(WINDOW *win, int y, int x);

    int in_wch(cchar_t *wcval);
    int win_wch(WINDOW *win, cchar_t *wcval);
    int mvin_wch(int y, int x, cchar_t *wcval);
    int mvwin_wch(WINDOW *win, int y, int x, cchar_t *wcval);

### Description

   The inch() functions retrieve the character and attribute from
   the current or specified window position, in the form of a
   chtype. If a NULL window is specified, (chtype)ERR is returned.

   The in_wch() functions are the wide-character versions; instead
   of returning a chtype, they store a cchar_t at the address
   specified by wcval, and return OK or ERR. (No value is stored
   when ERR is returned.) Note that in PDCurses, chtype and cchar_t
   are the same.

### Portability
                             X/Open    BSD    SYS V
    inch                        Y       Y       Y
    winch                       Y       Y       Y
    mvinch                      Y       Y       Y
    mvwinch                     Y       Y       Y
    in_wch                      Y
    win_wch                     Y
    mvin_wch                    Y
    mvwin_wch                   Y



--------------------------------------------------------------------------


inchstr
-------

### Synopsis

    int inchstr(chtype *ch);
    int inchnstr(chtype *ch, int n);
    int winchstr(WINDOW *win, chtype *ch);
    int winchnstr(WINDOW *win, chtype *ch, int n);
    int mvinchstr(int y, int x, chtype *ch);
    int mvinchnstr(int y, int x, chtype *ch, int n);
    int mvwinchstr(WINDOW *, int y, int x, chtype *ch);
    int mvwinchnstr(WINDOW *, int y, int x, chtype *ch, int n);

    int in_wchstr(cchar_t *wch);
    int in_wchnstr(cchar_t *wch, int n);
    int win_wchstr(WINDOW *win, cchar_t *wch);
    int win_wchnstr(WINDOW *win, cchar_t *wch, int n);
    int mvin_wchstr(int y, int x, cchar_t *wch);
    int mvin_wchnstr(int y, int x, cchar_t *wch, int n);
    int mvwin_wchstr(WINDOW *win, int y, int x, cchar_t *wch);
    int mvwin_wchnstr(WINDOW *win, int y, int x, cchar_t *wch, int n);

### Description

   These routines read a chtype or cchar_t string from the window,
   starting at the current or specified position, and ending at the
   right margin, or after n elements, whichever is less.

### Return Value

   All functions return the number of elements read, or ERR on
   error.

### Portability
                             X/Open    BSD    SYS V
    inchstr                     Y       -      4.0
    winchstr                    Y       -      4.0
    mvinchstr                   Y       -      4.0
    mvwinchstr                  Y       -      4.0
    inchnstr                    Y       -      4.0
    winchnstr                   Y       -      4.0
    mvinchnstr                  Y       -      4.0
    mvwinchnstr                 Y       -      4.0
    in_wchstr                   Y
    win_wchstr                  Y
    mvin_wchstr                 Y
    mvwin_wchstr                Y
    in_wchnstr                  Y
    win_wchnstr                 Y
    mvin_wchnstr                Y
    mvwin_wchnstr               Y



--------------------------------------------------------------------------


initscr
-------

### Synopsis

    WINDOW *initscr(void);
    WINDOW *Xinitscr(int argc, char *argv[]);
    int endwin(void);
    bool isendwin(void);
    SCREEN *newterm(const char *type, FILE *outfd, FILE *infd);
    SCREEN *set_term(SCREEN *new);
    void delscreen(SCREEN *sp);

    int resize_term(int nlines, int ncols);
    bool is_termresized(void);
    const char *curses_version(void);

### Description

   initscr() should be the first curses routine called.  It will
   initialize all curses data structures, and arrange that the
   first call to refresh() will clear the screen.  In case of
   error, initscr() will write a message to standard error and end
   the program.

   endwin() should be called before exiting or escaping from curses
   mode temporarily.  It will restore tty modes, move the cursor to
   the lower left corner of the screen and reset the terminal into
   the proper non-visual mode.  To resume curses after a temporary
   escape, call refresh() or doupdate().

   isendwin() returns TRUE if endwin() has been called without a
   subsequent refresh, unless SP is NULL.

   In some implementations of curses, newterm() allows the use of
   multiple terminals. Here, it's just an alternative interface for
   initscr(). It always returns SP, or NULL.

   delscreen() frees the memory allocated by newterm() or
   initscr(), since it's not freed by endwin(). This function is
   usually not needed. In PDCurses, the parameter must be the
   value of SP, and delscreen() sets SP to NULL.

   set_term() does nothing meaningful in PDCurses, but is included
   for compatibility with other curses implementations.

   resize_term() is effectively two functions: When called with
   nonzero values for nlines and ncols, it attempts to resize the
   screen to the given size. When called with (0, 0), it merely
   adjusts the internal structures to match the current size after
   the screen is resized by the user. On the currently supported
   platforms, SDL, Windows console, and X11 allow user resizing, while
   DOS, OS/2, SDL and Windows console allow programmatic resizing. If
   you want to support user resizing, you should check for getch()
   returning KEY_RESIZE, and/or call is_termresized() at appropriate
   times; if either condition occurs, call resize_term(0, 0). Then, with
   either user or programmatic resizing, you'll have to resize any
   windows you've created, as appropriate; resize_term() only
   handles stdscr and curscr.

   is_termresized() returns TRUE if the curses screen has been
   resized by the user, and a call to resize_term() is needed.
   Checking for KEY_RESIZE is generally preferable, unless you're
   not handling the keyboard.

   curses_version() returns a string describing the version of
   PDCurses.

### Return Value

   All functions return NULL on error, except endwin(), which always
   returns OK, and resize_term(), which returns either OK or ERR.

### Portability
                             X/Open    BSD    SYS V
    initscr                     Y       Y       Y
    endwin                      Y       Y       Y
    isendwin                    Y       -      3.0
    newterm                     Y       -       Y
    set_term                    Y       -       Y
    delscreen                   Y       -      4.0
    resize_term                 -       -       -
    is_termresized              -       -       -
    curses_version              -       -       -



--------------------------------------------------------------------------


inopts
------

### Synopsis

    int cbreak(void);
    int nocbreak(void);
    int echo(void);
    int noecho(void);
    int halfdelay(int tenths);
    int intrflush(WINDOW *win, bool bf);
    int keypad(WINDOW *win, bool bf);
    int meta(WINDOW *win, bool bf);
    int nl(void);
    int nonl(void);
    int nodelay(WINDOW *win, bool bf);
    int notimeout(WINDOW *win, bool bf);
    int raw(void);
    int noraw(void);
    void noqiflush(void);
    void qiflush(void);
    void timeout(int delay);
    void wtimeout(WINDOW *win, int delay);
    int typeahead(int fildes);

    int crmode(void);
    int nocrmode(void);

### Description

   cbreak() and nocbreak() toggle cbreak mode. In cbreak mode,
   characters typed by the user are made available immediately, and
   erase/kill character processing is not performed.  In nocbreak
   mode, typed characters are buffered until a newline or carriage
   return. Interrupt and flow control characters are unaffected by
   this mode. PDCurses always starts in cbreak mode.

   echo() and noecho() control whether typed characters are echoed
   by the input routine.  Initially, input characters are echoed.
   Subsequent calls to echo() and noecho() do not flush type-ahead.

   halfdelay() is similar to cbreak(), but allows for a time limit
   to be specified, in tenths of a second. This causes getch() to
   block for that period before returning ERR if no key has been
   received.  tenths must be between 1 and 255.

   keypad() controls whether getch() returns function/special keys
   as single key codes (e.g., the left arrow key as KEY_LEFT). Per
   X/Open, the default for keypad mode is OFF. You'll probably want
   it on. With keypad mode off, if a special key is pressed,
   getch() does nothing or returns ERR.

   nodelay() controls whether wgetch() is a non-blocking call. If
   the option is enabled, and no input is ready, wgetch() will
   return ERR. If disabled, wgetch() will hang until input is
   ready.

   nl() enables the translation of a carriage return into a newline
   on input. nonl() disables this. Initially, the translation does
   occur.

   raw() and noraw() toggle raw mode. Raw mode is similar to cbreak
   mode, in that characters typed are immediately passed through to
   the user program.  The difference is that in raw mode, the INTR,
   QUIT, SUSP, and STOP characters are passed through without being
   interpreted, and without generating a signal.

   In PDCurses, the meta() function sets raw mode on or off.

   timeout() and wtimeout() set blocking or non-blocking reads for
   the specified window. The delay is measured in milliseconds. If
   it's negative, a blocking read is used; if zero, then non-
   blocking reads are done -- if no input is waiting, ERR is
   returned immediately. If the delay is positive, the read blocks
   for the delay period; if the period expires, ERR is returned.

   intrflush(), notimeout(), noqiflush(), qiflush() and typeahead()
   do nothing in PDCurses, but are included for compatibility with
   other curses implementations.

   crmode() and nocrmode() are archaic equivalents to cbreak() and
   nocbreak(), respectively.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    cbreak                      Y       Y       Y
    nocbreak                    Y       Y       Y
    echo                        Y       Y       Y
    noecho                      Y       Y       Y
    halfdelay                   Y       -       Y
    intrflush                   Y       -       Y
    keypad                      Y       -       Y
    meta                        Y       -       Y
    nl                          Y       Y       Y
    nonl                        Y       Y       Y
    nodelay                     Y       -       Y
    notimeout                   Y       -       Y
    raw                         Y       Y       Y
    noraw                       Y       Y       Y
    noqiflush                   Y       -       Y
    qiflush                     Y       -       Y
    timeout                     Y       -       Y
    wtimeout                    Y       -       Y
    typeahead                   Y       -       Y
    crmode                      -
    nocrmode                    -



--------------------------------------------------------------------------


insch
-----

### Synopsis

    int insch(chtype ch);
    int winsch(WINDOW *win, chtype ch);
    int mvinsch(int y, int x, chtype ch);
    int mvwinsch(WINDOW *win, int y, int x, chtype ch);

    int insrawch(chtype ch);
    int winsrawch(WINDOW *win, chtype ch);
    int mvinsrawch(int y, int x, chtype ch);
    int mvwinsrawch(WINDOW *win, int y, int x, chtype ch);

    int ins_wch(const cchar_t *wch);
    int wins_wch(WINDOW *win, const cchar_t *wch);
    int mvins_wch(int y, int x, const cchar_t *wch);
    int mvwins_wch(WINDOW *win, int y, int x, const cchar_t *wch);

### Description

   The insch() functions insert a chtype into the window at the
   current or specified cursor position. The cursor is NOT
   advanced. A newline is equivalent to clrtoeol(); tabs are
   expanded; other control characters are converted as with
   unctrl().

   The ins_wch() functions are the wide-character
   equivalents, taking cchar_t pointers rather than chtypes.

   Video attributes can be combined with a character by ORing
   them into the parameter. Text, including attributes, can be
   copied from one place to another using inch() and insch().

   insrawch() etc. are PDCurses-specific wrappers for insch() etc.
   that disable the translation of control characters.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    insch                       Y       Y       Y
    winsch                      Y       Y       Y
    mvinsch                     Y       Y       Y
    mvwinsch                    Y       Y       Y
    insrawch                    -       -       -
    winsrawch                   -       -       -
    ins_wch                     Y
    wins_wch                    Y
    mvins_wch                   Y
    mvwins_wch                  Y



--------------------------------------------------------------------------


insstr
------

### Synopsis

    int insstr(const char *str);
    int insnstr(const char *str, int n);
    int winsstr(WINDOW *win, const char *str);
    int winsnstr(WINDOW *win, const char *str, int n);
    int mvinsstr(int y, int x, const char *str);
    int mvinsnstr(int y, int x, const char *str, int n);
    int mvwinsstr(WINDOW *win, int y, int x, const char *str);
    int mvwinsnstr(WINDOW *win, int y, int x, const char *str, int n);

    int ins_wstr(const wchar_t *wstr);
    int ins_nwstr(const wchar_t *wstr, int n);
    int wins_wstr(WINDOW *win, const wchar_t *wstr);
    int wins_nwstr(WINDOW *win, const wchar_t *wstr, int n);
    int mvins_wstr(int y, int x, const wchar_t *wstr);
    int mvins_nwstr(int y, int x, const wchar_t *wstr, int n);
    int mvwins_wstr(WINDOW *win, int y, int x, const wchar_t *wstr);
    int mvwins_nwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n);

### Description

   The insstr() functions insert a character string into a window
   at the current cursor position, by repeatedly calling winsch().
   When PDCurses is built with wide-character support enabled, the
   narrow-character functions treat the string as a multibyte
   string in the current locale, and convert it first. All
   characters to the right of the cursor are moved to the right,
   with the possibility of the rightmost characters on the line
   being lost.  The cursor position does not change (after moving
   to y, x, if specified).  The routines with n as the last
   argument insert at most n characters; if n is negative, then the
   entire string is inserted.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    insstr                      Y       -      4.0
    winsstr                     Y       -      4.0
    mvinsstr                    Y       -      4.0
    mvwinsstr                   Y       -      4.0
    insnstr                     Y       -      4.0
    winsnstr                    Y       -      4.0
    mvinsnstr                   Y       -      4.0
    mvwinsnstr                  Y       -      4.0
    ins_wstr                    Y
    wins_wstr                   Y
    mvins_wstr                  Y
    mvwins_wstr                 Y
    ins_nwstr                   Y
    wins_nwstr                  Y
    mvins_nwstr                 Y
    mvwins_nwstr                Y



--------------------------------------------------------------------------


instr
-----

### Synopsis

    int instr(char *str);
    int innstr(char *str, int n);
    int winstr(WINDOW *win, char *str);
    int winnstr(WINDOW *win, char *str, int n);
    int mvinstr(int y, int x, char *str);
    int mvinnstr(int y, int x, char *str, int n);
    int mvwinstr(WINDOW *win, int y, int x, char *str);
    int mvwinnstr(WINDOW *win, int y, int x, char *str, int n);

    int inwstr(wchar_t *wstr);
    int innwstr(wchar_t *wstr, int n);
    int winwstr(WINDOW *win, wchar_t *wstr);
    int winnwstr(WINDOW *win, wchar_t *wstr, int n);
    int mvinwstr(int y, int x, wchar_t *wstr);
    int mvinnwstr(int y, int x, wchar_t *wstr, int n);
    int mvwinwstr(WINDOW *win, int y, int x, wchar_t *wstr);
    int mvwinnwstr(WINDOW *win, int y, int x, wchar_t *wstr, int n);

### Description

   These functions take characters (or wide characters) from the
   current or specified position in the window, and return them as
   a string in str (or wstr). Attributes are ignored. The functions
   with n as the last argument return a string at most n characters
   long.

### Return Value

   Upon successful completion, innstr(), mvinnstr(), mvwinnstr()
   and winnstr() return the number of characters actually read into
   the string; instr(), mvinstr(), mvwinstr() and winstr() return
   OK. Otherwise, all these functions return ERR.

### Portability
                             X/Open    BSD    SYS V
    instr                       Y       -      4.0
    winstr                      Y       -      4.0
    mvinstr                     Y       -      4.0
    mvwinstr                    Y       -      4.0
    innstr                      Y       -      4.0
    winnstr                     Y       -      4.0
    mvinnstr                    Y       -      4.0
    mvwinnstr                   Y       -      4.0
    inwstr                      Y
    winwstr                     Y
    mvinwstr                    Y
    mvwinwstr                   Y
    innwstr                     Y
    winnwstr                    Y
    mvinnwstr                   Y
    mvwinnwstr                  Y



--------------------------------------------------------------------------


kernel
------

### Synopsis

    int def_prog_mode(void);
    int def_shell_mode(void);
    int reset_prog_mode(void);
    int reset_shell_mode(void);
    int resetty(void);
    int savetty(void);
    int ripoffline(int line, int (*init)(WINDOW *, int));
    int curs_set(int visibility);
    int napms(int ms);

    int draino(int ms);
    int resetterm(void);
    int fixterm(void);
    int saveterm(void);

### Description

   def_prog_mode() and def_shell_mode() save the current terminal
   modes as the "program" (in curses) or "shell" (not in curses)
   state for use by the reset_prog_mode() and reset_shell_mode()
   functions.  This is done automatically by initscr().

   reset_prog_mode() and reset_shell_mode() restore the terminal to
   "program" (in curses) or "shell" (not in curses) state.  These
   are done automatically by endwin() and doupdate() after an
   endwin(), so they would normally not be called before these
   functions.

   savetty() and resetty() save and restore the state of the
   terminal modes. savetty() saves the current state in a buffer,
   and resetty() restores the state to what it was at the last call
   to savetty().

   curs_set() alters the appearance of the cursor. A visibility of
   0 makes it disappear; 1 makes it appear "normal" (usually an
   underline) and 2 makes it "highly visible" (usually a block).

   ripoffline() reduces the size of stdscr by one line.  If the
   "line" parameter is positive, the line is removed from the top
   of the screen; if negative, from the bottom. Up to 5 lines can
   be ripped off stdscr by calling ripoffline() repeatedly. The
   function argument, init, is called from within initscr() or
   newterm(), so ripoffline() must be called before either of these
   functions.  The init function receives a pointer to a one-line
   WINDOW, and the width of the window. Calling ripoffline() with a
   NULL init function pointer is an error.

   napms() suspends the program for the specified number of
   milliseconds. draino() is an archaic equivalent.

   resetterm(), fixterm() and saveterm() are archaic equivalents
   for reset_shell_mode(), reset_prog_mode() and def_prog_mode(),
   respectively.

### Return Value

   All functions return OK on success and ERR on error, except
   curs_set(), which returns the previous visibility.

### Portability
                             X/Open    BSD    SYS V
    def_prog_mode               Y       Y       Y
    def_shell_mode              Y       Y       Y
    reset_prog_mode             Y       Y       Y
    reset_shell_mode            Y       Y       Y
    resetty                     Y       Y       Y
    savetty                     Y       Y       Y
    ripoffline                  Y       -      3.0
    curs_set                    Y       -      3.0
    napms                       Y       Y       Y
    draino                      -
    resetterm                   -
    fixterm                     -
    saveterm                    -



--------------------------------------------------------------------------


keyname
-------

### Synopsis

    char *keyname(int key);

    char *key_name(wchar_t c);

    bool has_key(int key);

### Description

   keyname() returns a string corresponding to the argument key.
   key may be any key returned by wgetch().

   key_name() is the wide-character version. It takes a wchar_t
   parameter, but still returns a char *.

   has_key() returns TRUE for recognized keys, FALSE otherwise.
   This function is an ncurses extension.

### Portability
                             X/Open    BSD    SYS V
    keyname                     Y       -      3.0
    key_name                    Y
    has_key                     -       -       -



--------------------------------------------------------------------------


mouse
-----

### Synopsis

    int mouse_set(unsigned long mbe);
    int mouse_on(unsigned long mbe);
    int mouse_off(unsigned long mbe);
    int request_mouse_pos(void);
    int map_button(unsigned long button);
    void wmouse_position(WINDOW *win, int *y, int *x);
    unsigned long getmouse(void);
    unsigned long getbmap(void);

    int mouseinterval(int wait);
    bool wenclose(const WINDOW *win, int y, int x);
    bool wmouse_trafo(const WINDOW *win, int *y, int *x, bool to_screen);
    bool mouse_trafo(int *y, int *x, bool to_screen);
    mmask_t mousemask(mmask_t mask, mmask_t *oldmask);
    int nc_getmouse(MEVENT *event);
    int ungetmouse(MEVENT *event);

### Description

   As of PDCurses 3.0, there are two separate mouse interfaces: the
   classic interface, which is based on the undocumented Sys V
   mouse functions; and an ncurses-compatible interface. Both are
   active at all times, and you can mix and match functions from
   each, though it's not recommended. The ncurses interface is
   essentially an emulation layer built on top of the classic
   interface; it's here to allow easier porting of ncurses apps.

   The classic interface: mouse_set(), mouse_on(), mouse_off(),
   request_mouse_pos(), map_button(), wmouse_position(),
   getmouse(), and getbmap(). An application using this interface
   would start by calling mouse_set() or mouse_on() with a non-zero
   value, often ALL_MOUSE_EVENTS. Then it would check for a
   KEY_MOUSE return from getch(). If found, it would call
   request_mouse_pos() to get the current mouse status.

   mouse_set(), mouse_on() and mouse_off() are analagous to
   attrset(), attron() and attroff().  These functions set the
   mouse button events to trap.  The button masks used in these
   functions are defined in curses.h and can be or'ed together.
   They are the group of masks starting with BUTTON1_RELEASED.

   request_mouse_pos() requests curses to fill in the Mouse_status
   structure with the current state of the mouse.

   map_button() enables the specified mouse action to activate the
   Soft Label Keys if the action occurs over the area of the screen
   where the Soft Label Keys are displayed.  The mouse actions are
   defined in curses.h in the group that starts with BUTTON_RELEASED.

   wmouse_position() determines if the current mouse position is
   within the window passed as an argument.  If the mouse is
   outside the current window, -1 is returned in the y and x
   arguments; otherwise the y and x coordinates of the mouse
   (relative to the top left corner of the window) are returned in
   y and x.

   getmouse() returns the current status of the trapped mouse
   buttons as set by mouse_set() or mouse_on().

   getbmap() returns the current status of the button action used
   to map a mouse action to the Soft Label Keys as set by the
   map_button() function.

   The ncurses interface: mouseinterval(), wenclose(),
   wmouse_trafo(), mouse_trafo(), mousemask(), nc_getmouse(), and
   ungetmouse(). A typical application using this interface would
   start by calling mousemask() with a non-zero value, often
   ALL_MOUSE_EVENTS. Then it would check for a KEY_MOUSE return
   from getch(). If found, it would call nc_getmouse() to get the
   current mouse status.

   mouseinterval() sets the timeout for a mouse click. On all
   current platforms, PDCurses receives mouse button press and
   release events, but must synthesize click events. It does this
   by checking whether a release event is queued up after a press
   event. If it gets a press event, and there are no more events
   waiting, it will wait for the timeout interval, then check again
   for a release. A press followed by a release is reported as
   BUTTON_CLICKED; otherwise it's passed through as BUTTON_PRESSED.
   The default timeout is 150ms; valid values are 0 (no clicks
   reported) through 1000ms. In x11, the timeout can also be set
   via the clickPeriod resource. The return value from
   mouseinterval() is the old timeout. To check the old value
   without setting a new one, call it with a parameter of -1. Note
   that although there's no classic equivalent for this function
   (apart from the clickPeriod resource), the value set applies in
   both interfaces.

   wenclose() reports whether the given screen-relative y, x
   coordinates fall within the given window.

   wmouse_trafo() converts between screen-relative and window-
   relative coordinates. A to_screen parameter of TRUE means to
   convert from window to screen; otherwise the reverse. The
   function returns FALSE if the coordinates aren't within the
   window, or if any of the parameters are NULL. The coordinates
   have been converted when the function returns TRUE.

   mouse_trafo() is the stdscr version of wmouse_trafo().

   mousemask() is nearly equivalent to mouse_set(), but instead of
   OK/ERR, it returns the value of the mask after setting it. (This
   isn't necessarily the same value passed in, since the mask could
   be altered on some platforms.) And if the second parameter is a
   non-null pointer, mousemask() stores the previous mask value
   there. Also, since the ncurses interface doesn't work with
   PDCurses' BUTTON_MOVED events, mousemask() filters them out.

   nc_getmouse() returns the current mouse status in an MEVENT
   struct. This is equivalent to ncurses' getmouse(), renamed to
   avoid conflict with PDCurses' getmouse(). But if you define
   NCURSES_MOUSE_VERSION (preferably as 2) before including
   curses.h, it defines getmouse() to nc_getmouse(), along with a
   few other redefintions needed for compatibility with ncurses
   code. nc_getmouse() calls request_mouse_pos(), which (not
   getmouse()) is the classic equivalent.

   ungetmouse() is the mouse equivalent of ungetch(). However,
   PDCurses doesn't maintain a queue of mouse events; only one can
   be pushed back, and it can overwrite or be overwritten by real
   mouse events.

### Portability
                             X/Open    BSD    SYS V
    mouse_set                   -       -      4.0
    mouse_on                    -       -      4.0
    mouse_off                   -       -      4.0
    request_mouse_pos           -       -      4.0
    map_button                  -       -      4.0
    wmouse_position             -       -      4.0
    getmouse                    -       -      4.0
    getbmap                     -       -      4.0
    mouseinterval               -       -       -
    wenclose                    -       -       -
    wmouse_trafo                -       -       -
    mouse_trafo                 -       -       -
    mousemask                   -       -       -
    nc_getmouse                 -       -       -
    ungetmouse                  -       -       -



--------------------------------------------------------------------------


move
----

### Synopsis

    int move(int y, int x);
    int mvcur(int oldrow, int oldcol, int newrow, int newcol);
    int wmove(WINDOW *win, int y, int x);

### Description

   move() and wmove() move the cursor associated with the window to
   the given location.  This does not move the physical cursor of the
   terminal until refresh() is called.  The position specified is
   relative to the upper left corner of the window, which is (0,0).

   mvcur() moves the physical cursor without updating any window
   cursor positions.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    move                        Y       Y       Y
    mvcur                       Y       Y       Y
    wmove                       Y       Y       Y



--------------------------------------------------------------------------


outopts
-------

### Synopsis

    int clearok(WINDOW *win, bool bf);
    int idlok(WINDOW *win, bool bf);
    void idcok(WINDOW *win, bool bf);
    void immedok(WINDOW *win, bool bf);
    int leaveok(WINDOW *win, bool bf);
    int setscrreg(int top, int bot);
    int wsetscrreg(WINDOW *win, int top, int bot);
    int scrollok(WINDOW *win, bool bf);

    int raw_output(bool bf);

### Description

   With clearok(), if bf is TRUE, the next call to wrefresh() with
   this window will clear the screen completely and redraw the
   entire screen.

   immedok(), called with a second argument of TRUE, causes an
   automatic wrefresh() every time a change is made to the
   specified window.

   Normally, the hardware cursor is left at the location of the
   window being refreshed.  leaveok() allows the cursor to be
   left wherever the update happens to leave it.  It's useful
   for applications where the cursor is not used, since it reduces
   the need for cursor motions.  If possible, the cursor is made
   invisible when this option is enabled.

   wsetscrreg() sets a scrolling region in a window; "top" and
   "bot" are the line numbers for the top and bottom margins. If
   this option and scrollok() are enabled, any attempt to move off
   the bottom margin will cause all lines in the scrolling region
   to scroll up one line. setscrreg() is the stdscr version.

   idlok() and idcok() do nothing in PDCurses, but are provided for
   compatibility with other curses implementations.

   raw_output() enables the output of raw characters using the
   standard *add* and *ins* curses functions (that is, it disables
   translation of control characters).

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    clearok                     Y       Y       Y
    idlok                       Y       Y       Y
    idcok                       Y       -      4.0
    immedok                     Y       -      4.0
    leaveok                     Y       Y       Y
    setscrreg                   Y       Y       Y
    wsetscrreg                  Y       Y       Y
    scrollok                    Y       Y       Y
    raw_output                  -       -       -



--------------------------------------------------------------------------


overlay
-------

### Synopsis

    int overlay(const WINDOW *src_w, WINDOW *dst_w)
    int overwrite(const WINDOW *src_w, WINDOW *dst_w)
    int copywin(const WINDOW *src_w, WINDOW *dst_w, int src_tr,
                int src_tc, int dst_tr, int dst_tc, int dst_br,
                int dst_bc, int _overlay)

### Description

   overlay() and overwrite() copy all the text from src_w into
   dst_w. The windows need not be the same size. Those characters
   in the source window that intersect with the destination window
   are copied, so that the characters appear in the same physical
   position on the screen. The difference between the two functions
   is that overlay() is non-destructive (blanks are not copied)
   while overwrite() is destructive (blanks are copied).

   copywin() is similar, but doesn't require that the two windows
   overlap. The arguments src_tc and src_tr specify the top left
   corner of the region to be copied. dst_tc, dst_tr, dst_br, and
   dst_bc specify the region within the destination window to copy
   to. The argument "overlay", if TRUE, indicates that the copy is
   done non-destructively (as in overlay()); blanks in the source
   window are not copied to the destination window. When overlay is
   FALSE, blanks are copied.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    overlay                     Y       Y       Y
    overwrite                   Y       Y       Y
    copywin                     Y       -      3.0



--------------------------------------------------------------------------


pad
---

### Synopsis

    WINDOW *newpad(int nlines, int ncols);
    WINDOW *subpad(WINDOW *orig, int nlines, int ncols,
                   int begy, int begx);
    int prefresh(WINDOW *win, int py, int px, int sy1, int sx1,
                 int sy2, int sx2);
    int pnoutrefresh(WINDOW *w, int py, int px, int sy1, int sx1,
                     int sy2, int sx2);
    int pechochar(WINDOW *pad, chtype ch);
    int pecho_wchar(WINDOW *pad, const cchar_t *wch);

### Description

   A pad is a special kind of window, which is not restricted by
   the screen size, and is not necessarily associated with a
   particular part of the screen.  You can use a pad when you need
   a large window, and only a part of the window will be on the
   screen at one time.  Pads are not refreshed automatically (e.g.,
   from scrolling or echoing of input).  You can't call wrefresh()
   with a pad as an argument; use prefresh() or pnoutrefresh()
   instead.  Note that these routines require additional parameters
   to specify the part of the pad to be displayed, and the location
   to use on the screen.

   newpad() creates a new pad data structure.

   subpad() creates a new sub-pad within a pad, at position (begy,
   begx), with dimensions of nlines lines and ncols columns. This
   position is relative to the pad, and not to the screen as with
   subwin.  Changes to either the parent pad or sub-pad will affect
   both.  When using sub-pads, you may need to call touchwin()
   before calling prefresh().

   pnoutrefresh() copies the specified pad to the virtual screen.

   prefresh() calls pnoutrefresh(), followed by doupdate().

   These routines are analogous to wnoutrefresh() and wrefresh().
   (py, px) specifies the upper left corner of the part of the pad
   to be displayed; (sy1, sx1) and (sy2, sx2) describe the screen
   rectangle that will contain the selected part of the pad.

   pechochar() is functionally equivalent to addch() followed by
   a call to prefresh(), with the last-used coordinates and
   dimensions. pecho_wchar() is the wide-character version.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    newpad                      Y       -       Y
    subpad                      Y       -       Y
    prefresh                    Y       -       Y
    pnoutrefresh                Y       -       Y
    pechochar                   Y       -      3.0
    pecho_wchar                 Y



--------------------------------------------------------------------------


panel
-----

### Synopsis

    int bottom_panel(PANEL *pan);
    int del_panel(PANEL *pan);
    int hide_panel(PANEL *pan);
    int move_panel(PANEL *pan, int starty, int startx);
    PANEL *new_panel(WINDOW *win);
    PANEL *panel_above(const PANEL *pan);
    PANEL *panel_below(const PANEL *pan);
    int panel_hidden(const PANEL *pan);
    const void *panel_userptr(const PANEL *pan);
    WINDOW *panel_window(const PANEL *pan);
    int replace_panel(PANEL *pan, WINDOW *win);
    int set_panel_userptr(PANEL *pan, const void *uptr);
    int show_panel(PANEL *pan);
    int top_panel(PANEL *pan);
    void update_panels(void);

### Description

   The panel library is built using the curses library, and any
   program using panels routines must call one of the curses
   initialization routines such as initscr(). A program using these
   routines must be linked with the panels and curses libraries.
   The header <panel.h> includes the header <curses.h>.

   The panels package gives the applications programmer a way to
   have depth relationships between curses windows; a curses window
   is associated with every panel. The panels routines allow curses
   windows to overlap without making visible the overlapped
   portions of underlying windows. The initial curses window,
   stdscr, lies beneath all panels. The set of currently visible
   panels is the 'deck' of panels.

   The panels package allows the applications programmer to create
   panels, fetch and set their associated windows, shuffle panels
   in the deck, and manipulate panels in other ways.

   bottom_panel() places pan at the bottom of the deck. The size,
   location and contents of the panel are unchanged.

   del_panel() deletes pan, but not its associated winwow.

   hide_panel() removes a panel from the deck and thus hides it
   from view.

   move_panel() moves the curses window associated with pan, so
   that its upper lefthand corner is at the supplied coordinates.
   (Do not use mvwin() on the window.)

   new_panel() creates a new panel associated with win and returns
   the panel pointer. The new panel is placed at the top of the
   deck.

   panel_above() returns a pointer to the panel in the deck above
   pan, or NULL if pan is the top panel. If the value of pan passed
   is NULL, this function returns a pointer to the bottom panel in
   the deck.

   panel_below() returns a pointer to the panel in the deck below
   pan, or NULL if pan is the bottom panel. If the value of pan
   passed is NULL, this function returns a pointer to the top panel
   in the deck.

   panel_hidden() returns OK if pan is hidden and ERR if it is not.

   panel_userptr() - Each panel has a user pointer available for
   maintaining relevant information. This function returns a
   pointer to that information previously set up by
   set_panel_userptr().

   panel_window() returns a pointer to the curses window associated
   with the panel.

   replace_panel() replaces the current window of pan with win.

   set_panel_userptr() - Each panel has a user pointer available
   for maintaining relevant information. This function sets the
   value of that information.

   show_panel() makes a previously hidden panel visible and places
   it back in the deck on top.

   top_panel() places pan on the top of the deck. The size,
   location and contents of the panel are unchanged.

   update_panels() refreshes the virtual screen to reflect the
   depth relationships between the panels in the deck. The user
   must use doupdate() to refresh the physical screen.

### Return Value

   Each routine that returns a pointer to an object returns NULL if
   an error occurs. Each panel routine that returns an integer,
   returns OK if it executes successfully and ERR if it does not.

### Portability
                             X/Open    BSD    SYS V
    bottom_panel                -       -       Y
    del_panel                   -       -       Y
    hide_panel                  -       -       Y
    move_panel                  -       -       Y
    new_panel                   -       -       Y
    panel_above                 -       -       Y
    panel_below                 -       -       Y
    panel_hidden                -       -       Y
    panel_userptr               -       -       Y
    panel_window                -       -       Y
    replace_panel               -       -       Y
    set_panel_userptr           -       -       Y
    show_panel                  -       -       Y
    top_panel                   -       -       Y
    update_panels               -       -       Y

  Credits:
    Original Author - Warren Tucker <wht@n4hgf.mt-park.ga.us>



--------------------------------------------------------------------------


printw
------

### Synopsis

    int printw(const char *fmt, ...);
    int wprintw(WINDOW *win, const char *fmt, ...);
    int mvprintw(int y, int x, const char *fmt, ...);
    int mvwprintw(WINDOW *win, int y, int x, const char *fmt,...);
    int vwprintw(WINDOW *win, const char *fmt, va_list varglist);
    int vw_printw(WINDOW *win, const char *fmt, va_list varglist);

### Description

   The printw() functions add a formatted string to the window at
   the current or specified cursor position. The format strings are
   the same as used in the standard C library's printf(). (printw()
   can be used as a drop-in replacement for printf().)

### Return Value

   All functions return the number of characters printed, or
   ERR on error.

### Portability
                             X/Open    BSD    SYS V
    printw                      Y       Y       Y
    wprintw                     Y       Y       Y
    mvprintw                    Y       Y       Y
    mvwprintw                   Y       Y       Y
    vwprintw                    Y       -      4.0
    vw_printw                   Y



--------------------------------------------------------------------------


refresh
-------

### Synopsis

    int refresh(void);
    int wrefresh(WINDOW *win);
    int wnoutrefresh(WINDOW *win);
    int doupdate(void);
    int redrawwin(WINDOW *win);
    int wredrawln(WINDOW *win, int beg_line, int num_lines);

### Description

   wrefresh() copies the named window to the physical terminal
   screen, taking into account what is already there in order to
   optimize cursor movement. refresh() does the same, using stdscr.
   These routines must be called to get any output on the terminal,
   as other routines only manipulate data structures. Unless
   leaveok() has been enabled, the physical cursor of the terminal
   is left at the location of the window's cursor.

   wnoutrefresh() and doupdate() allow multiple updates with more
   efficiency than wrefresh() alone. wrefresh() works by first
   calling wnoutrefresh(), which copies the named window to the
   virtual screen.  It then calls doupdate(), which compares the
   virtual screen to the physical screen and does the actual
   update. A series of calls to wrefresh() will result in
   alternating calls to wnoutrefresh() and doupdate(), causing
   several bursts of output to the screen.  By first calling
   wnoutrefresh() for each window, it is then possible to call
   doupdate() only once.

   In PDCurses, redrawwin() is equivalent to touchwin(), and
   wredrawln() is the same as touchline(). In some other curses
   implementations, there's a subtle distinction, but it has no
   meaning in PDCurses.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    refresh                     Y       Y       Y
    wrefresh                    Y       Y       Y
    wnoutrefresh                Y       Y       Y
    doupdate                    Y       Y       Y
    redrawwin                   Y       -      4.0
    wredrawln                   Y       -      4.0



--------------------------------------------------------------------------


scanw
-----

### Synopsis

    int scanw(const char *fmt, ...);
    int wscanw(WINDOW *win, const char *fmt, ...);
    int mvscanw(int y, int x, const char *fmt, ...);
    int mvwscanw(WINDOW *win, int y, int x, const char *fmt, ...);
    int vwscanw(WINDOW *win, const char *fmt, va_list varglist);
    int vw_scanw(WINDOW *win, const char *fmt, va_list varglist);

### Description

   These routines correspond to the standard C library's scanf()
   family. Each gets a string from the window via wgetnstr(), and
   uses the resulting line as input for the scan.

### Return Value

   On successful completion, these functions return the number of
   items successfully matched.  Otherwise they return ERR.

### Portability
                             X/Open    BSD    SYS V
    scanw                       Y       Y       Y
    wscanw                      Y       Y       Y
    mvscanw                     Y       Y       Y
    mvwscanw                    Y       Y       Y
    vwscanw                     Y       -      4.0
    vw_scanw                    Y



--------------------------------------------------------------------------


scr_dump
--------

### Synopsis

    int putwin(WINDOW *win, FILE *filep);
    WINDOW *getwin(FILE *filep);
    int scr_dump(const char *filename);
    int scr_init(const char *filename);
    int scr_restore(const char *filename);
    int scr_set(const char *filename);

### Description

   getwin() reads window-related data previously stored in a file
   by putwin(). It then creates and initialises a new window using
   that data.

   putwin() writes all data associated with a window into a file,
   using an unspecified format. This information can be retrieved
   later using getwin().

   scr_dump() writes the current contents of the virtual screen to
   the file named by filename in an unspecified format.

   scr_restore() function sets the virtual screen to the contents
   of the file named by filename, which must have been written
   using scr_dump(). The next refresh operation restores the screen
   to the way it looked in the dump file.

   In PDCurses, scr_init() does nothing, and scr_set() is a synonym
   for scr_restore(). Also, scr_dump() and scr_restore() save and
   load from curscr. This differs from some other implementations,
   where scr_init() works with curscr, and scr_restore() works with
   newscr; but the effect should be the same. (PDCurses has no
   newscr.)

### Return Value

   On successful completion, getwin() returns a pointer to the
   window it created. Otherwise, it returns a null pointer. Other
   functions return OK or ERR.

### Portability
                             X/Open    BSD    SYS V
    putwin                      Y
    getwin                      Y
    scr_dump                    Y
    scr_init                    Y
    scr_restore                 Y
    scr_set                     Y



--------------------------------------------------------------------------


scroll
------

### Synopsis

    int scroll(WINDOW *win);
    int scrl(int n);
    int wscrl(WINDOW *win, int n);

### Description

   scroll() causes the window to scroll up one line.  This involves
   moving the lines in the window data strcture.

   With a positive n, scrl() and wscrl() scroll the window up n
   lines (line i + n becomes i); otherwise they scroll the window
   down n lines.

   For these functions to work, scrolling must be enabled via
   scrollok(). Note also that scrolling is not allowed if the
   supplied window is a pad.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    scroll                      Y       Y       Y
    scrl                        Y       -      4.0
    wscrl                       Y       -      4.0



--------------------------------------------------------------------------


slk
---

### Synopsis

    int slk_init(int fmt);
    int slk_set(int labnum, const char *label, int justify);
    int slk_refresh(void);
    int slk_noutrefresh(void);
    char *slk_label(int labnum);
    int slk_clear(void);
    int slk_restore(void);
    int slk_touch(void);
    int slk_attron(const chtype attrs);
    int slk_attr_on(const attr_t attrs, void *opts);
    int slk_attrset(const chtype attrs);
    int slk_attr_set(const attr_t attrs, short color_pair, void *opts);
    int slk_attroff(const chtype attrs);
    int slk_attr_off(const attr_t attrs, void *opts);
    int slk_color(short color_pair);

    int slk_wset(int labnum, const wchar_t *label, int justify);

    int PDC_mouse_in_slk(int y, int x);
    void PDC_slk_free(void);
    void PDC_slk_initialize(void);

    wchar_t *slk_wlabel(int labnum)

### Description

   These functions manipulate a window that contain Soft Label Keys
   (SLK). To use the SLK functions, a call to slk_init() must be
   made BEFORE initscr() or newterm(). slk_init() removes 1 or 2
   lines from the useable screen, depending on the format selected.

   The line(s) removed from the screen are used as a separate
   window, in which SLKs are displayed.

   slk_init() requires a single parameter which describes the
   format of the SLKs as follows:

   0       3-2-3 format
   1       4-4 format
   2       4-4-4 format (ncurses extension)
   3       4-4-4 format with index line (ncurses extension)
   2 lines used
   55      5-5 format (pdcurses format)

   slk_refresh(), slk_noutrefresh() and slk_touch() are analogous
   to refresh(), noutrefresh() and touch().

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    slk_init                    Y       -       Y
    slk_set                     Y       -       Y
    slk_refresh                 Y       -       Y
    slk_noutrefresh             Y       -       Y
    slk_label                   Y       -       Y
    slk_clear                   Y       -       Y
    slk_restore                 Y       -       Y
    slk_touch                   Y       -       Y
    slk_attron                  Y       -       Y
    slk_attrset                 Y       -       Y
    slk_attroff                 Y       -       Y
    slk_attr_on                 Y
    slk_attr_set                Y
    slk_attr_off                Y
    slk_wset                    Y
    PDC_mouse_in_slk            -       -       -
    PDC_slk_free                -       -       -
    PDC_slk_initialize          -       -       -
    slk_wlabel                  -       -       -



--------------------------------------------------------------------------


termattr
--------

### Synopsis

    int baudrate(void);
    char erasechar(void);
    bool has_ic(void);
    bool has_il(void);
    char killchar(void);
    char *longname(void);
    chtype termattrs(void);
    attr_t term_attrs(void);
    char *termname(void);

    int erasewchar(wchar_t *ch);
    int killwchar(wchar_t *ch);

    char wordchar(void);

### Description

   baudrate() is supposed to return the output speed of the
   terminal. In PDCurses, it simply returns INT_MAX.

   has_ic and has_il() return TRUE. These functions have meaning in
   some other implementations of curses.

   erasechar() and killchar() return ^H and ^U, respectively -- the
   ERASE and KILL characters. In other curses implementations,
   these may vary by terminal type. erasewchar() and killwchar()
   are the wide-character versions; they take a pointer to a
   location in which to store the character, and return OK or ERR.

   longname() returns a pointer to a static area containing a
   verbose description of the current terminal. The maximum length
   of the string is 128 characters.  It is defined only after the
   call to initscr() or newterm().

   termname() returns a pointer to a static area containing a
   short description of the current terminal (14 characters).

   termattrs() returns a logical OR of all video attributes
   supported by the terminal.

   wordchar() is a PDCurses extension of the concept behind the
   functions erasechar() and killchar(), returning the "delete
   word" character, ^W.

### Portability
                             X/Open    BSD    SYS V
    baudrate                    Y       Y       Y
    erasechar                   Y       Y       Y
    has_ic                      Y       Y       Y
    has_il                      Y       Y       Y
    killchar                    Y       Y       Y
    longname                    Y       Y       Y
    termattrs                   Y       Y       Y
    termname                    Y       Y       Y
    erasewchar                  Y
    killwchar                   Y
    term_attrs                  Y
    wordchar                    -       -       -



--------------------------------------------------------------------------


touch
-----

### Synopsis

    int touchwin(WINDOW *win);
    int touchline(WINDOW *win, int start, int count);
    int untouchwin(WINDOW *win);
    int wtouchln(WINDOW *win, int y, int n, int changed);
    bool is_linetouched(WINDOW *win, int line);
    bool is_wintouched(WINDOW *win);

### Description

   touchwin() and touchline() throw away all information about
   which parts of the window have been touched, pretending that the
   entire window has been drawn on.  This is sometimes necessary
   when using overlapping windows, since a change to one window
   will affect the other window, but the records of which lines
   have been changed in the other window will not reflect the
   change.

   untouchwin() marks all lines in the window as unchanged since
   the last call to wrefresh().

   wtouchln() makes n lines in the window, starting at line y, look
   as if they have (changed == 1) or have not (changed == 0) been
   changed since the last call to wrefresh().

   is_linetouched() returns TRUE if the specified line in the
   specified window has been changed since the last call to
   wrefresh().

   is_wintouched() returns TRUE if the specified window
   has been changed since the last call to wrefresh().

### Return Value

   All functions return OK on success and ERR on error except
   is_wintouched() and is_linetouched().

### Portability
                             X/Open    BSD    SYS V
    touchwin                    Y       Y       Y
    touchline                   Y       -      3.0
    untouchwin                  Y       -      4.0
    wtouchln                    Y       Y       Y
    is_linetouched              Y       -      4.0
    is_wintouched               Y       -      4.0



--------------------------------------------------------------------------


util
----

### Synopsis

    char *unctrl(chtype c);
    void filter(void);
    void use_env(bool x);
    int delay_output(int ms);

    int getcchar(const cchar_t *wcval, wchar_t *wch, attr_t *attrs,
                 short *color_pair, void *opts);
    int setcchar(cchar_t *wcval, const wchar_t *wch, const attr_t attrs,
                 short color_pair, const void *opts);
    wchar_t *wunctrl(cchar_t *wc);

    int PDC_mbtowc(wchar_t *pwc, const char *s, size_t n);
    size_t PDC_mbstowcs(wchar_t *dest, const char *src, size_t n);
    size_t PDC_wcstombs(char *dest, const wchar_t *src, size_t n);

### Description

   unctrl() expands the text portion of the chtype c into a
   printable string. Control characters are changed to the "^X"
   notation; others are passed through. wunctrl() is the wide-
   character version of the function.

   filter() and use_env() are no-ops in PDCurses.

   delay_output() inserts an ms millisecond pause in output.

   getcchar() works in two modes: When wch is not NULL, it reads
   the cchar_t pointed to by wcval and stores the attributes in
   attrs, the color pair in color_pair, and the text in the
   wide-character string wch. When wch is NULL, getcchar() merely
   returns the number of wide characters in wcval. In either mode,
   the opts argument is unused.

   setcchar constructs a cchar_t at wcval from the wide-character
   text at wch, the attributes in attr and the color pair in
   color_pair. The opts argument is unused.

   Currently, the length returned by getcchar() is always 1 or 0.
   Similarly, setcchar() will only take the first wide character
   from wch, and ignore any others that it "should" take (i.e.,
   combining characters). Nor will it correctly handle any
   character outside the basic multilingual plane (UCS-2).

### Return Value

   unctrl() and wunctrl() return NULL on failure. delay_output()
   always returns OK.

   getcchar() returns the number of wide characters wcval points to
   when wch is NULL; when it's not, getcchar() returns OK or ERR.

   setcchar() returns OK or ERR.

### Portability
                             X/Open    BSD    SYS V
    unctrl                      Y       Y       Y
    filter                      Y       -      3.0
    use_env                     Y       -      4.0
    delay_output                Y       Y       Y
    getcchar                    Y
    setcchar                    Y
    wunctrl                     Y
    PDC_mbtowc                  -       -       -
    PDC_mbstowcs                -       -       -
    PDC_wcstombs                -       -       -



--------------------------------------------------------------------------


window
------

### Synopsis

    WINDOW *newwin(int nlines, int ncols, int begy, int begx);
    WINDOW *derwin(WINDOW* orig, int nlines, int ncols,
                   int begy, int begx);
    WINDOW *subwin(WINDOW* orig, int nlines, int ncols,
                   int begy, int begx);
    WINDOW *dupwin(WINDOW *win);
    int delwin(WINDOW *win);
    int mvwin(WINDOW *win, int y, int x);
    int mvderwin(WINDOW *win, int pary, int parx);
    int syncok(WINDOW *win, bool bf);
    void wsyncup(WINDOW *win);
    void wcursyncup(WINDOW *win);
    void wsyncdown(WINDOW *win);

    WINDOW *resize_window(WINDOW *win, int nlines, int ncols);
    int wresize(WINDOW *win, int nlines, int ncols);
    WINDOW *PDC_makelines(WINDOW *win);
    WINDOW *PDC_makenew(int nlines, int ncols, int begy, int begx);
    void PDC_sync(WINDOW *win);

### Description

   newwin() creates a new window with the given number of lines,
   nlines and columns, ncols. The upper left corner of the window
   is at line begy, column begx. If nlines is zero, it defaults to
   LINES - begy; ncols to COLS - begx. Create a new full-screen
   window by calling newwin(0, 0, 0, 0).

   delwin() deletes the named window, freeing all associated
   memory. In the case of overlapping windows, subwindows should be
   deleted before the main window.

   mvwin() moves the window so that the upper left-hand corner is
   at position (y,x). If the move would cause the window to be off
   the screen, it is an error and the window is not moved. Moving
   subwindows is allowed.

   subwin() creates a new subwindow within a window.  The
   dimensions of the subwindow are nlines lines and ncols columns.
   The subwindow is at position (begy, begx) on the screen.  This
   position is relative to the screen, and not to the window orig.
   Changes made to either window will affect both.  When using this
   routine, you will often need to call touchwin() before calling
   wrefresh().

   derwin() is the same as subwin(), except that begy and begx are
   relative to the origin of the window orig rather than the
   screen.  There is no difference between subwindows and derived
   windows.

   mvderwin() moves a derived window (or subwindow) inside its
   parent window.  The screen-relative parameters of the window are
   not changed.  This routine is used to display different parts of
   the parent window at the same physical position on the screen.

   dupwin() creates an exact duplicate of the window win.

   wsyncup() causes a touchwin() of all of the window's parents.

   If wsyncok() is called with a second argument of TRUE, this
   causes a wsyncup() to be called every time the window is
   changed.

   wcursyncup() causes the current cursor position of all of a
   window's ancestors to reflect the current cursor position of the
   current window.

   wsyncdown() causes a touchwin() of the current window if any of
   its parent's windows have been touched.

   resize_window() allows the user to resize an existing window. It
   returns the pointer to the new window, or NULL on failure.

   wresize() is an ncurses-compatible wrapper for resize_window().
   Note that, unlike ncurses, it will NOT process any subwindows of
   the window. (However, you still can call it _on_ subwindows.) It
   returns OK or ERR.

   PDC_makenew() allocates all data for a new WINDOW * except the
   actual lines themselves. If it's unable to allocate memory for
   the window structure, it will free all allocated memory and
   return a NULL pointer.

   PDC_makelines() allocates the memory for the lines.

   PDC_sync() handles wrefresh() and wsyncup() calls when a window
   is changed.

### Return Value

   newwin(), subwin(), derwin() and dupwin() return a pointer
   to the new window, or NULL on failure. delwin(), mvwin(),
   mvderwin() and syncok() return OK or ERR. wsyncup(),
   wcursyncup() and wsyncdown() return nothing.

### Errors

   It is an error to call resize_window() before calling initscr().
   Also, an error will be generated if we fail to create a newly
   sized replacement window for curscr, or stdscr. This could
   happen when increasing the window size. NOTE: If this happens,
   the previously successfully allocated windows are left alone;
   i.e., the resize is NOT cancelled for those windows.

### Portability
                             X/Open    BSD    SYS V
    newwin                      Y       Y       Y
    delwin                      Y       Y       Y
    mvwin                       Y       Y       Y
    subwin                      Y       Y       Y
    derwin                      Y       -       Y
    mvderwin                    Y       -       Y
    dupwin                      Y       -      4.0
    wsyncup                     Y       -      4.0
    syncok                      Y       -      4.0
    wcursyncup                  Y       -      4.0
    wsyncdown                   Y       -      4.0
    resize_window               -       -       -
    wresize                     -       -       -
    PDC_makelines               -       -       -
    PDC_makenew                 -       -       -
    PDC_sync                    -       -       -



--------------------------------------------------------------------------


clipboard
---------

### Synopsis

    int PDC_getclipboard(char **contents, long *length);
    int PDC_setclipboard(const char *contents, long length);
    int PDC_freeclipboard(char *contents);
    int PDC_clearclipboard(void);

### Description

   PDC_getclipboard() gets the textual contents of the system's
   clipboard. This function returns the contents of the clipboard
   in the contents argument. It is the responsibilitiy of the
   caller to free the memory returned, via PDC_freeclipboard().
   The length of the clipboard contents is returned in the length
   argument.

   PDC_setclipboard copies the supplied text into the system's
   clipboard, emptying the clipboard prior to the copy.

   PDC_clearclipboard() clears the internal clipboard.

### Return Values

   indicator of success/failure of call.
   PDC_CLIP_SUCCESS        the call was successful
   PDC_CLIP_MEMORY_ERROR   unable to allocate sufficient memory for
                           the clipboard contents
   PDC_CLIP_EMPTY          the clipboard contains no text
   PDC_CLIP_ACCESS_ERROR   no clipboard support

### Portability
                             X/Open    BSD    SYS V
    PDC_getclipboard            -       -       -
    PDC_setclipboard            -       -       -
    PDC_freeclipboard           -       -       -
    PDC_clearclipboard          -       -       -



--------------------------------------------------------------------------


pdckbd
------

### Synopsis

    unsigned long PDC_get_input_fd(void);

### Description

   PDC_get_input_fd() returns the file descriptor that PDCurses
   reads its input from. It can be used for select().

### Portability
                             X/Open    BSD    SYS V
    PDC_get_input_fd            -       -       -



--------------------------------------------------------------------------


pdcsetsc
--------

### Synopsis

    int PDC_set_blink(bool blinkon);
    int PDC_set_bold(bool boldon);
    void PDC_set_title(const char *title);

### Description

   PDC_set_blink() toggles whether the A_BLINK attribute sets an
   actual blink mode (TRUE), or sets the background color to high
   intensity (FALSE). The default is platform-dependent (FALSE in
   most cases). It returns OK if it could set the state to match
   the given parameter, ERR otherwise.

   PDC_set_bold() toggles whether the A_BOLD attribute selects an actual
   bold font (TRUE), or sets the foreground color to high intensity
   (FALSE). It returns OK if it could set the state to match the given
   parameter, ERR otherwise.

   PDC_set_title() sets the title of the window in which the curses
   program is running. This function may not do anything on some
   platforms.

### Portability
                             X/Open    BSD    SYS V
    PDC_set_blink               -       -       -
    PDC_set_title               -       -       -



--------------------------------------------------------------------------


sb
--

### Synopsis

    int sb_init(void)
    int sb_set_horz(int total, int viewport, int cur)
    int sb_set_vert(int total, int viewport, int cur)
    int sb_get_horz(int *total, int *viewport, int *cur)
    int sb_get_vert(int *total, int *viewport, int *cur)
    int sb_refresh(void);

### Description

   These functions manipulate the scrollbar.

### Return Value

   All functions return OK on success and ERR on error.

### Portability
                             X/Open    BSD    SYS V
    sb_init                     -       -       -
    sb_set_horz                 -       -       -
    sb_set_vert                 -       -       -
    sb_get_horz                 -       -       -
    sb_get_vert                 -       -       -
    sb_refresh                  -       -       -



--------------------------------------------------------------------------

X11 Considerations
==================

PDCurses for X11 uses the System V IPC shared memory facility, along
with sockets, to share data between the curses program and the child
process created to manage the X stuff.

When compiling your application, you need to include the \<curses.h\> or
\<xcurses.h\> that comes with PDCurses. You also need to link your code
with libXCurses. You may need to link with the following libraries under
X11R5:

    Xaw Xmu Xt X11

or, under X11R6:

    Xaw Xmu Xt X11 SM ICE Xext

You can run "xcurses-config --libs" to show the link parameters for your
system. If using dynamic linking, on some systems, "-lXCurses" suffices.

By calling Xinitscr() rather than initscr(), you can pass your program
name and resource overrides to PDCurses. The program name is used as the
title of the X window, and for defining X resources specific to your
program.


Interaction with stdio
----------------------

Be aware that curses programs that expect to have a normal tty
underneath them will be very disappointed! Output directed to stdout
will go to the xterm that invoked the PDCurses application, or to the
console if not invoked directly from an xterm. Similarly, stdin will
expect its input from the same place as stdout.


X Resources
-----------

PDCurses for X11 recognizes the following resources:

### lines

Specifies the number of lines the "screen" will have. Directly equates
to LINES. There is no theoretical maximum. The minimum value must be 2.
Default: 24

### cols

Specifies the number of columns the "screen" will have. Directly equates
to COLS. There is no theoretical maximum. The minimum value must be 2.
Default: 80

### normalFont

The name of a fixed width font. Defaults:
- narrow: -misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1
- wide: -misc-fixed-medium-r-normal--20-200-75-75-c-100-iso10646-1

### italicFont

The name of a fixed width font to be used for characters with A_ITALIC
attributes. Must have the same cell size as normalFont. Defaults:
- narrow: -misc-fixed-medium-o-normal--13-120-75-75-c-70-iso8859-1
- wide: -misc-fixed-medium-o-normal--20-200-75-75-c-100-iso10646-1

### boldFont

The name of a fixed width font to be used for characters with A_BOLD
attributes. Must have the same cell size as normalFont. Defaults:
- narrow: -misc-fixed-bold-r-normal--13-120-75-75-c-70-iso8859-1
- wide: -misc-fixed-bold-r-normal--20-200-75-75-c-100-iso10646-1

### pointer

The name of a valid pointer cursor. Default: xterm

### pointerForeColor

The foreground color of the pointer. Default: black

### pointerBackColor

The background color of the pointer. Default: white

### cursorColor

### textCursor

The alignment of the text cursor; horizontal or vertical. Default:
horizontal

### colorBlack

The color of the COLOR_BLACK attribute. Default: Black

### colorRed

The color of the COLOR_RED attribute. Default: red3

### colorGreen

The color of the COLOR_GREEN attribute. Default: green3

### colorYellow

The color of the COLOR_YELLOW attribute. Default: yellow3

### colorBlue

The color of the COLOR_BLUE attribute. Default: blue3

### colorMagenta

The color of the COLOR_MAGENTA attribute. Default: magenta3

### colorCyan

The color of the COLOR_CYAN attribute. Default: cyan3

### colorWhite

The color of the COLOR_WHITE attribute. Default: Grey

### colorBoldBlack

COLOR_BLACK combined with A_BOLD. Default: grey40

### colorBoldRed

COLOR_RED combined with A_BOLD. Default: red1

### colorBoldGreen

COLOR_GREEN combined with A_BOLD. Default: green1

### colorBoldYellow

COLOR_YELLOW combined with A_BOLD. Default: yellow1

### colorBoldBlue

COLOR_BLUE combined with A_BOLD. Default: blue1

### colorBoldMagenta

COLOR_MAGENTA combined with A_BOLD. Default: magenta1

### colorBoldCyan

COLOR_CYAN combined with A_BOLD. Default: cyan1

### colorBoldWhite

COLOR_WHITE combined with A_BOLD. Default: White

### bitmap

The name of a valid bitmap file of depth 1 (black and white) used for
the application's icon. The file is an X bitmap. Default: a 32x32 or
64x64 pixmap depending on the window manager

### pixmap

The name of a valid pixmap file of any depth supported by the window
manager (color) for the application's icon, The file is an X11 pixmap.
This resource is only available if the libXpm package has been installed
(most systems have this by default). This resource overrides the
"bitmap" resource. Default: none, uses default bitmap above

### translations

Translations enable the user to customize the action that occurs when a
key, combination of keys, or a button is pressed.  The translations are
similar to those used by xterm.

Defaults:

    <Key>:        XCursesKeyPress()
    <KeyUp>:      XCursesKeyPress()
    <BtnDown>:    XCursesButton()
    <BtnUp>:      XCursesButton()
    <BtnMotion>:  XCursesButton()

The most useful action for KeyPress translations is string(). The
argument to the string() action can be either a string or a hex
representation of a character; e.g., string(0x1b) will send the ASCII
escape character to the application; string("[11~") will send [ 1 1 ~ ,
as separate keystrokes.

### shmmin

On most systems, there are two Unix kernel parameters that determine the
allowable size of a shared memory segment. These parameters are usually
something like SHMMIN and SHMMAX. To use shared memory, a program must
allocate a segment of shared memory that is between these two values.
Usually these values are like 1 for SHMMIN and some large number for
SHMMAX. Sometimes the Unix kernel is configured to have a value of
SHMMIN that is bigger than the size of one of the shared memory segments
that libXCurses uses. On these systems an error message like:

    Cannot allocate shared memory for SCREEN: Invalid argument

will result. To overcome this problem, this resource should be set to
the kernel value for SHMMIN. This ensures that a shared memory segment
will always be bigger than the kernel value for SHMMIN (and hopefully
less than SHMMAX!) Default: 0

### borderColor

The color of the border around the screen. Default: black

### borderWidth

The width in pixels of the border around the screen. Default: 0

### clickPeriod

The period (in milliseconds) between a button press and a button release
that determines if a click of a button has occurred. Default: 100

### doubleClickPeriod

The period (in milliseconds) between two button press events that
determines if a double click of a button has occurred. Default: 200

### composeKey

The name of the X key that defines the "compose key", which is used to
enter characters in the Latin-1 character set above 0xA0. (See "Compose
Keys for Latin-1" below.) This is used only when PDCurses is built
without XIM support. While in compose mode, the text cursor will appear
as a hollow rectangle. Default: Multi_key


Using Resources
---------------

All applications have a top-level class name of "XCurses". If Xinitscr()
is used, it sets an application's top-level widget name. (Otherwise the
name defaults to "PDCurses".)

Examples for app-defaults or .Xdefaults:

    !
    ! resources for XCurses class of programs
    !
    XCurses*lines:  30
    XCurses*cols:   80
    XCurses*normalFont:     9x13
    XCurses*bitmap: /tmp/xcurses.xbm
    XCurses*pointer: top_left_arrow
    !
    ! resources for testcurs - XCurses
    !
    testcurs.colorRed:      orange
    testcurs.colorBlack:    midnightblue
    testcurs.lines: 25
    *testcurs.Translations: #override \n \
      <Key>F12:  string(0x1b) string("[11~") \n
    !
    ! resources for THE - XCurses
    !
    ! resources with the * wildcard can be overridden by a parameter passed
    ! to initscr()
    !
    the*normalFont: 9x15
    the*lines:      40
    the*cols:       86
    the*pointer:    xterm
    the*pointerForeColor: white
    the*pointerBackColor: black
    !
    ! resources with the . format can not be overridden by a parameter passed
    ! to Xinitscr()
    !
    the.bitmap:     /home/mark/the/the64.xbm
    the.pixmap:     /home/mark/the/the64.xpm

Resources may also be passed as parameters to the Xinitscr() function.
Parameters are strings in the form of switches; e.g., to set the color
"red" to "indianred", and the number of lines to 30, the string passed
to Xinitscr would be: "-colorRed indianred -lines 30"


Compose Keys for Latin-1
------------------------

When built without XIM support, PDCurses for X11 provides its own,
limited compose key system for Latin-1 characters. The available
combinations are listed here. For a given character, any of the
combinations shown in the last column may be used. To generate a
character, press the "compose" key followed by one of the pairs of
keystrokes. Where no key is evident, the spacebar is used.  Thus, to
generate the NO-BREAK SPACE, press the "compose" key followed by two
hits of the spacebar.

With a typical modern X server, you can get many more compose key
combinations by using XIM instead. Configure PDCurses with --enable-xim
to use XIM support.

This document is encoded in UTF-8.

    Hex| Dec |Chr| Description ISO 10646-1:1993(E) | Compose key combinations
    ---+-----+---+---------------------------------+-------------------------
    A0 | 160 |   | NO-BREAK SPACE                  |
    A1 | 161 | ¡ | INVERTED EXCLAMATION MARK       | !   !!
    A2 | 162 | ¢ | CENT SIGN                       | c|  |c  c/  c$  C$  C|
    A3 | 163 | £ | POUND SIGN                      | L-  L$  L=  l-  l$  l=  |-
    A4 | 164 | ¤ | CURRENCY SIGN                   | xo  ox  XO  g$
    A5 | 165 | ¥ | YEN SIGN                        | =y  y=  =Y  Y=  Y-  y$  y-
    A6 | 166 | ¦ | BROKEN BAR                      | |   ||  vb  VB  |^
    A7 | 167 | § | SECTION SIGN                    | SO  SS  s!  S!  so
    A8 | 168 | ¨ | DIAERESIS                       | "   ""
    A9 | 169 | © | COPYRIGHT SIGN                  | CO  co  OC
    AA | 170 | ª | FEMININE ORDINAL INDICATOR      | sa  SA  a_  A_
    AB | 171 | « | LEFT DOUBLE ANGLE QUOTES        | <<
    AC | 172 | ¬ | NOT SIGN                        | -,  no  NO
    AD | 173 | ­ | SOFT HYPHEN                     | -   --
    AE | 174 | ® | REGISTERED SIGN                 | RO  ro  OR
    AF | 175 | ¯ | MACRON                          | -^  _^  __
    B0 | 176 | ° | DEGREE SIGN                     | o   0^  0*  de  DE  ^0
    B1 | 177 | ± | PLUS-MINUS SIGN                 | -+  +-
    B2 | 178 | ² | SUPERSCRIPT TWO                 | 2   2^  s2  ^2
    B3 | 179 | ³ | SUPERSCRIPT THREE               | 3   3^  s3  ^3
    B4 | 180 | ´ | ACUTE ACCENT                    | '   ''
    B5 | 181 | µ | MICRO SIGN                      | u   /u  /U  *m  *M
    B6 | 182 | ¶ | PILCROW SIGN                    | p!  P!  pg  PG
    B7 | 183 | · | MIDDLE DOT                      | .   .^  ..
    B8 | 184 | ¸ | CEDILLA                         | ,   ,,
    B9 | 185 | ¹ | SUPERSCRIPT ONE                 | 1   1^  s1  ^1
    BA | 186 | º | MASCULINE ORDINAL INDICATOR     | o_  s0  S0
    BB | 187 | » | RIGHT DOUBLE ANGLE QUOTES       | >>
    BC | 188 | ¼ | VULGAR FRACTION ONE QUARTER     | 14
    BD | 189 | ½ | VULGAR FRACTION ONE HALF        | 12
    BE | 190 | ¾ | VULGAR FRACTION THREE QUARTERS  | 34
    BF | 191 | ¿ | INVERTED QUESTION MARK          | ?   ??
    C0 | 192 | À | CAPITAL A WITH GRAVE ACCENT     | `A  A`
    C1 | 193 | Á | CAPITAL A WITH ACUTE ACCENT     | 'A  A'
    C2 | 194 | Â | CAPITAL A WITH CIRCUMFLEX ACCENT| ^A  A^  A>
    C3 | 195 | Ã | CAPITAL A WITH TILDE            | ~A  A~  A-
    C4 | 196 | Ä | CAPITAL A WITH DIAERESIS        | "A  A"
    C5 | 197 | Å | CAPITAL A WITH RING ABOVE       | oA  Ao  A*  OA  *A
    C6 | 198 | Æ | CAPITAL LIGATURE AE             | AE
    C7 | 199 | Ç | CAPITAL C WITH CEDILLA          | ,C  C,
    C8 | 200 | È | CAPITAL E WITH GRAVE ACCENT     | `E  E`
    C9 | 201 | É | CAPITAL E WITH ACUTE ACCENT     | 'E  E'
    CA | 202 | Ê | CAPITAL E WITH CIRCUMFLEX ACCENT| ^E  E^  E>
    CB | 203 | Ë | CAPITAL E WITH DIAERESIS        | "E  E"
    CC | 204 | Ì | CAPITAL I WITH GRAVE ACCENT     | `I  I`
    CD | 205 | Í | CAPITAL I WITH ACUTE ACCENT     | 'I  I'
    CE | 206 | Î | CAPITAL I WITH CIRCUMFLEX ACCENT| ^I  I^  I>
    CF | 207 | Ï | CAPITAL I WITH DIAERESIS        | "I  I"
    D0 | 208 | Ð | CAPITAL ETH                     | D-
    D1 | 209 | Ñ | CAPITAL N WITH TILDE            | ~N  N~  N-
    D2 | 210 | Ò | CAPITAL O WITH GRAVE ACCENT     | `O  O`
    D3 | 211 | Ó | CAPITAL O WITH ACUTE ACCENT     | 'O  O'
    D4 | 212 | Ô | CAPITAL O WITH CIRCUMFLEX ACCENT| ^O  O^  O>
    D5 | 213 | Õ | CAPITAL O WITH TILDE            | ~O  O~  O-
    D6 | 214 | Ö | CAPITAL O WITH DIAERESIS        | "O  O"
    D7 | 215 | × | MULTIPLICATION SIGN             | x   xx  XX  mu  MU
    D8 | 216 | Ø | CAPITAL O WITH STROKE           | /O  O/
    D9 | 217 | Ù | CAPITAL U WITH GRAVE ACCENT     | `U  U`
    DA | 218 | Ú | CAPITAL U WITH ACUTE ACCENT     | 'U  U'
    DB | 219 | Û | CAPITAL U WITH CIRCUMFLEX ACCENT| ^U  U^  U>
    DC | 220 | Ü | CAPITAL U WITH DIAERESIS        | "U  U"
    DD | 221 | Ý | CAPITAL Y WITH ACUTE ACCENT     | 'Y  Y'
    DE | 222 | Þ | CAPITAL THORN                   | P   TH  |P
    DF | 223 | ß | SMALL SHARP S                   | ss
    E0 | 224 | à | SMALL A WITH GRAVE ACCENT       | `a  a`
    E1 | 225 | á | SMALL A WITH ACUTE ACCENT       | 'a  a'
    E2 | 226 | â | SMALL A WITH CIRCUMFLEX ACCENT  | ^a  a^  a>
    E3 | 227 | ã | SMALL A WITH TILDE              | ~a  a~  a-
    E4 | 228 | ä | SMALL A WITH DIAERESIS          | "a  a"
    E5 | 229 | å | SMALL A WITH RING ABOVE         | oa  ao  Oa  a*  *a
    E6 | 230 | æ | SMALL LIGATURE AE               | ae
    E7 | 231 | ç | SMALL C WITH CEDILLA            | ,c  c,
    E8 | 232 | è | SMALL E WITH GRAVE ACCENT       | `e  e`
    E9 | 233 | é | SMALL E WITH ACUTE ACCENT       | 'e  e'
    EA | 234 | ê | SMALL E WITH CIRCUMFLEX ACCENT  | ^e  e^  e>
    EB | 235 | ë | SMALL E WITH DIAERESIS          | "e  e"
    EC | 236 | ì | SMALL I WITH GRAVE ACCENT       | `i  i`
    ED | 237 | í | SMALL I WITH ACUTE ACCENT       | 'i  i'
    EE | 238 | î | SMALL I WITH CIRCUMFLEX ACCENT  | ^i  i^  i>
    EF | 239 | ï | SMALL I WITH DIAERESIS          | "i  i"
    F0 | 240 | ð | SMALL ETH                       | d-
    F1 | 241 | ñ | SMALL N WITH TILDE              | ~n  n~  n-
    F2 | 242 | ò | SMALL O WITH GRAVE ACCENT       | `o  o`
    F3 | 243 | ó | SMALL O WITH ACUTE ACCENT       | 'o  o'
    F4 | 244 | ô | SMALL O WITH CIRCUMFLEX ACCENT  | ^o  o^  o>
    F5 | 245 | õ | SMALL O WITH TILDE              | ~o  o~  o-
    F6 | 246 | ö | SMALL O WITH DIAERESIS          | "o  o"
    F7 | 247 | ÷ | DIVISION SIGN                   | -:  :-
    F8 | 248 | ø | SMALL O WITH OBLIQUE BAR        | /o  o/
    F9 | 249 | ù | SMALL U WITH GRAVE ACCENT       | `u  u`
    FA | 250 | ú | SMALL U WITH ACUTE ACCENT       | 'u  u'
    FB | 251 | û | SMALL U WITH CIRCUMFLEX ACCENT  | ^u  u^  u>
    FC | 252 | ü | SMALL U WITH DIAERESIS          | "u  u"
    FD | 253 | ý | SMALL Y WITH ACUTE ACCENT       | 'y  y'
    FE | 254 | þ | SMALL THORN                     | p   th  |p
    FF | 255 | ÿ | SMALL Y WITH DIAERESIS          | "y  y"


Deprecated
----------

XCursesProgramName is no longer used. To set the program name, you must
use Xinitscr(), or PDC_set_title() to set just the window title.

The XCursesExit() function is now called automatically via atexit().
(Multiple calls to it are OK, so you don't need to remove it if you've
already added it for previous versions of PDCurses.)

XCURSES is no longer defined automatically, but need not be defined,
unless you want the X11-specific prototypes. (Normal curses programs
won't need it.)


--------------------------------------------------------------------------

SDL Considerations
==================

There are no special requirements to use PDCurses for SDL -- all
PDCurses-compatible code should work fine. (In fact, you can even build
against the Windows console pdcurses.dll, and then swap in the SDL
pdcurses.dll.) Nothing extra is needed beyond the base SDL library.
However, there are some optional special features, described here.

The SDL ports operate in one of two ways, depending on whether or not
they were built with WIDE=Y:


8-bit mode
----------

The font is a simple BMP, 32 characters wide by 8 characters tall,
preferably with a palette. (BMPs without palettes still work, but in
that case, no attributes will be available, nor will the cursor work.)
The first entry in the palette (usually black) is treated as the
background color; the last entry (usually white) is treated as the
foreground. These are changed or made transparent as appropriate; any
other colors in the palette are passed through unchanged. So -- although
a one-bit depth is sufficient for a normal font -- you could redraw some
characters as multi-colored tiles.

The font must be monospaced. The size of each character is derived by
dividing the width of the BMP by 32 and the height by 8. There is no
constraint on the dimensions.

As provided in the default font and expected by acs_map[], the font is
in Code Page 437 form. But you can of course use any layout if you're
not relying on correct values for the ACS_* macros.

The font can be set via the environment variable PDC_FONT. If it's not
set, PDCurses looks for a file named "pdcfont.bmp" in the current
directory at the time of initscr(). If neither is found, it uses the
built-in default font encoded in deffont.h.


16-bit mode
-----------

Instead of a BMP, PDC_FONT points to a TrueType font. Only true
monospaced fonts work well. The font can be set at compile time via
PDC_FONT_PATH, and/or at runtime via pdc_ttffont. The environment
variable PDC_FONT_SIZE is also available to control the font size (also
as a compile-time define, and at runtime as pdc_font_size.) The
character mapping for chtypes is 16-bit Unicode (the Basic Multilingual
Plane).

The default font (if not redefined) is based on the OS:

- Windows: C:/Windows/Fonts/lucon.ttf

- Mac: /Library/Fonts/Courier New.ttf

- Other: /usr/share/fonts/truetype/freefont/FreeMono.ttf


Backgrounds
-----------

PDCurses for SDL supports an optional background image BMP. This is used
whenever start_color() has not been called (see the ptest demo for an
example), or when use_default_colors() has been called after
start_color(), and the background color of a pair has been set to -1
(see ozdemo, worm, and rain for examples). The usage parallels that of
ncurses in an appropriate terminal (e.g., Gnome Terminal). The image is
tiled to cover the PDCurses window, and can be any size or depth.

As with the font, you can point to a location for the background via the
environment variable PDC_BACKGROUND; "pdcback.bmp" is the fallback.
(There is no default background.)


Icons
-----

The icon (used with SDL_WM_SetIcon() -- not used for the executable
file) can be set via the environment variable PDC_ICON, and falls back
to "pdcicon.bmp", and then to the built-in icon from deficon.h. The
built-in icon is the PDCurses logo, as seen in ../x11/little_icon.xbm.
The SDL docs say that the icon must be 32x32, at least for use with MS
Windows.

If pdc_screen is preinitialized (see below), PDCurses does not attempt
to set the icon.


Screen size
-----------

The default screen size is 80x25 characters (whatever size they may be),
but you can override this via the environment variables PDC_COLS and/or
PDC_LINES. (Some other ports use COLS and LINES; this is not done here
because those values are, or should be, those of the controlling
terminal, and PDCurses for SDL is independent of the terminal.) If
pdc_screen is preinitialized (see below), these are ignored.


Integration with SDL
--------------------

If you want to go further, you can mix PDCurses and SDL functions. (Of
course this is extremely non-portable!) To aid you, there are several
external variables and functions specific to the SDL ports; you could
include pdcsdl.h, or just add the declarations you need in your code:

    PDCEX SDL_Surface *pdc_screen, *pdc_font, *pdc_icon, *pdc_back;
    PDCEX int pdc_sheight, pdc_swidth, pdc_yoffset, pdc_xoffset;

    PDCEX void PDC_update_rects(void);
    PDCEX void PDC_retile(void);

The SDL2 port adds:

    PDCEX SDL_Window *pdc_window;

pdc_screen is the main surface, unless it's preset before initscr(). In
SDL1, pdc_screen is created by SDL_SetVideoMode(); in SDL2, pdc_window
is the main window, created by SDL_CreateWindow(), and pdc_screen is set
by SDL_GetWindowSurface(pdc_window). (See sdltest.c for examples.) You
can perform normal SDL operations on this surface, but PDCurses won't
respect them when it updates. (For that, see PDC_retile().) As an
alternative, you can preinitialize this surface before calling
initscr(). In that case, you can use pdc_sheight, pdc_swidth,
pdc_yoffset and/or pdc_xoffset (q.v.) to confine PDCurses to only a
specific area of the surface, reserving the rest for other SDL
operations. If you preinitialize pdc_screen, you'll have to close it
yourself; PDCurses will ignore resize events, and won't try to set the
icon. Also note that if you preinitialize pdc_screen, it need not be the
display surface.

pdc_font (in 8-bit mode), pdc_icon, and pdc_back are the SDL_surfaces
for the font, icon, and background, respectively. You can set any or all
of them before initscr(), and thus override any of the other ways to set
them. But note that pdc_icon will be ignored if pdc_screen is preset.

pdc_sheight and pdc_swidth are the dimensions of the area of pdc_screen
to be used by PDCurses. You can preset them before initscr(); if either
is not set, it defaults to the full screen size minus the x or y offset,
as appropriate.

pdc_xoffset and pdc_yoffset are the x and y offset for the area of
pdc_screen to be used by PDCurses. See the sdltest demo for an example.

PDC_retile() makes a copy of pdc_screen, then tiles it with the
background image, if any. The resulting surface is used as the
background for transparent character cells. PDC_retile() is called from
initscr() and resize_term(). However, you can also use it at other
times, to take advantage of the way it copies pdc_screen: Draw some SDL
stuff; call PDC_retile(); do some curses stuff -- it will use whatever
was on pdc_screen as the background. Then you can erase the curses
screen, do some more SDL stuff, and call PDC_retile() again to make a
new background. (If you don't erase the curses screen, it will be
incorporated into the background when you call PDC_retile().) But this
only works if no background image is set.

PDC_update_rects() is how the screen actually gets updated. For
performance reasons, when drawing, PDCurses for SDL maintains a table of
rectangles that need updating, and only updates (by calling this
function) during getch(), napms(), or when the table gets full.
Normally, this is sufficient; but if you're pausing in some way other
than by using napms(), and you're not doing keyboard checks, you may get
an incomplete update. If that happens, you can call PDC_update_rects()
manually.


Interaction with stdio
----------------------

As with X11, it's a bad idea to mix curses and stdio calls. (In fact,
that's true for PDCurses on any platform; but especially these two,
which don't run under terminals.) Depending on how SDL is built, stdout
and stderr may be redirected to files.
