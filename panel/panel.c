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
#define	CURSES_LIBRARY	1

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <panel.h>

#ifdef UNIX
#include <defs.h>
#include <term.h>
#endif

#ifdef PDCDEBUG
char *rcsid_panel = "$Id: panel.c,v 1.2 2001/01/10 08:28:01 mark Exp $";
#endif


/*man-start*********************************************************************

  panels	- panel package for curses

  PDCurses Description:
 	The panel library is built using the curses library and any program
 	using panels routines must call one of the curses initialization
 	routines such as initscr(). A program using these routines must be
 	linked with the panels and curses libraries.

 	The panels package gives the applications programmer a way to have
 	depth relationships between curses windows; a curses window is
 	associated with every panel. The panels routines allow curses windows
 	to overlap without making visible the overlapped portions of 
 	underlying windows. The initial curses window, stdscr, lies beneath
 	all panels. The set of currently visible panels is the 'deck' of
 	panels.

 	The panels package allows the applications programmer to create
 	panels, fetch and set their associated windows, shuffle panels in 
 	the deck, and manipulate panels in other ways.

  PDCurses Return Value:
 	Each panels routine that returns a pointer to an object returns NULL
 	if an error occurs. Each panel routine that returns an integer, 
 	returns OK if it executes successfully and ERR if it does not.

  Notes
 	The header <panel.h> includes the header <curses.h>.

  Credits
 	Original Author - Warren Tucker N4HGF
 	{gatech,emory}!n4hgf!wht -or- wht@n4hgf.Mt-Park.GA.US

**man-end**********************************************************************/


/*
  Defined functions:
	int bottom_panel(PANEL *pan);
	int del_panel(PANEL *pan);
	int hide_panel(PANEL *pan);
	int move_panel(PANEL *pan,int starty,int startx);
	PANEL *new_panel(WINDOW *win);
	PANEL *panel_above(PANEL *pan);
	PANEL *panel_below(PANEL *pan);
	int panel_hidden(PANEL *pan);
	char *panel_userptr(PANEL *pan);
	WINDOW *panel_window(PANEL *pan);
	int replace_panel(PANEL *pan,WINDOW *win);
	int set_panel_userptr(PANEL *pan,char *uptr);
	int show_panel(PANEL *pan);
	int top_panel(PANEL *pan);
	void update_panels(void);

	Touchline(pan,start,count)
	Touchpan(pan)
	Wnoutrefresh(pan)
	__calculate_obscure()
	__free_obscure(pan)
	__override(pan,show)
	__panel_is_linked(pan)
	__panel_link_bottom(pan)
	__panel_link_top(pan)
	__panel_unlink(pan)
	__panels_overlapped(pan1,pan2)
	dPanel(text,pan)
	dStack(fmt,num,pan)
	open_dfp()

--------------------------------------------------------------------------*/
PANEL *__bottom_panel = (PANEL *)0;
PANEL *__top_panel = (PANEL *)0;
PANEL __stdscr_pseudo_panel = { (WINDOW *)0 };

#define STATIC static

/*+-------------------------------------------------------------------------
	dPanel(text,pan)
--------------------------------------------------------------------------*/
#ifdef PANEL_DEBUG
dPanel(text,pan)
char *text;
PANEL *pan;
{
	_tracef("%s id=%s b=%s a=%s y=%d x=%d",
		text,pan->user,
		(pan->below) ? pan->below->user : "--",
		(pan->above) ? pan->above->user : "--",
		pan->wstarty, pan->wstartx);
}	/* end of dPanel */
#else
#define dPanel(text,pan)
#endif

/*+-------------------------------------------------------------------------
	dStack(fmt,num,pan)
--------------------------------------------------------------------------*/
#ifdef PANEL_DEBUG
void
dStack(fmt,num,pan)
char *fmt;
int num;
PANEL *pan;
{
char s80[80];

	sprintf(s80,fmt,num,pan);
	_tracef("%s b=%s t=%s",s80,
		(__bottom_panel) ? __bottom_panel->user : "--",
		(__top_panel)    ? __top_panel->user    : "--");
	if(pan)
		_tracef("pan id=%s",pan->user);
	pan = __bottom_panel;
	while(pan)
	{
		dPanel("stk",pan);
		pan = pan->above;
	}
}	/* end of dStack */
#else
#define dStack(fmt,num,pan)
#endif

