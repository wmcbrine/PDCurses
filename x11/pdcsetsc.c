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
char *rcsid_PDCsetsc  = "$Id: pdcsetsc.c,v 1.2 2001/01/10 08:30:16 mark Exp $";
#endif


/*man-start*********************************************************************

  PDC_set_font()	- sets the current font size

  PDCurses Description:
 	This is a private PDCurses function.

 	This routine sets the current font size, if the adapter allows
 	such a change.

  PDCurses Return Value:
 	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
 	It is an error to attempt to change the font size on a "bogus"
 	adapter.  The reason for this is that we have a known video
 	adapter identity problem.  e.g. Two adapters report the same
 	identifying characteristics.

 	It is also an error to attempt to change the size of the Flexos
 	console (as there is currently no support for that).

  Portability:
 	PDCurses	int	PDC_set_font( int size );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_set_font(int size)
#else
int	PDC_set_font(size)
int size;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_font() - called\n");
#endif
	return(OK);
}

/*man-start*********************************************************************

  PDC_set_rows()	- sets the physical number of rows on screen

  PDCurses Description:
 	This is a private PDCurses function.

 	This routine attempts to set the number of rows on the physical
 	screen to the passed value.

  PDCurses Return Value:
 	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
 	It is an error to attempt to change the screen size on a "bogus"
 	adapter.  The reason for this is that we have a known video
 	adapter identity problem.  e.g. Two adapters report the same
 	identifying characteristics.

 	It is also an error to attempt to change the size of the Flexos
 	console (as there is currently no support for that).

  Portability:
 	PDCurses	int	PDC_set_rows( int rows );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_set_rows(int rows)
#else
int	PDC_set_rows(rows)
int rows;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_rows() - called\n");
#endif
	return(0);
}
/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_curs_set(int visibility)
#else
int	PDC_curs_set(visibility)
int visibility;
#endif
/***********************************************************************/
{
   int ret_vis = SP->visibility;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_curs_set() - called: visibility=%d\n",visibility);
#endif

   if ( visibility != -1 )
   {
      SP->visibility = visibility;
   }
   XCurses_display_cursor(SP->cursrow,SP->curscol,SP->cursrow,SP->curscol,visibility);
   return(ret_vis);
}

/*man-start*********************************************************************

  PDC_set_title()	- Set window title

  PDCurses Description:
 	Sets the title of the window in which the curses program is running.
 	This function may not do anything on some platforms.

  PDCurses Return Value:
 	N/A

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	void PDC_set_title( char *title );

**man-end**********************************************************************/
/***********************************************************************/
#ifdef HAVE_PROTO
void	PDC_set_title(char *title)
#else
void	PDC_set_title(title)
char *title;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_title() - called:<%s>\n",title);
#endif
	XCurses_set_title(title);
	return;
}
