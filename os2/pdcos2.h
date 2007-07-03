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

/* $Id: pdcos2.h,v 1.7 2007/07/03 00:11:45 wmcbrine Exp $ */

#ifdef _MSC_VER
# define USE_OS2_H 1         /* Use the os2.h for the compiler       */
# define APIRET USHORT
#endif

#include <stdlib.h>
#include <string.h>

#ifdef EMXVIDEO
# include <sys/video.h>
#else
# define INCL_DOS
# define INCL_DOSMISC
# define INCL_WIN
# define INCL_VIO
# define INCL_KBD
# define INCL_MOU
# include <os2.h>
#endif

#include <curspriv.h>

#ifdef __WATCOMC__
# define PDCTHUNK(x) ((ptr_16)(x))
# ifdef __386__
#  define SEG16 _Seg16
# else
#  define SEG16
# endif

typedef void * SEG16 ptr_16;

#else
# ifdef __EMX__
#  define PDCTHUNK(x) ((PCH)_emx_32to16(x))
# endif
#endif

extern unsigned char *pdc_atrtab;
extern int pdc_font;

extern void PDC_get_keyboard_info(void);
extern void PDC_set_keyboard_default(void);
