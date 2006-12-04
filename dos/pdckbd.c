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
                          
#ifdef __DJGPP__
# include <signal.h>
#else
# include <conio.h>
#endif

#include "pdcdos.h"

RCSID("$Id: pdckbd.c,v 1.64 2006/12/04 05:35:50 wmcbrine Exp $");

/************************************************************************
 *    Table for key code translation of function keys in keypad mode	*
 *    These values are for strict IBM keyboard compatibles only	  	*
 ************************************************************************/

static short key_table[]=
{
	-1,		ALT_ESC,	-1,		0,
	-1,		-1,		-1,		-1,
	-1,		-1,		CTL_PADENTER,	-1,
	-1,		PADENTER,	ALT_BKSP,	KEY_BTAB,
	ALT_Q,		ALT_W,		ALT_E,		ALT_R,
	ALT_T,		ALT_Y,		ALT_U,		ALT_I,
	ALT_O,		ALT_P,		ALT_LBRACKET,	ALT_RBRACKET,
	ALT_ENTER,	-1,		ALT_A,		ALT_S,
	ALT_D,		ALT_F,		ALT_G,		ALT_H,
	ALT_J,		ALT_K,		ALT_L,		ALT_SEMICOLON,
	ALT_FQUOTE,	ALT_BQUOTE,	-1,		ALT_BSLASH,
	ALT_Z,		ALT_X,		ALT_C,		ALT_V,
	ALT_B,		ALT_N,		ALT_M,		ALT_COMMA,
	ALT_STOP,	ALT_FSLASH,	-1,		ALT_PADSTAR,
	-1,		-1,		-1,		KEY_F(1),
	KEY_F(2),	KEY_F(3),	KEY_F(4),	KEY_F(5),
	KEY_F(6),	KEY_F(7),	KEY_F(8),	KEY_F(9),
	KEY_F(10),	-1,		-1,		KEY_HOME,
	KEY_UP,		KEY_PPAGE,	ALT_PADMINUS,	KEY_LEFT,
	KEY_B2,		KEY_RIGHT,	ALT_PADPLUS,	KEY_END,
	KEY_DOWN,	KEY_NPAGE,	KEY_IC,		KEY_DC,
	KEY_F(13),	KEY_F(14),	KEY_F(15),	KEY_F(16),
	KEY_F(17),	KEY_F(18),	KEY_F(19),	KEY_F(20),
	KEY_F(21),	KEY_F(22),	KEY_F(25),	KEY_F(26),
	KEY_F(27),	KEY_F(28),	KEY_F(29),	KEY_F(30),
	KEY_F(31),	KEY_F(32),	KEY_F(33),	KEY_F(34),
	KEY_F(37),	KEY_F(38),	KEY_F(39),	KEY_F(40),
	KEY_F(41),	KEY_F(42),	KEY_F(43),	KEY_F(44),
	KEY_F(45),	KEY_F(46),	-1,		CTL_LEFT,
	CTL_RIGHT,	CTL_END,	CTL_PGDN,	CTL_HOME,
	ALT_1,		ALT_2,		ALT_3,		ALT_4,
	ALT_5,		ALT_6,		ALT_7,		ALT_8,
	ALT_9,		ALT_0,		ALT_MINUS,	ALT_EQUAL,
	CTL_PGUP,	KEY_F(11),	KEY_F(12),	KEY_F(23),
	KEY_F(24),	KEY_F(35),	KEY_F(36),	KEY_F(47),
	KEY_F(48),	CTL_UP,		CTL_PADMINUS,	CTL_PADCENTER,
	CTL_PADPLUS,	CTL_DOWN,	CTL_INS,	CTL_DEL,
	CTL_TAB,	CTL_PADSLASH,	CTL_PADSTAR,	ALT_HOME,
	ALT_UP,		ALT_PGUP,	-1,		ALT_LEFT,
	-1,		ALT_RIGHT,	-1,		ALT_END,
	ALT_DOWN,	ALT_PGDN,	ALT_INS,	ALT_DEL,
	ALT_PADSLASH,	ALT_TAB,	ALT_PADENTER,	-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	KEY_SHOME,	KEY_SUP,	KEY_SPREVIOUS,	KEY_SLEFT,
	KEY_SRIGHT,	KEY_SEND,	KEY_SDOWN,	KEY_SNEXT,
	KEY_SIC,	KEY_SDC,	-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		PADSTAR,	PADPLUS,
	CTL_ENTER,	PADMINUS,	-1,		PADSLASH
};

