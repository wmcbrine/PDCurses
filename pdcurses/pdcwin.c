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
# include <config.h>
#endif
#include <curses.h>

#ifdef HAVE_MEMORY_H
# include <memory.h>
#endif

#ifndef HAVE_MEMMOVE
# define memmove PDC_memmove
#endif

#ifdef PDCDEBUG
char *rcsid_PDCwin  = "$Id: pdcwin.c,v 1.2 2002/05/26 01:13:31 mark Exp $";
#endif

/*man-start*********************************************************************

  PDC_copy_win()  - Common routine for copywin(), overlay() and overwrite()
    functions.

  PDCurses Description:
    This function copies the region of the source window specified
    over the specified region of the destination window. All validation
    of limits are done by the calling function.

    Thanks to Andreas Otte (venn@uni-paderborn.de) for the code changes.

  PDCurses Errors:
    ERR is returned if either src or dst windows are NULL;

  Portability:
    PDCurses int   PDC_copy_win( WINDOW* src_w, WINDOW* dst_w
         int src_tr,int src_tc,int src_br,int src_bc,
         int dst_tr,int dst_tc,int dst_br,int dst_bc,bool overlay);

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_copy_win(WINDOW *src_w, WINDOW *dst_w,int src_tr,int src_tc,int 
   src_br,int src_bc,int dst_tr,int dst_tc,int dst_br,int dst_bc,bool overlay)
#else
int PDC_copy_win(src_w,dst_w,src_tr,src_tc,src_br,src_bc,dst_tr,dst_tc,dst_br,dst_bc,overlay)
WINDOW *src_w;
WINDOW *dst_w;
int src_tr;
int src_tc;
int src_br;
int src_bc;
int dst_tr;
int dst_tc;
int dst_br;
int dst_bc;
bool overlay;
#endif
/***********************************************************************/
{
   register int fc=0;
   register int lc=0;
   int*  minchng=0;
   int*  maxchng=0;
   chtype*  w1ptr=NULL;
   chtype*  w2ptr=NULL;
   int   col=0;
   int   line=0;
   int   xdiff = src_bc - src_tc;
   int   ydiff = src_br - src_tr;
   int   y1=0;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_copy_win() - called\n");
#endif

   if (src_w == (WINDOW *)NULL)  return( ERR );
   if (dst_w == (WINDOW *)NULL)  return( ERR );

   minchng = dst_w->_firstch;
   maxchng = dst_w->_lastch;


   for (y1 = 0; y1 < dst_tr; y1++)
   {
      minchng++;
      maxchng++;
   }

   for (line = 0; line < ydiff; line++)
   {

      w1ptr = src_w->_y[line+src_tr]+src_tc;
      w2ptr = dst_w->_y[line+dst_tr]+dst_tc;
      fc    = _NO_CHANGE;

      for (col = 0; col < xdiff; col++)
      {
         if ((*w1ptr) != (*w2ptr)
         &&  !((*w1ptr & A_CHARTEXT) == src_w->_blank && overlay))
         {
            *w2ptr = *w1ptr;
            if (fc == _NO_CHANGE)
            {
               fc = col+dst_tc;
            }
            lc = col+dst_tc;
         }
         w1ptr++;
         w2ptr++;
      }

      if (*minchng == _NO_CHANGE)
      {
         *minchng = fc;
         *maxchng = lc;
      }
      else  if (fc != _NO_CHANGE)
      {
         if (fc < *minchng)   *minchng = fc;
         if (lc > *maxchng)   *maxchng = lc;
      }
      minchng++;
      maxchng++;
   }
   return( OK );
}

