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
#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

#include <string.h>

#ifdef HAVE_MEMORY_H
#  include <memory.h>
#endif

#ifdef UNIX
#  include <defs.h>
#  include <term.h>
#endif

#ifndef HAVE_MEMMOVE
#  define memmove PDC_memmove
#endif

#ifdef PDCDEBUG
char *rcsid_PDCdisp  = "$Id: pdcdisp.c,v 1.1 2001/01/10 08:31:06 mark Exp $";
#endif

/*man-start*********************************************************************

  PDC_backchar()	- Visually erase character in window

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
 	PDCurses	int	PDC_backchar( WINDOW* w, char* ch, int* len );

**man-end**********************************************************************/

#ifdef THESE_FUNCTIONS_ARENT_USED
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_backchar(WINDOW *w, char *ch, int *len)
#else
int	PDC_backchar(w,ch,len)
WINDOW *w;
char *ch;
int *len;
#endif
/***********************************************************************/
{
	int	nbs = 0;
	int	x = w->_curx;
	int	ts = w->_tabsize;
	chtype	s = (w->_y[w->_cury][x - 1] & CHR_MSK);
	char*	p = c_strbeg;
	bool	save_raw_out = SP->raw_out;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_backchar() - called\n");
#endif

	if (w == (WINDOW *)NULL)
		return( ERR );

	(*len)--;		/* Now we are zero relative */
	(*len)--;		/* Now we are looking at the previous
				 * character */
	if( *len >= 0 ) {
		nbs++;
		/*
		 * Determine number of characters to erase...
		 */
		if ((ch[*len] < ' ') || (s == 0x7f))	/* ctrl-char has size 2	 */
		{
			nbs++;
			(*len)--;
		}

		if( *len >= 0 )
		if (ch[*len] == '\t')	/* tabs are very special */
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
				if (x >= w->_maxx)	/* go to next line? */
					x = 0;
			}
			if (!(w->_curx))
				nbs = w->_maxx - x;
			else
				nbs = w->_curx - x;
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
/*			waddstr(w, "\b \b");*/
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

  PDC_chg_attr_pair()	- Writes character and attribute to physical screen

  PDCurses Description:
 	This is a private PDCurses function.

 	Writes a single character 'chr' with attribute 'attr' to the
 	current cursor location.

 	NOTE:	Though passed as 16 bit quantities, only the lower 8 bits
 		will be used to create a character/attribute pair.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function under DOS.

 	An ERR may be returned under FLEXOS if s_copy() fails.  See the
 	Flexos Programmer's Reference Manual for details on the error.

  Portability:
 	PDCurses	int PDC_chg_attr_pair( chtype chr, chtype attr );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_chg_attr_pair(chtype chr, chtype attr)
#else
int	PDC_chg_attr_pair(chr,attr)
chtype chr;
chtype attr;
#endif
/***********************************************************************/
{
	extern unsigned	char atrtab[MAX_ATRTAB];
	int	phys_attr=chtype_attr(attr);

#ifdef	OS2
# ifdef  EMXVIDEO
	int curCol, curRow,cell;
# else
	USHORT curCol, curRow, cell;
# endif
#endif

#ifdef	FLEXOS
	UBYTE	c = (UBYTE) chr;
	UBYTE	a = (UBYTE) phys_attr;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_chg_attr_pair() - called\n");
#endif

#ifdef	FLEXOS
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

#ifdef	DOS
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

#ifdef	OS2
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
int	PDC_chadd(register WINDOW *win, chtype ch,bool xlat, bool advance)
#else
int	PDC_chadd(win,ch,xlat,advance)
register WINDOW *win;
chtype ch;
bool xlat;
bool advance;
#endif
/***********************************************************************/
{
	int	retval = ERR;
	int	x=0;
	int	y=0;
	int	newx=0;
	chtype	attr=0, bktmp=0;
	int	ts=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_chadd() - called: char=%c attr=0x%x xlat=%d advance=%d\n",ch & A_CHARTEXT,ch & A_ATTRIBUTES,xlat,advance);
#endif

	if (win	== (WINDOW *)NULL)
		return(	retval );

	x	= win->_curx;
	y	= win->_cury;
	ts	= win->_tabsize;

/* 
   Remove any A_ALTCHASET attribute from the ch before any further
   testing. If the character has A_ALTCHARSET, set xlat to FALSE.
 */
	if (ch & A_ALTCHARSET)
		{
		xlat = FALSE;
		ch = ch & (~A_ALTCHARSET);
		}
/*
   if the incoming character doesn't have its own attribute
   then	use the	current	attributes for the window.
   if the incoming character has attributes but	not a colour
   component, or the attributes	to the current attributes
   for the window.
   if the incoming character has a colour component use	the
   attributes solely from the incoming character 
 */

	if ((ch	& A_ATTRIBUTES)	== 0)
	   attr	= win->_attrs;
	else
	   if ((ch & A_COLOR) == 0)
	      attr = (ch & A_ATTRIBUTES) | win->_attrs;
	   else
	      attr = (ch & A_ATTRIBUTES);

	ch	= (ch &	A_CHARTEXT);

/*  wrs (4/10/93) */
/*  apply the same sort of logic for the window background, in that it only */
/*  takes precedence if other color attributes are not there and that */
/*  the background character will only print if the printing character is */
/*  blank. */

	if ((attr & A_COLOR) == 0)
	   attr = (attr | (win->_bkgd & A_ATTRIBUTES));
	else {
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
		return(	retval );
	}

	if (xlat)
	{
		switch (ch) {
		case '\t':
			for (newx = ((x	/ ts) +	1) * ts; x < newx; x++)
			{
				if (waddch(win,	' ') ==	ERR)
				{
					return(	retval );
				}
				/*
				 * if tab to next line
				 */
				if (win->_curx == 0)
				{
					/*
					 * exit	the loop
					 */
					return(	OK );
				}
			}
			return(	OK );

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
				return(	retval );
			if (advance)
			  {
			   win->_cury =	y;
			   win->_curx =	x;
			  }
			return(	OK );

		case '\r':
			if (advance)
			   win->_curx =	x = 0;
			return(	OK );

		case '\b':
			if (--x	< 0)
			{
				/*
				 * no back over	left margin
				 */
				x = 0;
			}
			if (advance)
			   win->_curx =	x;
			return(	OK );

		case 0x7f:
			if (waddch(win,	'^') ==	ERR)
			{
				return(	retval );
			}
			retval = waddch(win, '?');
			return(	retval );

		default:
			break;
		}		/* switch */

		if (ch < ' ')
		{
			/*
			 * handle control chars
			 */
			if (waddch(win,	'^') ==	ERR)
				return(	retval );

			retval = (waddch(win, ch + '@'));
			return(	retval );
		}
	}

	/*
	 *	Add the	attribute back into the	character.
	 */
	ch	|= attr;
/*********************************************************************/
/* only change _firstch/_lastch if character to be added is different */
/* to the character/attribute that is already in that position in the */
/* window.                                                            */
/* Removing this fixes display problems with different windows in the */
/* same physical position. MH 20-03-93                                */
/* Restored again.         MH 02-04-93                                */
/*********************************************************************/
	if (win->_y[y][x] !=	ch)
	{
		/*
		 * only	if data	change
		 */
		if (win->_firstch[y] ==	_NO_CHANGE)
		{
			win->_firstch[y] = win->_lastch[y] = x;
		}
		else
		{
			if (x <	win->_firstch[y])
			{
				win->_firstch[y] = x;
			}
			else
			{
				if (x >	win->_lastch[y])
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
		 * wrap	around test
		 */
		x = 0;
		if ((y = PDC_newline(win, y)) < 0)
			return(	retval );
	}
	if (advance)
	  {
	   win->_curx =	x;
	   win->_cury =	y;
	  }
	return(	OK );
}

/*man-start*********************************************************************

  PDC_chg_attrs()	- Change attributes in a rectangle

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
 	PDCurses	int PDC_chg_attrs( WINDOW* w, chtype attr,
 					int sy, int sx,
 					int ey, int ex );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_chg_attrs(WINDOW *w, chtype attr, int sy, int sx, int ey, int ex)
#else
int	PDC_chg_attrs(w,attr,sy,sx,ey,ex)
WINDOW *w;
chtype attr;
int sy;
int sx;
int ey;
int ex;
#endif
/***********************************************************************/
{
	chtype	oldattr = w->_attrs;
	int	c=0;
	int	l=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_chr_attrs() - called\n");
#endif

	if (w == (WINDOW *)NULL)		return( ERR );
	if (sy > w->_maxy)	return( ERR );
	if (sx > w->_maxx)	return( ERR );
	if (ey >= w->_maxy)	ey = w->_maxy - 1;
	if (ex >= w->_maxx)	ex = w->_maxx - 1;

	wattrset(w, attr);
	for (l = sy; l <= ey; l++)
	{
		for (c = sx; c <= ex; c++)
			w->_y[l][c] = (w->_y[l][c] & A_CHARTEXT) | attr;

		if (w->_firstch[l] == _NO_CHANGE)
		{
			w->_firstch[l] = sx;
			w->_lastch[l] = ex;
		}
		else
		if (w->_firstch[l] != _NO_CHANGE)
		{
			if (sx < w->_firstch[l])
				w->_firstch[l] = sx;
			if (ex > w->_lastch[l])
				w->_lastch[l] = ex;
		}
	}
	w->_attrs = oldattr;
	return( OK );
}

/*man-start*********************************************************************

  PDC_chins()	- Low-level insert character in window

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
 	PDCurses	int PDC_chins( WINDOW* win, chtype c, bool xlat );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_chins(WINDOW *win, chtype c, bool xlat)
#else
int	PDC_chins(win,c,xlat)
WINDOW *win;
chtype c;
bool xlat;
#endif
/***********************************************************************/
{
	int	retval = ERR;
	int	x=0;
	int	y=0;
	int	maxx=0;
	int	offset=0;
	chtype *temp1;
	char	ch	= (c & A_CHARTEXT);

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_chins() - called\n");
#endif

	if (win == (WINDOW *)NULL)
		return( retval );

	x	= win->_curx;
	y	= win->_cury;
	maxx	= win->_maxx;
	offset	= 1;
	temp1	= &win->_y[y][x];

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

  PDC_clr_scrn()	- Clears the physical screen and homes the cursor.

  PDCurses Description:
 	This is an internal routine called by the doupdate() routines.

  PDCurses Return Value:
 	This routine always returns OK.

  Portability:
 	PDCurses	int PDC_clr_scrn( WINDOW* win );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_clr_scrn(WINDOW *win)
#else
int	PDC_clr_scrn(win)
WINDOW *win;
#endif
/***********************************************************************/
{
#if defined(UNIX_WCLR)
	chtype	attrs = win->_attrs;
#else
	chtype	attrs = win->_bkgd;
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

  PDC_clr_update()	- Updates the screen with a full redraw.

  PDCurses Description:
 	Updates the screen by clearing it and then redraw it in its
 	entirety. If SP->refrbrk is TRUE, and there is pending
 	input characters, the update will be prematurely terminated.

  PDCurses Return Value:
 	This routine returns ERR if it is unable to accomplish it's task.
 	This return value is ONLY under FLEXOS.

 	The return value OK is returned if there were no errors.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_clr_update( WINDOW* s );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_clr_update(WINDOW *s)
#else
int	PDC_clr_update(s)
WINDOW *s;
#endif
/***********************************************************************/
{
register int	i=0,j=0;
	WINDOW*	w=NULL;
#if defined(UNIX)
	chtype*	ch;
#else
	unsigned short*	ch;
#endif
	bool rc=FALSE;

#if defined(DOS)
#  if SMALL || MEDIUM
struct SREGS segregs;
int ds=0;
#  endif
#endif

#ifdef	FLEXOS
	char	line[80];
	char	attr[80];
	FFRAME	sframe;
	RECT	drect,
		srect;
#endif
#if defined(DOS) && defined(NDP)
	int *VIDPOINT;
#endif

	extern unsigned	char atrtab[MAX_ATRTAB];

/* the next two variables have been changed from chtype to unsigned short */
/* as this is the correct datatype for a physical character/attribute */
	unsigned short temp_line[256]; /* this should be enough for the maximum width of a screen. MH-920715 */
	unsigned short chr;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_clr_update() - called\n");
#endif

	w = curscr;
	if (w == (WINDOW *)NULL)
		return( ERR );
/*	if (SP->full_redraw)
		PDC_clr_scrn(s); *//* clear physical screen */

	s->_clear = FALSE;
	for (i = 0; i < LINES; i++)	/* update physical screen */
	{
		if (s != w)	/* copy s to curscr */
			memcpy(w->_y[i], s->_y[i], COLS * sizeof(chtype));

		ch = temp_line; /* now have ch pointing to area to contain real attributes. MH-920715 */


#if defined(DOS) || defined(OS2)
#  if 0
		memcpy(ch,s->_y[i],COLS*sizeof(chtype)); /* copy current line to temp_line. MH-920715 */
		for (j=0;j<COLS;j++)          /* for each chtype in the line... */
		   {
		    chr = temp_line[j] & A_CHARTEXT;
		    temp_line[j] = chtype_attr(temp_line[j]) | chr;
		   }
#  else
		for (j=0;j<COLS;j++)          /* for each chtype in the line... */
		   {
		    chr = (unsigned short)(s->_y[i][j] & A_CHARTEXT);
		    temp_line[j] = chtype_attr(s->_y[i][j]) | chr;
		   }
#  endif
#endif

		if (SP->direct_video)
		{
#ifdef	FLEXOS
			PDC_split_plane(w, &line[0], &attr[0], i, 0, i, COLS);
/* need to translate attr[] array to real attributes before displaying it. MH-920715 */
			drect.r_row = i;
			drect.r_col = 0;
			drect.r_nrow = 1;
			drect.r_ncol = COLS;

			sframe.fr_pl[0] = (UBYTE *) line;
			sframe.fr_pl[1] = (UBYTE *) attr;
			sframe.fr_nrow = 1;
			sframe.fr_ncol = COLS;
			sframe.fr_use = 0x03;

			srect.r_col = 0;
			srect.r_row = 0;
			srect.r_nrow = 1;
			srect.r_ncol = COLS;

			s_copy(0x03, 0x01L, 0L, (far unsigned short *) &drect,
				(far unsigned short *) &sframe,
				(far unsigned short *) &srect);
#endif

#ifdef	DOS
#  ifdef GO32
			dosmemput (ch, COLS * sizeof(unsigned short),
				(void *)_FAR_POINTER(SP->video_seg,
				SP->video_ofs + (i * COLS * sizeof(unsigned short))));
#  else
#    if	(SMALL || MEDIUM)
		segread(&segregs);
		ds = segregs.ds;
		movedata(ds, (int)ch,
				SP->video_seg,
				SP->video_ofs + (i*COLS*sizeof(unsigned short)),
			        (COLS * sizeof(unsigned short)));
#    else
/*
#    ifdef	NDP
			VIDPOINT = (int *) mapdev(MK_FP(SP->video_seg,
			  SP->video_ofs + (i * COLS * sizeof(unsigned short))),
			  (COLS * sizeof(unsigned short)));
			memcpy(VIDPOINT, (int *) ch, (COLS * sizeof(unsigned short)));
*/
			memcpy((void *)_FAR_POINTER(SP->video_seg,
			  SP->video_ofs + (i * COLS * sizeof(unsigned short))),
			       ch, (COLS * sizeof(unsigned short)));
#    endif
#  endif
#endif

#ifdef	OS2
# ifdef EMXVIDEO
				v_putline ((char *)ch, 0, i, COLS);
# else
				VioWrtCellStr ((PCH)ch, (USHORT)(COLS * sizeof(unsigned short)), (USHORT)i, 0, 0);
# endif
#endif
		}
		else
		{

#ifdef UNIX
			PDC_gotoxy(i, 0);
			for (j = 0; j < COLS; j++)
			{
				PDC_putc( (*ch & A_CHARTEXT), (*ch & A_ATTRIBUTES) );
				ch++;
			}
#endif

#if defined(DOS) || defined(OS2)
			for (j = 0; j < COLS; j++)
			{
				PDC_gotoxy(i, j);
				PDC_putc( (*ch & A_CHARTEXT), (*ch & A_ATTRIBUTES) >> 8 );
				ch++;
			}
#endif

#if defined(XCURSES)
		XCurses_transform_line(w->_y[i],i,0,COLS);
#endif

		}

		if (SP->refrbrk && (SP->cbreak || SP->raw_inp)) 
		{
			rc = PDC_breakout();
			if(rc) 
				break;
		}
		w->_firstch[i] = _NO_CHANGE;
		w->_lastch[i] = _NO_CHANGE;
	}
#if 0
#if defined(XCURSES)
	XCurses_wait_for_display();
#endif
#endif
	return( OK );
}

/*man-start*********************************************************************

  PDC_cursor_on()	- Turns on the hardware cursor.

  PDCurses Description:
 	Turns on the hardware curses, it does nothing if it is already on.

  PDCurses Return Value:
 	Returns OK upon success, ERR upon failure.

  Portability:
 	PDCurses	int PDC_cursor_on( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_cursor_on(void)
#else
int	PDC_cursor_on()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_cursor_on() - called\n");
#endif

#ifndef UNIX
	if	(!SP->visible_cursor)
	{
		SP->visible_cursor = TRUE;
/*		PDC_fix_cursor(SP->orig_emulation);*/
		if (SP->bogus_adapter)
		{
			PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
				     (SP->cursor & 0x00ff));
		}
		else
		{
#ifdef     OS2
			PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
					     (SP->cursor & 0x00ff));
#else
			switch (SP->adapter) {
			case _MDA:
			case _CGA:
			case _EGACOLOR:
			case _EGAMONO:
			case _VGACOLOR:
			case _VGAMONO:
			case _MDS_GENIUS:
/*				PDC_set_cursor_mode(SP->font - 2, SP->font - 1);*/
				PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
					     (SP->cursor & 0x00ff));
				break;
			case _MCGACOLOR:
			case _MCGAMONO:
/*				PDC_set_cursor_mode(SP->font - 1, SP->font - 2);*/
				PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
					     (SP->cursor & 0x00ff));
				break;
			case _FLEXOS:
				PDC_set_cursor_mode(SP->visible_cursor, 0);
				break;
			default:
				break;
			}
#endif
		}
	}
#endif
	return( OK );
}

