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
#define  CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCsetsc  = "$Id: pdcsetsc.c,v 1.3 2002/01/12 04:04:18 mark Exp $";
#endif

/*man-start*********************************************************************

  PDC_set_80x25() - force a known screen state: 80x25 text mode.

  PDCurses Description:
   This is a private PDCurses function.

   Forces the appropriate 80x25 alpha mode given the display adapter.

   Since we currently do not support changing the virtual console size,
   this routine is a NOP under Flexos.

  PDCurses Return Value:
   This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
   No errors are defined for this routine.

  Portability:
   PDCurses int   PDC_set_80x25( void );

**man-end**********************************************************************/

/***********************************************************************/
int PDC_set_80x25(void)
/***********************************************************************/
{
#ifndef EMXVIDEO
   VIOMODEINFO modeInfo={0};
#endif
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_set_80x25() - called\n");
#endif

#ifndef EMXVIDEO
   modeInfo.cb = sizeof(modeInfo);
   /* set most parameters of modeInfo */
   VioGetMode(&modeInfo, 0);
   modeInfo.fbType = 1;
   VioSetMode(&modeInfo, 0);
#endif
   return( OK );
}

/*man-start*********************************************************************

  PDC_set_cursor_mode() - Set the cursor start and stop scan lines.

  PDCurses Description:
   Sets the cursor type to begin in scan line startrow and end in
   scan line endrow.  Both values should be 0-31.

  PDCurses Return Value:
   This function returns OK on success and ERR on error.

  PDCurses Errors:
   No errors are defined for this function.

  Portability:
   PDCurses int PDC_set_cursor_mode( int startrow, int endrow );

**man-end**********************************************************************/

/***********************************************************************/
int PDC_set_cursor_mode( int startrow, int endrow )
/***********************************************************************/
{
#ifndef EMXVIDEO
   VIOCURSORINFO cursorInfo={0};
#endif
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_set_cursor_mode() - called: startrow %d endrow %d\n",startrow,endrow);
#endif

#ifdef EMXVIDEO
   if (endrow <= startrow)
      v_hidecursor();
   else
      v_ctype (startrow, endrow);
   return( OK );
#else
   cursorInfo.yStart = startrow;
   cursorInfo.cEnd = endrow;
   cursorInfo.cx = 1;
   cursorInfo.attr = 0;
   return (VioSetCurType (&cursorInfo, 0) == 0);
#endif
}

/*man-start*********************************************************************

  PDC_set_font()  - sets the current font size

  PDCurses Description:
   This is a private PDCurses function.

   This routine sets the current font size, if the adapter allows
   such a change.

  PDCurses Return Value:
   This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
   It is an error to attempt to change the font size on a "bogus"
   adapter.  The reason for this is that we have a known video
   adapter identity problem.  e.g. Two adapters report the same
   identifying characteristics.

   It is also an error to attempt to change the size of the Flexos
   console (as there is currently no support for that).

  Portability:
   PDCurses int   PDC_set_font( int size );

**man-end**********************************************************************/

/***********************************************************************/
int PDC_set_font(int size)
/***********************************************************************/
{
#ifndef EMXVIDEO
   VIOMODEINFO modeInfo={0};
#endif
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_set_font() - called\n");
#endif

#ifndef EMXVIDEO
   if (SP->sizeable && (SP->font != size))
   {
      modeInfo.cb = sizeof(modeInfo);
      /* set most parameters of modeInfo */
      VioGetMode(&modeInfo, 0);
      modeInfo.cb = 8;     /* ignore horiz an vert resolution */
      modeInfo.row = modeInfo.vres / size;
      VioSetMode(&modeInfo, 0);
   }
   if (SP->visible_cursor)
      PDC_cursor_on();
   else
      PDC_cursor_off();
   SP->font = PDC_get_font();
#endif
   return( OK );
}

