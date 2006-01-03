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
#define CURSES_LIBRARY  1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

/* undefine any macros for functions defined in this module */
#undef scr_dump
#undef scr_init
#undef scr_restore
#undef scr_set

/* undefine any macros for functions called by this module if in debug mode */
#ifdef PDCDEBUG
#endif

#ifdef PDCDEBUG
char *rcsid_terminfo  = "$Id: scr_dump.c,v 1.2 2006/01/03 07:34:43 wmcbrine Exp $";
#endif

/*man-start*********************************************************************

  Name:                                                       scr_dump

  Synopsis:
    int scr_dump(const char *filename);
    int scr_init(const char *filename);
    int scr_restore(const char *filename);
    int scr_set(const char *filename);

  X/Open Description:
    The scr_dump() function writes the current contents of the virtual 
    screen to the file named by filename in an unspecified format.

    The scr_restore() function sets the virtual screen to the contents 
    of the file named by filename, which must have been written using 
    scr_dump(). The next refresh operation restores the screen to the 
    way it looked in the dump file.

    The scr_init() function reads the contents of the file named by 
    filename and uses them to initialise the Curses data structures to 
    what the terminal currently has on its screen. The next refresh 
    operation bases any updates on this information, unless either of 
    the following conditions is true:

        * The terminal has been written to since the virtual screen was 
          dumped to filename

        * The terminfo capabilities rmcup and nrrmc are defined for the 
          current terminal.

    The scr_set() function is a combination of scr_restore() and 
    scr_init(). It tells the program that the information in the file 
    named by filename is what is currently on the screen, and also what 
    the program wants on the screen. This can be thought of as a screen 
    inheritance function.

  PDCurses Description:
    These functions are not yet meaningfully implemented in PDCurses. 
    This file is a placeholder.

  X/Open Return Value:
    On successful completion, these functions return OK. Otherwise, they 
    return ERR.

  X/Open Errors:
    No errors are defined. 

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int scr_dump(const char *filename)
#else
int scr_dump(filename)
char *filename;
#endif
/***********************************************************************/
{
	PDC_LOG(("scr_dump() - called: filename %s\n", filename));

	return ERR;
}
/***********************************************************************/
#ifdef HAVE_PROTO
int scr_init(const char *filename)
#else
int scr_init(filename)
char *filename;
#endif
/***********************************************************************/
{
	PDC_LOG(("scr_init() - called: filename %s\n", filename));

	return ERR;
}
/***********************************************************************/
#ifdef HAVE_PROTO
int scr_restore(const char *filename)
#else
int scr_restore(filename)
char *filename;
#endif
/***********************************************************************/
{
	PDC_LOG(("scr_restore() - called: filename %s\n", filename));

	return ERR;
}
/***********************************************************************/
#ifdef HAVE_PROTO
int scr_set(const char *filename)
#else
int scr_set(filename)
char *filename;
#endif
/***********************************************************************/
{
	PDC_LOG(("scr_set() - called: filename %s\n", filename));

	return ERR;
}