/*man-start*********************************************************************

  PDC_cursor_off()	- Turns off the hardware cursor.

  PDCurses Description:
 	Turns off the hardware curses, it does nothing if it is already off.

  PDCurses Return Value:
 	Returns OK upon success, ERR upon failure.

  PDCurses Errors:
 	ERR will be returned (in the case of FLEXOS) if the hardware cursor
 	can not be disabled.

  Portability:
 	PDCurses	int PDC_cursor_off( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_cursor_off(void)
#else
int	PDC_cursor_off()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_cursor_off() - called\n");
#endif

#ifndef UNIX
	if	(SP->visible_cursor)
	{
		SP->visible_cursor = FALSE;
#ifdef     OS2
		PDC_set_cursor_mode(32, 33);	/* turn it off */
#else
		switch (SP->adapter)
		{
		case _FLEXOS:
			PDC_set_cursor_mode(SP->visible_cursor, 0);
			break;
		default:
			PDC_set_cursor_mode(32, 33);	/* turn it off */
			break;
		}
#endif
	}
#endif
	return( OK );
}

/*man-start*********************************************************************

  PDC_fix_cursor()	- Fix the cursor start and stop scan lines (if necessary)

  PDCurses Description:
 	This is a private PDCurses routine.

 	This routine will fix the cursor shape for certain video adapters.
 	Normally, the values used are correct, but some adapters choke.
 	The most noticable choke is on a monochrome adapter.  The "correct"
 	scan lines will result in the cursor being set in the middle of the
 	character cell, rather than at the bottom.

 	The passed flag indicates whether the cursor is visible or not.

 	This only applies to the DOS platform.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_fix_cursor( int flag );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_fix_cursor(int flag)
#else
int	PDC_fix_cursor(flag)
int flag;
#endif
/***********************************************************************/
{
#ifdef	DOS

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_fix_cursor() - called\n");
#endif

#ifdef	FLEXOS
	return( OK );
#endif

	if (SP->bogus_adapter)
		return( OK );

	switch (SP->adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
	case _MDS_GENIUS:		/* Some clones look like a Genius;-)  */
		if (flag & 0x01)
			setdosmembyte (0x487, getdosmembyte (0x487) | 0x01); /* Enable emulation */
		else
			setdosmembyte (0x487, getdosmembyte (0x487) & ~0x01); /* Disable emulation */
		break;

	case _VGACOLOR:
	case _VGAMONO:
		if (flag & 0x01)
#ifdef WATCOMC
			regs.w.ax = 0x1200;		/* Enable  emulation */
#else
			regs.x.ax = 0x1200;		/* Enable  emulation */
#endif
		else
#ifdef WATCOMC
			regs.w.ax = 0x1201;		/* Disable emulation */
#else
			regs.x.ax = 0x1201;		/* Disable emulation */
#endif
		regs.h.bl = 0x34;
		int86(0x10, &regs, &regs);

		break;

	case _MCGACOLOR:
	case _MCGAMONO:
	case _MDA:
	case _CGA:
	case _NONE:
	default:
		break;
	}
	return( OK );
#endif

#ifdef     OS2
        return( OK );
#endif

#ifdef     UNIX
        return( OK );
#endif
}
#ifdef UNIX
#include <defs.h>
#include <term.h>
#endif

