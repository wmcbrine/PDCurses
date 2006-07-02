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
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcx11.h"

#include <stdlib.h>

RCSID("$Id: pdcclip.c,v 1.18 2006/07/02 19:03:59 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_getclipboard()	- Gets the contents of the clipboard

  PDCurses Description:
	This is a PDCurses only routine.

	Gets the textual contents of the system's clipboard. This
	function returns the contents of the clipboard in the contents
	argument. It is the responsibilitiy of the caller to free the 
	memory returned with the PDC_freeclipboard() call.  The length 
	of the clipboard contents is returned in the length argument.

  PDCurses Return Value:
	indicator of success/failure of call.
	PDC_CLIP_SUCCESS	the call was successful
	PDC_CLIP_ACCESS_ERROR	an error occured while accessing the
				clipboard
	PDC_CLIP_MEMORY_ERROR	unable to allocate sufficient memory for 
				the clipboard contents
	PDC_CLIP_EMPTY	the clipboard contains no text

  Portability:
	PDCurses  int PDC_getclipboard(char **contents, long *length);

**man-end****************************************************************/

int PDC_getclipboard(char **contents, long *length)
{
	int result = 0;
	int len;

	PDC_LOG(("PDC_getclipboard() - called\n"));

	XCursesInstructAndWait(CURSES_GET_SELECTION);

	if (read_socket(display_sock, (char *)&result, sizeof(int)) < 0)
	    XCursesExitCursesProcess(5,
		"exiting from XCurses_getclipboard");

	if (result == PDC_CLIP_SUCCESS)
	{
	    if (read_socket(display_sock, (char *)&len, sizeof(int)) < 0)
		XCursesExitCursesProcess(5,
		    "exiting from XCurses_getclipboard");

	    if (len != 0)
	    {
		*contents = (char *)malloc(len + 1);

		if (!*contents)
		    XCursesExitCursesProcess(6, "exiting from "
			"XCurses_getclipboard - synchronization error");

		if (read_socket(display_sock, *contents, len) < 0)
		    XCursesExitCursesProcess(5,
			"exiting from XCurses_getclipboard");

		*length = len;
	    }
	}

	return result;
}

/*man-start**************************************************************

  PDC_setclipboard()	- Sets the contents of the clipboard

  PDCurses Description:
	This is a PDCurses only routine.

	Copies the supplied text into the system's clipboard, emptying
	the clipboard prior to the copy.

  PDCurses Return Value:
	indicator of success/failure of call.
	PDC_CLIP_SUCCESS	the call was successful
	PDC_CLIP_ACCESS_ERROR	an error occured while accessing the
				clipboard

  Portability:
	PDCurses  int PDC_getclipboard(const char *contents, long length);

**man-end****************************************************************/

int PDC_setclipboard(const char *contents, long length)
{
	int rc;

	PDC_LOG(("PDC_setclipboard() - called\n"));

	XCursesInstruct(CURSES_SET_SELECTION);

	/* Write, then wait for X to do its stuff; expect return code. */

	if (write_socket(display_sock, (char *)&length, sizeof(long)) >= 0)
	    if (write_socket(display_sock, contents, length) >= 0)
		if (read_socket(display_sock, (char *)&rc, sizeof(int)) >= 0)
		    return rc;

	XCursesExitCursesProcess(5, "exiting from PDC_setclipboard");

	return ERR;	/* not reached */
}

/*man-start**************************************************************

  PDC_freeclipboard()	- Frees the memory associated with the contents
			  of the clipboard

  PDCurses Description:
	This is a PDCurses only routine.

	Frees the memory allocated by PDC_getclipboard().

  PDCurses Return Value:
	Always returns PDC_CLIP_SUCCESS

  Portability:
	PDCurses  int PDC_freeclipboard(char *contents);

**man-end****************************************************************/

int PDC_freeclipboard(char *contents)
{
	PDC_LOG(("PDC_freeclipboard() - called\n"));

	free(contents);
	return PDC_CLIP_SUCCESS;
}

/*man-start**************************************************************

  PDC_clearclipboard()  - Clears the contents of the clipboard

  PDCurses Description:
	This is a PDCurses only routine.

	Clears the internal clipboard.

  PDCurses Return Value:
	Always returns PDC_CLIP_SUCCESS

  Portability:
	PDCurses  int PDC_clearclipboard(void);

**man-end****************************************************************/

int PDC_clearclipboard(void)
{
	int rc;
	long len = 0;

	PDC_LOG(("PDC_clearclipboard() - called\n"));

	XCursesInstruct(CURSES_CLEAR_SELECTION);

	/* Write, then wait for X to do its stuff; expect return code. */

	if (write_socket(display_sock, (char *)&len, sizeof(long)) >= 0)
	    if (read_socket(display_sock, (char *)&rc, sizeof(int)) >= 0)
		return rc;

	XCursesExitCursesProcess(5, "exiting from PDC_clearclipboard");

	return ERR;	/* not reached */
}
