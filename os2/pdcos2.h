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

/* $Id: pdcos2.h,v 1.3 2006/10/15 02:42:25 wmcbrine Exp $ */

#include <stdlib.h>

#ifdef EMXVIDEO
# include <sys/video.h>
#else
# define INCL_DOS
# define INCL_DOSMISC
# define INCL_WIN
# define INCL_VIO
# define INCL_KBD
# include <os2.h>
#endif

#include <curspriv.h>

extern int pdc_font;

extern void PDC_get_keyboard_info(void);
extern void PDC_set_keyboard_default(void);
