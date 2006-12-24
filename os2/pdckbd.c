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

#include "pdcos2.h"

#if defined(__EMX__) || defined(__WATCOMC__) || defined(__IBMC__) || \
    defined(__TURBOC__)
# define HAVE_SIGNAL
# include <signal.h>
#else
# define INCL_DOSSIGNALS
# define INCL_NOCOMMON
# include <bsedos.h>
#endif

#ifdef EMXVIDEO
# include <termios.h>
static int tahead = -1;
#else
static KBDINFO kbdinfo;		/* default keyboard mode */
static HMOU mouse_handle = 0;
static MOUSE_STATUS old_mouse_status;
static USHORT old_shift = 0;
static bool key_pressed = FALSE;
static int mouse_events = 0;
#endif

RCSID("$Id: pdckbd.c,v 1.75 2006/12/24 06:07:25 wmcbrine Exp $");

/************************************************************************
 *   Table for key code translation of function keys in keypad mode	*
 *   These values are for strict IBM keyboard compatibles only		*
 ************************************************************************/

static short key_table[] =
{
	-1,		ALT_ESC,	-1,		0,
	-1,		-1,		-1,		-1,
	-1,		-1,		-1,		-1,
	-1,		-1,		ALT_BKSP,	KEY_BTAB,
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
	ALT_PADSLASH,	ALT_TAB,	ALT_PADENTER,	-1
};

unsigned long pdc_key_modifiers = 0L;

/*man-start**************************************************************

  PDC_get_input_fd()    - Get file descriptor used for PDCurses input

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

#ifndef EMXVIDEO

void PDC_get_keyboard_info(void)
{
	PDC_LOG(("PDC_get_keyboard_info() - called\n"));

	kbdinfo.cb = sizeof(kbdinfo);
	KbdGetStatus(&kbdinfo, 0);

	PDC_LOG(("PDC_get_keyboard_info(). cb: %x, fsMask: %x, "
		"chTurnAround: %x, fsInterim: %x, fsState: %x\n",
		kbdinfo.cb, kbdinfo.fsMask, kbdinfo.chTurnAround, 
		kbdinfo.fsInterim, kbdinfo.fsState));
}

void PDC_set_keyboard_default(void)
{
	PDC_LOG(("PDC_set_keyboard_default(). cb: %x, fsMask: %x, "
		"chTurnAround: %x, fsInterim: %x, fsState: %x\n",
		kbdinfo.cb, kbdinfo.fsMask, kbdinfo.chTurnAround, 
		kbdinfo.fsInterim, kbdinfo.fsState));

	KbdSetStatus(&kbdinfo, 0);
}

#endif /* ifndef EMXVIDEO */

void PDC_set_keyboard_binary(bool on)
{
	PDC_LOG(("PDC_set_keyboard_binary() - called\n"));

#ifndef EMXVIDEO
	PDC_LOG(("PDC_set_keyboard_binary() - before. cb: %x, fsMask: %x, "
		"chTurnAround: %x, fsInterim: %x, fsState: %x\n",
		kbdinfo.cb, kbdinfo.fsMask, kbdinfo.chTurnAround, 
		kbdinfo.fsInterim, kbdinfo.fsState));

	if (on)
	{
		kbdinfo.fsMask &= ~(KEYBOARD_ASCII_MODE);
		kbdinfo.fsMask |= KEYBOARD_BINARY_MODE;
	}
	else
	{
		kbdinfo.fsMask &= ~(KEYBOARD_BINARY_MODE);
		kbdinfo.fsMask |= KEYBOARD_ASCII_MODE;
	}

	KbdSetStatus(&kbdinfo, 0);

	PDC_LOG(("PDC_set_keyboard_binary() - after. cb: %x, fsMask: %x, "
		"chTurnAround: %x, fsInterim: %x, fsState: %x\n",
		kbdinfo.cb, kbdinfo.fsMask, kbdinfo.chTurnAround, 
		kbdinfo.fsInterim, kbdinfo.fsState));
#endif
}

/*man-start**************************************************************

  PDC_check_bios_key()  - Check BIOS key data area for input

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
#if !defined(_MSC_VER) && !defined(EMXVIDEO)
	KBDKEYINFO keyInfo = {0};
#endif
	PDC_LOG(("PDC_check_bios_key() - called\n"));

#ifdef EMXVIDEO
	if (tahead == -1)		/* Nothing typed yet */
	{                    
		tahead = _read_kbd(0, 0, 0);

		/* Read additional */

		if (tahead == 0)	
			tahead = _read_kbd(0, 1, 0) << 8;
	}

	return (tahead != -1);