/*man-start*********************************************************************

  PDC_gotoxy()	- position hardware cursor at (x, y)

  PDCurses Description:
 	This is a private PDCurses routine.

 	Moves the physical cursor to the desired address on the
 	screen. We don't optimize here -- on a PC, it takes more time
 	to optimize than to do things directly.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_gotoxy( int row, int col );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_gotoxy(int row, int col)
#else
int	PDC_gotoxy(row,col)
int row;
int col;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_gotoxy() - called: row %d col %d\n",row,col);
#endif

#ifndef UNIX
	if ((SP->cursrow == row) && (SP->curscol == col))
		return( OK );
#endif

#ifdef	FLEXOS
	retcode = s_get(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	if (retcode < 0L)
		return( ERR );
	vir.vc_cursor.pos_row = row;
	vir.vc_cursor.pos_col = col;
	retcode = s_set(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef	DOS
	regs.h.ah = 0x02;
	regs.h.bh = SP->video_page;
	regs.h.dh = (unsigned char) row;
	regs.h.dl = (unsigned char) col;
	int86(0x10, &regs, &regs);
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	v_gotoxy (col, row);
# else
	VioSetCurPos (row, col, 0);
# endif
	return(OK);
#endif

#ifdef UNIX
	if (cursor_address != NULL)
		{
		putp(tparm(cursor_address,row,col));
		}
	return(OK);
#endif

#if defined (XCURSES)
	XCurses_display_cursor(SP->cursrow,SP->curscol,row,col);
#if 0
	XCurses_display_cursor(SP->cursrow,SP->curscol,
		curscr->_y[SP->cursrow][SP->curscol],
		row,col,curscr->_y[row][col]);
#endif
	return(OK);
#endif
}

/*man-start*********************************************************************

  PDC_newline()	- Advances 1 newline from supplied line number.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Does line advance and returns the new cursor line.  If error,
 	return -1.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_newline( WINDOW* win, int lin );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_newline(WINDOW *win, int lin)
#else
int	PDC_newline(win,lin)
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
 */
 			wrefresh(win);
		}
		else
			return( -1 );
	}
	return( lin );
}

