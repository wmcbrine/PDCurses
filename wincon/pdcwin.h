/* Public Domain Curses */

#if defined(PDC_WIDE) && !defined(UNICODE)
# define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
/* missing definitions in old wincon.h */
#ifndef COMMON_LVB_GRID_HORIZONTAL
#define COMMON_LVB_GRID_HORIZONTAL	0x0400  /* Top horizontal */
#endif
#ifndef COMMON_LVB_GRID_LVERTICAL
#define COMMON_LVB_GRID_LVERTICAL	0x0800	/* Left vertical */
#endif
#ifndef COMMON_LVB_GRID_RVERTICAL
#define COMMON_LVB_GRID_RVERTICAL	0x1000	/* Right vertical */
#endif
#ifndef COMMON_LVB_REVERSE_VIDEO
#define COMMON_LVB_REVERSE_VIDEO	0x4000	/* Reverse foreground and background attribute */
#endif
#ifndef COMMON_LVB_UNDERSCORE
#define COMMON_LVB_UNDERSCORE	0x8000	/* Underscore */
#endif
/* missing definitions in old wincon.h end */
#undef MOUSE_MOVED
#include <curspriv.h>

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE 1   /* kill nonsense warnings */
#endif

extern HANDLE pdc_con_out, pdc_con_in;
extern DWORD pdc_quick_edit;
extern DWORD pdc_last_blink;
extern short pdc_curstoreal[16], pdc_curstoansi[16];
extern short pdc_oldf, pdc_oldb, pdc_oldu;
extern bool pdc_conemu, pdc_ansi;

extern void PDC_blink_text(void);
