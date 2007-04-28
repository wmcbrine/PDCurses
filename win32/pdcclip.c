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

RCSID("$Id: pdcclip.c,v 1.24 2007/04/28 01:08:40 wmcbrine Exp $");

/*man-start**************************************************************

  Name:								clipboard

  Synopsis:
	int PDC_getclipboard(char **contents, long *length);
	int PDC_setclipboard(const char *contents, long length);
	int PDC_freeclipboard(char *contents);
	int PDC_clearclipboard(void);

  Description:
	PDC_getclipboard() gets the textual contents of the system's 
	clipboard. This function returns the contents of the clipboard 
	in the contents argument. It is the responsibilitiy of the 
	caller to free the memory returned, via PDC_freeclipboard().
	The length of the clipboard contents is returned in the length 
	argument.

	PDC_setclipboard copies the supplied text into the system's 
	clipboard, emptying the clipboard prior to the copy.

	PDC_clearclipboard() clears the internal clipboard.

  Return Values:
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

#ifdef PDC_WIDE
	if ((handle = GetClipboardData(CF_UNICODETEXT)) == NULL)
#else
	if ((handle = GetClipboardData(CF_TEXT)) == NULL)
#endif
	{
		CloseClipboard();
		return PDC_CLIP_EMPTY;
	}

#ifdef PDC_WIDE
	len = wcslen((wchar_t *)handle);
#else
	len = strlen((char *)handle);
#endif
	*contents = (char *)GlobalAlloc(GMEM_FIXED,
		(len + 1) * sizeof(TCHAR));

	if (!*contents)
	{
		CloseClipboard();
		return PDC_CLIP_MEMORY_ERROR;
	}

#ifdef PDC_WIDE
	PDC_wcstombs((char *)*contents, (wchar_t *)handle, len * 2);
	*length = strlen((char *)*contents);
#else
	strcpy((char *)*contents, (char *)handle);
	*length = len;
#endif
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

#ifdef PDC_WIDE
	PDC_mbstowcs((wchar_t *)ptr2, contents, length);
#else
	memcpy((char *)ptr2, contents, length + 1);
#endif
	GlobalUnlock(ptr1);
	EmptyClipboard();

#ifdef PDC_WIDE
	if (!SetClipboardData(CF_UNICODETEXT, ptr1))
#else
	if (!SetClipboardData(CF_TEXT, ptr1))
#endif
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
