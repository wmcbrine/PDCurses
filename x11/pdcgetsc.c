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

#ifdef PDCDEBUG
char *rcsid_PDCgetsc  = "$Id: pdcgetsc.c,v 1.1 2001/01/10 08:30:08 mark Exp $";
#endif

/*man-start*********************************************************************

  PDC_get_columns()	- return width of screen/viewport.

  PDCurses Description:
 	This is a private PDCurses function

 	This function will return the width of the current screen.

  PDCurses Return Value:
 	This routine will return OK upon success and otherwise ERR will be
 	returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_columns( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_columns(void)
#else
int	PDC_get_columns()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_columns() - called\n");
#endif
	return(XCurses_get_cols());
}

/*man-start*********************************************************************

  PDC_get_rows()	- Return number of screen rows.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Returns the maximum number of rows supported by the display.
 	e.g.  25, 28, 43, 50, 60, 66...

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_get_rows( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_rows(void)
#else
int	PDC_get_rows()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_rows() - called\n");
#endif
	return(XCurses_get_rows());
}

/*man-start*********************************************************************

  PDC_get_font()	- Get the current font size

  PDCurses Description:
 	This is a private PDCurses routine.

 	This function returns the current font size.  This function only
 	works if the #define FAST_VIDEO is true.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	An ERR will be returned if FAST_VIDEO is not true.

  Portability:
 	PDCurses	int PDC_get_font( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_font(void)
#else
int	PDC_get_font()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_font() - called\n");
#endif
	return(OK);
}