/*man-start*********************************************************************

  PDC_putc()	- Output a character in the current attribute.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Outputs character 'chr' to screen in tty fashion. If a colour
 	mode is active, the character is written with colour 'colour'.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_putc( chtype character, chtype color );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_putc( chtype character, chtype color )
#else
int	PDC_putc(character,color)
chtype character;
chtype color;
#endif
/***********************************************************************/
{
#ifdef	OS2
	int curRow=0, curCol=0;
# ifdef EMXVIDEO
	char Cell[2];
# endif
#endif

#ifdef UNIX
static chtype last_attribute=0;
#  ifdef CHTYPE_LONG
static bool last_acs=FALSE;
#  endif
static int curses_to_ansi[] = {0,4,2,6,1,5,3,7};
short fore=0,back=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_putc() - called:char=%c attrib=0x%x color=0x%x\n",character & A_CHARTEXT,character & A_ATTRIBUTES,color);
#endif

#ifdef	FLEXOS
	retcode = s_write(0x00, 0x01L, (_far char *) &character, 1L, 0);
	return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef	DOS
	regs.h.ah = 0x09;	/* Avoid screen wrap.  Don't advance cursor. */
	regs.h.al = (unsigned char) (character & A_CHARTEXT);
	regs.h.bh = SP->video_page;
	regs.h.bl = (unsigned char) (color);
#ifdef WATCOMC
	regs.w.cx = 1;
#else
	regs.x.cx = 1;
#endif
	int86(0x10, &regs, &regs);
	return( OK );
#endif

#ifdef	OS2
	PDC_get_cursor_pos (&curRow, &curCol);
# ifdef EMXVIDEO
	Cell[0] = (char)character;
	Cell[1] = (char)color;
	v_putline (Cell, curCol, curRow, 1);
# else
	VioWrtTTY ((PCH)&character, 1, 0);
	VioWrtNAttr ((PBYTE)&color, 1, (USHORT)curRow, (USHORT)curCol, 0);
	PDC_gotoxy (curRow, curCol);
# endif
	return( OK );
#endif

#ifdef UNIX
	if (last_attribute != color)
		{
		if (exit_attribute_mode != NULL)
			putp(exit_attribute_mode);
		last_attribute = color;
		if ((color & A_COLOR) && has_colors())
			{
			pair_content(PAIR_NUMBER(color),&fore,&back);
			putp(tparm(set_foreground,curses_to_ansi[fore]));
			putp(tparm(set_background,curses_to_ansi[back]));
			}
		if (color & A_BOLD)
			if (enter_bold_mode != NULL)
				putp(enter_bold_mode);
		if (color & A_BLINK)
			if (enter_blink_mode != NULL)
				putp(enter_blink_mode);
		if (color & A_REVERSE)
			if (enter_reverse_mode != NULL)
				putp(enter_reverse_mode);
#ifdef CHTYPE_LONG
		if (color & A_STANDOUT)
			if (enter_standout_mode != NULL)
				putp(enter_standout_mode);
		if (color & A_ALTCHARSET)
			{
			if (last_acs == FALSE)
				if (enter_alt_charset_mode != NULL)
					putp(enter_alt_charset_mode);
			last_acs = TRUE;
			}
		else
			{
			last_acs = FALSE;
			if (exit_alt_charset_mode != NULL)
				putp(exit_alt_charset_mode);
			}
#endif
		}
	putchar(character & A_CHARTEXT);
	return( OK );
#endif
}

