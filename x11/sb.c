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
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef	sb_init
#undef	sb_set_horz
#undef	sb_set_vert
#undef	sb_get_horz
#undef	sb_get_vert
#undef	sb_refresh

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_sb  = "$Id: sb.c,v 1.2 2002/07/04 10:23:50 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                          sb

  Synopsis:

  	int sb_init(void)
  	int sb_set_horz(int total, int viewport, int cur)
  	int sb_set_vert(int total, int viewport, int cur)
  	int sb_get_horz(int *total, int *viewport, int *cur)
  	int sb_get_vert(int *total, int *viewport, int *cur)
  	int sb_refresh(void);

  PDCurses Description:
 	These functions manipulate the scrollbar...

  PDCurses Return Value:
 	All functions return OK on success and ERR on error.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      sb_init                               -        -       -
      sb_set_horz                           -        -       -
      sb_set_vert                           -        -       -
      sb_get_horz                           -        -       -
      sb_get_vert                           -        -       -
      sb_refresh                            -        -       -

**man-end**********************************************************************/

bool sb_started = FALSE;

/***********************************************************************/
/*
   sb_init()	is the sb initialization routine.
   This must be called before initscr().
 */
#ifdef HAVE_PROTO
int	PDC_CDECL	sb_init(void)
#else
int	PDC_CDECL	sb_init()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("sb_init() - called\n");
#endif

	if (SP)
		return (ERR);

	sb_started = TRUE;
	return(OK);
}

/***********************************************************************/
/*
	sb_set_horz()	Used to set horizontal scrollbar.

   total = total number of columns
   viewport = size of viewport in columns
   cur = current column in total
 */
#ifdef HAVE_PROTO
int	PDC_CDECL	sb_set_horz(int total, int viewport, int cur)
#else
int	PDC_CDECL	sb_set_horz(total, viewport, cur)
int total, viewport, cur;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("sb_set_horz() - called: total %d viewport %d cur %d\n",total, viewport, cur);
#endif
	if (!SP)
		return(ERR);

	SP->sb_total_x = total;
	SP->sb_viewport_x = viewport;
	SP->sb_cur_x = cur;
	return( OK );
}

/***********************************************************************/
/*
	sb_set_vert()	Used to set vertical scrollbar.

   total = total number of columns on line
   viewport = size of viewport in columns
   cur = current column in total
 */
#ifdef HAVE_PROTO
int	PDC_CDECL	sb_set_vert(int total, int viewport, int cur)
#else
int	PDC_CDECL	sb_set_vert(total, viewport, cur)
int total, viewport, cur;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("sb_set_vert() - called: total %d viewport %d cur %d\n",total, viewport, cur);
#endif
	if (!SP)
		return(ERR);

	SP->sb_total_y = total;
	SP->sb_viewport_y = viewport;
	SP->sb_cur_y = cur;
	return( OK );
}

/***********************************************************************/
/*
	sb_get_horz()	Used to get horizontal scrollbar.

   total = total number of lines
   viewport = size of viewport in lines
   cur = current line in total
 */
#ifdef HAVE_PROTO
int	PDC_CDECL	sb_get_horz(int *total, int *viewport, int *cur)
#else
int	PDC_CDECL	sb_get_horz(total, viewport, cur)
int *total, *viewport, *cur;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("sb_get_horz() - called\n");
#endif
	if (!SP)
		return(ERR);

	if (total) *total = SP->sb_total_x;
	if (viewport) *viewport = SP->sb_viewport_x;
	if (cur) *cur = SP->sb_cur_x;
	return( OK );
}

/***********************************************************************/
/*
	sb_get_vert()	Used to get vertical scrollbar.

   total = total number of lines
   viewport = size of viewport in lines
   cur = current line in total
 */
#ifdef HAVE_PROTO
int	PDC_CDECL	sb_get_vert(int *total, int *viewport, int *cur)
#else
int	PDC_CDECL	sb_get_vert(total, viewport, cur)
int *total, *viewport, *cur;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("sb_get_vert() - called\n");
#endif
	if (!SP)
		return(ERR);

	if (total) *total = SP->sb_total_y;
	if (viewport) *viewport = SP->sb_viewport_y;
	if (cur) *cur = SP->sb_cur_y;
	return( OK );
}

/***********************************************************************/
/*
	sb_refresh()	Used to draw the scrollbars.
 */
#ifdef HAVE_PROTO
int	PDC_CDECL	sb_refresh(void)
#else
int	PDC_CDECL	sb_refresh()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("sb_refresh() - called\n");
#endif

	if (!SP)
		return(ERR);

	return(XCurses_refresh_scrollbar());
}