/*man-start*********************************************************************

  PDC_makenew()   - Create a WINDOW* (sans line allocation)

  PDCurses Description:
    This is a private PDCurses routine.

    Allocates all data for a new WINDOW* except the actual lines
    themselves.

  PDCurses Return Value:
    This function returns a valid WINDOW* on success and NULL on error.

  PDCurses Errors:
    If PDC_makenew() is unable to allocate memory for the window
    structure, it will free all allocated memory and return
    a NULL pointer.

  Portability:
    PDCurses WINDOW* PDC_makenew( int num_lines, int num_columns,
                int begy, int begx );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
WINDOW* PDC_makenew(int num_lines, int num_columns, int begy, int begx)
#else
WINDOW* PDC_makenew(num_lines,num_columns,begy,begx)
int num_lines;
int num_columns;
int begy;
int begx;
#endif
/***********************************************************************/
{
#ifdef HAVE_PROTO
extern   void* (*mallc)( size_t );
extern   void* (*callc)( size_t, size_t );
extern   void  (*fre)( void* );
#else
extern   void* (*mallc)();
extern   void* (*callc)();
extern   void  (*fre)();
#endif

   short i=0;
   WINDOW *win=NULL;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_makenew() - called: lines %d cols %d begy %d begx %d\n",num_lines,num_columns,begy,begx);
#endif

   /*
   *  Use the standard runtime malloc/calloc package or use
   *  the user's emalloc/ecalloc package.
   *
   *  Allocate the window structure itself
   */
   if ((win = (*mallc)(sizeof(WINDOW))) == (WINDOW *)NULL)
   {
      return( win );
   }

   /*
   * allocate the line pointer array
   */
   if ((win->_y = (*callc)(num_lines, sizeof(chtype *))) == NULL)
   {
      (*fre)(win);
      return( (WINDOW *)NULL );
   }

   /*
   * allocate the minchng and maxchng arrays
   */
   if ((win->_firstch = (*callc)(num_lines, sizeof(int))) == NULL)
   {
      (*fre)(win->_y);
      (*fre)(win);
      return( (WINDOW *)NULL );
   }
   if ((win->_lastch = (*callc)(num_lines, sizeof(int))) == NULL)
   {
      (*fre)(win->_firstch);
      (*fre)(win->_y);
      (*fre)(win);
      return( (WINDOW *)NULL );
   }

   /*
   * initialize window variables
   */
   win->_curx = 0;
   win->_cury = 0;
   win->_maxy = num_lines;    /* real max screen size */
   win->_maxx = num_columns;  /* real max screen size */
   win->_pmaxy = num_lines;   /* real max window size */
   win->_pmaxx = num_columns; /* real max window size */
   win->_begy = begy;
   win->_begx = begx;
   win->_lastpy = 0;
   win->_lastpx = 0;
   win->_lastsy1 = 0;
   win->_lastsx1 = 0;
   win->_lastsy2 = LINES-1;
   win->_lastsx2 = COLS-1;
   win->_flags = 0;
   win->_attrs = 0;     /* No attributes */
   win->_bkgd = ' '; /* wrs 4/10/93 -- initialize background to blank */
   win->_tabsize = 8;
   win->_clear = (bool) ((num_lines == LINES) && (num_columns == COLS));
   win->_leaveit = FALSE;
   win->_scroll = FALSE;
   win->_nodelay = FALSE;
   win->_immed = FALSE;
   win->_sync = FALSE;
   win->_use_keypad = FALSE;
   win->_use_idl = FALSE;
   win->_use_idc = TRUE;
   win->_tmarg = 0;
   win->_bmarg = num_lines - 1;
   win->_title = NULL;
   win->_title_ofs = 1;
   win->_title_attr = win->_attrs;
   win->_blank = ' ';
   win->_parx = win->_pary = -1;
   win->_parent = NULL;
   /*
   * init to say window unchanged
   */
   for (i = 0; i < num_lines; i++)
   {
      win->_firstch[i] = 0;
      win->_lastch[i] = num_columns - 1;
   }

   /*
   * set flags for window properties
   */
   if ((begy + num_lines) == LINES)
   {
      win->_flags |= _ENDLINE;
      if ((begx == 0) &&
          (num_columns == COLS) &&
          (begy == 0))
      {
         win->_flags |= _FULLWIN;
      }
   }

   if (((begy + num_lines) == LINES) &&
       ((begx + num_columns) == COLS))
   {
      win->_flags |= _SCROLLWIN;
   }
   return( win );
}

