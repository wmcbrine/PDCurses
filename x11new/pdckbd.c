/* PDCurses */

#include "pdcx11.h"

/* check if a key or mouse event is waiting */

bool PDC_check_key(void)
{
    XtInputMask s = XtAppPending(app_context);

    PDC_LOG(("%s:PDC_check_key() - returning %s\n", XCLOGMSG,
             s ? "TRUE" : "FALSE"));

    return xc_resize_now || !!s;
}

/* return the next available key or mouse event */

int PDC_get_key(void)
{
    XEvent event;
    unsigned long newkey = 0;
    int key = 0;

    if (xc_resize_now)
    {
        xc_resize_now = FALSE;
        SP->key_code = TRUE;
        return KEY_RESIZE;
    }

    XtAppNextEvent(app_context, &event);

    switch (event.type)
    {
    case KeyPress:
    case KeyRelease:
        newkey = XCursesKeyPress(&event);
        break;
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
        newkey = XCursesMouse(&event);
        break;
    default:
        XtDispatchEvent(&event);
        return -1;
    }

    if ((unsigned long)(-1) == newkey)
        return -1;

    pdc_key_modifiers = (newkey >> 24) & 0xFF;
    key = (int)(newkey & 0x00FFFFFF);

    PDC_LOG(("%s:PDC_get_key() - key %d returned\n", XCLOGMSG, key));

    return key;
}

void PDC_set_keyboard_binary(bool on)
{
    PDC_LOG(("PDC_set_keyboard_binary() - called\n"));
}

/* discard any pending keyboard or mouse input -- this is the core
   routine for flushinp() */

void PDC_flushinp(void)
{
    PDC_LOG(("PDC_flushinp() - called\n"));

    while (PDC_check_key())
        PDC_get_key();
}

bool PDC_has_mouse(void)
{
    return TRUE;
}

int PDC_mouse_set(void)
{
    return OK;
}

int PDC_modifiers_set(void)
{
    return OK;
}