unsigned long pdc_key_modifiers = 0L;

static bool mouse_avail = FALSE, mouse_vis = FALSE, mouse_moved = FALSE,
	key_pressed = FALSE;

static PDCREGS ms_regs, old_ms;
static unsigned short shift_status, old_shift = 0;
static unsigned char keyboard_function = 0xff, shift_function = 0xff;

/*man-start**************************************************************

  PDC_get_input_fd()	- Get file descriptor used for PDCurses input

  PDCurses Description:
	This is a private PDCurses routine.

	This routine will return the file descriptor that PDCurses reads
	its input from. It can be used for select().

  PDCurses Return Value:
	Returns a file descriptor.

  Portability:
	PDCurses  int PDC_get_input_fd(void);

**man-end****************************************************************/

unsigned long PDC_get_input_fd(void)
{
	PDC_LOG(("PDC_get_input_fd() - called\n"));

	return (unsigned long)fileno(stdin);
}

void PDC_set_keyboard_binary(bool on)
{
	PDC_LOG(("PDC_set_keyboard_binary() - called\n"));
}

/*man-start**************************************************************

  PDC_check_bios_key()	- Check BIOS key data area for input

  PDCurses Description:
	This is a private PDCurses routine.

	This routine will check the BIOS for any indication that
	keystrokes are pending.

  PDCurses Return Value:
	Returns 1 if a keyboard character is available, 0 otherwise.

  Portability:
	PDCurses  bool PDC_check_bios_key(void);

**man-end****************************************************************/

bool PDC_check_bios_key(void)
{
	PDCREGS regs;

	PDC_LOG(("PDC_check_bios_key() - called\n"));

	if (shift_function == 0xff)
	{
		int scan;

		/* get shift status for all keyboards */

		regs.h.ah = 0x02;
		PDCINT(0x16, regs);
		scan = regs.h.al;

		/* get shift status for enhanced keyboards */

		regs.h.ah = 0x12;
		PDCINT(0x16, regs);

		if (scan == regs.h.al && getdosmembyte(0x496) == 0x10)
		{
			keyboard_function = 0x10;
			shift_function = 0x12;
		}
		else
		{
			keyboard_function = 0;
			shift_function = 2;
		}
	}

	regs.h.ah = shift_function;
	PDCINT(0x16, regs);

	shift_status = regs.W.ax;

	if (mouse_vis)
	{
		ms_regs.W.ax = 3;
		PDCINT(0x33, ms_regs);

		mouse_moved = ms_regs.h.bl &&
			ms_regs.h.bl == old_ms.h.bl &&
			(((ms_regs.W.cx ^ old_ms.W.cx) >> 3) ||
			 ((ms_regs.W.dx ^ old_ms.W.dx) >> 3));

		if (ms_regs.h.bl != old_ms.h.bl || mouse_moved || ms_regs.h.bh)
			return TRUE;
	}

	if (old_shift && !shift_status)		/* modifier released */
	{
		if (!key_pressed && SP->return_key_modifiers)
			return TRUE;
	}
	else if (!old_shift && shift_status)	/* modifier pressed */
		key_pressed = FALSE;

	old_shift = shift_status;

	return kbhit();
}

/*man-start**************************************************************

  PDC_get_bios_key()	- Returns the next key available.

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the next key code struck at the keyboard.

  Portability:
	PDCurses  int PDC_get_bios_key(void);

**man-end****************************************************************/