/*man-start*********************************************************************

  PDC_sync()   - Cause window synchronisation to occur

  PDCurses Description:
    This is a private PDCurses routine, NOT to be called by an application.

    Handles wrefresh() and wsyncup() calls when a window is changed.

  PDCurses Return Value:
    This function returns void.

  PDCurses Errors:

  Portability:
    PDCurses WINDOW* PDC_sync( WINDOW *win );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
void PDC_sync(WINDOW *win)
#else
void PDC_sync(win)
WINDOW *win;
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_sync() - called:\n");
#endif

   if (win->_immed) 
      wrefresh(win);
   if (win->_sync) 
      wsyncup(win);
   return;
}

/*man-start*********************************************************************

  PDC_backchar()  - Visually erase character in window

  PDCurses Description:
    This is a private PDCurses function

    This routine will visually erase a character.  It is called by
    the PDCurses character I/O routines.

  PDCurses Return Value:
    This routine will return OK upon success and otherwise ERR will be
    returned.

  PDCurses Errors:
    It is an error to pass a NULL WINDOW pointer.

  Portability:
    PDCurses int   PDC_backchar( WINDOW* w, char* ch, int* len );

**man-end**********************************************************************/

#ifdef THESE_FUNCTIONS_ARENT_USED
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_backchar(WINDOW *w, char *ch, int *len)
#else
int PDC_backchar(w,ch,len)
WINDOW *w;
char *ch;
int *len;
#endif
/***********************************************************************/
{
   int   nbs = 0;
   int   x = w->_curx;
   int   ts = w->_tabsize;
   chtype   s = (w->_y[w->_cury][x - 1] & CHR_MSK);
   char* p = c_strbeg;
   bool  save_raw_out = SP->raw_out;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_backchar() - called\n");
#endif

   if (w == (WINDOW *)NULL)
      return( ERR );

   (*len)--;      /* Now we are zero relative */
   (*len)--;      /* Now we are looking at the previous
             * character */
   if( *len >= 0 ) 
   {
      nbs++;
      /*
       * Determine number of characters to erase...
       */
      if ((ch[*len] < ' ') || (s == 0x7f))   /* ctrl-char has size 2  */
      {
         nbs++;
         (*len)--;
      }

      if( *len >= 0 )
      {
         if (ch[*len] == '\t')   /* tabs are very special */
         {
            for (; p < ch; p++)
            {
               if (*p == '\t')
                  x = ((x / ts) + 1) * ts;
               else
               {
                  if ((*p < ' ') || (*p == 0x7f))
                     x += 2;
                  else
                     x++;
               }
               if (x >= w->_maxx)   /* go to next line? */
                  x = 0;
            }
            if (!(w->_curx))
               nbs = w->_maxx - x;
            else
               nbs = w->_curx - x;
         }
      }
   }
   if( *len < 0 )
   {
      beep();
      *len = 0;
   }
   /*
    * Erase the characters and update...
    */
   SP->raw_out = FALSE;  /* ensure backspace handled in xlat mode */
   while ( nbs-- > 0 )
   {
      if (w->_curx > 0)
      {
/*       waddstr(w, "\b \b");*/
         mvwaddch(w, w->_cury, w->_curx-1, ' ');
         wmove(w, w->_cury, w->_curx-1);
      }
      else
      if (w->_cury)
      {
         mvwaddch(w, w->_cury - 1, w->_maxx - 1, ' ');
         wmove(w, w->_cury - 1, w->_maxx - 1);
      }
   }
   ch[*len] = '\0';
   SP->raw_out = save_raw_out;
   wrefresh(w);
   return( OK );
}

