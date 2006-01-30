/**************************************************************************
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
**************************************************************************/

#define	CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>
#include <string.h>

#ifdef PDCDEBUG
const char *rcsid_PDCdisp =
	"$Id: pdcdisp.c,v 1.14 2006/01/30 02:10:55 wmcbrine Exp $";
#endif

extern unsigned char atrtab[MAX_ATRTAB];

/*man-start*********************************************************************

  PDC_clr_update()	- Updates the screen with a full redraw.

  PDCurses Description:
	Updates the screen by clearing it and then redraw it in its
	entirety. If SP->refrbrk is TRUE, and there is pending
	input characters, the update will be prematurely terminated.

  PDCurses Return Value:
	This routine returns ERR if it is unable to accomplish its task.

	The return value OK is returned if there were no errors.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_clr_update(WINDOW *s);

**man-end**********************************************************************/

int PDC_clr_update(WINDOW *s)
{
	int i, j;
	unsigned short *ch;

	/* the next two variables have been changed from chtype to 
	   unsigned short as this is the correct datatype for a physical 
	   character/attribute */

	/* this should be enough for the maximum width of a screen. */

	unsigned short temp_line[256];
	unsigned short chr;

	PDC_LOG(("PDC_clr_update() - called\n"));

	if (curscr == (WINDOW *)NULL)
		return ERR;
#if 0
	if (SP->full_redraw)
		PDC_clr_scrn(s);	/* clear physical screen */
#endif
	s->_clear = FALSE;

	for (i = 0; i < LINES; i++)	/* update physical screen */
	{
		/* copy s to curscr -- must be same size */

		if (s != curscr)
			memcpy(curscr->_y[i], s->_y[i], COLS * sizeof(chtype));

		/* now have ch pointing to area to contain real 
		   attributes. MH-920715 */

		ch = temp_line;

		/* for each chtype in the line... */

		for (j = 0; j < COLS; j++)
		{
			chr = (unsigned short)(s->_y[i][j] & A_CHARTEXT);
			temp_line[j] = chtype_attr(s->_y[i][j]) | chr;
		}

		if (SP->direct_video)
#ifdef EMXVIDEO
			v_putline ((char *)ch, 0, i, COLS);
#else
			VioWrtCellStr((PCH)ch,
				(USHORT)(COLS * sizeof(unsigned short)),
				(USHORT)i, 0, 0);
#endif
		else
			for (j = 0; j < COLS; j++)
			{
				PDC_gotoxy(i, j);
				PDC_putc((*ch & 0x00FF), (*ch & 0xFF00) >> 8);
				ch++;
			}

		if (SP->refrbrk && (SP->cbreak || SP->raw_inp)
		    && PDC_breakout())
			break;

		curscr->_firstch[i] = _NO_CHANGE;
		curscr->_lastch[i] = _NO_CHANGE;
	}

	return OK;
}

/*man-start*********************************************************************

  PDC_cursor_on()	- Turns on the hardware cursor.

  PDCurses Description:
	Turns on the hardware cursor; does nothing if it is already on.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  Portability:
	PDCurses  int PDC_cursor_on(void);

**man-end**********************************************************************/

int PDC_cursor_on(void)
{
	PDC_LOG(("PDC_cursor_on() - called\n"));

	if (!SP->visible_cursor)
	{
		SP->visible_cursor = TRUE;
		PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
			     (SP->cursor & 0x00ff));
	}

	return OK;
}

/*man-start*********************************************************************

  PDC_cursor_off()	- Turns off the hardware cursor.

  PDCurses Description:
	Turns off the hardware cursor; does nothing if it is already off.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  PDCurses Errors:
	ERR will be returned if the hardware cursor can not be disabled.

  Portability:
	PDCurses  int PDC_cursor_off(void);

**man-end**********************************************************************/

int PDC_cursor_off(void)
{
	PDC_LOG(("PDC_cursor_off() - called\n"));

	if (SP->visible_cursor)
	{
		SP->visible_cursor = FALSE;
		PDC_set_cursor_mode(32, 33);	/* turn it off */
	}

	return OK;
}

/*man-start*********************************************************************

  PDC_fix_cursor()	- Fix the cursor start and stop scan lines
			  (if necessary)

  PDCurses Description:
	This is a private PDCurses routine.

	This routine will fix the cursor shape for certain video 
	adapters. Normally, the values used are correct, but some 
	adapters choke. The most noticable choke is on a monochrome 
	adapter.  The "correct" scan lines will result in the cursor 
	being set in the middle of the character cell, rather than at 
	the bottom.

	The passed flag indicates whether the cursor is visible or not.

	This only applies to the DOS platform.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_fix_cursor(int flag);

**man-end**********************************************************************/

int PDC_fix_cursor(int flag)
{
	return OK;
}

/*man-start*********************************************************************

  PDC_gotoxy()	- position hardware cursor at (x, y)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the screen. 
	We don't optimize here -- on a PC, it takes more time to 
	optimize than to do things directly.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_gotoxy(int row, int col);

**man-end**********************************************************************/

