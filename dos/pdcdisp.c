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

#include "pdcdos.h"

#include <string.h>

RCSID("$Id: pdcdisp.c,v 1.48 2006/07/30 03:55:46 wmcbrine Exp $");

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

  PDC_gotoyx()	- position hardware cursor at (y, x)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the
	screen. We don't optimize here -- on a PC, it takes more time
	to optimize than to do things directly.

  Portability:
	PDCurses  void PDC_gotoyx(int row, int col);

**man-end****************************************************************/

void PDC_gotoyx(int row, int col)
{
	PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

	regs.h.ah = 0x02;
	regs.h.bh = 0;
	regs.h.dh = (unsigned char) row;
	regs.h.dl = (unsigned char) col;
	int86(0x10, &regs, &regs);
}

/*man-start**************************************************************

  PDC_putc()	- Output a character and attribute.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs 'ch' to screen, 'count' times.

  Portability:
	PDCurses  void PDC_putc(chtype ch, unsigned short count);

**man-end****************************************************************/

static void PDC_putc(chtype ch, unsigned short count)
{
	PDC_LOG(("PDC_putc() - called\n"));

	regs.h.ah = 0x09;	/* Avoid screen wrap.  Don't advance cursor. */
	regs.h.al = (unsigned char) (ch & 0xff);
	regs.h.bh = 0;
	regs.h.bl = chtype_attr(ch);
#ifdef __WATCOMC__
	regs.w.cx = count;
#else
	regs.x.cx = count;
#endif
	int86(0x10, &regs, &regs);
}

/*man-start**************************************************************

  PDC_putctty()	- Output a character and attribute in TTY fashion.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs 'ch' to screen in tty fashion.

	This function moves the physical cursor after writing so the
	screen will scroll if necessary.

  Portability:
	PDCurses  void PDC_putctty(chtype ch);

**man-end****************************************************************/

void PDC_putctty(chtype ch)
{
	PDC_LOG(("PDC_putctty() - called\n"));

	regs.h.ah = 0x0e;	/* Write in TTY fashion, advance cursor. */
	regs.h.al = (unsigned char) (ch & 0xff);
	regs.h.bh = 0;
	regs.h.bl = chtype_attr(ch);
	int86(0x10, &regs, &regs);
}

/*man-start**************************************************************

  PDC_transform_line()	- display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  Portability:
	PDCurses  void PDC_transform_line(int lineno, int x, int len, 
					  const chtype *srcp);

**man-end****************************************************************/

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
	int j;

#if SMALL || MEDIUM
# ifndef __PACIFIC__
	struct SREGS segregs;
# endif
	int ds;
#endif

	PDC_LOG(("PDC_transform_line() - called: line %d\n", lineno));

	if (SP->direct_video)
	{
		/* this should be enough for the maximum width of a 
		   screen */

		unsigned short temp_line[256];

		/* replace the attribute part of the chtype with the 
		   actual colour value for each chtype in the line */

		for (j = 0; j < len; j++)
			temp_line[j] =
				(chtype_attr(srcp[j]) << 8) | (srcp[j] & 0xff);

#ifdef __DJGPP__
		dosmemput(temp_line, len * 2,
			(unsigned long)_FAR_POINTER(SP->video_seg,
			SP->video_ofs + (lineno * curscr->_maxx + x) * 2));
#else
# if SMALL || MEDIUM
#  ifdef __PACIFIC__
		ds = FP_SEG((void far *) temp_line);
#  else
		segread(&segregs);
		ds = segregs.ds;
#  endif
		movedata(ds, (int)temp_line, SP->video_seg,
			SP->video_ofs + (lineno * curscr->_maxx + x) * 2,
			len * 2);
# else
		memcpy((void *)_FAR_POINTER(SP->video_seg,
			SP->video_ofs + (lineno * curscr->_maxx + x) * 2),
			temp_line, len * 2);
# endif
#endif

	}
	else
		for (j = 0; j < len;)
		{
			unsigned short count = 1;

			while ((j + count < len) &&
				(srcp[j] == srcp[j + count]))
					count++;

			PDC_gotoyx(lineno, j + x);
			PDC_putc(srcp[j], count);

			j += count;
		}
}