/*+-------------------------------------------------------------------------
	Wnoutrefresh(pan) - debugging hook for wnoutrefresh
--------------------------------------------------------------------------*/
#ifdef PANEL_DEBUG
STATIC int
Wnoutrefresh(pan)
PANEL *pan;
{
	dPanel("wnoutrefresh",pan);
	wnoutrefresh(pan->win);
}	/* end of Wnoutrefresh */
#else
#define Wnoutrefresh(pan) wnoutrefresh((pan)->win)
#endif

/*+-------------------------------------------------------------------------
	Touchpan(pan)
--------------------------------------------------------------------------*/
#ifdef PANEL_DEBUG
STATIC int
Touchpan(pan)
PANEL *pan;
{
	dPanel("Touchpan",pan);
	touchwin(pan->win);
}	/* end of Touchpan */
#else
#define Touchpan(pan) touchwin((pan)->win)
#endif

/*+-------------------------------------------------------------------------
	Touchline(pan,start,count)
--------------------------------------------------------------------------*/
#ifdef PANEL_DEBUG
STATIC int
Touchline(pan,start,count)
PANEL *pan;
int start;
int count;
{
char s80[80];
	sprintf(s80,"Touchline s=%d c=%d",start,count);
	dPanel(s80,pan);
	touchline(pan->win,start,count);
}	/* end of Touchline */
#else
#define Touchline(pan,start,count) touchline((pan)->win,start,count)
#endif

/*+-------------------------------------------------------------------------
	__panels_overlapped(pan1,pan2) - check panel overlapped
--------------------------------------------------------------------------*/
STATIC int
__panels_overlapped(pan1,pan2)
register PANEL *pan1;
register PANEL *pan2;
{
	if(!pan1 || !pan2)
		return(0);
	return((pan1->wstarty >= pan2->wstarty && pan1->wstarty < pan2->wendy) ||
		(pan2->wstarty >= pan1->wstarty && pan2->wstarty < pan1->wendy))
	&& ((pan1->wstartx >= pan2->wstartx && pan1->wstartx < pan2->wendx) ||
		(pan2->wstartx >= pan1->wstartx && pan2->wstartx < pan1->wendx));
}	/* end of __panels_overlapped */

/*+-------------------------------------------------------------------------
	__free_obscure(pan)
--------------------------------------------------------------------------*/
STATIC void
__free_obscure(pan)
PANEL *pan;
{
PANELOBS *tobs = pan->obscure;				/* "this" one */
PANELOBS *nobs;								/* "next" one */

	while(tobs)
	{
		nobs = tobs->above;
		free((char *)tobs);
		tobs = nobs;
	}
	pan->obscure = (PANELOBS *)0;
}	/* end of __free_obscure */

/*+-------------------------------------------------------------------------
	__override(pan,show)
--------------------------------------------------------------------------*/
STATIC void
__override(pan,show)
PANEL *pan;
int show;
{
register int y;
register PANEL *pan2;
PANELOBS *tobs = pan->obscure;				/* "this" one */


	if(show == 1)
		Touchpan(pan);
	else if(!show)
	{
		Touchpan(pan);
/*
		Touchline(&__stdscr_pseudo_panel,pan->wendy,getmaxy(pan->win));
*/
		Touchpan(&__stdscr_pseudo_panel);
	}
	else if(show == -1)
	{
		while(tobs && (tobs->pan != pan))
			tobs = tobs->above;
	}

	while(tobs)
	{
		if((pan2 = tobs->pan) != pan)
		{
			for(y = pan->wstarty; y < pan->wendy; y++)
			{
				if( (y >= pan2->wstarty) && (y < pan2->wendy) &&
					((is_linetouched(pan->win,y - pan->wstarty)) ||
					(is_linetouched(stdscr,y))))
				{
					Touchline(pan2,y - pan2->wstarty,1);
				}
			}
		}
		tobs = tobs->above;
	}
}	/* end of __override */

