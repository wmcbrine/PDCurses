/* PDCurses */

#include "pdcx11.h"

#include <stdlib.h>

/*man-start**************************************************************

clipboard
---------

### Synopsis

    int PDC_getclipboard(char **contents, long *length);
    int PDC_setclipboard(const char *contents, long length);
    int PDC_freeclipboard(char *contents);
    int PDC_clearclipboard(void);

### Description

   PDC_getclipboard() gets the textual contents of the system's
   clipboard. This function returns the contents of the clipboard in the
   contents argument. It is the responsibility of the caller to free the
   memory returned, via PDC_freeclipboard(). The length of the clipboard
   contents is returned in the length argument.

   PDC_setclipboard copies the supplied text into the system's
   clipboard, emptying the clipboard prior to the copy.

   PDC_clearclipboard() clears the internal clipboard.

### Return Values

   indicator of success/failure of call.
   PDC_CLIP_SUCCESS        the call was successful
   PDC_CLIP_MEMORY_ERROR   unable to allocate sufficient memory for
                           the clipboard contents
   PDC_CLIP_EMPTY          the clipboard contains no text
   PDC_CLIP_ACCESS_ERROR   no clipboard support

### Portability
                             X/Open  ncurses  NetBSD
    PDC_getclipboard            -       -       -
    PDC_setclipboard            -       -       -
    PDC_freeclipboard           -       -       -
    PDC_clearclipboard          -       -       -

**man-end****************************************************************/

int PDC_getclipboard(char **contents, long *length)
{
    XEvent event;

    PDC_LOG(("PDC_getclipboard() - called\n"));

    xc_selection = NULL;
    xc_selection_len = -1;

    XCursesProcessRequest(CURSES_GET_SELECTION);
    while (-1 == xc_selection_len)
    {
        XtAppNextEvent(app_context, &event);
        XtDispatchEvent(&event);
    }

    if (xc_selection && xc_selection_len)
    {
        *contents = malloc(xc_selection_len + 1);

        if (!*contents)
            return PDC_CLIP_MEMORY_ERROR;

        memcpy(*contents, xc_selection, xc_selection_len);

        (*contents)[xc_selection_len] = '\0';
        *length = xc_selection_len;

        return PDC_CLIP_SUCCESS;
    }

    return PDC_CLIP_EMPTY;
}

int PDC_setclipboard(const char *contents, long length)
{
#ifdef PDC_WIDE
    wchar_t *wcontents;
#endif
    int rc;

    PDC_LOG(("PDC_setclipboard() - called\n"));

#ifdef PDC_WIDE
    wcontents = malloc((length + 1) * sizeof(wchar_t));
    if (!wcontents)
        return PDC_CLIP_MEMORY_ERROR;

    length = PDC_mbstowcs(wcontents, contents, length);
#endif
    XCursesProcessRequest(CURSES_SET_SELECTION);

    /* Write, then wait for X to do its stuff; expect return code. */

    //if (XC_write_socket(xc_display_sock, &length, sizeof(long)) >= 0)
    //{
    //    if (XC_write_socket(xc_display_sock,
#ifdef PDC_WIDE
    //        wcontents, length * sizeof(wchar_t)) >= 0)
    //    {
    //        free(wcontents);
#else
    //        contents, length) >= 0)
    //    {
#endif
    //        if (XC_read_socket(xc_display_sock, &rc, sizeof(int)) >= 0)
    //            return rc;
    //    }
    //}

    //XCursesExitCursesProcess(5, "exiting from PDC_setclipboard");

    return PDC_CLIP_ACCESS_ERROR;   /* not reached */
}

int PDC_freeclipboard(char *contents)
{
    PDC_LOG(("PDC_freeclipboard() - called\n"));

    free(contents);
    return PDC_CLIP_SUCCESS;
}

int PDC_clearclipboard(void)
{
    int rc;
    long len = 0;

    PDC_LOG(("PDC_clearclipboard() - called\n"));

    XCursesProcessRequest(CURSES_CLEAR_SELECTION);

    /* Write, then wait for X to do its stuff; expect return code. */

    //if (XC_write_socket(xc_display_sock, &len, sizeof(long)) >= 0)
    //    if (XC_read_socket(xc_display_sock, &rc, sizeof(int)) >= 0)
    //        return rc;

    //XCursesExitCursesProcess(5, "exiting from PDC_clearclipboard");

    return PDC_CLIP_ACCESS_ERROR;   /* not reached */
}
