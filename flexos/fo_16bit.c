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

#ifdef	PDCDEBUG
char *rcsid_fo_16bit  = "$Id: fo_16bit.c,v 1.1 2001/01/10 08:31:02 mark Exp $";
#endif




#ifdef	FLEXOS
/*man-start*********************************************************************

  _flexos_16bitmode()	- Puts console 16 bit mode.

  PDCurses Description:
 	This routine sets 16 bit mode (8 bit character +
 	8 bit attribute) for Flexos.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	Under FLEXOS, an ERR will be returned if the s_get() call
 	fails, or if the s_set() call fails.  You are referred to
 	the Flexos Programmer's Reference Guide for details on the
 	actual error.

  Portability:
 	PDCurses	int _flexos_16bitmode( void );

**man-end**********************************************************************/

int	_flexos_16bitmode(void)
{
extern	VIRCON	vir;		/* Allocate a Virtual Console Structure */

	retcode = s_get(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	if (retcode < 0L)
		return( ERR );

	vir.vc_kbmode = vir.vc_kbmode | (VCKM_NABORT | VCKM_NSTOP |
					 VCKM_NXLAT  | VCKM_NESC  |
					 VCKM_16BIT  | VCKM_NECHO |
					 VCKM_NEOF);

	vir.vc_smode = vir.vc_smode | VCSM_16BIT;

	retcode = s_set(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	return( (retcode < 0L) ? ERR : OK );
}
#endif
