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

/* $Id: panel.h,v 1.15 2006/10/15 02:42:24 wmcbrine Exp $ */

/*----------------------------------------------------------------------*
 *			   Panels for PDCurses				*
 *----------------------------------------------------------------------*/

#include <curses.h>

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
extern "C"
{
#endif

#ifndef __PDCURSES_PANEL_H__
#define __PDCURSES_PANEL_H__ 1

typedef struct panelobs
{
	struct panelobs *above;
	struct panel *pan;
} PANELOBS;

typedef struct panel
{
	WINDOW *win;
	int wstarty;
	int wendy;
	int wstartx;
	int wendx;
	struct panel *below;
	struct panel *above;
	const void *user;
	struct panelobs *obscure;
} PANEL;

WINDOW *panel_window(const PANEL *pan);
void	update_panels(void);
int	hide_panel(PANEL *pan);
int	show_panel(PANEL *pan);
int	del_panel(PANEL *pan);
int	top_panel(PANEL *pan);
int	bottom_panel(PANEL *pan);
PANEL  *new_panel(WINDOW *win);
PANEL  *panel_above(const PANEL *pan);
PANEL  *panel_below(const PANEL *pan);
int	panel_hidden(const PANEL *pan);
int	set_panel_userptr(PANEL *pan, const void *uptr);
const void *panel_userptr(const PANEL *pan);
int	move_panel(PANEL *pan, int starty, int startx);
int	replace_panel(PANEL *pan, WINDOW *win);

#endif /* __PDCURSES_PANEL_H__ */

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
}
#endif