#else
# ifndef _MSC_VER

	KbdGetStatus(&kbdinfo, 0);

	if (mouse_handle)
	{
		MOUQUEINFO queue;

		MouGetNumQueEl(&queue, mouse_handle);
		mouse_events = queue.cEvents;

		if (mouse_events)
			return TRUE;
	}

	if (old_shift && !kbdinfo.fsState)	/* modifier released */
	{
		if (!key_pressed && SP->return_key_modifiers)
			return TRUE;
	}
	else if (!old_shift && kbdinfo.fsState)	/* modifier pressed */
		key_pressed = FALSE;

	old_shift = kbdinfo.fsState;

	KbdPeek(&keyInfo, 0);   /* peek at keyboard  */
	return (keyInfo.fbStatus != 0);
# else
	return kbhit();
# endif
#endif
}         

#ifndef EMXVIDEO

static int _process_mouse_events(void)
{
	MOUEVENTINFO event;
	static const USHORT button_mask[] = {6, 96, 24},
		move_mask[] = {2, 32, 8}, press_mask[] = {4, 64, 16};
	USHORT count = 1;
	short shift_flags = 0;
	int i;

	MouReadEventQue(&event, &count, mouse_handle);
	mouse_events--;

	for (i = 0; i < 3; i++)
	{
		pdc_mouse_status.button[i] =
			((event.fs & move_mask[i]) ? BUTTON_MOVED : 0) |
			((event.fs & press_mask[i]) ? BUTTON_PRESSED : 0);

		/* PRESS events are sometimes mistakenly reported as 
		   MOVE events. A MOVE should always follow a PRESS, so 
		   treat a MOVE immediately after a RELEASE as a PRESS. */

		if ((pdc_mouse_status.button[i] == BUTTON_MOVED) &&
		    (old_mouse_status.button[i] == BUTTON_RELEASED))
		{
			pdc_mouse_status.button[i] = BUTTON_PRESSED;
		}

		if (pdc_mouse_status.button[i] == BUTTON_PRESSED &&
		    SP->mouse_wait)
		{
			/* Check for a click -- a PRESS followed 
			   immediately by a release */

			if (!mouse_events)
			{
				MOUQUEINFO queue;

				napms(SP->mouse_wait);

				MouGetNumQueEl(&queue, mouse_handle);
				mouse_events = queue.cEvents;
			}

			if (mouse_events)
			{
				MouReadEventQue(&event, &count, mouse_handle);

				if (!(event.fs & button_mask[i]))
					pdc_mouse_status.button[i] = 
						BUTTON_CLICKED;
			}
		}
	}

	pdc_mouse_status.x = event.col;
	pdc_mouse_status.y = event.row;

	pdc_mouse_status.changes = 0;

	for (i = 0; i < 3; i++)
	{
		if (old_mouse_status.button[i] != pdc_mouse_status.button[i])
			pdc_mouse_status.changes |= (1 << i);

		if (pdc_mouse_status.button[i] == BUTTON_MOVED)
		{
			/* Discard non-moved "moves" */

			if (pdc_mouse_status.x == old_mouse_status.x &&
			    pdc_mouse_status.y == old_mouse_status.y)
				return -1;

			/* Motion events always flag the button as changed */

			pdc_mouse_status.changes |= (1 << i);
			pdc_mouse_status.changes |= PDC_MOUSE_MOVED;
			break;
		}
	}

	old_mouse_status = pdc_mouse_status;

	/* Treat click events as release events for comparison purposes */

	for (i = 0; i < 3; i++)
	{
		if (old_mouse_status.button[i] == BUTTON_CLICKED)
			old_mouse_status.button[i] = BUTTON_RELEASED;
	}

	/* Check for SHIFT/CONTROL/ALT */

	if (kbdinfo.fsState & KBDSTF_ALT)
		shift_flags |= BUTTON_ALT;

	if (kbdinfo.fsState & KBDSTF_CONTROL)
		shift_flags |= BUTTON_CONTROL;

	if (kbdinfo.fsState & (KBDSTF_LEFTSHIFT|KBDSTF_RIGHTSHIFT))
		shift_flags |= BUTTON_SHIFT;

	for (i = 0; i < 3; i++)
	{
		if (pdc_mouse_status.changes & (1 << i))
			pdc_mouse_status.button[i] |= shift_flags;
	}

	old_shift = kbdinfo.fsState;
	key_pressed = TRUE;

	SP->key_code = TRUE;
	return KEY_MOUSE;
}

