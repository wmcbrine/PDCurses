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
#undef	mvcur

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_terminfo  = "$Id: terminfo.c,v 1.1 2001/01/10 08:27:31 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                      terminfo

  Synopsis:
  	int mvcur(int oldrow, int oldcol, int newrow, int newcol)

  X/Open Description:
 	The mvcur() function controls low-level cursor motion with 
 	optimization.

 	NOTE: The remainer of the terminfo functions have not been
 			implemented.

  X/Open Return Value:
 	All functions return OK on success and ERR on error.

  X/Open Errors:
 	No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      mvcur                                 Y        Y       Y

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_CDECL	mvcur(int oldrow, int oldcol, int newrow, int newcol)
#else
int	PDC_CDECL	mvcur(oldrow,oldcol,newrow,newcol)
int oldrow;
int oldcol;
int newrow;
int newcol;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("mvcur() - called: oldrow %d oldcol %d newrow %d newcol %d\n",oldrow,oldcol,newrow,newcol);
#endif

#ifdef	TC
#  pragma argsused
#endif
	if ((newrow >= LINES)	||
	    (newcol >= COLS)	||
	    (newrow < 0)	||
	    (newcol < 0))
	{
		return( ERR );
	}
	PDC_gotoxy( newrow, newcol );
	SP->cursrow = newrow;
	SP->curscol = newcol;
	return( OK );
}
