/* Public Domain Curses */

#ifdef PDC_WIDE
# define UNICODE
#endif

#include <windows.h>
#undef MOUSE_MOVED
#include <curspriv.h>

#ifdef CHTYPE_LONG
# define PDC_ATTR_SHIFT 19
#else
# define PDC_ATTR_SHIFT 8
#endif

extern unsigned char *pdc_atrtab;
extern HANDLE pdc_con_out, pdc_con_in;
extern DWORD pdc_quick_edit;

extern int PDC_get_buffer_rows(void);