/*man-start*********************************************************************

  PDC_chg_attr_pair()   - Writes character and attribute to physical screen

  PDCurses Description:
    This is a private PDCurses function.

    Writes a single character 'chr' with attribute 'attr' to the
    current cursor location.

    NOTE: Though passed as 16 bit quantities, only the lower 8 bits
      will be used to create a character/attribute pair.

  PDCurses Return Value:
    This function returns OK on success and ERR on error.

  PDCurses Errors:
    No errors are defined for this function under DOS.

    An ERR may be returned under FLEXOS if s_copy() fails.  See the
    Flexos Programmer's Reference Manual for details on the error.

  Portability:
    PDCurses int PDC_chg_attr_pair( chtype chr, chtype attr );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_chg_attr_pair(chtype chr, chtype attr)
#else
int PDC_chg_attr_pair(chr,attr)
chtype chr;
chtype attr;
#endif
/***********************************************************************/
{
   extern unsigned   char atrtab[MAX_ATRTAB];
   int   phys_attr=chtype_attr(attr);

#ifdef   OS2
# ifdef  EMXVIDEO
   int curCol, curRow,cell;
# else
   USHORT curCol, curRow, cell;
# endif
#endif

#ifdef   FLEXOS
   UBYTE c = (UBYTE) chr;
   UBYTE a = (UBYTE) phys_attr;
#endif

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_chg_attr_pair() - called\n");
#endif

#ifdef   FLEXOS
   drect.r_row = PDC_get_cur_row();
   drect.r_col = PDC_get_cur_col();
   drect.r_nrow = 1;
   drect.r_ncol = 1;

   sframe.fr_pl[0] = (UBYTE *) & c;
   sframe.fr_pl[1] = (UBYTE *) & a;
   sframe.fr_pl[2] = (UBYTE *) " ";
   sframe.fr_nrow = 1;
   sframe.fr_ncol = 1;
   sframe.fr_use = 0x00;

   srect.r_col = 0;
   srect.r_row = 0;
   srect.r_nrow = 1;
   srect.r_ncol = 1;

   retcode = s_copy(0x03, 0x01L, 0L, (far unsigned short *) &drect, (far unsigned short *) &sframe, (far unsigned short *) &srect);
   return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef   DOS
   regs.h.ah = 0x09;
   regs.h.al = chr & A_CHARTEXT;
   regs.h.bh = SP->video_page;
   regs.h.bl = (char)(phys_attr >> 8);
# ifdef WATCOMC
   regs.w.cx = 0x01;
# else
   regs.x.cx = 0x01;
# endif
   int86(0x10, &regs, &regs);
   return( OK );
#endif

#ifdef   OS2
   /* find the current cursor position */
# ifdef EMXVIDEO
   cell = (int)((chr & A_CHARTEXT) | phys_attr);
   v_getxy (&curCol, &curRow);
   v_putline ((char*)&cell, curCol, curRow, 1);
# else
   cell = (USHORT)((chr & A_CHARTEXT) | phys_attr);
   VioGetCurPos((PUSHORT) &curRow, (PUSHORT) &curCol, 0);
   VioWrtNCell((PBYTE)&cell,1,curRow,curCol,0);
# endif
   return( OK );
#endif

#ifdef UNIX
/* INCOMPLETE - check attribute and output attr and/or color */
   putchar(chr & A_CHARTEXT);
   return(OK);
#endif
}
#endif

