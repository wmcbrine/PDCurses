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

#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

/* undefine any macros for functions defined in this module */
#undef   refresh
#undef   wrefresh
#undef   wnoutrefresh
#undef   doupdate
#undef   redrawwin
#undef   wredrawln

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef wattrset
#  undef mvwprintw
#  undef wmove
#  undef wattrset
#  undef touchwin
#  undef reset_prog_mode
#endif

#ifdef PDCDEBUG
char *rcsid_refresh  = "$Id: refresh.c,v 1.2 2002/12/16 06:58:28 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                       refresh

  Synopsis:
   int   refresh(void);
   int   wrefresh(WINDOW *win);
   int   wnoutrefresh(register WINDOW *win);
   int   doupdate(void);
   int   redrawwin(WINDOW *win);
   int   redrawln(WINDOW *win, int beg_line, int num_lines);

  X/Open Description:
   The routine wrefresh() copies the named window to the physical
   terminal screen, taking into account what is already there in
   order to optimize cursor movement.
   The routine refresh() does the same, using stdscr as a default
   screen.
   These routines must be called to get any output on the
   terminal, as other routines only manipulate data structures.
   Unless leaveok has been enabled, the physical cursor of the
   terminal is left at the location of the window's cursor.

   The wnoutrefresh() and doupdate() routines allow multiple updates
   with more efficiency than wrefresh() alone.  In addition to all 
   of the window structures representing the terminal screen: a physical
   screen, describing what is actually on the screen and a virtual screen,
   describing what the programmer wants to have on the screen.

   The wrefresh() function works by first calling wnoutrefresh(),
   which copies the named window to the virtual screen.  It then
   calls doupdate(), which compares the virtual screen to the
   physical screen and does the actual update.  If the programmer
   wishes to output several windows at once, a series of cals to
   wrefresh() will result in alternating calls to wnoutrefresh()
   and doupdate(), causing several bursts of output to the
   screen.  By first calling wnoutrefresh() for each window, it
   is then possible to call doupdate() once.  This results in
   only one burst of output, with probably fewer total characters
   transmitted and certainly less CPU time used.

  X/Open Return Value:
   All functions return OK on success and ERR on error.

  X/Open Errors:
   No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      refresh                               Y        Y       Y
      wrefresh                              Y        Y       Y
      wnoutrefresh                          Y        Y       Y
      doupdate                              Y        Y       Y
      redrawwin                             -        -      4.0
      wredrawln                             -        -      4.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   refresh( void )
#else
int   PDC_CDECL   refresh()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("refresh() - called\n");
#endif

   return( wrefresh(stdscr) );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   wrefresh(WINDOW *win)
