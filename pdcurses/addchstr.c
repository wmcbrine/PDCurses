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
#define  CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

#ifdef HAVE_STRING_H
# include <string.h>
#endif

/* undefine any macros for functions defined in this module */
#undef   addchstr
#undef   addchnstr
#undef   waddchstr
#undef   waddchnstr
#undef   mvaddchstr
#undef   mvaddchnstr
#undef   mvwaddchstr
#undef   mvwaddchnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef move
#  undef wmove
#endif

#ifdef PDCDEBUG
char *rcsid_addchstr  = "$Id: addchstr.c,v 1.3 2002/12/16 09:41:34 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        addchstr

  Synopsis:
   int addchstr(chtype *ch);
   int addchnstr(chtype *ch, int n);
   int waddchstr(WINDOW *win, chtype *ch);
   int waddchnstr(WINDOW *win, chtype *ch, int n);
   int mvaddchstr(int y, int x, chtype *ch);
   int mvaddchnstr(int y, int x, chtype *ch, int n);
   int mvwaddchstr(WINDOW *, int y, int x, chtype *ch);
   int mvwaddchnstr(WINDOW *, int y, int x, chtype *ch, int n);

  X/Open Description:
   These routines write a chtype directly into the window structure
   starting at the current position.
   The four routines with n as the last argument copy at most n
   elements, but no more than will fit on the line.
   If n=-1 then the whole string is copied, to the maximum number
   that will fit on the line.

   The cursor position is not advanced. These routines do not check for
   newline or other special characters, no does any line wrapping occur.

   NOTE: addchstr(), mvaddchstr(), mvwaddchstr() addchnstr(), 
      mvaddchnstr(), and mvwaddchnstr() are implemented as macros.

  X/Open Return Value:
   All functions return OK on success and ERR on error.

  X/Open Errors:
   No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      addchstr                              -        -      4.0
      waddchstr                             -        -      4.0
      mvaddchstr                            -        -      4.0
      mvwaddchstr                           -        -      4.0
      addchnstr                             -        -      4.0
      waddchnstr                            -        -      4.0
      mvaddchnstr                           -        -      4.0
      mvwaddchnstr                          -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   addchstr(chtype *ch)
