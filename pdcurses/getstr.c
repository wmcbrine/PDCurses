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
#define CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef getstr
#undef wgetstr
#undef mvgetstr
#undef mvwgetstr
#undef ungetstr
#undef getnstr
#undef wgetnstr

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
# undef wrefresh
# undef waddch
# undef wgetch
# undef cbreak
# undef move
# undef wmove
#endif

#ifdef PDCDEBUG
char *rcsid_getstr  = "$Id: getstr.c,v 1.5 2002/11/27 11:23:43 mark Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                        getstr

  Synopsis:
    int getstr(char *str);
    int wgetstr(WINDOW *win, char *str);
    int mvgetstr(int y, int x, char *str);
    int mvwgetstr(WINDOW *win, int y, int x, char *str);
    int getnstr(char *str, int ch);
    int wgetnstr(WINDOW *win, char *str, int ch);

  X/Open Description:
    The effect of getstr() is as though a series of calls to getch()
    were made, until a newline or carriage return is received. The
    resulting value is placed in the area pointed to by *str. The user's
    erase and kill characters are interpreted, as well as any special
    keys; such as function keys.

    With wgetnstr(), a series of characters are read until a newline
    or carriage return is received.  The resulting value is placed
    in the area pointed to by the character pointer str.  The user's
    erase and kill characters are interpreted.  This differs from
    wgetstr() in that the number of characters read is limited by a passed
    argument.

    NOTE: getstr(), getnstr(), mvgetstr() and mvwgetstr() are
    implemented as macros.

    WARNING:  There is no way to know how long the buffer passed to
       wgetstr() is, so it is possible to overwrite wrong
       memory or code!! This is the reason for the wgetnstr()
       function :-)

  X/Open Return Value:
    This functions return ERR on failure or any other value on success.

  X/Open Errors:
    No errors are defined for this function.

  Portability                             X/Open    BSD    SYS V
                                          Dec '88
      getstr                                Y        Y       Y
      wgtdch                                Y        Y       Y
      mvgetstr                              Y        Y       Y
      mvwgetstr                             Y        Y       Y
      getnstr                               -        -      4.0
      wgetnstr                              -        -      4.0

**man-end**********************************************************************/

#define MAXLINE 255

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL getstr(char *str)
#else
int PDC_CDECL getstr(str)
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("getstr() - called\n");
#endif

   if (stdscr == (WINDOW *)NULL)
      return (ERR);
   return(wgetnstr(stdscr,str,MAXLINE));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL wgetstr(WINDOW *win, char *str)
#else
int PDC_CDECL wgetstr(win,str)
WINDOW *win;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("wgetstr() - called\n");
#endif

   if (win == (WINDOW *)NULL)
      return (ERR);

   return(wgetnstr(win,str,MAXLINE));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL mvgetstr(int y, int x, char *str)
#else
int PDC_CDECL mvgetstr(y,x,str)
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("mvgetstr() - called\n");
#endif

   if (stdscr == (WINDOW *)NULL)
      return (ERR);
   if (move(y,x) == ERR)
      return (ERR);
   return(wgetnstr(stdscr,str,MAXLINE));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL mvwgetstr(WINDOW *win, int y, int x, char *str)
#else
int PDC_CDECL mvwgetstr(win,y,x,str)
WINDOW *win;
int y;
int x;
char *str;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("mvwgetstr() - called\n");
#endif

   if (win == (WINDOW *)NULL)
      return (ERR);
   if (wmove(win,y,x) == ERR)
      return (ERR);
   return(wgetnstr(win,str,MAXLINE));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL getnstr(char *str, int n)
#else
int PDC_CDECL getnstr(str,n)
char *str;
int n;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
   if (trace_on) PDC_debug("getnstr() - called\n");
#endif

   if (stdscr == (WINDOW *)NULL)
      return (ERR);
   return(wgetnstr(stdscr,str,n));
}
/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_CDECL wgetnstr(WINDOW *win, char *str, int n)
#else
int PDC_CDECL wgetnstr(win,str,n)
WINDOW *win;
char *str;
int n;
#endif
/***********************************************************************/
{
   int   ch, i, num,chars=0;
   int   t = win->_tabsize;
   int   x = win->_curx;
   char* p = str;
   bool  stop = FALSE;
   bool  oldecho;
   bool  oldcbreak;
   bool  oldnodelay;

#ifdef PDCDEBUG
   if (trace_on) PDC_debug("wgetnstr() - called\n");
#endif

   if (win == (WINDOW *)NULL)
      return (ERR);

#ifdef UNIX
/*
 * this code is very dodgy
 *
   wrefresh(win);

   while ((*str = wgetch(win)) != ERR && *str != '\n')
      ;
   if (*str == ERR) {
      *str = '\0';
      waddstr(win,p);
      return ERR;
   }
   *str = '\0';
   waddstr(win,p);
   return OK;
*/
#else
   oldcbreak = SP->cbreak; /* remember states    */
   oldecho = SP->echo;
   oldnodelay = win->_nodelay;

   SP->echo = FALSE;    /* we do echo ourselves */
   cbreak();      /* ensure each key is returned immediately */
   win->_nodelay = FALSE;     /* don't return   -1  */

   wrefresh (win);

   while (!stop)
   {
      ch = wgetch (win);
      /* ignore modifier keys on their own */
      if ( ch == KEY_SHIFT_L
      ||   ch == KEY_SHIFT_R
      ||   ch == KEY_CONTROL_L
      ||   ch == KEY_CONTROL_R
      ||   ch == KEY_ALT_L
      ||   ch == KEY_ALT_R )
         continue;
      ch = ch & A_CHARTEXT;
      switch( ch )
      {
         case '\t':
            ch = ' ';
            num = t - (win->_curx - x)%t;
            for (i=0; i<num; i++)
            {
               if (chars < n)
               {
                  if (oldecho) 
                     waddch (win, ch);
                  *p++ = ch;
                  ++chars;
               }
               else
                  beep();
            }
            break;

         case _ECHAR:  /* CTRL-H */  /* Delete character */
            if (p > str)
            {
               if (oldecho) 
                  waddstr (win, "\b \b");
               ch = *--p;
               if ((ch < ' ') && (oldecho))
                  waddstr (win, "\b \b");
            }
            chars--;
            break;

         case _DLCHAR:  /* CTRL-U */    /* Delete line   */
            while (p > str)
            {
               if (oldecho) 
                  waddstr (win, "\b \b");
               ch = *--p;
               if ((ch < ' ') && (oldecho))
                  waddstr (win, "\b \b");
            }
            chars = 0;
            break;

         case _DWCHAR: /* CTRL-W */ /* Delete word */
            while ((p > str) && (*(p-1) == ' '))
            {
               if (oldecho) 
                  waddstr (win, "\b \b");
               --p; /* remove space */
               chars--;
            }
            while ((p > str) && (*(p-1) != ' '))
            {
               if (oldecho) 
                  waddstr (win, "\b \b");
               ch = *--p;
               if ((ch < ' ') && (oldecho))
                  waddstr (win, "\b \b");
               chars--;
            }
            break;

         case '\n':
         case '\r':
            stop = TRUE;
            if (oldecho) 
               waddch (win, '\n');
            break;

         default:
            if (chars < n)
            {
               *p++ = ch;
               if (oldecho) 
                  waddch (win, ch);
               chars ++;
            }
            else
               beep();
            break;
      }
      wrefresh (win);
   }
   *p = '\0';

   SP->echo = oldecho;  /* restore old settings */
   SP->cbreak = oldcbreak;
   win->_nodelay = oldnodelay;

   return (OK);
#endif
}
