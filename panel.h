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
/*
$Id: panel.h,v 1.6 2005/12/14 19:40:28 wmcbrine Exp $
*/
/*
*----------------------------------------------------------------------
*				Panels for PDCurses
*----------------------------------------------------------------------
*/

#include <curses.h>

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
extern "C"
{
#endif

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
WINDOW *panel_window(const PANEL *pan);
void update_panels(void);
int hide_panel(PANEL *pan);
int show_panel(PANEL *pan);
int del_panel(PANEL *pan);
int top_panel(PANEL *pan);
int bottom_panel(PANEL *pan);
PANEL *new_panel(WINDOW *win);
PANEL *panel_above(const PANEL *pan);
PANEL *panel_below(const PANEL *pan);
int panel_hidden(const PANEL *pan);
int set_panel_userptr(PANEL *pan, const void *uptr);
const void *panel_userptr(const PANEL *pan);
int move_panel(PANEL *pan, int starty, int startx);
int replace_panel(PANEL *pan, WINDOW *win);
#else
WINDOW *panel_window();
void update_panels();
int hide_panel();
int show_panel();
int del_panel();
int top_panel();
int bottom_panel();
PANEL *new_panel();
PANEL *panel_above();
PANEL *panel_below();
int panel_hidden();
int set_panel_userptr();
void *panel_userptr();
int move_panel();
int replace_panel();
#endif

/* end of panel.h */

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
}
#endif
