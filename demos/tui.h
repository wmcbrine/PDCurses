/*
***************************************************************************
* This file comprises part of PDCurses. PDCurses is Public Domain software.
* You may use this code for whatever purposes you desire. This software
* is provided AS IS with NO WARRANTY whatsoever.
* Should this software be used in another application, an acknowledgement
* that PDCurses code is used would be appreciated, but is not mandatory.
*
* Any changes which you make to this software which may improve or enhance
* it, should be forwarded to the current maintainer for the benefit of 
* other users.
*
* The only restriction placed on this code is that no distribution of
* modified PDCurses code be made under the PDCurses name, by anyone
* other than the current maintainer.
* 
* See the file maintain.er for details of the current maintainer.
***************************************************************************
*/
/*
 * File   : tui.h      'textual user interface'
 * Author : P.J. Kunst  (kunst@prl.philips.nl)
 * Date   : 25-02-93
 * Version: 1.02
 */

#ifndef _TUI_H_
#define _TUI_H_

#include <curses.h>

#if defined(HAVE_PROTO) && !defined(__STDC__)
# define __STDC__ 1
#endif

#ifndef Args
# if __STDC__
#  define Args(x) x
# else
#  define Args(x) ()
# endif
#endif

#ifndef getbegyx
#define getbegyx(w,y,x)        ( y = (w)->_begy, x = (w)->_begx )
#endif
#ifndef getmaxyx
#define getmaxyx(w,y,x)        ( y = (w)->_maxy, x = (w)->_maxx )
#endif

#ifdef A_COLOR
#define A_ATTR  (A_ATTRIBUTES ^ A_COLOR)  /* A_BLINK, A_REVERSE, A_BOLD */
#else
#define A_ATTR  (A_ATTRIBUTES)            /* standard UNIX attributes */
#endif


#define MAXSTRLEN  256
#define KEY_ESC    0x1b     /* Escape */

#if __STDC__
typedef void (*FUNC)(void);
#else
typedef void (*FUNC)();
#endif

typedef struct 
{
  char *name;   /* item label */
  FUNC  func;   /* (pointer to) function */
  char *desc;   /* function description */
} menu;

/* ANSI C function prototypes: */

void    clsbody    Args((void));
int     bodylen    Args((void));
WINDOW *bodywin    Args((void));

void    rmerror    Args((void));
void    rmstatus   Args((void));

void    titlemsg   Args((char *msg));
void    bodymsg    Args((char *msg));
void    errormsg   Args((char *msg));
void    statusmsg  Args((char *msg));

bool    keypressed Args((void));
int     getkey     Args((void));
int     waitforkey Args((void));

void    DoExit     Args((void));
void    startmenu  Args((menu *mp, char *title));
void    domenu     Args((menu *mp));

int     weditstr   Args((WINDOW *win, char *buf, int field));
WINDOW *winputbox  Args((WINDOW *win, int nlines, int ncols));
int     getstrings Args((char *desc[], char *buf[], int field));

#define editstr(s,f)           (weditstr(stdscr,s,f))
#define mveditstr(y,x,s,f)     (move(y,x)==ERR?ERR:editstr(s,f))
#define mvweditstr(w,y,x,s,f)  (wmove(w,y,x)==ERR?ERR:weditstr(w,s,f))

#define inputbox(l,c)          (winputbox(stdscr,l,c))
#define mvinputbox(y,x,l,c)    (move(y,x)==ERR?w:inputbox(l,c))
#define mvwinputbox(w,y,x,l,c) (wmove(w,y,x)==ERR?w:winputbox(w,l,c))

#endif
