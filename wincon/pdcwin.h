/* Public Domain Curses */

#if defined(PDC_WIDE) && !defined(UNICODE)
# define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef MOUSE_MOVED
#include <curspriv.h>

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE 1   /* kill nonsense warnings */
#endif

#ifdef CHTYPE_LONG
# define PDC_ATTR_SHIFT 17
#else
# define PDC_ATTR_SHIFT 8
#endif

extern WORD *pdc_atrtab;
extern HANDLE pdc_con_out, pdc_con_in;
extern DWORD pdc_quick_edit;
extern DWORD pdc_last_blink;

extern int PDC_get_buffer_rows(void);
extern void PDC_blink_text(void);