/*man-start*********************************************************************

  PDC_chadd()      - Low level; Put a character to a window

  PDCurses Description:
    This is a private PDCurses function.

    This routine will insert the character 'c' at the current cursor
    position in the passed window.

    If 'xlat' is TRUE, PDC_chadd() will handle things in a cooked
    manner (tabs, newlines, carriage returns, etc).  If 'xlat' is
    FALSE, the characters are simply output directly.

    If 'advance' is TRUE, PDC_chadd() will move the current cusor position
    appropriately. The *addch functions call PDC_chadd() with advance TRUE,
    while the *insch functions call PDC_chadd() with advance FALSE.

    The normal curses routines (non-raw-output-mode) call PDC_chadd()
    with 'xlat' TRUE.

    If an alternate character is to be displayed, the character is
    displayed without translation (minus the A_ALTCHARSET of course).

  PDCurses Return Value:
    This function returns OK on success and ERR on error.

  PDCurses Errors:
    It is an error to call this function with a NULL window pointer.

  Portability:
    PDCurses        int PDC_chadd( WINDOW* win, chtype ch, bool xlat, bool advance );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL PDC_chadd(register WINDOW *win, chtype ch,bool xlat, bool advance)
#else
int PDC_CDECL PDC_chadd(win,ch,xlat,advance)
register WINDOW *win;
chtype ch;
bool xlat;
bool advance;
#endif
/***********************************************************************/
{
   int   retval = ERR;
   int   x=0;
   int   y=0;
   int   newx=0;
   chtype   attr=0, bktmp=0;
   int   ts=0;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_chadd() - called: win=%x ch=%x (char=%c attr=0x%x) xlat=%d advance=%d\n",win,ch,ch & A_CHARTEXT,ch & A_ATTRIBUTES,xlat,advance);
#endif

   if (win  == (WINDOW *)NULL)
      return(  retval );

   x  = win->_curx;
   y  = win->_cury;
   ts = win->_tabsize;

   /* 
    * Remove any A_ALTCHASET attribute from the ch before any further
    * testing. If the character has A_ALTCHARSET, set xlat to FALSE.
    */
   if (ch & A_ALTCHARSET)
   {
      xlat = FALSE;
      ch = ch & (~A_ALTCHARSET);
   }
   /*
    * if the incoming character doesn't have its own attribute
    * then  use the  current  attributes for the window.
    * if the incoming character has attributes but not a colour
    * component, or the attributes  to the current attributes
    * for the window.
    * if the incoming character has a colour component use  the
    * attributes solely from the incoming character
    */

   if ((ch  & A_ATTRIBUTES)   == 0)
      attr  = win->_attrs;
   else
   {
      if ((ch & A_COLOR) == 0)
         attr = (ch & A_ATTRIBUTES) | win->_attrs;
      else
         attr = (ch & A_ATTRIBUTES);
   }

   ch = (ch &  A_CHARTEXT);

   /*  wrs (4/10/93) */
   /*  apply the same sort of logic for the window background, in that it only */
   /*  takes precedence if other color attributes are not there and that */
   /*  the background character will only print if the printing character is */
   /*  blank. */

   if ((attr & A_COLOR) == 0)
      attr = (attr | (win->_bkgd & A_ATTRIBUTES));
   else 
   {
      bktmp = (win->_bkgd & A_COLOR);
      attr = (attr | ( (win->_bkgd & A_ATTRIBUTES) ^ bktmp ));
   }

   if ( ch == ' ' )
      ch = ((win->_bkgd & A_CHARTEXT));


   if ((y > win->_maxy) ||
       (x > win->_maxx) ||
       (y < 0) ||
       (x < 0))
   {
      return(  retval );
   }

   if (xlat)
   {
      switch (ch) 
      {
         case '\t':
            for (newx = ((x   / ts) +  1) * ts; x < newx; x++)
            {
               if (waddch(win,   ' ') ==  ERR)
               {
                  PDC_sync(win);
                  return(  retval );
               }
               /*
                * if tab to next line
                */
               if (win->_curx == 0)
               {
                  /*
                   * exit  the loop
                   */
                  PDC_sync(win);
                  return(  OK );
               }
            }
            PDC_sync(win);
            return(  OK );

         case '\n':
            if (SP->autocr && !(SP->raw_out))
            {
               /*
                * if lf -> crlf
                */
               x = 0;
            }
            wclrtoeol( win );
            if ((y = PDC_newline(win, y)) < 0)
            {
               PDC_sync(win);
               return(  retval );
            }
            if (advance)
            {
               win->_cury =   y;
               win->_curx =   x;
            }
            PDC_sync(win);
            return(  OK );

         case '\r':
            if (advance)
               win->_curx =   x = 0;
            PDC_sync(win);
            return(  OK );

         case '\b':
            if (--x  < 0)
            {
               /*
                * no back over   left margin
                */
               x = 0;
            }
            if (advance)
               win->_curx =   x;
            PDC_sync(win);
            return(  OK );

         case 0x7f:
            if (waddch(win,   '^') ==  ERR)
            {
               PDC_sync(win);
               return(  retval );
            }
            retval = waddch(win, '?');
            PDC_sync(win);
            return(  retval );

         default:
            break;
      }     /* switch */

      if (ch < ' ')
      {
         /*
          * handle control chars
          */
         if (waddch(win,   '^') ==  ERR)
         {
            PDC_sync(win);
            return(  retval );
         }

         retval = (waddch(win, ch + '@'));
         PDC_sync(win);
         return(  retval );
      }
   }

   /*
    * Add the  attribute back into the character.
    */
   ch |= attr;
   /*********************************************************************/
   /* only change _firstch/_lastch if character to be added is different */
   /* to the character/attribute that is already in that position in the */
   /* window.                                                            */
   /* Removing this fixes display problems with different windows in the */
   /* same physical position. MH 20-03-93                                */
   /* Restored again.         MH 02-04-93                                */
   /*********************************************************************/
   if (win->_y[y][x] != ch)
   {
      /*
       * only  if data  change
       */
      if (win->_firstch[y] == _NO_CHANGE)
      {
         win->_firstch[y] = win->_lastch[y] = x;
      }
      else
      {
         if (x <  win->_firstch[y])
         {
            win->_firstch[y] = x;
         }
         else
         {
            if (x >  win->_lastch[y])
            {
               win->_lastch[y] = x;
            }
         }
      }
   }
   win->_y[y][x++] = ch;
   if (x >= win->_maxx)
   {
      /*
       * wrap  around test
       */
      x = 0;
      if ((y = PDC_newline(win, y)) < 0)
      {
         PDC_sync(win);
         return(  retval );
      }
   }
   if (advance)
   {
      win->_curx =   x;
      win->_cury =   y;
   }
   PDC_sync(win);
   return(  OK );
}

