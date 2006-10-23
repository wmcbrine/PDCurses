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

#include <curspriv.h>

RCSID("$Id: scr_dump.c,v 1.19 2006/10/23 05:03:31 wmcbrine Exp $");

/*man-start**************************************************************

  Name:                                                       scr_dump

  Synopsis:
	int putwin(WINDOW *win, FILE *filep);
	WINDOW *getwin(FILE *filep);
	int scr_dump(const char *filename);
	int scr_init(const char *filename);
	int scr_restore(const char *filename);
	int scr_set(const char *filename);

  X/Open Description:
	The getwin() function reads window-related data stored in the 
	file by putwin(). The function then creates and initialises a 
	new window using that data.

	The putwin() function writes all data associated with win into 
	the stdio stream to which filep points, using an unspecified 
	format. This information can be retrieved later using getwin().

	The scr_dump() function writes the current contents of the 
	virtual screen to the file named by filename in an unspecified 
	format.

	The scr_restore() function sets the virtual screen to the 
	contents of the file named by filename, which must have been 
	written using scr_dump(). The next refresh operation restores 
	the screen to the way it looked in the dump file.

	The scr_init() function reads the contents of the file named by 
	filename and uses them to initialise the Curses data structures 
	to what the terminal currently has on its screen. The next 
	refresh operation bases any updates on this information, unless 
	either of the following conditions is true:

	* The terminal has been written to since the virtual screen was 
	  dumped to filename

	* The terminfo capabilities rmcup and nrrmc are defined for the 
	  current terminal.

	The scr_set() function is a combination of scr_restore() and 
	scr_init(). It tells the program that the information in the 
	file named by filename is what is currently on the screen, and 
	also what the program wants on the screen. This can be thought 
	of as a screen inheritance function.

  PDCurses Description:
	These functions are not yet meaningfully implemented in 
	PDCurses. This file is a placeholder.

  X/Open Return Value:
	On successful completion, getwin() returns a pointer to the 
	window it created. Otherwise, it returns a null pointer. Other 
	functions return OK or ERR.

  Portability				     X/Open    BSD    Sys V
	putwin					Y
	getwin					Y
	scr_dump				Y
	scr_init				Y
	scr_restore				Y
	scr_set					Y

**man-end****************************************************************/

int putwin(WINDOW *win, FILE *filep)
{
        PDC_LOG(("putwin() - called\n"));

        return ERR;
}

WINDOW *getwin(FILE *filep)
{
        PDC_LOG(("getwin() - called\n"));

        return (WINDOW *)NULL;
}

int scr_dump(const char *filename)
{
	PDC_LOG(("scr_dump() - called: filename %s\n", filename));

	return ERR;
}

int scr_init(const char *filename)
{
	PDC_LOG(("scr_init() - called: filename %s\n", filename));

	return ERR;
}

int scr_restore(const char *filename)
{
	PDC_LOG(("scr_restore() - called: filename %s\n", filename));

	return ERR;
}

int scr_set(const char *filename)
{
	PDC_LOG(("scr_set() - called: filename %s\n", filename));

	return ERR;
}