#endif

/*man-start**************************************************************

  PDC_get_bios_key() - Returns the next key available.

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the next key code struck at the keyboard.

  Portability:
	PDCurses  int PDC_get_bios_key(void);

**man-end****************************************************************/

int PDC_get_bios_key(void)
{
	int key, scan;
#ifndef EMXVIDEO
	KBDKEYINFO keyInfo = {0};
#endif
	PDC_LOG(("PDC_get_bios_key() - called\n"));

#ifdef EMXVIDEO
	if (tahead == -1)
	{
		tahead = _read_kbd(0, 1, 0);

		/* Read additional */

		if (tahead == 0)
			tahead = _read_kbd(0, 1, 0) << 8;
	}

	key = tahead & 0xff;
	scan = tahead >> 8;
	pdc_key_modifiers = 0L;

	tahead = -1;
#else
	pdc_key_modifiers = 0L;

	if (mouse_handle && mouse_events)
		return _process_mouse_events();

	if (old_shift && !kbdinfo.fsState)
	{
		key = -1;

		if (old_shift & KBDSTF_LEFTALT)
		{
			key = KEY_ALT_L;
		}
		else if (old_shift & KBDSTF_RIGHTALT)
		{
			key = KEY_ALT_R;
		}
		else if (old_shift & KBDSTF_LEFTCONTROL)
		{
			key = KEY_CONTROL_L;
		}
		else if (old_shift & KBDSTF_RIGHTCONTROL)
		{
			key = KEY_CONTROL_R;
		}
		else if (old_shift & KBDSTF_LEFTSHIFT)
		{
			key = KEY_SHIFT_L;
		}
		else if (old_shift & KBDSTF_RIGHTSHIFT)
		{
			key = KEY_SHIFT_R;
		}

		key_pressed = FALSE;
		old_shift = kbdinfo.fsState;

		SP->key_code = TRUE;
		return key;
	}

	KbdCharIn(&keyInfo, IO_WAIT, 0);	/* get a character */

	key = keyInfo.chChar;
	scan = keyInfo.chScan;

	if (SP->save_key_modifiers)
	{
		if (keyInfo.fsState & KBDSTF_ALT)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_ALT;

		if (keyInfo.fsState & KBDSTF_CONTROL)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_CONTROL;

		if (keyInfo.fsState & KBDSTF_NUMLOCK_ON)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_NUMLOCK;

		if (keyInfo.fsState & (KBDSTF_LEFTSHIFT|KBDSTF_RIGHTSHIFT))
			pdc_key_modifiers |= PDC_KEY_MODIFIER_SHIFT;
	}
#endif
	if (scan == 0x1c && key == 0x0a)	/* ^Enter */
		key = CTL_ENTER;
	else if (scan == 0xe0 && key == 0x0d)	/* PadEnter */
		key = PADENTER;
	else if (scan == 0xe0 && key == 0x0a)	/* ^PadEnter */
		key = CTL_PADENTER;
	else if (scan == 0x37 && key == 0x2a)	/* Star */
		key = PADSTAR;
	else if (scan == 0x4a && key == 0x2d)	/* Minus */
		key = PADMINUS;
	else if (scan == 0x4e && key == 0x2b)	/* Plus */
		key = PADPLUS;
	else if (scan == 0xe0 && key == 0x2f)	/* Slash */
		key = PADSLASH;
	else if (key == 0x00 || (key == 0xe0 && scan > 53 && scan != 86))
		key = (scan & 0x80) ? -1 : key_table[scan];

	if (keyInfo.fsState & (KBDSTF_LEFTSHIFT|KBDSTF_RIGHTSHIFT))
	{
		switch (key)
		{
		case KEY_HOME:	/* Shift Home */
			key = KEY_SHOME;
			break;
		case KEY_UP:	/* Shift Up */
			key = KEY_SUP;
			break;
		case KEY_PPAGE: /* Shift PgUp */
			key = KEY_SPREVIOUS;
			break;
		case KEY_LEFT:	/* Shift Left */
			key = KEY_SLEFT;
			break;
		case KEY_RIGHT: /* Shift Right */
			key = KEY_SRIGHT;
			break;
		case KEY_END:	/* Shift End */
			key = KEY_SEND;
			break;
		case KEY_DOWN:	/* Shift Down */
			key = KEY_SDOWN;
			break;
		case KEY_NPAGE: /* Shift PgDn */
			key = KEY_SNEXT;
			break;
		case KEY_IC:	/* Shift Ins */
			key = KEY_SIC;
			break;
		case KEY_DC:	/* Shift Del */
			key = KEY_SDC;
		}
	}

	key_pressed = TRUE;
	SP->key_code = ((unsigned)key >= 256);

	return key;
}