/*man-start*********************************************************************

  PDC_chg_attrs() - Change attributes in a rectangle

  PDCurses Description:
    This routine will change the attribute(s) from a starting (y,x)
    position to an ending (y,x) position to the specified attribute.

  PDCurses Return Value:
    This function returns OK on success and ERR on error.

  PDCurses Errors:
    It is an error to call this function with a NULL window pointer.
    It is also an error to pass rectangular coordinates that lay
    outside of window.

  Portability:
    PDCurses int PDC_chg_attrs( WINDOW* w, chtype attr,
               int sy, int sx,
               int ey, int ex );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_chg_attrs(WINDOW *win, chtype attr, int sy, int sx, int ey, int ex)
#else
int PDC_chg_attrs(win,attr,sy,sx,ey,ex)
WINDOW *win;
chtype attr;
int sy;
int sx;
int ey;
int ex;
#endif
/***********************************************************************/
{
   chtype   oldattr = win->_attrs;
   int   c=0;
   int   l=0;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_chr_attrs() - called\n");
#endif

   if (win == (WINDOW *)NULL)    return( ERR );
   if (sy > win->_maxy) return( ERR );
   if (sx > win->_maxx) return( ERR );
   if (ey >= win->_maxy)   ey = win->_maxy - 1;
   if (ex >= win->_maxx)   ex = win->_maxx - 1;

   wattrset(win, attr);
   for (l = sy; l <= ey; l++)
   {
      for (c = sx; c <= ex; c++)
         win->_y[l][c] = (win->_y[l][c] & A_CHARTEXT) | attr;

      if (win->_firstch[l] == _NO_CHANGE)
      {
         win->_firstch[l] = sx;
         win->_lastch[l] = ex;
      }
      else if (win->_firstch[l] != _NO_CHANGE)
      {
         if (sx < win->_firstch[l])
            win->_firstch[l] = sx;
         if (ex > win->_lastch[l])
            win->_lastch[l] = ex;
      }
   }
   win->_attrs = oldattr;
   PDC_sync(win);
   return( OK );
}