/*+-------------------------------------------------------------------------
	__calculate_obscure()
--------------------------------------------------------------------------*/
STATIC void
__calculate_obscure()
{
PANEL *pan;
register PANEL *pan2;
register PANELOBS *tobs;			/* "this" one */
PANELOBS *lobs = (PANELOBS *)0;		/* last one */

	pan = __bottom_panel;
	while(pan)
	{
		if(pan->obscure)
			__free_obscure(pan);
		lobs = (PANELOBS *)0;		/* last one */
		pan2 = __bottom_panel;
		while(pan2)
		{
			if(__panels_overlapped(pan,pan2))
			{
				if(!(tobs = (PANELOBS *)malloc(sizeof(PANELOBS))))
					return;
				tobs->pan = pan2;
				dPanel("obscured",pan2);
				tobs->above = (PANELOBS *)0;
				if(lobs)
					lobs->above = tobs;
				else
					pan->obscure = tobs;
				lobs  = tobs;
			}
			pan2 = pan2->above;
		}
		__override(pan,1);
		pan = pan->above;
	}

}	/* end of __calculate_obscure */

/*+-------------------------------------------------------------------------
	__panel_is_linked(pan) - check to see if panel is in the stack
--------------------------------------------------------------------------*/
STATIC int
__panel_is_linked(pan)
PANEL *pan;
{
register PANEL *pan2 = __bottom_panel;

	while(pan2)
	{
		if(pan2 == pan)
			return(1);
		pan2 = pan2->above;
	}
	return(OK);
}	/* end of __panel_is_linked */

/*+-------------------------------------------------------------------------
	__panel_link_top(pan) - link panel into stack at top
--------------------------------------------------------------------------*/
STATIC void
__panel_link_top(pan)
PANEL *pan;
{

#ifdef PANEL_DEBUG
	dStack("<lt%d>",1,pan);
	if(__panel_is_linked(pan))
		return;
#endif

	pan->above = (PANEL *)0;
	pan->below = (PANEL *)0;
	if(__top_panel)
	{
		__top_panel->above = pan;
		pan->below = __top_panel;
	}
	__top_panel = pan;
	if(!__bottom_panel)
		__bottom_panel = pan;
	__calculate_obscure();
	dStack("<lt%d>",9,pan);

}	/* end of __panel_link_top */

/*+-------------------------------------------------------------------------
	__panel_link_bottom(pan) - link panel into stack at bottom
--------------------------------------------------------------------------*/
STATIC void
__panel_link_bottom(pan)
PANEL *pan;
{

#ifdef PANEL_DEBUG
	dStack("<lb%d>",1,pan);
	if(__panel_is_linked(pan))
		return;
#endif

	pan->above = (PANEL *)0;
	pan->below = (PANEL *)0;
	if(__bottom_panel)
	{
		__bottom_panel->below = pan;
		pan->above = __bottom_panel;
	}
	__bottom_panel = pan;
	if(!__top_panel)
		__top_panel = pan;
	__calculate_obscure();
	dStack("<lb%d>",9,pan);

}	/* end of __panel_link_bottom */

/*+-------------------------------------------------------------------------
	__panel_unlink(pan) - unlink panel from stack
--------------------------------------------------------------------------*/
STATIC void
__panel_unlink(pan)
PANEL *pan;
{
register PANEL *prev;
register PANEL *next;

#ifdef PANEL_DEBUG
	dStack("<u%d>",1,pan);
	if(!__panel_is_linked(pan))
		return;
#endif

	__override(pan,0);
	__free_obscure(pan);

	prev = pan->below;
	next = pan->above;

	if(prev)		/* if non-zero, we will not update the list head */
	{
		prev->above = next;
		if(next)
			next->below = prev;
	}
	else if(next)
		next->below = prev;
	if(pan == __bottom_panel)
		__bottom_panel = next;
	if(pan == __top_panel)
		__top_panel = prev;

	__calculate_obscure();

	pan->above = (PANEL *)0;
	pan->below = (PANEL *)0;
	dStack("<u%d>",9,pan);

}	/* end of __panel_unlink */