#else
int   PDC_CDECL   addchstr(ch)
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("addchstr() - called\n");
#endif

   return( addchnstr( ch, -1) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   addchnstr(chtype *ch, int n)
#else
int   PDC_CDECL   addchnstr(ch,n)
chtype *ch;
int n;
#endif
/***********************************************************************/
{
   int y,x,num,maxx;
   chtype *ptr;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("addchnstr() - called\n");
#endif

   if (stdscr == (WINDOW *)NULL)
      return( ERR );

   if (n == 0
   ||  n < (-1))
      return( ERR );

   x = stdscr->_curx;
   y = stdscr->_cury;
   ptr = &(stdscr->_y[y][x]);


   if (n == (-1))
   {
      for (num = stdscr->_maxx - x; *ch && num--; num++)
      {
         *ptr++ = *ch++;
      }
      maxx = num;
   }
   else
   {
      num = min(stdscr->_maxx - x,n);
      memcpy((char *)ptr, (char *)ch, (int)(num * sizeof(chtype)));
      maxx = x+num-1;
   }

   if (stdscr->_firstch[y] == _NO_CHANGE)
   {
      stdscr->_firstch[y] = x;
      stdscr->_lastch[y] = maxx;
   }
   else
   {
      if (x <  stdscr->_firstch[y])
      {
         stdscr->_firstch[y] = x;
      }
      if (maxx >  stdscr->_lastch[y])
      {
         stdscr->_lastch[y] = maxx;
      }
   }
   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   waddchstr(WINDOW *win, chtype *ch)
#else
int   PDC_CDECL   waddchstr(win,ch)
WINDOW *win;
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("waddchstr() - called: win=%x\n",win);
#endif

   if (win == (WINDOW *)NULL)
      return( ERR );

   return( waddchnstr( win, ch, -1) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   waddchnstr(WINDOW *win, chtype *ch, int n)
#else
int   PDC_CDECL   waddchnstr(win,ch,n)
WINDOW *win;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
   register int i,y,x,num,maxx,minx;
   int first=1;
   chtype *ptr,*mptr;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("waddchnstr() - called: win=%x n=%d\n",win,n);
#endif

   if (win == (WINDOW *)NULL)
      return( ERR );

   if (n == 0
   ||  n < (-1))
      return( ERR );

   x = win->_curx;
   y = win->_cury;
   ptr = mptr = &(win->_y[y][x]);

#if 0
   if (n == (-1))
      {
      for (num = win->_maxx - x; *ch && num--; num++)
         *ptr++ = *ch++;
      maxx = num;
      }
   else
      {
      num = min(win->_maxx - x,n);
      maxx = x+num-1;
      memcpy((char *)ptr, (char *)ch, (int)(num * sizeof(chtype)));
      }

   if (win->_firstch[y] == _NO_CHANGE)
      {
      win->_firstch[y] = x;
      win->_lastch[y] = maxx;
      }
   else
      {
      if (x <  win->_firstch[y])
         {
         win->_firstch[y] = x;
         }
      if (maxx >  win->_lastch[y])
         {
         win->_lastch[y] = maxx;
         }
      }

#else

   if (n == (-1))
      num = win->_maxx - x;
   else
      num = min(win->_maxx - x,n);

   minx = win->_firstch[y];
   maxx = win->_lastch[y];
   for (i=x; num && *ch; num--, i++)
   {
      if (i < win->_firstch[y]
      ||  win->_firstch[y] == _NO_CHANGE)
      {
         if (*ptr != *ch && first)
         {
            minx = i;
            first = 0;
         }
         }
         if (i > win->_lastch[y])
         {
            if (*ptr != *ch)
               maxx = i;
         }

#ifdef PDCDEBUG
      if (trace_on)
      {
         PDC_debug("y %d i %d minx %d maxx %d *ptr %x *ch %x firstch: %d lastch: %d\n",
                   y,i,minx,maxx,
                   *ptr,*ch,
                   win->_firstch[y],win->_lastch[y]
                   );
      }
#endif

      *ptr++ = *ch++;
   }

   win->_firstch[y] = minx;
   win->_lastch[y] = maxx;
#endif

   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvaddchstr(int y, int x, chtype *ch)
#else
int   PDC_CDECL   mvaddchstr(y,x,ch)
int y;
int x;
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("mvaddchstr() - called: y %d x %d\n",y,x);
#endif

   if (stdscr == (WINDOW *)NULL)
      return( ERR );

   if (wmove(stdscr,y,x) == ERR)
      return( ERR );

   return( addchnstr( ch, -1) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvaddchnstr(int y, int x, chtype *ch, int n)
#else
int   PDC_CDECL   mvaddchnstr(y,x,ch,n)
int y;
int x;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("mvaddchnstr() - called: y %d x %d n %d\n",y,x,n);
#endif

   if (stdscr == (WINDOW *)NULL)
      return( ERR );

   if (wmove(stdscr,y,x) == ERR)
      return( ERR );

   return( addchnstr( ch, n) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvwaddchstr(WINDOW *win, int y, int x, chtype *ch)
#else
int   PDC_CDECL   mvwaddchstr(win,y,x,ch)
WINDOW *win;
int y;
int x;
chtype *ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("waddchstr() - called:\n");
#endif

   if (win == (WINDOW *)NULL)
      return( ERR );

   if (wmove(win,y,x) == ERR)
      return( ERR );

   return( waddchnstr( win, ch, -1) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   mvwaddchnstr(WINDOW *win,int y, int x, chtype *ch, int n)
#else
int   PDC_CDECL   mvwaddchnstr(win,y,x,ch,n)
WINDOW *win;
int y;
int x;
chtype *ch;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("mvwaddchnstr() - called: y %d x %d n %d \n",y,x,n);
#endif

   if (win == (WINDOW *)NULL)
      return( ERR );

   if (wmove(win,y,x) == ERR)
      return( ERR );

   return( waddchnstr( win, ch, n) );
}