/*man-start*********************************************************************

  PDC_set_rows()  - sets the physical number of rows on screen

  PDCurses Description:
   This is a private PDCurses function.

   This routine attempts to set the number of rows on the physical
   screen to the passed value.

  PDCurses Return Value:
   This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
   It is an error to attempt to change the screen size on a "bogus"
   adapter.  The reason for this is that we have a known video
   adapter identity problem.  e.g. Two adapters report the same
   identifying characteristics.

   It is also an error to attempt to change the size of the Flexos
   console (as there is currently no support for that).

  Portability:
   PDCurses int   PDC_set_rows( int rows );

**man-end**********************************************************************/

/***********************************************************************/
int PDC_set_rows(int rows)
/***********************************************************************/
{
#ifndef EMXVIDEO
   VIOMODEINFO modeInfo={0};
   USHORT result=0;
#endif
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_set_rows() - called\n");
#endif

#ifdef EMXVIDEO
   return (ERR);
#else
   modeInfo.cb = sizeof(modeInfo);
   /* set most parameters of modeInfo */
   VioGetMode(&modeInfo, 0);
   modeInfo.fbType = 1;
   modeInfo.row = rows;
   result = VioSetMode(&modeInfo, 0);
   SP->font = PDC_get_font();
   LINES = PDC_get_rows();
   COLS = PDC_get_columns();
   return ((result == 0) ? OK : ERR);
#endif
}

/*man-start*********************************************************************

  PDC_set_scrn_mode()   - Set BIOS Video Mode

  PDCurses Description:
   Sets the BIOS Video Mode Number ONLY if it is different from
   the current video mode.  This routine is for DOS systems only.

  PDCurses Return Value:
   This function returns OK on success and ERR on error.

  PDCurses Errors:
   No errors are defined for this function.

  Portability:
   PDCurses int PDC_set_scrn_mode( int new_mode );

**man-end**********************************************************************/

/***********************************************************************/
#if !defined( EMXVIDEO )
int PDC_set_scrn_mode(VIOMODEINFO new_mode)
#else
int PDC_set_scrn_mode(int new_mode)
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_set_scrn_mode() - called\n");
#endif

#ifdef EMXVIDEO
   return ( OK );
#else
   if (VioSetMode (&new_mode, 0) != 0)
   {
      SP->font = PDC_get_font();
      memcpy((char *)&SP->scrnmode,(char *)&new_mode,sizeof(VIOMODEINFO));
      LINES = PDC_get_rows();
      COLS = PDC_get_columns();
      return( OK );
   }
   else
      return (ERR);
#endif
}
/***********************************************************************/
int PDC_curs_set(int visibility)
/***********************************************************************/
{
#ifndef EMXVIDEO
 VIOCURSORINFO pvioCursorInfo;
#endif
 int start=0,end=0;
 int ret_vis=0,hidden=0;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_curs_set() - called: visibility=%d\n",visibility);
#endif

   ret_vis = SP->visibility;
   SP->visibility = visibility;

   switch(visibility)
   {
      case 0:  /* invisible */
#ifdef EMXVIDEO
         start = end = 0;
#else
         start = SP->font / 4;
         end = SP->font;
#endif
         hidden = (-1);
         break;
      case 2:  /* highly visible */
         start = 2;   /* almost full-height block */
         end = SP->font-1;
         break;
      default:  /* normal visibility */
         start = SP->font - (SP->font / 4);
         end = SP->font-1;
         break;
   }

#ifdef EMXVIDEO
   if (!visibility)
      v_hidecursor();
   else
      v_ctype (start, end);
#else
   pvioCursorInfo.yStart = (USHORT)start;
   pvioCursorInfo.cEnd = (USHORT)end;
   pvioCursorInfo.cx = (USHORT)1;
   pvioCursorInfo.attr = hidden;
   VioSetCurType((PVIOCURSORINFO)&pvioCursorInfo,0);
#endif
   return( ret_vis );
}

/*man-start*********************************************************************

  PDC_set_title() - Set window title

  PDCurses Description:
   Sets the title of the window in which the curses program is running.
   This function may not do anything on some platforms.

  PDCurses Return Value:
   N/A

  PDCurses Errors:
   No errors are defined for this function.

  Portability:
   PDCurses void PDC_set_title( char *title );

**man-end**********************************************************************/
/***********************************************************************/
void PDC_set_title(char *title)
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("PDC_set_title() - called:<%s>\n",title);
#endif
   return;
}