int PDC_gotoxy(int row, int col)
{
	PDC_LOG(("PDC_gotoxy() - called: row %d col %d\n", row, col));

#ifdef EMXVIDEO
	v_gotoxy(col, row);
#else
	VioSetCurPos(row, col, 0);
#endif
	return OK;
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
	PDCurses  int PDC_putc(chtype character, chtype color);

**man-end**********************************************************************/

int PDC_putc(chtype character, chtype color)
{
	int curRow = 0, curCol = 0;
#ifdef EMXVIDEO
	char Cell[2];
#endif
	PDC_LOG(("PDC_putc() - called: char=%c attrib=0x%x color=0x%x\n",
		character & A_CHARTEXT, character & A_ATTRIBUTES, color));

	PDC_get_cursor_pos(&curRow, &curCol);
#ifdef EMXVIDEO
	Cell[0] = (char)character;
	Cell[1] = (char)color;
	v_putline(Cell, curCol, curRow, 1);
#else
	VioWrtTTY((PCH)&character, 1, 0);
	VioWrtNAttr((PBYTE)&color, 1, (USHORT)curRow, (USHORT)curCol, 0);
	PDC_gotoxy(curRow, curCol);
#endif
	return OK;
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
	PDCurses  int PDC_putctty(chtype character, chtype color);

**man-end**********************************************************************/

int PDC_putctty(chtype character, chtype color)
{
	int curRow=0, curCol=0;

	PDC_LOG(("PDC_putctty() - called\n"));

	PDC_get_cursor_pos(&curRow, &curCol);
#ifdef EMXVIDEO
	v_attrib(color);
	v_putc(character);
#else
	VioWrtTTY((PCH)&character, 1, 0);
	VioWrtNAttr((PBYTE)&color, 1, (USHORT)curRow, (USHORT)curCol, 0);
#endif
	return OK;
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
	The PDC_scroll() function returns OK on success otherwise ERR is 
	returned.

  Portability:
	PDCurses  int PDC_scroll(int urow, int lcol, int rcol,
				 int nlines, chtype attr);

**man-end**********************************************************************/

int PDC_scroll(int urow, int lcol, int lrow, int rcol, int nlines, chtype attr)
{
	int phys_attr = chtype_attr(attr);

#ifndef EMXVIDEO
	USHORT ch = (phys_attr | SP->blank);
#endif

	PDC_LOG(("PDC_scroll() - called: urow %d lcol %d lrow %d "
		"rcol %d nlines %d\n", urow, lcol, lrow, rcol, nlines));

#ifdef EMXVIDEO
	v_attrib(phys_attr);

	if (nlines > 0)
		v_scroll(lcol, urow, rcol, lrow, nlines, V_SCROLL_UP);
	else
		if (nlines < 0)
			v_scroll(lcol, urow, rcol, lrow, -nlines,
				V_SCROLL_DOWN);
		else	/* this clears the whole screen */
			v_scroll(lcol, urow, rcol, lrow, -1, V_SCROLL_CLEAR);
#else
	if (nlines > 0)
		VioScrollUp(urow, lcol, lrow, rcol, nlines, (PBYTE)&ch, 0);
	else
		if (nlines < 0)
			VioScrollDn(urow, lcol, lrow, rcol, nlines,
				(PBYTE)&ch, 0);
		else	/* this clears the whole screen ?? */
			VioScrollUp(0, 0, -1, -1, -1, (PBYTE)&ch, 0);
#endif
	return OK;
}

/*man-start*********************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  PDCurses Return Value:
	This routine returns TRUE if a premature refresh end is allowed, 
	and there is an input character pending.  Otherwise, FALSE is 
	returned.

  PDCurses Errors:
	No errors are defined for this routine.

  Portability:
	PDCurses  bool PDC_transform_line(int lineno);

**man-end**********************************************************************/

bool PDC_transform_line(int lineno)
{
	chtype *srcp;
	int j, x, endx, len;

	/* this should be enough for the maximum width of a screen. */

	unsigned short temp_line[256];
	unsigned short chr;
	unsigned short *ch;

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	if (curscr == (WINDOW *)NULL)
		return FALSE;

	x = curscr->_firstch[lineno];
	endx = curscr->_lastch[lineno];
	srcp = curscr->_y[lineno] + x;
	len = endx - x + 1;

	/* now have ch pointing to area to contain real attributes. 
	   MH-920715 */

	ch = temp_line;

	/* replace the attribute part of the chtype with the actual 
	   colour value for each chtype in the line */

	for (j = 0; j < len; j++)
	{
		chr = srcp[j] & A_CHARTEXT;
		temp_line[j] = chtype_attr(srcp[j]) | chr;
	}

	if (SP->direct_video)
#ifdef EMXVIDEO
		v_putline((char*)ch, x, lineno, len);
#else
		VioWrtCellStr((PCH)ch, (USHORT)(len * sizeof(unsigned short)),
			(USHORT)lineno, (USHORT)x, 0);
#endif
	else
		for (; x <= endx; x++)
		{
			PDC_gotoxy(lineno, x);
			PDC_putc((*ch & A_CHARTEXT),
				(*ch & A_ATTRIBUTES) >> 8);
			ch++;
		}

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;

	if (SP->refrbrk && (SP->cbreak || SP->raw_inp) && PDC_breakout())
		return TRUE;

	return FALSE;
}
