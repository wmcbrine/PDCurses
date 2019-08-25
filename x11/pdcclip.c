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

    XC_get_selection();
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
    PDC_LOG(("PDC_setclipboard() - called\n"));

    return XC_set_selection(contents, length);
}

int PDC_freeclipboard(char *contents)
{
    PDC_LOG(("PDC_freeclipboard() - called\n"));

    free(contents);
    return PDC_CLIP_SUCCESS;
}

int PDC_clearclipboard(void)
{
    PDC_LOG(("PDC_clearclipboard() - called\n"));

    return PDC_CLIP_SUCCESS;
}
