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

#ifdef __STDC__
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

#ifdef __STDC__
void    clsbody    (void);
int     bodylen    (void);
WINDOW *bodywin    (void);

void    rmerror    (void);
void    rmstatus   (void);

void    titlemsg   (char *msg);
void    bodymsg    (char *msg);
void    errormsg   (char *msg);
void    statusmsg  (char *msg);

bool    keypressed (void);
int     getkey     (void);
void    flushkeys  (void);
int     waitforkey (void);

void    DoExit     (void);
void    startmenu  (menu *mp, char *title);
void    domenu     (menu *mp);

int     weditstr   (WINDOW *win, char *buf, int field);
WINDOW *winputbox  (WINDOW *win, int nlines, int ncols);
int     getstrings (char *desc[], char *buf[], int field);
#else
void    clsbody    ();
int     bodylen    ();
WINDOW *bodywin    ();

void    rmerror    ();
void    rmstatus   ();

void    titlemsg   ();
void    bodymsg    ();
void    errormsg   ();
void    statusmsg  ();

bool    keypressed ();
int     getkey     ();
void    flushkeys  ();
int     waitforkey ();

void    DoExit     ();
void    startmenu  ();
void    domenu     ();

int     weditstr   ();
WINDOW *winputbox  ();
int     getstrings ();
#endif

#define editstr(s,f)           (weditstr(stdscr,s,f))
#define mveditstr(y,x,s,f)     (move(y,x)==ERR?ERR:editstr(s,f))
#define mvweditstr(w,y,x,s,f)  (wmove(w,y,x)==ERR?ERR:weditstr(w,s,f))

#define inputbox(l,c)          (winputbox(stdscr,l,c))
#define mvinputbox(y,x,l,c)    (move(y,x)==ERR?w:inputbox(l,c))
#define mvwinputbox(w,y,x,l,c) (wmove(w,y,x)==ERR?w:winputbox(w,l,c))

#endif