/**********************************************************************/
/* The following are the public functions for the panels library.     */
/**********************************************************************/

/*man-start*********************************************************************

  bottom_panel	- puts panel at bottom of deck

  PDCurses Description:
 	This function places pan at the bottom of the deck. The size, location
 	and contents of the panel are unchanged.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int bottom_panel( PANEL *pan );
 	SYS V Curses	int bottom_panel( PANEL *pan );

**man-end**********************************************************************/

int
bottom_panel(pan)
register PANEL *pan;
{
	if(!pan)
		return(ERR);
	if(pan == __bottom_panel)
		return(OK);
	if(__panel_is_linked(pan))
		(void)hide_panel(pan);
	__panel_link_bottom(pan);
	return(OK);
}	/* end of bottom_panel */

/*man-start*********************************************************************

  del_panel	- deletes a panel

  PDCurses Description:
 	This function deletes pan but not its associated winwow.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int del_panel( PANEL *pan );
 	SYS V Curses	int del_panel( PANEL *pan );

**man-end**********************************************************************/

int
del_panel(pan)
register PANEL *pan;
{
	if(pan)
	{
		if(__panel_is_linked(pan))
			(void)hide_panel(pan);
		free((char *)pan);
		return(OK);
	}
	return(ERR);
}	/* end of del_panel */

/*man-start*********************************************************************

  hide_panel	- removes a panel from the deck

  PDCurses Description:
 	This function removes a panel from the deck and thus hides it from
 	view.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int hide_panel( PANEL *pan );
 	SYS V Curses	int hide_panel( PANEL *pan );

**man-end**********************************************************************/

int
hide_panel(pan)
register PANEL *pan;
{

	if(!pan)
		return(ERR);

	if(!__panel_is_linked(pan))
	{
		pan->above = (PANEL *)0;
		pan->below = (PANEL *)0;
		return(ERR);
	}

	__panel_unlink(pan);

	return(OK);
}	/* end of hide_panel */

/*man-start*********************************************************************

  move_panel	- move a window on the virtual screen

  PDCurses Description:
 	This function move the curses window associated with pan so that
 	its upper lefthand corner is at the supplied coordinates. Do not
 	use mvwin() on the window.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL or an error occurs when
 	trying to move the curses window.

  Portability:
 	PDCurses	int move_panel( PANEL *pan, int starty, int startx);
 	SYS V Curses	int move_panel( PANEL *pan, int starty, int startx);

**man-end**********************************************************************/

int
move_panel(pan,starty,startx)
PANEL *pan;
int starty;
int startx;
{
WINDOW *win;

	if(!pan)
		return(ERR);
	if(__panel_is_linked(pan))
		__override(pan,0);
	win = pan->win;
	if(mvwin(win,starty,startx) == ERR)
		return(ERR);
	pan->wstarty = getbegy(win);
	pan->wstartx = getbegx(win);
	pan->wendy = pan->wstarty + getmaxy(win);
	pan->wendx = pan->wstartx + getmaxx(win);
	if(__panel_is_linked(pan))
		__calculate_obscure();
	return(OK);
}	/* end of move_panel */

/*man-start*********************************************************************

  new_panel	- create a new panel

  PDCurses Description:
 	This function creates a new panel associated with win and returns
 	the panel pointer. The new panel is placed at the top of the deck.

  PDCurses Return Value:
 	Returns pointer to new panel, or NULL if an error occurs.

  PDCurses Errors:
 	Returns NULL if an error occurs.

  Portability:
 	PDCurses	PANEL *new_panel( WINDOW *win );
 	SYS V Curses	PANEL *new_panel( WINDOW *win );

**man-end**********************************************************************/

