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
char *rcsid_fo_gname  = "$Id: fo_gname.c,v 1.1 2001/01/10 08:31:04 mark Exp $";
#endif




#ifdef	FLEXOS
VIRCON	vir;			/* Allocate a Virtual Console Structure */

/*man-start*********************************************************************

  _flexos_gname()	- FLEXOS: Return virtual console name

  PDCurses Description:
 	This is a Flexos platform PDCurses function.

 	This routine returns the name of the Flexos virtual console.
 	The VIRCON vir structure is initialized within the initscr()
 	calltree.

  PDCurses Return Value:
 	This routine returns a character pointer.

  PDCurses Errors:
 	No errors are defined for this routine.

  Portability:
 	PDCurses	char*	_flexos_gname( void );

**man-end**********************************************************************/

char*	_flexos_gname(void)
{
	return (&vir.vc_wname[0]);
}
#endif
