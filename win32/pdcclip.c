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
#define	INCLUDE_WINDOWS_H
#define	CURSES_LIBRARY	1
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCclip  = "$Id: pdcclip.c,v 1.3 2002/11/27 11:17:49 mark Exp $";
#endif


/*man-start*********************************************************************

  PDC_getclipboard()	- Gets the contents of the clipboard

  PDCurses Description:
 	This is a PDCurses only routine.

 	Gets the textual contents of the system's clipboard. This
 	function returns the contents of the clipboard in the contents
 	argument. It is the responsibilitiy of the caller to free the
 	memory returned with the PDC_freeclipboard() call.  The length of the
 	clipboard contents is returned in the length argument.

  PDCurses Return Value:
 	indicator of success/failure of call.
 	PDC_CLIP_SUCCESS	the call was successful
 	PDC_CLIP_ACCESS_ERROR	an error occured while accessing the
 		clipboard
 	PDC_CLIP_MEMORY_ERROR	unable to allocate sufficient memory for 
 		the clipboard contents
 	PDC_CLIP_EMPTY	the clipboard contains no text

  Portability:
 	PDCurses	int PDC_getclipboard( char **contents, long *length );

**man-end**********************************************************************/

int	PDC_CDECL	PDC_getclipboard(char **contents, long *length)
{
 HANDLE handle;
 long len=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_getclipboard() - called\n");
#endif

 if (OpenClipboard(NULL) == 0)
 {
    return PDC_CLIP_ACCESS_ERROR;
 }
 handle = GetClipboardData( CF_TEXT );
 if (handle == NULL)
 {
    CloseClipboard();
    return PDC_CLIP_EMPTY;
 }

 len = strlen((char *)handle);
 *contents = (char *)GlobalAlloc(GMEM_FIXED,len+1);
 if (!*contents)
 {
    CloseClipboard();
    return PDC_CLIP_MEMORY_ERROR;
 }
 strcpy((char *)*contents,(char *)handle);
 *length = len;
 CloseClipboard();

	return( PDC_CLIP_SUCCESS );
}


/*man-start*********************************************************************

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
 	PDCurses	int PDC_getclipboard( char *contents, long length );

**man-end**********************************************************************/

int	PDC_CDECL	PDC_setclipboard(char *contents, long length)
{
 HGLOBAL ptr1;
 LPTSTR ptr2;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_setclipboard() - called\n");
#endif


 if (OpenClipboard(NULL) == 0)
 {
    return PDC_CLIP_ACCESS_ERROR;
 }
 ptr1 = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, (length+1)*sizeof(TCHAR) );
 if (!ptr1)
 {
    return PDC_CLIP_MEMORY_ERROR;
 }

 ptr2 = GlobalLock(ptr1);
 memcpy((char *)ptr2, (char *)contents, length+1 );
 GlobalUnlock(ptr1);
 EmptyClipboard();
 if (SetClipboardData(CF_TEXT, ptr1) == NULL)
 {
    GlobalFree(ptr1);
    return PDC_CLIP_ACCESS_ERROR;
 }
 CloseClipboard();
 GlobalFree(ptr1);
 return PDC_CLIP_SUCCESS;
}

/*man-start*********************************************************************

  PDC_freeclipboard()	- Frees the memory associated with the contents of the clipboard

  PDCurses Description:
 	This is a PDCurses only routine.

 	Frees the memory allocated by PDC_getclipboard().

  PDCurses Return Value:
 	Always returns PDC_CLIP_SUCCESS

  Portability:
 	PDCurses	int PDC_freeclipboard( char *contents );

**man-end**********************************************************************/

int	PDC_CDECL	PDC_freeclipboard(char *contents)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_freeclipboard() - called\n");
#endif

 GlobalFree(contents);
 return PDC_CLIP_SUCCESS;
}

/*man-start*********************************************************************

  PDC_clearclipboard()	- Clears the contents of the clipboard

  PDCurses Description:
 	This is a PDCurses only routine.

 	Clears the internal clipboard.

  PDCurses Return Value:
 	Always returns PDC_CLIP_SUCCESS

  Portability:
 	PDCurses	int PDC_clearclipboard( void );

**man-end**********************************************************************/

int	PDC_CDECL	PDC_clearclipboard( void )
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_clearclipboard() - called\n");
#endif

 EmptyClipboard();

 return PDC_CLIP_SUCCESS;
}