/*man-start*********************************************************************

  PDC_putctty()	- Output a character and attribute in TTY fashion.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Outputs character 'chr' to screen in tty fashion. If a colour
 	mode is active, the character is written with colour 'colour'.

 	This function moves the physical cursor after writing so the
 	screen will scroll if necessary.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_putctty( chtype character, chtype color );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_putctty( chtype character, chtype color )
#else
int	PDC_putctty(character,color)
chtype character;
chtype color;
#endif
/***********************************************************************/
{
#ifdef	OS2
	int curRow=0, curCol=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_putctty() - called\n");
#endif

#ifdef	FLEXOS
	retcode = s_write(0x00, 0x01L, (_far char *) &character, 1L, 0);
	return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef	DOS
	regs.h.ah = 0x0e;	/* Write in TTY fashion, advance cursor. */
	regs.h.al = (unsigned char) (character & A_CHARTEXT);
	regs.h.bh = SP->video_page;
	regs.h.bl = (unsigned char) ((color & A_ATTRIBUTES) >> 8);
	int86(0x10, &regs, &regs);
	return( OK );
#endif

#ifdef	OS2
	PDC_get_cursor_pos (&curRow, &curCol);
# ifdef EMXVIDEO
	v_attrib (color);
	v_putc (character);
# else
	VioWrtTTY ((PCH)&character, 1, 0);
	VioWrtNAttr ((PBYTE)&color, 1, (USHORT)curRow, (USHORT)curCol, 0);
# endif
	return( OK );
#endif

#ifdef UNIX
/* INCOMPLETE */
	return( OK );
#endif
}

