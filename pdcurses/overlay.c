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

/* undefine any macros for functions defined in this module */
#undef	overlay
#undef	overwrite
#undef	copywin

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#  undef	move
#  undef	wmove
#endif

#ifdef PDCDEBUG
char *rcsid_overlay  = "$Id: overlay.c,v 1.1 2001/01/10 08:27:18 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                       overlay

  Synopsis:
  	int	overlay(WINDOW *src_w, WINDOW *dst_w)
  	int	overwrite(WINDOW *src_w, WINDOW *dst_w)
  	int	copywin(WINDOW *src_w, WINDOW *dst_w, int src_tr,
  			int src_tc, int dst_tr, int dst_tc, int dst_br, int dst_bc,
  			bool overlay)

  X/Open Description:
 	The overlay() and overwrite() functions overlay src_w on top of 
 	dst_w; that is, all text in src_w is copied into dst_w. The windows
 	src_w and dst_w are not required to be the same size. The
 	copy starts at (0, 0) on each window. The difference between
 	the two functions is that overlay() is non-destructive
 	(blanks are not copied) while overwrite() is destructive
 	(blanks are copied).

 	copywin() is similar to overwrite() and overlay() but copywin()
 	does not require that the two windows overlap.
 	The arguments src_tc and src_tr specify the top left corner of the
 	region to be copied to the destination window.
 	The arguments dst_tc,dst_tr,dst_br,dst_bc specify the region within
 	the destination window to where the copy is made.
 	The argument overlay, if TRUE, indicates that the copy is done
 	non-destructively (as in overlay()). Blanks in the source window
 	are not copied to the destination window. When overlay is FALSE,
 	(as in overwrite()), the copy is destructive; blanks are copied
 	to the destination window.

  PDCurses Description:
 	The above description for overlay() and overwrite() is misleading 
 	in the actual behaviour exhibited by both SysV and BSD curses. 
 	The above implies that the character in 0,0 of the source window 
 	is copied to 0,0 of the destination window. What actually happens 
 	is that those characters in the source window that intersect with 
 	characters in the destination window RELATIVE TO ABSOLUTE 0,0 ON 
 	THE SCREEN, are copied to the destination window so that the 
 	characters appear in the same physical position on the screen.

 	Thanks to Andreas Otte (venn@@uni-paderborn.de) for the correction
 	and code changes.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      overlay                               Y        Y       Y
      overwrite                             Y        Y       Y
      copywin                               -        -      3.0

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	overlay(WINDOW *src_w, WINDOW *dst_w)
#else
int	PDC_CDECL	overlay(src_w,dst_w)
WINDOW *src_w;
WINDOW *dst_w;
#endif
/***********************************************************************/
{
	int	last_line;
	int	last_col;
	int	first_line;
	int	first_col;
	int	src_start_x;
	int	src_start_y;
	int	dst_start_x;
	int	dst_start_y;
	int	xdiff;
	int	ydiff;
	int	rc;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("overlay() - called\n");
#endif

	if (src_w == (WINDOW *)NULL)	return( ERR );
	if (dst_w == (WINDOW *)NULL)	return( ERR );

	first_col  = max(dst_w->_begx,src_w->_begx);
	first_line = max(dst_w->_begy,src_w->_begy);
	last_col   = min(src_w->_begx+src_w->_maxx, dst_w->_begx+dst_w->_maxx);
	last_line  = min(src_w->_begy+src_w->_maxy, dst_w->_begy+dst_w->_maxy);

/* determine the overlapping region of the two windows in real coordinates */
	if ((last_col < first_col) || (last_line < first_line))
    return(OK);  /* if no overlapping region, do nothing */

/* size of overlapping region */
	xdiff = last_col - first_col;
	ydiff = last_line - first_line;

	if (src_w->_begx <= dst_w->_begx)
		{
		src_start_x = dst_w->_begx - src_w->_begx;
		dst_start_x = 0;
		}
	else
		{
		dst_start_x = src_w->_begx - dst_w->_begx;
		src_start_x = 0;
		}
	if (src_w->_begy <= dst_w->_begy)
		{
		src_start_y = dst_w->_begy - src_w->_begy;
		dst_start_y = 0;
		}
	else
		{
		dst_start_y = src_w->_begy - dst_w->_begy;
		src_start_y = 0;
		}

	rc = PDC_copy_win(src_w,dst_w,src_start_y,src_start_x,
		src_start_y+ydiff,src_start_x+xdiff,dst_start_y,dst_start_x,
		dst_start_y+ydiff,dst_start_x+xdiff,TRUE);

	return( rc );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	overwrite(WINDOW *src_w, WINDOW *dst_w)
#else
int	PDC_CDECL	overwrite(src_w,dst_w)
WINDOW *src_w;
WINDOW *dst_w;
#endif
/***********************************************************************/
{
	int	last_line;
	int	last_col;
	int	first_line;
	int	first_col;
	int	src_start_x;
	int	src_start_y;
	int	dst_start_x;
	int	dst_start_y;
	int	xdiff;
	int	ydiff;
	int	rc;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("overwrit() - called\n");
#endif

	if (src_w == (WINDOW *)NULL)	return( ERR );
	if (dst_w == (WINDOW *)NULL)	return( ERR );

	first_col  = max(dst_w->_begx,src_w->_begx);
	first_line = max(dst_w->_begy,src_w->_begy);
	last_col   = min(src_w->_begx+src_w->_maxx, dst_w->_begx+dst_w->_maxx);
	last_line  = min(src_w->_begy+src_w->_maxy, dst_w->_begy+dst_w->_maxy);

/* determine the overlapping region of the two windows in real coordinates */
	if ((last_col < first_col) || (last_line < first_line))
    return(OK);  /* if no overlapping region, do nothing */

/* size of overlapping region */
	xdiff = last_col - first_col;
	ydiff = last_line - first_line;

	if (src_w->_begx <= dst_w->_begx)
		{
		src_start_x = dst_w->_begx - src_w->_begx;
		dst_start_x = 0;
		}
	else
		{
		dst_start_x = src_w->_begx - dst_w->_begx;
		src_start_x = 0;
		}
	if (src_w->_begy <= dst_w->_begy)
		{
		src_start_y = dst_w->_begy - src_w->_begy;
		dst_start_y = 0;
		}
	else
		{
		dst_start_y = src_w->_begy - dst_w->_begy;
		src_start_y = 0;
		}

	rc = PDC_copy_win(src_w,dst_w,src_start_y,src_start_x,
		src_start_y+ydiff,src_start_x+xdiff,dst_start_y,dst_start_x,
		dst_start_y+ydiff,dst_start_x+xdiff,FALSE);

	return( rc );
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	copywin(WINDOW *src_w, WINDOW *dst_w, int src_tr, int src_tc, int dst_tr,
	int dst_tc, int dst_br, int dst_bc, int overlay)
#else
int	PDC_CDECL	copywin(src_w,dst_w,src_tr,src_tc,dst_tr,dst_tc,dst_br,dst_bc,overlay)
WINDOW *src_w;
WINDOW *dst_w;
int src_tr;
int src_tc;
int dst_tr;
int dst_tc;
int dst_br;
int dst_bc;
int overlay;
#endif
/***********************************************************************/
{
	int	src_start_x = src_tc;
	int	src_start_y = src_tr;
	int	dst_start_x = dst_tc;
	int	dst_start_y = dst_tr;
	int	src_end_x;
	int	src_end_y;
	int	dst_end_x;
	int	dst_end_y;
	int	src_rows,src_cols;
	int	dst_rows,dst_cols;
	int	min_rows,min_cols;
	int	rc;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("copywin() - called\n");
#endif

	if (src_w == (WINDOW *)NULL
	||  dst_w == (WINDOW *)NULL)
		return( ERR );

	if (dst_w == curscr)	return( ERR );

#if 0
	if (dst_br >= dst_w->_maxy
	||  dst_bc >= dst_w->_maxx
	||  dst_tr < 0
	||  dst_tc < 0)
		return( ERR );
#else
	if (dst_br > dst_w->_maxy
	||  dst_bc > dst_w->_maxx
	||  dst_tr < 0
	||  dst_tc < 0)
		return( ERR );
#endif

	src_rows = src_w->_maxy - src_tr;
	src_cols = src_w->_maxx - src_tc;
	dst_rows = dst_br - dst_tr;
	dst_cols = dst_bc - dst_tc;

	min_rows = min(src_rows,dst_rows);
	min_cols = min(src_cols,dst_cols);

	src_end_y = src_tr + min_rows;
	src_end_x = src_tc + min_cols;
	dst_end_y = dst_tr + min_rows;
	dst_end_x = dst_tc + min_cols;

	rc = PDC_copy_win(src_w,dst_w,src_start_y,src_start_x,src_end_y,src_end_x,
		dst_start_y,dst_start_x,dst_end_y,dst_end_x,overlay);

	return( rc );
}
