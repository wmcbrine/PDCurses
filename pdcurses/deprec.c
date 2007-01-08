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

#include <curspriv.h>

RCSID("$Id: deprec.c,v 1.2 2007/01/08 02:52:33 wmcbrine Exp $");

/* Deprecated functions. These should not be used, and will eventually 
   be removed. They're here solely for the benefit of applications that 
   linked to them in older versions of PDCurses. */

bool PDC_check_bios_key(void)
{
	return PDC_check_key();
}

int PDC_get_bios_key(void)
{
	return PDC_get_key();
}

bool PDC_get_ctrl_break(void)
{
	return !SP->raw_inp;
}

int PDC_set_ctrl_break(bool setting)
{
	return setting ? noraw() : raw();
}