/*man-start*********************************************************************

  PDC_scroll()	- low level screen scroll

  PDCurses Description:
 	Scrolls a window in the current page up or down. Urow, lcol,
 	lrow, rcol are the window coordinates.	Lines is the number of
 	lines to scroll. If 0, clears the window, if < 0 scrolls down,
 	if > 0 scrolls up.  Blanks areas that are left, and sets
 	character attributes to attr. If in a colour graphics mode,
 	fills them with the colour 'attr' instead.

  PDCurses Return Value:
 	The PDC_scroll() function returns OK on success otherwise ERR is returned.

  PDCurses Errors:
 	An error will only be returned on the Flexos platform if s_copy()
 	fails.

  Portability:
 	PDCurses	int PDC_scroll( int urow, int lcol, int rcol,
 				     int nlines, chtype attr );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_scroll(int urow, int lcol, int lrow, int rcol, int nlines, chtype attr)
#else
int	PDC_scroll(urow,lcol,lrow,rcol,nlines,attr)
int urow;
int lcol;
int lrow;
int rcol;
int nlines;
chtype attr;
#endif
/***********************************************************************/
{
	extern unsigned	char atrtab[MAX_ATRTAB];
	int	phys_attr=chtype_attr(attr);
#ifdef	FLEXOS
	int	srow=0;
	int	scol=0;
	int	drow=0;
	int	dcol=0;
	int	nrows=0;
	int	ncols=0;
	char	blank = (char) SP->blank;
#endif

#ifdef	OS2
# ifndef EMXVIDEO
	USHORT ch=(phys_attr | SP->blank);
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_scroll() - called: urow %d lcol %d lrow %d rcol %d nlines %d\n",urow,lcol,lrow,rcol,nlines);
#endif

#ifdef	FLEXOS
	if (nlines == 0)
	{
		sframe.fr_pl[0] = (UBYTE *) & blank;
		sframe.fr_pl[1] = (UBYTE *) & attr;
		sframe.fr_pl[2] = (UBYTE *) " ";
		sframe.fr_nrow = 1;
		sframe.fr_ncol = 1;
		sframe.fr_use = 0x00;
		nrows = lrow;
		ncols = rcol;
		srow = drow = 0;
		scol = dcol = 0;
	}
	else
	if (nlines < 0)
	{
		srow = urow;
		scol = lcol;
		drow = lrow;
		dcol = rcol;
	}
	else
	if (nlines > 0)
	{
		srow = urow;
		scol = lcol;
		drow = lrow;
		dcol = lcol;
	}

	drect.r_row = drow;
	drect.r_col = dcol;
	drect.r_nrow = nrows;
	drect.r_ncol = ncols;

	srect.r_col = scol;
	srect.r_row = srow;
	srect.r_nrow = nrows;
	srect.r_ncol = ncols;

	if (nlines != 0)
		retcode = s_copy(0x03, 0x01L, 0L, (far unsigned short *) &drect, 0L, (far unsigned short *) &srect);
	else
		retcode = s_copy(0x03, 0x01L, 0L, (far unsigned short *) &drect, (far unsigned short *) &sframe, (far unsigned short *) &srect);
	return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef	DOS
	if (nlines >= 0)
	{
		regs.h.ah = 0x06;
		regs.h.al = (unsigned char) nlines;
	}
	else
	{
		regs.h.ah = 0x07;
		regs.h.al = (unsigned char) (-nlines);
	}
	regs.h.bh = (unsigned char)(phys_attr >> 8);
	regs.h.ch = (unsigned char) urow;
	regs.h.cl = (unsigned char) lcol;
	regs.h.dh = (unsigned char) lrow;
	regs.h.dl = (unsigned char) rcol;
	int86(0x10, &regs, &regs);
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	v_attrib (phys_attr);
	if (nlines > 0)
		v_scroll (lcol, urow, rcol, lrow, nlines, V_SCROLL_UP);
	else
		if (nlines < 0)
			v_scroll (lcol, urow, rcol, lrow, -nlines, V_SCROLL_DOWN);
		else	/* this clears the whole screen */
			v_scroll (lcol, urow, rcol, lrow, -1, V_SCROLL_CLEAR);
# else
	if (nlines > 0)
		VioScrollUp(urow, lcol, lrow, rcol, nlines, (PBYTE)&ch, 0);
	else
		if (nlines < 0)
			VioScrollDn(urow, lcol, lrow, rcol, nlines, (PBYTE)&ch, 0);
		else
/* this clears the whole screen ?? */
			VioScrollUp(0, 0, -1, -1, -1, (PBYTE)&ch, 0);
# endif
#endif

#ifdef UNIX
/* INCOMPLETE */
#endif
}

