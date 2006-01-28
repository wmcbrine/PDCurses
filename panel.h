/**************************************************************************
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
**************************************************************************/

/* $Id: panel.h,v 1.9 2006/01/28 13:27:23 wmcbrine Exp $ */

/* ----------------------------------------------------------------------
			     Panels for PDCurses
   ---------------------------------------------------------------------- */

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
#ifdef HAVE_PROTO
	const void *user;
#else
	void *user;
#endif
	struct panelobs *obscure;
} PANEL;

#ifdef HAVE_PROTO
# define Args(x) x
#else
# define Args(x) ()
#endif

WINDOW *panel_window Args((const PANEL *pan));
void update_panels Args((void));
int hide_panel Args((PANEL *pan));
int show_panel Args((PANEL *pan));
int del_panel Args((PANEL *pan));
int top_panel Args((PANEL *pan));
int bottom_panel Args((PANEL *pan));
PANEL *new_panel Args((WINDOW *win));
PANEL *panel_above Args((const PANEL *pan));
PANEL *panel_below Args((const PANEL *pan));
int panel_hidden Args((const PANEL *pan));
int set_panel_userptr Args((PANEL *pan, const void *uptr));
const void *panel_userptr Args((const PANEL *pan));
int move_panel Args((PANEL *pan, int starty, int startx));
int replace_panel Args((PANEL *pan, WINDOW *win));

#undef Args

#endif /* __PDCURSES_PANEL_H__ */

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
}
#endif
