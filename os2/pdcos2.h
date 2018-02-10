/* Public Domain Curses */

#include <stdlib.h>
#include <string.h>

#define INCL_DOS
#define INCL_DOSMISC
#define INCL_WIN
#define INCL_VIO
#define INCL_KBD
#define INCL_MOU
#include <os2.h>

#include <curspriv.h>

#ifdef CHTYPE_LONG
# define PDC_ATTR_SHIFT 19
#else
# define PDC_ATTR_SHIFT 8
#endif

extern unsigned char *pdc_atrtab;
extern int pdc_font;

extern void PDC_get_keyboard_info(void);
extern void PDC_set_keyboard_default(void);