/*man-start*********************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
 	This is a private PDCurses function.

 	Updates the given physical line to look like the corresponding
 	line in _curscr.

  PDCurses Return Value:
 	This routine returns TRUE if a premature refresh end
 	is allowed, and there is an input character pending.  Otherwise,
 	FALSE is returned.

  PDCurses Errors:
 	No errors are defined for this routine.

  Portability:
 	PDCurses	bool	PDC_transform_line( int lineno );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_transform_line(register int lineno)
#else
bool	PDC_transform_line(lineno)
register int lineno;
#endif
/***********************************************************************/
{
#if defined (MSC) && defined (DOS)
chtype*	dstp=NULL;
chtype*	srcp=NULL;
#else
register chtype*	dstp=NULL;
register chtype*	srcp=NULL;
#endif

#ifdef DOS
#if SMALL || MEDIUM
struct SREGS segregs;
int ds=0;
#endif
#endif

	int	x=0;
	int	endx=0;
	int	len=0;
#if defined(DOS) && defined(NDP)
	int *VIDPOINT;
#endif
	extern unsigned	char atrtab[MAX_ATRTAB];

#if defined(DOS) || defined(OS2)
	unsigned short temp_line[256]; /* this should be enough for the maximum width of a screen. MH-920715 */
	unsigned short chr=0;
	unsigned short*	ch=NULL;
#else
	chtype temp_line[256]; /* this should be enough for the maximum width of a screen. MH-920715 */
	chtype chr=0;
	chtype*	ch=NULL;
#endif

	register int j=0;
	bool rc=FALSE;

#ifdef	FLEXOS
	char	line[80];
	char	attr[80];
	FFRAME	sframe;
	RECT	drect;
	RECT	srect;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_transform_line() - called: line %d\n",lineno);
#endif

	if (curscr == (WINDOW *)NULL)
		return( FALSE );

	memset(temp_line,' ',sizeof(temp_line));

	x = curscr->_firstch[lineno];
	endx = curscr->_lastch[lineno];
	dstp = curscr->_y[lineno] + x;
	srcp = curscr->_y[lineno] + x;
	len = endx-x+1;

	ch = temp_line; /* now have ch pointing to area to contain real attributes. MH-920715 */

/* replace the attribute part of the chtype with the actual colour value */
/* replacing the number that points to the actual colour value.          */

#if defined(DOS) || defined(OS2)
#  if 0
		memcpy(ch, srcp, len * sizeof(chtype));
	for (j=0;j<len;j++)          /* for each chtype in the line... */
	   {
	    chr = temp_line[j] & A_CHARTEXT;
	    temp_line[j] = chtype_attr(temp_line[j]) | chr;
	   }
#  else
	for (j=0;j<len;j++)          /* for each chtype in the line... */
	   {
	    chr = *(srcp+j) & A_CHARTEXT;
	    temp_line[j] = chtype_attr(*(srcp+j)) | chr;
	   }
#  endif
#endif

	if (SP->direct_video)
	{

#if IS_THIS_REALLY_NEEDED
		memcpy(dstp, srcp, len * sizeof(chtype));
#endif

#ifdef	FLEXOS
		_split_plane(curscr, &line[0], &attr[0], lineno, x, lineno, endx);

		drect.r_row = lineno;
		drect.r_col = x;
		drect.r_nrow = 1;
		drect.r_ncol = len;

		sframe.fr_pl[0] = (UBYTE *) line;
		sframe.fr_pl[1] = (UBYTE *) attr;
		sframe.fr_nrow = 1;
		sframe.fr_ncol = len;
		sframe.fr_use = 0x03;

		srect.r_col = 0;
		srect.r_row = 0;
		srect.r_nrow = 1;
		srect.r_ncol = len;

		s_copy(0x03, 0x01L, 0L, (far unsigned short *) &drect,
			(far unsigned short *) &sframe, (far unsigned
			short *) &srect);

#endif

#ifdef	DOS
#  ifdef GO32
		dosmemput (ch, len * sizeof(unsigned short),
			(void *)_FAR_POINTER(SP->video_seg,
			SP->video_ofs + (((lineno * curscr->_maxx) + x) * sizeof(unsigned short))));
#  else
#    if	SMALL || MEDIUM
		segread(&segregs);
		ds = segregs.ds;
		movedata(ds,(int)ch,
			SP->video_seg,
			SP->video_ofs+(((lineno*curscr->_maxx)+x)*sizeof(unsigned short)),
		        len * sizeof(unsigned short));
#    else
		memcpy((void *)_FAR_POINTER(SP->video_seg,
			     SP->video_ofs + (((lineno * curscr->_maxx) + x) * sizeof(unsigned short))),
		       ch, len * sizeof(unsigned short));
#    endif
#  endif
#endif

#ifdef	OS2
# ifdef EMXVIDEO
					v_putline ((char*)ch, x, lineno, len);
# else
					VioWrtCellStr ((PCH)ch, (USHORT)(len*sizeof(unsigned short)), (USHORT)lineno, (USHORT)x, 0);
# endif
#endif
	}
	else
	{

#ifdef UNIX
		PDC_gotoxy(lineno,x);
		for (; x <= endx; x++)
		{
			PDC_putc( (*ch & A_CHARTEXT),(*ch & A_ATTRIBUTES) );
			ch++;
		}
#endif

#if defined(DOS) || defined(OS2)
		for (; x <= endx; x++)
		{
			PDC_gotoxy(lineno, x);
			PDC_putc( (*ch & A_CHARTEXT),(*ch & A_ATTRIBUTES) >> 8 );
			ch++;
		}
#endif

#if defined(XCURSES)
		XCurses_transform_line(dstp,lineno,x,len);
#endif

	}
	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;

	if (SP->refrbrk && (SP->cbreak || SP->raw_inp)) 
	{
		rc = PDC_breakout();
		if(rc) 
			return(TRUE);
	}
	return(FALSE);
}
