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
#define CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

#include <string.h>

#ifdef HAVE_MEMORY_H
# include <memory.h>
#endif

/* undefine any macros for functions defined in this module */
#undef newpad
#undef subpad
#undef prefresh
#undef pnoutrefresh
#undef pechochar

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef doupdate
#endif

#ifdef PDCDEBUG
char *rcsid_pad  = "$Id: pad.c,v 1.3 2002/05/26 01:12:45 mark Exp $";
#endif

/*
 * save values for pechochar()
 */
static int save_pminrow;
static int save_pmincol;
static int save_sminrow;
static int save_smincol;
static int save_smaxrow;
static int save_smaxcol;

/*man-start*********************************************************************

  Name:                                                           pad

  Synopsis:
    WINDOW* newpad( int nlines, int ncols );
    WINDOW* subpad(WINDOW* orig,int nlines,int ncols,int begin_y,int begin_x);
    int prefresh(WINDOW* win,int py,int px,int sy1,int sx1,int sy2,int sx2);
    int pnoutrefresh(WINDOW* w,int py,int px,int sy1,int sx1,int sy2,int sx2);
    int pechochar(WINDOW *pad, chtype ch);

  X/Open Description:
    newpad() creates a new pad data structure.  A pad is a special case
    of a window, which is not restricted by the screen size, and is not
    necessarily associated with a particular part of the screen.  A
    pad can be used when a large window is needed, and only a part
    of the window will be on the screen at one tme.  Automatic
    refreshes of pads (e.g., from scrolling or echoing of input) do
    not occur.  It is not legal to call refresh() with a pad as an
    argument; the routines prefresh() or pnoutrefresh() should be
    called instead.  Note that these routines require additional
    parameters to specify the part of the pad to be displayed and
    the location on the screen to be used for display.

    The subpad() routine creates a new sub-pad within a pad.  The
    dimensions of the sub-pad are nlines lines and ncols
    columns.  The sub-pad is at position (begin_y, begin_x) in the
    the parent pad.  This position is relative to the pad, and not
    to the screen like with subwin.
    The sub-pad is made in the middle of the pad orig, so
    that changes made to either pad will affect both.  When
    using this routine, it will often be necessary to call
    touchwin before calling prefresh.

    The prefresh() routine copies the specified pad to the physical
    terminal screen.  It takes account of what is already
    displayed on the screen to optimize cursor movement.
    The pnoutrefresh() routine copies the named pad to the virtual
    screen. It then compares the virtual screen with the physical
    screen and performs the actual update.
    These routines are analogous to the routines wrefresh() and
    wnoutrefresh() except that pads, instead of windows, are
    involved.  Additional parameters are also needed to indicate
    what part of the pad and screen are involved. The upper left
    corner of the part of the pad to be displayed is specified by
    py and px.  The coordinates sy1, sx1, sy2, and sx2 specify the
    edges of the screen rectangle that will contain the selected
    part of the pad.
    The lower right corner of the pad rectangle to be displayed is
    calculated from the screen co-ordinates.  This ensures that
    the screen rectangle and the pad rectangle are the same size.
    Both rectangles must be entirely contained within their
    respective structures.

    The pechochar() is functionally equivalent to adch() followed
    by a call to refresh().

  PDCurses Description:
    PDCurses (as a library) provides the developer with the ability to
    hook in their own malloc debugging  package.  See the details in
    INITSCR.C for details on how to accomplish this.

  X/Open Return Value:
    All functions return OK on success and ERR on error.

  X/Open Errors:
    No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      newpad                                Y        -       Y
      subpad                                Y        -       Y
      prefresh                              Y        -       Y
      pnoutrefresh                          Y        -       Y
      pechochar                             -        -      3.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
WINDOW* PDC_CDECL newpad( int nlines, int ncols )
#else
WINDOW* PDC_CDECL newpad(nlines,ncols)
int nlines;
int ncols;
#endif
/***********************************************************************/
{
#ifdef HAVE_PROTO
   extern void* (*callc)( size_t, size_t );
   extern void  (*fre)( void* );
#else
   extern void* (*callc)();
   extern void  (*fre)();
#endif

   WINDOW* win;
   chtype* ptr;
   int   i;
   int   j;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("newpad() - called: lines=%d cols=%d\n",nlines,ncols);
#endif

   if ((win = PDC_makenew( nlines, ncols, -1, -1 )) == (WINDOW *)NULL)
      return( (WINDOW *)NULL );

   for (i = 0; i < nlines; i++)
   {
      /*
       * make and clear the lines
       */
      if ((win->_y[i] = (*callc)(ncols, sizeof(chtype))) == NULL)
      {
         for (j = 0; j < i; j++)
         {
            /*
             * if error, free all the data
             */
            (*fre)(win->_y[j]);
         }
         (*fre)(win->_firstch);
         (*fre)(win->_lastch);
         (*fre)(win->_y);
         (*fre)(win);
         return( (WINDOW *)NULL );
      }
      else
      {
         for (ptr = win->_y[i];
              ptr < win->_y[i] + ncols;)
         {
            /*
            * Retain the original screen attributes...
            */

            *ptr++ = SP->blank;  
         }
      }
   }
   win->_flags = _PAD;
   /*
    * save default values in case pechochar() is the first call
    * to prefresh().
    */
   save_pminrow=0;
   save_pmincol=0;
   save_sminrow=0;
   save_smincol=0;
   save_smaxrow=min(LINES,nlines)-1;
   save_smaxcol=min(COLS,ncols)-1;
   return( win );
}
/***********************************************************************/
#ifdef HAVE_PROTO
WINDOW* PDC_CDECL subpad(WINDOW* orig,int nlines,int ncols,int begin_y,int begin_x)
#else
WINDOW* PDC_CDECL subpad(orig,nlines,ncols,begin_y,begin_x)
WINDOW* orig;
int nlines;
int ncols;
int begin_y;
int begin_x;
#endif
/***********************************************************************/
{
   WINDOW*  win;
   int   i;
   int   j = begin_y - orig->_begy;
   int   k = begin_x - orig->_begx;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("subpad() - called: lines=%d cols=%d begy=%d begx=%d\n",nlines,ncols,begin_y,begin_x);
#endif

   if (!orig)
      return( (WINDOW *)NULL );

   if (!(orig->_flags & _PAD))
      return( (WINDOW *)NULL );

   /*
    * make sure window fits inside the original one
    */
   if ((begin_y < orig->_begy) ||
       (begin_x < orig->_begx) ||
       (begin_y + nlines) > (orig->_begy + orig->_maxy) ||
       (begin_x + ncols)  > (orig->_begx + orig->_maxx))
   {
      return( (WINDOW *)NULL );
   }
   if (!nlines) 
      nlines = orig->_maxy - 1 - j;
   if (!ncols) 
      ncols = orig->_maxx - 1 - k;
   if ((win = PDC_makenew(nlines, ncols, begin_y, begin_x)) == (WINDOW *) NULL)
   {
      return( (WINDOW *)NULL );
   }

   /*
    * initialize window variables
    */
   win->_attrs = orig->_attrs;
   win->_leaveit  = orig->_leaveit;
   win->_scroll   = orig->_scroll;
   win->_nodelay  = orig->_nodelay;
   win->_use_keypad  = orig->_use_keypad;
   win->_parent   = orig;

   for (i = 0; i < nlines; i++)
   {
      win->_y[i] = (orig->_y[j++]) + k;
   }

   win->_flags = _SUBPAD;
   /*
    * save default values in case pechochar() is the first call
    * to prefresh().
    */
   save_pminrow=0;
   save_pmincol=0;
   save_sminrow=0;
   save_smincol=0;
   save_smaxrow=min(LINES,nlines)-1;
   save_smaxcol=min(COLS,ncols)-1;
   return (win);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL prefresh(WINDOW* win,int py,int px,int sy1,int sx1,int sy2,int sx2)
#else
int PDC_CDECL prefresh(win,py,px,sy1,sx1,sy2,sx2)
WINDOW* win;
int py;
int px;
int sy1;
int sx1;
int sy2;
int sx2;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("prefresh() - called\n");
#endif

   if (win == (WINDOW *)NULL)
      return( ERR );

   if (pnoutrefresh(win, py, px, sy1, sx1, sy2, sx2) == ERR)
      return( ERR );

   doupdate();
   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL pnoutrefresh(WINDOW* w,int py,int px,int sy1,int sx1,int sy2,int sx2)
#else
int PDC_CDECL pnoutrefresh(w,py,px,sy1,sx1,sy2,sx2)
WINDOW* w;
int py;
int px;
int sy1;
int sx1;
int sy2;
int sx2;
#endif
/***********************************************************************/
{
   WINDOW*     s = curscr;
   int      sline = sy1;
   int      pline = py;
   int      num_cols = min((sx2-sx1+1),(w->_maxx-px));

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("pnoutrefresh() - called\n");
#endif


   if (w == (WINDOW *)NULL)
      return( ERR );

   if (sy2 < sy1 || sx2 < sx1) return(ERR);

   if (!(w->_flags == _PAD)
   &&  !(w->_flags == _SUBPAD))
      return( ERR );

   while (sline <= sy2)
   {
      if (pline < w->_maxy)
      {
         memcpy(&(s->_y[sline][sx1]), &(w->_y[pline][px]), (num_cols) * sizeof(chtype));

         if ((s->_firstch[sline] == _NO_CHANGE) 
         ||  (s->_firstch[sline] > sx1))
         {
            s->_firstch[sline] = sx1;
         }

         if (sx2 > s->_lastch[sline])
            s->_lastch[sline] = sx2;

         w->_firstch[pline] = _NO_CHANGE;  /* updated now */
         w->_lastch[pline] = _NO_CHANGE;  /* updated now */
      }
      sline++;
      pline++;
   }

   w->_lastpy = py;
   w->_lastpx = px;
   w->_lastsy1 = sy1;
   w->_lastsx1 = sx1;
   w->_lastsy2 = sy2;
   w->_lastsx2 = sx2;

   if (w->_clear)
   {
      w->_clear = FALSE;
      s->_clear = TRUE;
   }

   /* position the cursor to the pad's current position if possible */
   if (!w->_leaveit)
   {
      /* is the pad current position going to end up displayed ? if not
       * then don't move the cursor, if so move it to the correct place 
       */
      if (w->_cury >= py
      &&  w->_curx >= px
      &&  w->_cury <= py + (sy2 - sy1+1)
      &&  w->_curx <= px + (sx2 - sx1+1))
      {
         s->_cury = (w->_cury - py) + sy1;
         s->_curx = (w->_curx - px) + sx1;
      }
   }
   return( OK );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL pechochar(WINDOW *pad, chtype ch)
#else
int PDC_CDECL pechochar(pad,ch)
WINDOW *pad;
chtype ch;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("pechochar() - called\n");
#endif

   if ( PDC_chadd( pad, ch, !(SP->raw_out), TRUE ) == ERR )
      return(ERR);
   return( prefresh( pad,
                     save_pminrow,
                     save_pmincol,
                     save_sminrow,
                     save_smincol,
                     save_smaxrow,
                     save_smaxcol ) );
}