PANEL *
new_panel(win)
WINDOW *win;
{
PANEL *pan = (PANEL *)malloc(sizeof(PANEL));

	if(!__stdscr_pseudo_panel.win)
	{
		__stdscr_pseudo_panel.win = stdscr;
		__stdscr_pseudo_panel.wstarty = 0;
		__stdscr_pseudo_panel.wstartx = 0;
		__stdscr_pseudo_panel.wendy = LINES;
		__stdscr_pseudo_panel.wendx = COLS;
		__stdscr_pseudo_panel.user = "stdscr";
		__stdscr_pseudo_panel.obscure = (PANELOBS *)0;
	}

	if(pan)
	{
		pan->win = win;
		pan->above = (PANEL *)0;
		pan->below = (PANEL *)0;
		pan->wstarty = getbegy(win);
		pan->wstartx = getbegx(win);
		pan->wendy = pan->wstarty + getmaxy(win);
		pan->wendx = pan->wstartx + getmaxx(win);
#ifdef PANEL_DEBUG
		pan->user = "new";
#else
		pan->user = (char *)0;
#endif
		pan->obscure = (PANELOBS *)0;
		(void)show_panel(pan);
	}

	return(pan);
}	/* end of new_panel */

/*man-start*********************************************************************

  panel_above	- return pointer to panel above

  PDCurses Description:
 	This function returns a pointer to the panel in the deck above
 	pan. If the value of pan passed is NULL, this function returns
 	a pointer to the bottom panel in the deck.

  PDCurses Return Value:
 	Returns pointer to panel above pan, or NULL if pan is the top
 	panel.

  PDCurses Errors:
 	Returns NULL if an error occurs.

  Portability:
 	PDCurses	PANEL *panel_above( PANEL *pan );
 	SYS V Curses	PANEL *panel_above( PANEL *pan );

**man-end**********************************************************************/

PANEL *
panel_above(pan)
PANEL *pan;
{
	if(!pan)
		return(__bottom_panel);
	else
		return(pan->above);
}	/* end of panel_above */

/*man-start*********************************************************************

  panel_below	- return pointer to panel below

  PDCurses Description:
 	This function returns a pointer to the panel in the deck below
 	pan. If the value of pan passed is NULL, this function returns
 	a pointer to the top panel in the deck.

  PDCurses Return Value:
 	Returns pointer to panel below pan, or NULL if pan is the bottom
 	panel.

  PDCurses Errors:
 	Returns NULL if an error occurs.

  Portability:
 	PDCurses	PANEL *panel_below( PANEL *pan );
 	SYS V Curses	PANEL *panel_below( PANEL *pan );

**man-end**********************************************************************/

PANEL *
panel_below(pan)
PANEL *pan;
{
	if(!pan)
		return(__top_panel);
	else
		return(pan->below);
}	/* end of panel_below */

/*man-start*********************************************************************

  panel_hidden	- indicates if panel is hidden

  PDCurses Description:
 	This function returns TRUE if pan is hidden and FALSE if it is not.

  PDCurses Return Value:
 	TRUE or FALSE.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int panel_hidden( PANEL *pan );
 	SYS V Curses	int panel_hidden( PANEL *pan );

**man-end**********************************************************************/

int
panel_hidden(pan)
PANEL *pan;
{
	if(!pan)
		return(ERR);
	return(__panel_is_linked(pan) ? ERR : OK);
}	/* end of panel_hidden */

/*man-start*********************************************************************

  panel_userptr	- return user information

  PDCurses Description:
 	Each panel has a user pointer available for maintaining relevant
 	information. This function returns a pointer to that information
 	previously set up by set_panel_userptr().

  PDCurses Return Value:
 	Returns pointer to user information.

  PDCurses Errors:
 	Returns NULL if pan is NULL or no user information exists.

  Portability:
 	PDCurses	char *panel_userptr( PANEL *pan );
 	SYS V Curses	char *panel_userptr( PANEL *pan );

**man-end**********************************************************************/

char *
panel_userptr(pan)
PANEL *pan;
{
	if(!pan)
		return((char *)0);
	return(pan->user);
}	/* end of panel_userptr */

/*man-start*********************************************************************

  panel_window	- returns pointer to curses window

  PDCurses Description:
 	This function returns a pointer to the curses window associated
 	with the panel.

  PDCurses Return Value:
 	Pointer to panel's window.

  PDCurses Errors:
 	Return NULL on error.

  Portability:
 	PDCurses	WINDOW *panel_window(PANEL *);
 	SYS V Curses	WINDOW *panel_window(PANEL *);

**man-end**********************************************************************/