/*man-start**************************************************************

  PDC_get_ctrl_break()  - return OS control break state

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
#ifdef HAVE_SIGNAL
# ifdef __TURBOC__
	void __cdecl (*oldAction) (int);
# else
	void (*oldAction) (int);
# endif
#endif
	PDC_LOG(("PDC_get_ctrl_break() - called\n"));

#ifdef HAVE_SIGNAL

	oldAction = signal(SIGINT, SIG_DFL);

	if (oldAction == SIG_ERR)
		return FALSE;
	else
		signal(SIGINT, oldAction);

	return (oldAction != SIG_IGN);
#else
	PFNSIGHANDLER oldHandler, oldHandler1;
	USHORT oldAction, oldAction1;

	/* get the current state, and set to ignore */

	DosSetSigHandler((PFNSIGHANDLER) NULL, &oldHandler, &oldAction,
		SIGA_IGNORE, SIG_CTRLBREAK);

	/* restore the previous state */

	DosSetSigHandler(oldHandler, &oldHandler1, &oldAction1,
		oldAction, SIG_CTRLBREAK);

	return (oldAction != SIGA_IGNORE);
#endif
}

/*man-start**************************************************************

  PDC_set_ctrl_break()  - Enables/Disables the host OS BREAK key check.

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
	PDC_LOG(("PDC_set_ctrl_break() - called. Setting: %d\n", setting));

#ifdef HAVE_SIGNAL
	signal(SIGINT, setting ? SIG_DFL : SIG_IGN);
	signal(SIGBREAK, setting ? SIG_DFL : SIG_IGN);
#else
	PFNSIGHANDLER oldHandler;
	USHORT oldAction, Action;

	/* turn off control C checking */

	if (setting)
		Action = SIGA_KILL;
	else
		Action = SIGA_IGNORE;

	DosSetSigHandler((PFNSIGHANDLER) NULL, &oldHandler, &oldAction,
		Action, SIG_CTRLBREAK);
	DosSetSigHandler((PFNSIGHANDLER) NULL, &oldHandler, &oldAction,
		Action, SIG_CTRLC);
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

#ifdef EMXVIDEO
	tcflush(0, TCIFLUSH);
#else
	if (mouse_handle)
		MouFlushQue(mouse_handle);

	KbdFlushBuffer(0);
#endif
}

int PDC_mouse_set(void)
{
#ifndef EMXVIDEO

	unsigned long mbe = SP->_trap_mbe;

	if (mbe && !mouse_handle)
	{
		memset(&old_mouse_status, 0, sizeof(MOUSE_STATUS));
		MouOpen(NULL, &mouse_handle);
		if (mouse_handle)
			MouDrawPtr(mouse_handle);
	}
	else if (!mbe && mouse_handle)
	{
		MouClose(mouse_handle);
		mouse_handle = 0;
	}

	if (mbe && mouse_handle)
	{
		USHORT mask =

			((mbe & (BUTTON1_PRESSED | BUTTON1_CLICKED | 
			BUTTON1_MOVED)) ? 6 : 0) |

			((mbe & (BUTTON3_PRESSED | BUTTON3_CLICKED | 
			BUTTON3_MOVED)) ? 24 : 0) |

			((mbe & (BUTTON2_PRESSED | BUTTON2_CLICKED | 
			BUTTON2_MOVED)) ? 96 : 0);

		MouSetEventMask(&mask, mouse_handle);
	}
#endif
	return OK;
}

int PDC_modifiers_set(void)
{
	key_pressed = FALSE;

	return OK;
}
