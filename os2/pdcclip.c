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

#define CURSES_LIBRARY 1
#ifndef EMXVIDEO
# define INCL_DOS
# define INCL_WIN
#endif
#include <curses.h>
#include <stdlib.h>
#include <string.h>

RCSID("$Id: pdcclip.c,v 1.20 2006/03/29 20:06:40 wmcbrine Exp $");

/*man-start**************************************************************

  PDC_getclipboard() - Gets the contents of the clipboard

  PDCurses Description:
	This is a PDCurses only routine.

	Gets the textual contents of the system's clipboard. This 
	function returns the contents of the clipboard in the contents 
	argument. It is the responsibilitiy of the caller to free the 
	memory returned with the PDC_freeclipboard() call.  The length 
	of the clipboard contents is returned in the length argument.

  PDCurses Return Value:
	indicator of success/failure of call.
	PDC_CLIP_SUCCESS  the call was successful
	PDC_CLIP_ACCESS_ERROR   an error occured while accessing the
				clipboard
	PDC_CLIP_MEMORY_ERROR   unable to allocate sufficient memory for 
				the clipboard contents
	PDC_CLIP_EMPTY the clipboard contains no text

  Portability:
	PDCurses  int PDC_getclipboard(char **contents, long *length);

**man-end****************************************************************/

int PDC_getclipboard(char **contents, long *length)
{
#ifndef EMXVIDEO
	HMQ hmq;
	HAB hab;
	PTIB ptib;
	PPIB ppib;
	ULONG ulRet;
	long len;
	int rc;
#endif
	PDC_LOG(("PDC_getclipboard() - called\n"));

#ifndef EMXVIDEO
	DosGetInfoBlocks(&ptib, &ppib);
	ppib->pib_ultype = 3;
	hab = WinInitialize(0);
	hmq = WinCreateMsgQueue(hab, 0);

	if (!WinOpenClipbrd(hab))
	{
		WinDestroyMsgQueue(hmq);
		WinTerminate(hab);
		return PDC_CLIP_ACCESS_ERROR;
	}

	rc = PDC_CLIP_EMPTY;

	ulRet = WinQueryClipbrdData(hab, CF_TEXT);

	if (ulRet)
	{
		len = strlen((char *)ulRet);
		*contents = (char *)malloc(len + 1);

		if (!*contents)
			rc = PDC_CLIP_MEMORY_ERROR;
		else
		{
			strcpy((char *)*contents, (char *)ulRet);
			*length = len;
			rc = PDC_CLIP_SUCCESS;
		}
	}

	WinCloseClipbrd(hab);
	WinDestroyMsgQueue(hmq);
	WinTerminate(hab);

	return rc;
#else
	return PDC_CLIP_ACCESS_ERROR;
#endif
}

/*man-start**************************************************************

  PDC_setclipboard() - Sets the contents of the clipboard

  PDCurses Description:
	This is a PDCurses only routine.

	Copies the supplied text into the system's clipboard, emptying
	the clipboard prior to the copy.

  PDCurses Return Value:
	indicator of success/failure of call.
	PDC_CLIP_SUCCESS  the call was successful
	PDC_CLIP_ACCESS_ERROR   an error occured while accessing the
				clipboard

  Portability:
	PDCurses  int PDC_getclipboard(const char *contents, long length);

**man-end****************************************************************/

int PDC_setclipboard(const char *contents, long length)
{
#ifndef EMXVIDEO
	HAB hab;
	PTIB ptib;
	PPIB ppib;
	ULONG ulRC;
	PSZ szTextOut = NULL;
	int rc;
#endif
	PDC_LOG(("PDC_setclipboard() - called\n"));

#ifndef EMXVIDEO
	DosGetInfoBlocks(&ptib, &ppib);
	ppib->pib_ultype = 3;
	hab = WinInitialize(0);

	if (!WinOpenClipbrd(hab))
	{
		WinTerminate(hab);
		return PDC_CLIP_ACCESS_ERROR;
	}

	rc = PDC_CLIP_MEMORY_ERROR;

	ulRC = DosAllocSharedMem((PVOID)&szTextOut, NULL, length + 1,
		PAG_WRITE | PAG_COMMIT | OBJ_GIVEABLE);

	if (ulRC == 0)
	{
		strcpy(szTextOut, contents);
		WinEmptyClipbrd(hab);

		if (WinSetClipbrdData(hab, (ULONG)szTextOut,
		    CF_TEXT, CFI_POINTER))
			rc = PDC_CLIP_SUCCESS;
		else
		{
			DosFreeMem(szTextOut);
			rc = PDC_CLIP_ACCESS_ERROR;
		}
	}

	WinCloseClipbrd(hab);
	WinTerminate(hab);

	return rc;
#else
	return PDC_CLIP_ACCESS_ERROR;
#endif
}

/*man-start**************************************************************

  PDC_freeclipboard()   - Frees the memory associated with the contents
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

	if (contents)
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
#ifndef EMXVIDEO
	HAB hab;
	PTIB ptib;
	PPIB ppib;
#endif
	PDC_LOG(("PDC_clearclipboard() - called\n"));

#ifndef EMXVIDEO
	DosGetInfoBlocks(&ptib, &ppib);
	ppib->pib_ultype = 3;
	hab = WinInitialize(0);

	WinEmptyClipbrd(hab);

	WinCloseClipbrd(hab);
	WinTerminate(hab);

	return PDC_CLIP_SUCCESS;
#else
	return PDC_CLIP_ACCESS_ERROR;
#endif
}