int PDC_get_bios_key(void)
{
	PDCREGS regs;
	int key, scan;

	pdc_key_modifiers = 0;

	if (mouse_vis)
	{
		/* Scroll wheel support for CuteMouse */

		if (ms_regs.h.bh)
		{
			memset(&pdc_mouse_status, 0, sizeof(pdc_mouse_status));

			pdc_mouse_status.changes = ms_regs.h.bh & 0x80 ?
				PDC_MOUSE_WHEEL_UP : PDC_MOUSE_WHEEL_DOWN;

			pdc_mouse_status.x = -1;
			pdc_mouse_status.y = -1;

			key_pressed = TRUE;
			old_shift = shift_status;

			SP->key_code = TRUE;
			return KEY_MOUSE;
		}

		if (ms_regs.h.bl != old_ms.h.bl || mouse_moved)
		{
			short shift_flags = 0;

			pdc_mouse_status.changes = mouse_moved ? 
				PDC_MOUSE_MOVED : 0;

			pdc_mouse_status.button[0] = (ms_regs.h.bl & 1) ?
				(mouse_moved ? BUTTON_MOVED : BUTTON_PRESSED)
				: BUTTON_RELEASED;

			pdc_mouse_status.button[1] = (ms_regs.h.bl & 4) ?
				(mouse_moved ? BUTTON_MOVED : BUTTON_PRESSED)
				: BUTTON_RELEASED;

			pdc_mouse_status.button[2] = (ms_regs.h.bl & 2) ?
				(mouse_moved ? BUTTON_MOVED : BUTTON_PRESSED)
				: BUTTON_RELEASED;

			/* moves always flag the button as changed */

			if (((ms_regs.h.bl & 1) ^ (old_ms.h.bl & 1)) ||
			    (pdc_mouse_status.button[0] == BUTTON_MOVED))
				pdc_mouse_status.changes |= 1;

			if (((ms_regs.h.bl & 4) ^ (old_ms.h.bl & 4)) ||
			    (pdc_mouse_status.button[1] == BUTTON_MOVED))
				pdc_mouse_status.changes |= 2;

			if (((ms_regs.h.bl & 2) ^ (old_ms.h.bl & 2)) ||
			    (pdc_mouse_status.button[2] == BUTTON_MOVED))
				pdc_mouse_status.changes |= 4;

			pdc_mouse_status.x = ms_regs.W.cx >> 3;
			pdc_mouse_status.y = ms_regs.W.dx >> 3;

			old_ms = ms_regs;

			/* Set shift modifiers */

			if (shift_status & 3)
				shift_flags |= BUTTON_SHIFT;

			if (shift_status & 4)
				shift_flags |= BUTTON_CONTROL;

			if (shift_status & 8)
				shift_flags |= BUTTON_ALT;

			if (pdc_mouse_status.changes & 1)
				pdc_mouse_status.button[0] |= shift_flags;

			if (pdc_mouse_status.changes & 2)
				pdc_mouse_status.button[1] |= shift_flags;

			if (pdc_mouse_status.changes & 4)
				pdc_mouse_status.button[2] |= shift_flags;

			key_pressed = TRUE;
			old_shift = shift_status;

			SP->key_code = TRUE;
			return KEY_MOUSE;
		}
	}

	/* Return modifiers as keys? */

	if (old_shift && !shift_status)
	{
		key = -1;

		if (old_shift & 1)
			key = KEY_SHIFT_R;

		if (old_shift & 2)
			key = KEY_SHIFT_L;

		if (shift_function == 0x12)
		{
			if (old_shift & 0x400)
				key = KEY_CONTROL_R;

			if (old_shift & 0x100)
				key = KEY_CONTROL_L;

			if (old_shift & 0x800)
				key = KEY_ALT_R;

			if (old_shift & 0x200)
				key = KEY_ALT_L;
		}
		else
		{
			if (old_shift & 4)
				key = KEY_CONTROL_R;

			if (old_shift & 8)
				key = KEY_ALT_R;
		}

		key_pressed = FALSE;
		old_shift = shift_status;

		SP->key_code = TRUE;
		return key;
	}

	regs.h.ah = keyboard_function;
	PDCINT(0x16, regs);
	key = regs.h.al;
	scan = regs.h.ah;

	if (SP->save_key_modifiers)
	{
		if (shift_status & 3)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_SHIFT;

		if (shift_status & 4)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_CONTROL;

		if (shift_status & 8)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_ALT;

		if (shift_status & 0x20)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_NUMLOCK;
	}

	if (scan == 0x1c && key == 0x0a)  /* ^Enter */
		key = CTL_ENTER;
	else if ((scan == 0xe0 && key == 0x0d)  /* PadEnter */
		|| (scan == 0xe0 && key == 0x0a)) /* ^PadEnter */
			key = key_table[key];
	else if ((scan == 0x37 && key == 0x2a)  /* Star */
		|| (scan == 0x4a && key == 0x2d)  /* Minus */
		|| (scan == 0x4e && key == 0x2b)  /* Plus */
		|| (scan == 0xe0 && key == 0x2f)) /* Slash */
			key = key_table[(key & 0x0f) | 0xf0];
	else if (key == 0xe0 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT)
	{
		switch (scan)
		{
		case 0x47: /* Shift Home */
			key = KEY_SHOME;
			break;
		case 0x48: /* Shift Up */
			key = KEY_SUP;
			break;
		case 0x49: /* Shift PgUp */
			key = KEY_SPREVIOUS;
			break;
		case 0x4b: /* Shift Left */
			key = KEY_SLEFT;
			break;
		case 0x4d: /* Shift Right */
			key = KEY_SRIGHT;
			break;
		case 0x4f: /* Shift End */
			key = KEY_SEND;
			break;
		case 0x50: /* Shift Down */
			key = KEY_SDOWN;
			break;
		case 0x51: /* Shift PgDn */
			key = KEY_SNEXT;
			break;
		case 0x52: /* Shift Ins */
			key = KEY_SIC;
			break;
		case 0x53: /* Shift Del */
			key = KEY_SDC;
		}
	}
	else if (key == 0x00 || (key == 0xe0 && scan > 53 && scan != 86))
		key = key_table[scan];

	key_pressed = TRUE;
	SP->key_code = ((unsigned)key >= 256);

	return key;
}

