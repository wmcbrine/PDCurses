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

/* $Id: pdcsdl.h,v 1.10 2007/06/27 02:51:09 wmcbrine Exp $ */

#include <curspriv.h>

#include <SDL/SDL.h>

PDCEX SDL_Surface *pdc_screen, *pdc_font, *pdc_icon;
PDCEX int pdc_sheight, pdc_swidth, pdc_yoffset, pdc_xoffset;

extern SDL_Color pdc_color[16];
extern Uint32 pdc_mapped[16];
extern int pdc_fheight, pdc_fwidth, pdc_flastc;
extern bool pdc_own_screen;
extern WINDOW *pdc_lastscr;

void PDC_update_rects(void);
