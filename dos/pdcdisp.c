/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>
#include <string.h>

#ifdef PDCDEBUG
const char *rcsid_PDCdisp =
	"$Id: pdcdisp.c,v 1.30 2006/03/28 13:45:06 wmcbrine Exp $";
#endif

extern unsigned char atrtab[MAX_ATRTAB];

#ifdef __PACIFIC__
void movedata(unsigned sseg, unsigned soff, unsigned dseg,
	unsigned doff, unsigned n)
{
	far char *src = MK_FP(sseg, soff);
	far char *dst = MK_FP(dseg, doff);

	while (n--)
		*dst++ = *src++;
}
#endif

/*man-start**************************************************************

  PDC_cursor_on()	- Turns on the hardware cursor.

  PDCurses Description:
	Turns on the hardware curses, it does nothing if it is already on.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  Portability:
	PDCurses  int PDC_cursor_on(void);

**man-end****************************************************************/

int PDC_cursor_on(void)
{
	PDC_LOG(("PDC_cursor_on() - called\n"));

	if (!SP->visible_cursor)
	{
		SP->visible_cursor = TRUE;
/*		PDC_fix_cursor(SP->orig_emulation);*/

		if (SP->bogus_adapter)
			PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
				     (SP->cursor & 0x00ff));
		else
		{
			switch (SP->adapter) {
			case _MDA:
			case _CGA:
			case _EGACOLOR:
			case _EGAMONO:
			case _VGACOLOR:
			case _VGAMONO:
			case _MDS_GENIUS:
			case _MCGACOLOR:
			case _MCGAMONO:
				PDC_set_cursor_mode((SP->cursor & 0xff00) >> 8,
					     (SP->cursor & 0x00ff));
			}
		}
	}

	return OK;
}

/*man-start**************************************************************

  PDC_cursor_off()	- Turns off the hardware cursor.

  PDCurses Description:
	Turns off the hardware curses, it does nothing if it is already 
	off.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  PDCurses Errors:
	ERR will be returned if the hardware cursor can not be disabled.

  Portability:
	PDCurses  int PDC_cursor_off(void);

**man-end****************************************************************/

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

/*man-start**************************************************************

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

**man-end****************************************************************/

int PDC_fix_cursor(int flag)
{
	PDC_LOG(("PDC_fix_cursor() - called\n"));

	if (SP->bogus_adapter)
		return OK;

	/* flag & 1: enable emulation */

	switch (SP->adapter)
	{
	case _EGACOLOR:
	case _EGAMONO:
	case _MDS_GENIUS:		/* Some clones look like a Genius */
		if (flag & 1)
			setdosmembyte(0x487, getdosmembyte(0x487) | 0x01);
		else
			setdosmembyte(0x487, getdosmembyte(0x487) & ~0x01);
		break;

	case _VGACOLOR:
	case _VGAMONO:
#ifdef __WATCOMC__
		regs.w.ax = 0x1201 ^ (flag & 1);
#else
		regs.x.ax = 0x1201 ^ (flag & 1);
#endif
		regs.h.bl = 0x34;
		int86(0x10, &regs, &regs);
	}

	return OK;
}

/*man-start**************************************************************

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
	PDCurses  int PDC_gotoxy(int row, int col);

**man-end****************************************************************/

int PDC_gotoxy(int row, int col)
{
	PDC_LOG(("PDC_gotoxy() - called: row %d col %d\n", row, col));

	regs.h.ah = 0x02;
	regs.h.bh = SP->video_page;
	regs.h.dh = (unsigned char) row;
	regs.h.dl = (unsigned char) col;
	int86(0x10, &regs, &regs);

	return OK;
}

/*man-start**************************************************************

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

**man-end****************************************************************/

int PDC_putc(chtype character, chtype color)
{
	PDC_LOG(("PDC_putc() - called: char=%c attrib=0x%x color=0x%x\n",
		character & A_CHARTEXT, character & A_ATTRIBUTES, color));

	regs.h.ah = 0x09;	/* Avoid screen wrap.  Don't advance cursor. */
	regs.h.al = (unsigned char) (character & 0x00FF);
	regs.h.bh = SP->video_page;
	regs.h.bl = (unsigned char) (color);
#ifdef __WATCOMC__
	regs.w.cx = 1;
#else
	regs.x.cx = 1;
#endif
	int86(0x10, &regs, &regs);

	return OK;
}

/*man-start**************************************************************

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

**man-end****************************************************************/

int PDC_putctty(chtype character, chtype color)
{
	PDC_LOG(("PDC_putctty() - called\n"));

	regs.h.ah = 0x0e;	/* Write in TTY fashion, advance cursor. */
	regs.h.al = (unsigned char) (character & 0x00FF);
	regs.h.bh = SP->video_page;
	regs.h.bl = (unsigned char) ((color & 0xFF00) >> 8);
	int86(0x10, &regs, &regs);

	return OK;
}

/*man-start**************************************************************

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

**man-end****************************************************************/

int PDC_scroll(int urow, int lcol, int lrow, int rcol, int nlines, chtype attr)
{
	int phys_attr = chtype_attr(attr);

	PDC_LOG(("PDC_scroll() - called: urow %d lcol %d lrow %d "
		"rcol %d nlines %d\n", urow, lcol, lrow, rcol, nlines));

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

	return OK;
}

/*man-start**************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  PDCurses Errors:
	No errors are defined for this routine.

  Portability:
	PDCurses  void PDC_transform_line(int lineno);

**man-end****************************************************************/

void PDC_transform_line(int lineno)
{
	chtype *srcp;
	int j, x, len;

#if SMALL || MEDIUM
# ifndef __PACIFIC__
	struct SREGS segregs;
# endif
	int ds;
#endif

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	if (curscr == (WINDOW *)NULL)
		return;

	x = curscr->_firstch[lineno];
	len = curscr->_lastch[lineno] - x + 1;
	srcp = curscr->_y[lineno] + x;

	if (SP->direct_video)
	{
		/* this should be enough for the maximum width of a 
		   screen */

		unsigned short temp_line[256];

		/* replace the attribute part of the chtype with the 
		   actual colour value for each chtype in the line */

		for (j = 0; j < len; j++)
			temp_line[j] = chtype_attr(srcp[j]) |
				(srcp[j] & A_CHARTEXT);

#ifdef __DJGPP__
		dosmemput(temp_line, len * sizeof(unsigned short),
			(unsigned long)_FAR_POINTER(SP->video_seg,
			SP->video_ofs + (((lineno * curscr->_maxx) + x) *
			sizeof(unsigned short))));
#else
# if SMALL || MEDIUM
#  ifdef __PACIFIC__
		ds = FP_SEG((void far *) temp_line);
#  else
		segread(&segregs);
		ds = segregs.ds;
#  endif
		movedata(ds, (int)temp_line, SP->video_seg,
			SP->video_ofs + (((lineno * curscr->_maxx) + x) *
			sizeof(unsigned short)), len * sizeof(unsigned short));
# else
		memcpy((void *)_FAR_POINTER(SP->video_seg,
			SP->video_ofs + (((lineno * curscr->_maxx) + x) *
			sizeof(unsigned short))), temp_line,
			len * sizeof(unsigned short));
# endif
#endif

	}
	else
		for (j = 0; j < len; j++)
		{
			PDC_gotoxy(lineno, j + x);
			PDC_putc(srcp[j] & A_CHARTEXT,
				chtype_attr(srcp[j]) >> 8);
		}

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;
}