/*man-start**************************************************************

  PDC_get_ctrl_break()	- return OS control break state

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the current OS Control Break Check state.

  PDCurses Return Value:
	This function returns TRUE if the Control Break Check is enabled 
	otherwise FALSE is returned.

  Portability:
	PDCurses  bool PDC_get_ctrl_break(void);

**man-end****************************************************************/

bool PDC_get_ctrl_break(void)
{
	PDCREGS regs;

	PDC_LOG(("PDC_get_ctrl_break() - called\n"));

	regs.h.ah = 0x33;
	regs.h.al = 0x00;
	PDCINT(0x21, regs);

	return (bool)regs.h.dl;
}

/*man-start**************************************************************

  PDC_set_ctrl_break()	- Enables/Disables the host OS BREAK key check.

  PDCurses Description:
	This is a private PDCurses routine.

	Enables/Disables the host OS BREAK key check. If the supplied 
	setting is TRUE, this enables CTRL/C and CTRL/BREAK to abort the 
	process. If FALSE, CTRL/C and CTRL/BREAK are ignored.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  Portability:
	PDCurses  int PDC_set_ctrl_break(bool setting);

**man-end****************************************************************/

int PDC_set_ctrl_break(bool setting)
{
#ifndef __DJGPP__
	PDCREGS regs;
#endif
	PDC_LOG(("PDC_set_ctrl_break() - called\n"));

#ifdef __DJGPP__
	signal(SIGINT, setting ? SIG_DFL : SIG_IGN);
/*	__djgpp_set_ctrl_c(setting);*/
	setcbrk(setting);
#else
	regs.h.ah = 0x33;
	regs.h.al = 0x01;
	regs.h.dl = setting;
	PDCINT(0x21, regs);
#endif
	return OK;
}

/*man-start**************************************************************

  PDC_flushinp()		- Low-level input flush

  PDCurses Description:
	This is a private PDCurses routine.

	Discards any pending keyboard and mouse input. Called by 
	flushinp().

  Portability:
	PDCurses  void PDC_flushinp(void);

**man-end****************************************************************/

void PDC_flushinp(void)
{
	PDC_LOG(("PDC_flushinp() - called\n"));

	/* Force the BIOS keyboard buffer head and tail pointers to be 
	   the same...  Real nasty trick... */

	setdosmemword(0x41a, getdosmemword(0x41c));
}

int PDC_mouse_set(void)
{
	PDCREGS regs;
	unsigned long mbe = SP->_trap_mbe;

	if (mbe && !mouse_avail)
	{
		regs.W.ax = 0;
		PDCINT(0x33, regs);

		mouse_avail = !!(regs.W.ax);
	}

	if (mbe)
	{
		if (mouse_avail && !mouse_vis)
		{
			memset(&old_ms, 0, sizeof(old_ms));

			regs.W.ax = 1;
			PDCINT(0x33, regs);

			mouse_vis = TRUE;
		}
	}
	else
	{
		if (mouse_avail && mouse_vis)
		{
			regs.W.ax = 2;
			PDCINT(0x33, regs);

			mouse_vis = FALSE;
		}
	}

	return (mouse_avail || !mbe) ? OK : ERR;
}

int PDC_modifiers_set(void)
{
	key_pressed = FALSE;

	return OK;
}