WINDOW *
panel_window(pan)
PANEL *pan;
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("panel_window() - called\n");
#endif
	return(pan->win);
}	/* end of panel_window */

/*man-start*********************************************************************

  replace_panel	- set curses window contents

  PDCurses Description:
 	This function replaces the current window of pan with win.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int replace_panel( PANEL *pan, WINDOW *win );
 	SYS V Curses	int replace_panel( PANEL *pan, WINDOW *win );

**man-end**********************************************************************/

int
replace_panel(pan,win)
PANEL *pan;
WINDOW *win;
{
	if(!pan)
		return(ERR);
	if(__panel_is_linked(pan))
		__override(pan,0);
	pan->win = win;
	pan->wstarty = getbegy(win);
	pan->wstartx = getbegx(win);
	pan->wendy = pan->wstarty + getmaxy(win);
	pan->wendx = pan->wstartx + getmaxx(win);
	if(__panel_is_linked(pan))
		__calculate_obscure();
	return(OK);
}	/* end of replace_panel */

/*man-start*********************************************************************

  set_panel_userptr	- sets user information for a panel

  PDCurses Description:
 	Each panel has a user pointer available for maintaining relevant
 	information. This function sets the value of that information.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int set_panel_userptr( PANEL *pan, char *uptr );
 	SYS V Curses	int set_panel_userptr( PANEL *pan, char *uptr );

**man-end**********************************************************************/

int
set_panel_userptr(pan,uptr)
PANEL *pan;
char *uptr;
{
	if(!pan)
		return(ERR);
	pan->user = uptr;
	return(OK);
}	/* end of set_panel_userptr */

/*man-start*********************************************************************

  show_panel	- displays a panel

  PDCurses Description:
 	This function makes a previously hidden panel visible and places
 	it back in the deck on top.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int show_panel( PANEL *pan );
 	SYS V Curses	int show_panel( PANEL *pan );

**man-end**********************************************************************/

int
show_panel(pan)
register PANEL *pan;
{

	if(!pan)
		return(ERR);
	if(pan == __top_panel)
		return(OK);
	if(__panel_is_linked(pan))
		(void)hide_panel(pan);
	__panel_link_top(pan);
	return(OK);
}	/* end of show_panel */

/*man-start*********************************************************************

  top_panel	- puts panel on top of deck

  PDCurses Description:
 	This function places pan on the top of the deck. The size, location
 	and contents of the panel are unchanged.

  PDCurses Return Value:
 	Returns OK or ERR.

  PDCurses Errors:
 	Returns ERR if pan is NULL.

  Portability:
 	PDCurses	int top_panel( PANEL *pan );
 	SYS V Curses	int top_panel( PANEL *pan );

**man-end**********************************************************************/

int
top_panel(pan)
register PANEL *pan;
{
	return(show_panel(pan));
}	/* end of top_panel */

/*man-start*********************************************************************

  update_panels	- panels virtual screen refresh routine

  PDCurses Description:
 	This function refreshes the virtual screen to reflect the depth
 	relationships between the panels in the deck. The user must use
 	doupdate() to refresh the physical screen.

  PDCurses Return Value:
 	None

  PDCurses Errors:
 	None

  Portability:
 	PDCurses	void update_panels( void )
 	SYS V Curses	void update_panels( void )

**man-end**********************************************************************/

void
update_panels()
{
PANEL *pan;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("update_panels() - called\n");
#endif

	pan = __bottom_panel;
	while(pan)
	{
		__override(pan,-1);
		pan = pan->above;
	}

	if(is_wintouched(stdscr))
		Wnoutrefresh(&__stdscr_pseudo_panel);
	
	pan = __bottom_panel;
	if(pan)
	{
		while(pan)
		{
			if(is_wintouched(pan->win) || !pan->above)
				Wnoutrefresh(pan);
			pan = pan->above;
		}
	}
}	/* end of update_panels */

/* end of panel.c */
