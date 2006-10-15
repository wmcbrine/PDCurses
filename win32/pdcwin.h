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

/* $Id: pdcwin.h,v 1.3 2006/10/15 02:42:26 wmcbrine Exp $ */

#ifdef PDC_WIDE
# define UNICODE
#endif

#include <windows.h>
#undef MOUSE_MOVED
#include <curspriv.h>

extern HANDLE pdc_con_out, pdc_con_in;

extern int PDC_get_buffer_rows(void);
