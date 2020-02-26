/* Public Domain Curses */

/* $Id: pdcwin.h,v 1.6 2008/07/13 06:36:32 wmcbrine Exp $ */

#ifdef PDC_WIDE
   #if !defined( UNICODE)
      # define UNICODE
   #endif
   #if !defined( _UNICODE)
      # define _UNICODE
   #endif
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE 1   /* kill nonsense warnings */
#endif

#if defined( GS_8BIT_INDICES) && defined( PDC_WIDE)
     /* We only need the 'fallback font' for the wide-char version, */
     /* and it'll only work in Win2000 or later (earlier versions   */
     /* lack the necessary GetFontUnicodeRanges() function.)  Comment */
     /* out the following line if you're dealing with a really old  */
     /* system,  or just don't want to use a fallback font.         */
   #define USE_FALLBACK_FONT
#endif

/* Windows.h will #define MOUSE_MOVED to 0x1,  which is what the
Windows API expects.  In Curses,  MOUSE_MOVED is a macro indicating
whether the mouse has moved,  so we'd get a compiler warning that
we're re-defining MOUSE_MOVED.  That warning may be a Good Thing in
your software,  letting you know:  "Be careful;  in a Curses program,
MOUSE_MOVED won't have the same meaning it does in a 'real' Windows
program."

   But in building WinGUI itself,  we're expecting the Curses
meaning for MOUSE_MOVED,  and the compiler warning would just be
a nuisance.  So we'll #undefine MOUSE_MOVED before getting to the
Curses definition :                 */

#undef MOUSE_MOVED
#include <curspriv.h>

extern int PDC_get_buffer_rows(void);

/* The following probably ought to go into 'curses.h' at some point. */
/* Or at least,  not be here;  this is a temporary place for them.   */

/* WinGUI has some drastically extended cursor possibilities.  The
following #defines can be used with curs_set().  For all other flavors
of PDCurses,  only the invisible,  "normal",  and "intense" cursor
states will be recognized.  In Win32,  caret,  half-block,  central
block,  cross,  and outlined block cursors are available.

   By default,  all such cursors will blink.  However,  in WinGUI,
the input value to curs_set() is treated as a two-byte value,  with
the cursor blinking between those two states.  For example,
curs_set( 0x0102) would blink between a "normal" (underline) cursor and
an "intense" (full-block) cursor.  One can see that this behavior is
backward-compatible;  for example,  curs_set( 1) would blink between a
"normal" underline cursor and an invisible cursor.  (But curs_set( 0x0101)
would result in a non-blinking underline cursor.)  Note that one can use
the PDC_CURSOR macro for this,  as in...

   curs_set( PDC_CURSOR( PDC_CURSOR_OUTLINE, PDC_CURSOR_INTENSE));   */

#define PDC_CURSOR_INVISIBLE                    0x00
#define PDC_CURSOR_NORMAL                       0x01
#define PDC_CURSOR_INTENSE                      0x02
#define PDC_CURSOR_OUTLINE                      0x03
#define PDC_CURSOR_CARET                        0x04
#define PDC_CURSOR_HALF_BLOCK                   0x05
#define PDC_CURSOR_CENTRAL_BLOCK                0x06
#define PDC_CURSOR_CROSS                        0x07
#define PDC_CURSOR_OUTLINE_BLOCK                0x08

#define PDC_CURSOR( A, B) ((A)<<8 | (B))
#define PDC_CURSOR_IS_BLINKING    \
          ((SP->visibility >> 8) != (SP->visibility & 0xff))

#define PDC_REAL_ATTR_SHIFT  17

#define KEY_QUEUE_SIZE    30

extern int PDC_key_queue_low, PDC_key_queue_high;
extern int PDC_key_queue[KEY_QUEUE_SIZE];

extern HWND PDC_hWnd;
void PDC_transform_line_given_hdc( const HDC hdc, const int lineno,
                             int x, int len, const chtype *srcp);
int PDC_choose_a_new_font( void);                     /* pdcdisp.c */
HFONT PDC_get_font_handle( const int is_bold);            /* pdcdisp.c */

#ifdef USE_FALLBACK_FONT
extern GLYPHSET *PDC_unicode_range_data;
#endif         /* #ifdef USE_FALLBACK_FONT */

extern TCHAR PDC_font_name[];
extern int PDC_font_size;

extern int PDC_cxChar, PDC_cyChar;

extern int PDC_n_rows, PDC_n_cols;

extern bool PDC_bDone;

extern int PDC_blink_state;

extern COLORREF PDC_palette[PDC_MAXCOL];

int debug_printf( const char *format, ...);        /* pdcscrn.c */