#else
int   PDC_CDECL   wrefresh(win)
WINDOW *win;
#endif
/***********************************************************************/
{
   bool save_clear=FALSE;
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("wrefresh() - called\n");
#endif

   if (win == (WINDOW *)NULL) return( ERR );
   if (win->_flags & _PAD) return( ERR );
   if (win->_flags & _SUBPAD) return( ERR );

   save_clear = win->_clear;
   if (win == curscr)
      curscr->_clear = TRUE;
   else  
      wnoutrefresh(win);
   if (save_clear
   &&  win->_maxy == SP->lines
   &&  win->_maxx == SP->cols)
      {
      curscr->_clear = TRUE;
      }
   doupdate();
   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   wnoutrefresh(WINDOW *win)
#else
int   PDC_CDECL   wnoutrefresh(win)
WINDOW *win;
#endif
/***********************************************************************/
{
register int      first=0; /* first changed char on line */
register int      last=0;  /* last changed char on line  */
   int      begy=0,begx=0; /* window's place on screen   */
   int      i=0,j=0,x=0,y=0;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("wnoutrefresh() - called: win=%x\n",win);
#endif
   if (win == (WINDOW *)NULL)
      return( ERR );

   if ((win->_flags == _PAD)
   ||  (win->_flags == _SUBPAD))
      return( ERR );

   y = win->_cury;
   x = win->_curx;

   if (win->_title != NULL)
   {
      int len = strlen(win->_title);
      chtype attrs = win->_attrs;
   /*
    * There may be a better place to implement window titles, but this
    * seems to be the best place. -- Frotz
    */
      if ((len > 0)
      && !(win->_flags & _SUBWIN)
      && !(win->_flags & _SUBPAD))
      {
         wattrset(win, win->_title_attr);
         mvwprintw(win, 0, (win->_title_ofs), "%s", (long) win->_title);
         wmove(win, y, x); /* restore cursor postion */
         wattrset(win, attrs);   /* restore attributes     */
      }
   }

   if (win->_flags & _PAD)
      return( ERR );

   begy = win->_begy;
   begx = win->_begx;

   for (i = 0, j = begy; i < win->_maxy; i++, j++)
   {
      if (win->_firstch[i] != _NO_CHANGE)
      {
         first = win->_firstch[i];
         last = win->_lastch[i];

         memcpy(&(curscr->_y[j][begx + first]),
                &(win->_y[i][first]),
                (last - first + 1) * sizeof(chtype));

         first += begx; /* s's min/max change positions */
         last += begx;

         if (curscr->_firstch[j] != _NO_CHANGE)
            curscr->_firstch[j] = min(curscr->_firstch[j], first);
         else
            curscr->_firstch[j] = first;

         curscr->_lastch[j] = max(curscr->_lastch[j], last);

         win->_firstch[i] = _NO_CHANGE;   /* updated now */
      }
      win->_lastch[i] = _NO_CHANGE; /* updated now */
   }

   if (win->_clear)
      win->_clear = FALSE;

   if (!win->_leaveit)
   {
      curscr->_cury = win->_cury + begy;
      curscr->_curx = win->_curx + begx;
   }
   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   doupdate(void)
#else
int   PDC_CDECL   doupdate()
#endif
/***********************************************************************/
{
register int   i;
bool rc;
#ifdef   REGISTERWINDOWS
   WINDS*   next = SP->visible;
#endif

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("doupdate() - called\n");
#endif

   if (isendwin())  /* coming back after endwin() called */
   {
      reset_prog_mode();
      curscr->_clear = TRUE;
      SP->alive = TRUE; /* so isendwin() result is correct */
   }

#ifdef   REGISTERWINDOWS
   if (SP->refreshall)
   {
      while (next != NULL)
      {
         if (next->w->_parent != NULL)
         {
            touchwin(next->w->_parent);
            wnoutrefresh(next->w->_parent);
         }
         touchwin(next->w);
         wnoutrefresh(next->w);
         next = next->next;
      }
   }
#endif

   if (SP->shell)
      reset_prog_mode();

   if (curscr == (WINDOW *)NULL)
      return( ERR );

/* if checking for typeahead, bail out here if any is found */
   
   if (SP->refrbrk && (SP->cbreak || SP->raw_inp)) 
   {
      rc = PDC_breakout();
      if(rc)
         return( OK );
   }

   if (curscr->_clear)
      PDC_clr_update(curscr);
   else
   {
#ifdef WIN32
      if (!SP->refrbrk
      &&  getenv("PDC_FULL_DISPLAY") != NULL)
         PDC_doupdate();
      else
      {
         for (i = 0; i < SP->lines; i++)
         {
# ifdef PDCDEBUG
   if (trace_on) PDC_debug("doupdate() - Transforming line %d of %d: %s\n",i,SP->lines,(curscr->_firstch[i] != _NO_CHANGE)?"Yes" : "No");
#endif
            if (curscr->_firstch[i] != _NO_CHANGE)
               if (PDC_transform_line(i))  /* if test new */
                  break;
         }
      }
#else
      for (i = 0; i < SP->lines; i++)
      {
         if (curscr->_firstch[i] != _NO_CHANGE)
            if (PDC_transform_line(i))  /* if test new */
               break;
      }
#endif
   }

# if defined (XCURSES)
   XCursesInstructAndWait(CURSES_REFRESH);
# endif

   if (SP->cursrow != curscr->_cury
   ||  SP->curscol != curscr->_curx)
   {
      PDC_gotoxy(curscr->_cury, curscr->_curx);
      SP->cursrow = curscr->_cury;
      SP->curscol = curscr->_curx;
   }

   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   redrawwin(WINDOW *win)
#else
int   PDC_CDECL   redrawwin(win)
WINDOW *win;
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("redrawwin() - called: win=%x\n",win);
#endif

   if (win == (WINDOW *)NULL)
      return( ERR );

   return(wredrawln(win,0,win->_maxy));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int   PDC_CDECL   wredrawln(WINDOW *win, int start, int num)
#else
int   PDC_CDECL   wredrawln(win,start,num)
WINDOW *win;
int start;
int num;
#endif
/***********************************************************************/
{
   register int i;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("wredrawln() - called: win=%x start=%d num=%d\n",win,start,num);
#endif

   if (win == (WINDOW *)NULL)
      return( ERR );

   if  (start > win->_maxy || start + num > win->_maxy)
      return( ERR );
   for(i=start;i<start+num;i++)
      {
      win->_firstch[i] = 0;
      win->_lastch[i] = win->_maxx - 1;
      }
   return( OK );
}
