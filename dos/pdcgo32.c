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
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcdos.h"

RCSID("$Id: pdcgo32.c,v 1.12 2006/10/15 02:42:25 wmcbrine Exp $");

#include <pc.h>    
#include <sys/movedata.h> /* prototypes of dosmemget() and dosmemput() */

unsigned char getdosmembyte(int offset)
{
	unsigned char b;

	dosmemget(offset, sizeof(unsigned char), &b);
	return b;
}

unsigned short getdosmemword(int offset)
{
	unsigned short w;

	dosmemget(offset, sizeof(unsigned short), &w);
	return w;
}

void setdosmembyte(int offset, unsigned char b)
{
	dosmemput(&b, sizeof(unsigned char), offset);
}

void setdosmemword(int offset, unsigned short w)
{
	dosmemput(&w, sizeof(unsigned short), offset);
}
