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

#include "pdcwin.h"

RCSID("$Id: pdcclip.c,v 1.22 2007/01/02 13:48:31 wmcbrine Exp $");

/*man-start**************************************************************

  Name:								clipboard

  Synopsis:
	int PDC_getclipboard(char **contents, long *length);
	int PDC_setclipboard(const char *contents, long length);
	int PDC_freeclipboard(char *contents);
	int PDC_clearclipboard(void);

  PDCurses Description:
	PDC_getclipboard() gets the textual contents of the system's 
	clipboard. This function returns the contents of the clipboard 
	in the contents argument. It is the responsibilitiy of the 
	caller to free the memory returned, via PDC_freeclipboard().
	The length of the clipboard contents is returned in the length 
	argument.

	PDC_setclipboard copies the supplied text into the system's 
	clipboard, emptying the clipboard prior to the copy.

	PDC_clearclipboard() clears the internal clipboard.

  PDCurses Return Values:
	indicator of success/failure of call.
	PDC_CLIP_SUCCESS	the call was successful
	PDC_CLIP_MEMORY_ERROR	unable to allocate sufficient memory for 
				the clipboard contents
	PDC_CLIP_EMPTY		the clipboard contains no text
	PDC_CLIP_ACCESS_ERROR	no clipboard support

  Portability				     X/Open    BSD    SYS V
	PDC_getclipboard			-	-	-
	PDC_setclipboard			-	-	-
	PDC_freeclipboard			-	-	-
	PDC_clearclipboard			-	-	-

**man-end****************************************************************/

int PDC_getclipboard(char **contents, long *length)
{
	HANDLE handle;
	long len;

	PDC_LOG(("PDC_getclipboard() - called\n"));

	if (!OpenClipboard(NULL))
		return PDC_CLIP_ACCESS_ERROR;

	if ((handle = GetClipboardData(CF_TEXT)) == NULL)
	{
		CloseClipboard();
		return PDC_CLIP_EMPTY;
	}

	len = strlen((char *)handle);
	*contents = (char *)GlobalAlloc(GMEM_FIXED, len + 1);

	if (!*contents)
	{
		CloseClipboard();
		return PDC_CLIP_MEMORY_ERROR;
	}

	strcpy((char *)*contents, (char *)handle);
	*length = len;
	CloseClipboard();

	return PDC_CLIP_SUCCESS;
}

int PDC_setclipboard(const char *contents, long length)
{
	HGLOBAL ptr1;
	LPTSTR ptr2;

	PDC_LOG(("PDC_setclipboard() - called\n"));

	if (!OpenClipboard(NULL))
		return PDC_CLIP_ACCESS_ERROR;

	ptr1 = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, 
		(length + 1) * sizeof(TCHAR));

	if (!ptr1)
		return PDC_CLIP_MEMORY_ERROR;

	ptr2 = GlobalLock(ptr1);

	memcpy((char *)ptr2, contents, length + 1);
	GlobalUnlock(ptr1);
	EmptyClipboard();

	if (!SetClipboardData(CF_TEXT, ptr1))
	{
		GlobalFree(ptr1);
		return PDC_CLIP_ACCESS_ERROR;
	}

	CloseClipboard();
	GlobalFree(ptr1);

	return PDC_CLIP_SUCCESS;
}

int PDC_freeclipboard(char *contents)
{
	PDC_LOG(("PDC_freeclipboard() - called\n"));

	GlobalFree(contents);
	return PDC_CLIP_SUCCESS;
}

int PDC_clearclipboard(void)
{
	PDC_LOG(("PDC_clearclipboard() - called\n"));

	EmptyClipboard();

	return PDC_CLIP_SUCCESS;
}
