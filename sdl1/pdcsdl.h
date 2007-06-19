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

/* $Id: pdcsdl.h,v 1.7 2007/06/19 04:28:12 wmcbrine Exp $ */

#include <curspriv.h>

#include <SDL/SDL.h>

PDCEX SDL_Surface *pdc_screen, *pdc_font;
PDCEX SDL_Color pdc_color[16];
PDCEX Uint32 pdc_mapped[16];
PDCEX int pdc_fheight, pdc_fwidth, pdc_sheight, pdc_swidth;
PDCEX int pdc_yoffset, pdc_xoffset;
PDCEX bool pdc_own_screen;

void PDC_update_rects(void);