/*man-start*********************************************************************

  PDC_chins()  - Low-level insert character in window

  PDCurses Description:
    This is a private PDCurses routine.

    This routine provides the basic functionality for the X/Open
    [mv][w]insch() routines.  The xlat flag indicates that normal
    character translation is performed or not.  If not, then the
    character is output as is.

    The 'xlat' flag is TRUE for the normal curses routines.

  PDCurses Return Value:
    This function returns OK on success and ERR on error.

  PDCurses Errors:
    It is an error to call this function with a NULL window pointer.

  Portability:
    PDCurses int PDC_chins( WINDOW* win, chtype c, bool xlat );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_chins(WINDOW *win, chtype c, bool xlat)
#else
int PDC_chins(win,c,xlat)
WINDOW *win;
chtype c;
bool xlat;
#endif
/***********************************************************************/
{
   int   retval = ERR;
   int   x=0;
   int   y=0;
   int   maxx=0;
   int   offset=0;
   chtype *temp1;
   char  ch = (c & A_CHARTEXT);

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_chins() - called: win=%x ch=%x (char=%c attr=0x%x) xlat=%d\n",win,ch,ch & A_CHARTEXT,ch & A_ATTRIBUTES,xlat);
#endif

   if (win == (WINDOW *)NULL)
      return( retval );

   x  = win->_curx;
   y  = win->_cury;
   maxx  = win->_maxx;
   offset   = 1;
   temp1 = &win->_y[y][x];

   if ((ch < ' ') && xlat)
   {
      offset++;
   }

   memmove( temp1+offset, temp1, (maxx - x -offset) * sizeof(chtype) );

   win->_lastch[y] = maxx-1;

   if ((win->_firstch[y] == _NO_CHANGE) ||
       (win->_firstch[y] > x))
   {
      win->_firstch[y] = x;
   }
   /*
    * PDC_chadd() fixes CTRL-chars too
    */
   retval = (PDC_chadd(win, c, xlat,FALSE));
   return( retval );
}

/*man-start*********************************************************************

  PDC_clr_scrn()  - Clears the physical screen and homes the cursor.

  PDCurses Description:
    This is an internal routine called by the doupdate() routines.

  PDCurses Return Value:
    This routine always returns OK.

  Portability:
    PDCurses int PDC_clr_scrn( WINDOW* win );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_clr_scrn(WINDOW *win)
#else
int PDC_clr_scrn(win)
WINDOW *win;
#endif
/***********************************************************************/
{
#if !defined(XCURSES)
# if defined(UNIX_WCLR)
   chtype   attrs = win->_attrs;
# else
   chtype   attrs = win->_bkgd;
# endif
#endif

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_clr_scrn() - called\n");
#endif

#if defined(XCURSES)
   XCursesInstruct(CURSES_CLEAR);
#else
   PDC_scroll(0, 0, LINES - 1, COLS - 1, 0, attrs);
   PDC_gotoxy(0, 0);
#endif

   return( OK );
}

/*man-start*********************************************************************

  PDC_newline()   - Advances 1 newline from supplied line number.

  PDCurses Description:
    This is a private PDCurses routine.

    Does line advance and returns the new cursor line.  If error,
    return -1.

  PDCurses Return Value:
    This function returns OK on success and ERR on error.

  PDCurses Errors:
    No errors are defined for this function.

  Portability:
    PDCurses int PDC_newline( WINDOW* win, int lin );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_newline(WINDOW *win, int lin)
#else
int PDC_newline(win,lin)
WINDOW *win;
int lin;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_newline() - called: line %d\n",lin);
#endif

   if (win == (WINDOW *)NULL)
      return( -1 );

   if (++lin > win->_bmarg)
   {
      lin--;
      if (win->_scroll)
      {
         scroll(win);

         /* wrs -- 7/11/93 ** it seems that System V Curses automatically refreshes
          *                   a window when scrolling occurs via a newline.  This
          *                   could be a feature that isn't intended, but I'll
          *                   implement it here as well for consistency.
          * fh -- 7/30/99 ** but it causes problems with panels -- a lower panel
          *                  might be refreshed without updating the obscuring
          *                  parts of other panels.
          */
         /*         wrefresh(win); */
      }
      else
         return( -1 );
   }
   return( lin );
}
