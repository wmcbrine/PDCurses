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
/*#define NEEDS_OS2       1*/

#if	defined (DOS)
#include <conio.h>
#endif

#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

#ifdef OS2
#  if defined (CURSES__32BIT__) || defined(CSET2) || defined(TC)
#     include <signal.h>
#  else
#     define INCL_DOSSIGNALS
#     define INCL_NOCOMMON
#     include <bsedos.h>
#  endif
#endif

#ifdef UNIX
#include <signal.h>
#endif

#ifdef PDCDEBUG
char *rcsid_PDCkbd  = "$Id: pdckbd.c,v 1.1 2001/01/10 08:31:09 mark Exp $";
#endif

static unsigned long pdc_key_modifiers=0L;

/*******************************************************************************
*	Table for key code translation of function keys in keypad mode
*	These values are for strict IBM keyboard compatibles only
*******************************************************************************/


#if	defined(DOS) || defined(OS2)
static int kptab[] =
{
 /* Normal Function Keys	 */
 0x3b, KEY_F(1), 0x3c, KEY_F(2), 0x3d, KEY_F(3), 0x3e, KEY_F(4),
 0x3f, KEY_F(5), 0x40, KEY_F(6), 0x41, KEY_F(7), 0x42, KEY_F(8),
 0x43, KEY_F(9), 0x44, KEY_F(10),

 /* Normal Keypad		 */
 0x47, KEY_HOME, 0x48, KEY_UP,   0x49, KEY_PPAGE,
 0x4b, KEY_LEFT, 0x4c, KEY_B2,   0x4d, KEY_RIGHT,
 0x4f, KEY_END,  0x50, KEY_DOWN, 0x51, KEY_NPAGE,
 0x52, KEY_IC,   0x53, KEY_DC,

 /* Shft-Function Keys	 */
 0x54, KEY_F(11), 0x55, KEY_F(12), 0x56, KEY_F(13), 0x57, KEY_F(14),
 0x58, KEY_F(15), 0x59, KEY_F(16), 0x5a, KEY_F(17), 0x5b, KEY_F(18),
 0x5c, KEY_F(19), 0x5d, KEY_F(20),

 /* Ctl-Function Keys	 */
 0x5e, KEY_F(21), 0x5f, KEY_F(22), 0x60, KEY_F(23), 0x61, KEY_F(24),
 0x62, KEY_F(25), 0x63, KEY_F(26), 0x64, KEY_F(27), 0x65, KEY_F(28),
 0x66, KEY_F(29), 0x67, KEY_F(30),

 /* Alt-Function Keys	 */
 0x68, KEY_F(31), 0x69, KEY_F(32), 0x6a, KEY_F(33), 0x6b, KEY_F(34),
 0x6c, KEY_F(35), 0x6d, KEY_F(36), 0x6e, KEY_F(37), 0x6f, KEY_F(38),
 0x70, KEY_F(39), 0x71, KEY_F(40),

 /* Control-Keypad		 */
 0x77, CTL_HOME, 0x84, CTL_PGUP,
 0x73, CTL_LEFT, 0x74, CTL_RIGHT,
 0x75, CTL_END, 0x76, CTL_PGDN,

 /* Alt-Numbers		 */
 0x78, ALT_1, 0x79, ALT_2, 0x7a, ALT_3, 0x7b, ALT_4,
 0x7c, ALT_5, 0x7d, ALT_6, 0x7e, ALT_7, 0x7f, ALT_8,
 0x80, ALT_9, 0x81, ALT_0,

 /* Extended codes		 */
 0x85, KEY_F(41), 0x86, KEY_F(42), 0x87, KEY_F(43), 0x88, KEY_F(44),
 0x89, KEY_F(45), 0x8a, KEY_F(46), 0x8b, KEY_F(47), 0x8c, KEY_F(48),
 0x03, 0, /* NULL */

#if defined(NUMKEYPAD)
 0xff, (int)'/',     0x0d, (int)'\n',
 0xfa, (int)'*',     0xfd, (int)'-',      0xfb, (int)'+',
#else
 0xff, PADSLASH,     0x0d, PADENTER,      
 0xfa, PADSTAR,      0xfd, PADMINUS,      0xfb, PADPLUS,
#endif

 0x0a, CTL_PADENTER,
 0xa6, ALT_PADENTER, 0x53, SHF_PADSTOP,   0xfc, CTL_ENTER,
 0x93, CTL_DEL,      0x8f, CTL_PADCENTER, 0x90, CTL_PADPLUS,
 0x8e, CTL_PADMINUS, 0x95, CTL_PADSLASH,  0x96, CTL_PADSTAR,
 0x4e, ALT_PADPLUS,  0x4a, ALT_PADMINUS,  0xa4, ALT_PADSLASH,
 0x37, ALT_PADSTAR,  0x92, CTL_INS,       0xa2, ALT_INS,
 0xa3, ALT_DEL,      0x8d, CTL_UP,        0x91, CTL_DOWN,
 0x94, CTL_TAB,      0xa5, ALT_TAB,       0x82, ALT_MINUS,
 0x83, ALT_EQUAL,    0x99, ALT_PGUP,      0xa1, ALT_PGDN,
 0x9f, ALT_END,      0x98, ALT_UP,        0xa0, ALT_DOWN,
 0x9d, ALT_RIGHT,    0x9b, ALT_LEFT,      0x1c, ALT_ENTER,
 0x97, ALT_HOME,     0x01, ALT_ESC,       0x0e, ALT_BKSP,
 0x29, ALT_BQUOTE,   0x1a, ALT_LBRACKET,  0x1b, ALT_RBRACKET,
 0x27, ALT_SEMICOLON,0x28, ALT_FQUOTE,    0x33, ALT_COMMA,
 0x34, ALT_STOP,     0x35, ALT_FSLASH,    0x2b, ALT_BSLASH,

 /* Alt-Alphabet		 */
 0x1e, ALT_A, 0x30, ALT_B, 0x2e, ALT_C, 0x20, ALT_D,
 0x12, ALT_E, 0x21, ALT_F, 0x22, ALT_G, 0x23, ALT_H,
 0x17, ALT_I, 0x24, ALT_J, 0x25, ALT_K, 0x26, ALT_L,
 0x32, ALT_M, 0x31, ALT_N, 0x18, ALT_O, 0x19, ALT_P,
 0x10, ALT_Q, 0x13, ALT_R, 0x1f, ALT_S, 0x14, ALT_T,
 0x16, ALT_U, 0x2f, ALT_V, 0x11, ALT_W, 0x2d, ALT_X,
 0x15, ALT_Y, 0x2c, ALT_Z,
 0x0f, KEY_BTAB,
 0x100, (-1)
};
 /* End of kptab[]		 */
#endif

#ifdef	FLEXOS
int	kptab[] =
{
 /* Normal Function Keys	 */
 0x1001, KEY_F(1), 0x1002, KEY_F(2), 0x1003, KEY_F(3), 0x1004, KEY_F(4),
 0x1005, KEY_F(5), 0x1006, KEY_F(6), 0x1007, KEY_F(7), 0x1008, KEY_F(8),
 0x1009, KEY_F(9), 0x100A, KEY_F(10),

 /* Normal Keypad		 */
 0x2018, KEY_HOME, 0x2010, KEY_UP, 0x2014, KEY_PPAGE,
 0x2012, KEY_LEFT, 0x2035, KEY_B2, 0x2013, KEY_RIGHT,
 0x2008, KEY_LL, 0x2011, KEY_DOWN, 0x2015, KEY_NPAGE,
 0x2009, KEY_IC, 0x200A, KEY_DC,

 /* Shft-Function Keys	 */
 0x1401, KEY_F(11), 0x1402, KEY_F(12), 0x1403, KEY_F(13), 0x1404, KEY_F(14),
 0x1405, KEY_F(15), 0x1406, KEY_F(16), 0x1407, KEY_F(17), 0x1408, KEY_F(18),
 0x1409, KEY_F(19), 0x140A, KEY_F(20),

 /* Ctl-Function Keys	 */
 0x1101, KEY_F(21), 0x1102, KEY_F(22), 0x1103, KEY_F(23), 0x1104, KEY_F(24),
 0x1105, KEY_F(25), 0x1106, KEY_F(26), 0x1107, KEY_F(27), 0x1108, KEY_F(28),
 0x1109, KEY_F(29), 0x110A, KEY_F(30),

 /* Alt-Function Keys	 */
 0x1201, KEY_F(31), 0x1202, KEY_F(32), 0x1203, KEY_F(33), 0x1204, KEY_F(34),
 0x1205, KEY_F(35), 0x1206, KEY_F(36), 0x1207, KEY_F(37), 0x1208, KEY_F(38),
 0x1209, KEY_F(39), 0x120A, KEY_F(40),

 /* Control-Keypad		 */
 0x2118, CTL_HOME, 0x2114, CTL_PGUP,
 0x2112, CTL_LEFT, 0x2113, CTL_RIGHT,
 0x2108, CTL_END, 0x2115, CTL_PGDN,

 /* Alt-Numbers		 */
 0x2231, ALT_1, 0x2232, ALT_2, 0x2233, ALT_3, 0x2234, ALT_4,
 0x2235, ALT_5, 0x2236, ALT_6, 0x2237, ALT_7, 0x2238, ALT_8,
 0x2239, ALT_9, 0x2230, ALT_0,

 /* Alt-Alphabet		 */
 0x0200 | (unsigned) 'A', ALT_A, 0x0200 | (unsigned) 'B', ALT_B,
 0x0200 | (unsigned) 'C', ALT_C, 0x0200 | (unsigned) 'D', ALT_D,
 0x0200 | (unsigned) 'E', ALT_E, 0x0200 | (unsigned) 'F', ALT_F,
 0x0200 | (unsigned) 'G', ALT_G, 0x0200 | (unsigned) 'H', ALT_H,
 0x0200 | (unsigned) 'I', ALT_I, 0x0200 | (unsigned) 'J', ALT_J,
 0x0200 | (unsigned) 'K', ALT_K, 0x0200 | (unsigned) 'L', ALT_L,
 0x0200 | (unsigned) 'M', ALT_M, 0x0200 | (unsigned) 'N', ALT_N,
 0x0200 | (unsigned) 'O', ALT_O, 0x0200 | (unsigned) 'P', ALT_P,
 0x0200 | (unsigned) 'Q', ALT_Q, 0x0200 | (unsigned) 'R', ALT_R,
 0x0200 | (unsigned) 'S', ALT_S, 0x0200 | (unsigned) 'T', ALT_T,
 0x0200 | (unsigned) 'U', ALT_U, 0x0200 | (unsigned) 'V', ALT_V,
 0x0200 | (unsigned) 'W', ALT_W, 0x0200 | (unsigned) 'X', ALT_X,
 0x0200 | (unsigned) 'Y', ALT_Y, 0x0200 | (unsigned) 'Z', ALT_Z,

 0x0200 | (unsigned) 'a', ALT_A, 0x0200 | (unsigned) 'b', ALT_B,
 0x0200 | (unsigned) 'c', ALT_C, 0x0200 | (unsigned) 'd', ALT_D,
 0x0200 | (unsigned) 'e', ALT_E, 0x0200 | (unsigned) 'f', ALT_F,
 0x0200 | (unsigned) 'g', ALT_G, 0x0200 | (unsigned) 'h', ALT_H,
 0x0200 | (unsigned) 'i', ALT_I, 0x0200 | (unsigned) 'j', ALT_J,
 0x0200 | (unsigned) 'k', ALT_K, 0x0200 | (unsigned) 'l', ALT_L,
 0x0200 | (unsigned) 'm', ALT_M, 0x0200 | (unsigned) 'n', ALT_N,
 0x0200 | (unsigned) 'o', ALT_O, 0x0200 | (unsigned) 'p', ALT_P,
 0x0200 | (unsigned) 'q', ALT_Q, 0x0200 | (unsigned) 'r', ALT_R,
 0x0200 | (unsigned) 's', ALT_S, 0x0200 | (unsigned) 't', ALT_T,
 0x0200 | (unsigned) 'u', ALT_U, 0x0200 | (unsigned) 'v', ALT_V,
 0x0200 | (unsigned) 'w', ALT_W, 0x0200 | (unsigned) 'x', ALT_X,
 0x0200 | (unsigned) 'y', ALT_Y, 0x0200 | (unsigned) 'z', ALT_Z,

 0x0f, KEY_BTAB,

 /* End of kptab[]		 */
 0x100, -1
};
#endif

/*man-start*********************************************************************

  PDC_breakout()	- check for type-ahead

  X/Open Description:
 	The curses package does the "line-breakout optimisation" by
 	looking for type-ahead periodically while updating the screen.
 	If input is found, the current update will be postponed until
 	refresh() or doupdate() are called again.  This allows faster
 	response to commands typed in advance.  Normally, the input FILE
 	pointer passed to newterm(), or stdin in the case when initscr()
 	was called, will be used to do this type-ahead checking.  This routine
 	will do the actual check for PDcurses to see if input is pending.

  PDCurses Description:
 	This routine does what the PDcurses typeahead routine used to do.  But
 	to make that routine consistent with its System V counterpart, all it
 	does now is set whether or not typeahead checking is performed.

  X/Open Return Value:
 	The PDC_breakout() routine returns TRUE if keyboard input is pending
 	otherwise FALSE is returned.

  Portability:
 	PDCurses	bool PD_breakout( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_breakout( void )
#else
bool	PDC_breakout()
#endif
/***********************************************************************/
{
extern	int	c_pindex;			/* putter index */
extern	int	c_gindex;			/* getter index */
extern	int	c_ungind;			/* wungetch() push index */
#ifdef	TC
#  pragma argsused
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_breakout() - called\n");
#endif

	if (c_ungind)
		return (TRUE);			/* ungotten char */
	if (c_pindex > c_gindex)
		return (TRUE);			/* buffered char */
	if (SP->raw_inp)
		return((bool)PDC_check_bios_key());/* raw mode test */

	return((bool)PDC_check_bios_key());	/* normal mode test */
}

/*man-start*********************************************************************

  PDC_get_input_fd()	- Get file descriptor used for PDCurses input

  PDCurses Description:
 	This is a private PDCurses routine.

 	This routine will return the file descriptor that PDCurses reads
 	its input from. It can be used for select().

  PDCurses Return Value:
 	Returns a file descriptor.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int	PDC_get_input_fd( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int PDC_get_input_fd(void)
#else
int PDC_get_input_fd()
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_input_fd() - called\n");
#endif

	return fileno (stdin);
}

/*man-start*********************************************************************

  PDC_check_bios_key()	- Check BIOS key data area for input

  PDCurses Description:
 	This is a private PDCurses routine.

 	This routine will check the BIOS for any indication that
 	keystrokes are pending.

  PDCurses Return Value:
 	Returns 1 if a keyboard character is available, 0 otherwise.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	bool	PDC_check_bios_key( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
bool PDC_check_bios_key(void)
#else
bool PDC_check_bios_key()
#endif
/***********************************************************************/
{
#ifdef	OS2
# if !defined(MSC) && !defined(EMXVIDEO)
	KBDKEYINFO keyInfo={0};
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_check_bios_key() - called\n");
#endif

#ifdef	FLEXOS
	retcode = s_get(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	return( (retcode < 0L) ? ERR : vir.vc_tahead );
#endif

#if	defined (DOS)
 return(kbhit());
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	if (SP->tahead == -1)	/* Nothing typed yet */
	{							/* See if there's anything */
		SP->tahead = _read_kbd (0, 0, 0);
		if (SP->tahead == 0)	/* Read additional */
			SP->tahead = (_read_kbd (0, 1, 0) << 8);
	}
	return(SP->tahead != -1) ? 1 : 0;
# else
#  if !defined(MSC)
/*	KbdCharIn(&keyInfo, IO_NOWAIT, 0);*/	/* get a character	*/
	KbdPeek(&keyInfo, 0);	/* peek at keyboard	*/
	return (keyInfo.fbStatus != 0);
#  else
 return(kbhit());
#  endif
# endif
#endif

#ifdef UNIX
	return(PDC_kbhit());
#endif

#if defined(XCURSES)
	return(XCurses_kbhit());
#endif
}         

/*man-start*********************************************************************

  PDC_get_bios_key()	- Returns the next key available from the BIOS.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Returns the next key code struck at the keyboard. If the low 8
 	bits are 0, the upper bits contain the extended character
 	code. If bit 0-7 are non-zero, the upper bits = 0.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_get_bios_key( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_get_bios_key(void)
#else
int	PDC_get_bios_key()
#endif
/***********************************************************************/
{
#ifdef	FLEXOS
unsigned ch = 0;
#endif

#if	defined (DOS)
	int ascii=0,scan=0;
	static unsigned char keyboard_function=0xFF;
#endif

#if	defined (OS2)
	int ascii=0,scan=0;
# ifndef EMXVIDEO
	KBDKEYINFO keyInfo={0};
# endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_bios_key() - called\n");
#endif

#ifdef	FLEXOS
	retcode = s_read(0x00, 0L, (char *) &ch, 2L, 0L);
	return( (retcode < 0L) ? ERR : ch );
#endif

#if	defined (DOS)
	if (keyboard_function == 0xFF)
		{
		regs.h.ah = 0x02;             /* get shift status for all keyboards */
		int86(0x16, &regs, &regs);
		scan = regs.h.al;
		regs.h.ah = 0x12;             /* get shift status for enhanced keyboards */
		int86(0x16, &regs, &regs);
		if (scan == regs.h.al
		&&  getdosmembyte(0x496) == 0x10)
			keyboard_function = 0x10;
		else
			keyboard_function = 0x0;
		}
	regs.h.ah = keyboard_function;
	int86(0x16, &regs, &regs);
	ascii = regs.h.al;
	scan = regs.h.ah;
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	if (SP->tahead == -1)
	{
		SP->tahead = _read_kbd (0, 1, 0);
		if (SP->tahead == 0)	/* Read additional */
			SP->tahead = (_read_kbd (0, 1, 0) << 8);
	}
	ascii = SP->tahead & 0xff;
	scan  = SP->tahead >> 8;
	SP->tahead = -1;
# else
	KbdCharIn(&keyInfo, IO_WAIT, 0);	/* get a character	*/
	ascii = keyInfo.chChar;
	scan = keyInfo.chScan;
# endif
#endif

#if	defined (DOS) || defined (OS2)
	if (scan == 0x1c && ascii == 0x0a)  /* ^Enter */
		return ((int) (0xfc00));
	if ((scan == 0x03 && ascii == 0x00)  /* ^@ - Null */
	||  (scan == 0xe0 && ascii == 0x0d)  /* PadEnter */
	||  (scan == 0xe0 && ascii == 0x0a)) /* ^PadEnter */
		return ((int) (ascii << 8));
	if ((scan == 0x37 && ascii == 0x2a)  /* Star */
	||  (scan == 0x4a && ascii == 0x2d)  /* Minus */
	||  (scan == 0x4e && ascii == 0x2b)  /* Plus */
	||  (scan == 0xe0 && ascii == 0x2f)) /* Slash */
		return ((int) ((ascii & 0x0f) | 0xf0) << 8);
	if (ascii == 0x00 || ascii == 0xe0)
		return ((int) (scan << 8));
	return ((int) (ascii));
#endif
}

/*man-start*********************************************************************

  PDC_get_ctrl_break()	- return OS control break state

  PDCurses Description:
 	This is a private PDCurses routine.

 	Returns the current OS Control Break Check state.

  PDCurses Return Value:
 	DOS:
 		This function returns TRUE on if the Control Break
 		Check is enabled otherwise FALSE is returned.

 	FLEXOS:
 		This function returns TRUE on if the Keyboard Mode
 		allows a break to bre reported otherwise FALSE is returned.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	bool	PDC_get_ctrl_break( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
bool	PDC_get_ctrl_break(void)
#else
bool	PDC_get_ctrl_break()
#endif
/***********************************************************************/
{
#ifdef	OS2
#  if defined(CURSES__32BIT__) || defined(CSET2) || defined(TC)
#     if defined(TC)
	void __cdecl (*oldAction) (int);
#     else
	void (*oldAction) (int);
#     endif
#  endif
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_ctrl_break() - called\n");
#endif

#ifdef	FLEXOS
	return ((kbmode & 0x01) ? TRUE : FALSE);
#endif

#ifdef	DOS
	regs.h.ah = 0x33;
	regs.h.al = 0x00;
	int86(0x21, &regs, &regs);
	return ((bool) regs.h.dl);
#endif

#ifdef	OS2
#  if defined(CURSES__32BIT__) || defined(CSET2) || defined(TC)
	oldAction = signal (SIGINT, SIG_DFL);
	if (oldAction == SIG_ERR) return FALSE;
	else signal (SIGINT, oldAction);
	return (oldAction != SIG_IGN);
#  else
	PFNSIGHANDLER oldHandler, oldHandler1;
	USHORT oldAction, oldAction1;
	/* get the current state, and set to ignore */
	DosSetSigHandler((PFNSIGHANDLER) NULL, &oldHandler, &oldAction,
		SIGA_IGNORE, SIG_CTRLBREAK);
	/* restore the previous state */
	DosSetSigHandler(oldHandler, &oldHandler1, &oldAction1,
		oldAction, SIG_CTRLBREAK);
	return(oldAction != SIGA_IGNORE);
#  endif
#endif

#ifdef UNIX
/* INCOMPLETE - use signals?*/
#endif
}

/*man-start*********************************************************************

  PDC_rawgetch()	- Returns the next uninterpreted character (if available).

  PDCurses Description:
 	Gets a character without any interpretation at all and returns
 	it. If keypad mode is active for the designated window,
 	function key translation will be performed.  Otherwise,
 	function keys are ignored.  If nodelay mode is active in the
 	window, then PDC_rawgetch() returns -1 if no character is
 	available.

 	WARNING:  It is unknown whether the FUNCTION key translation
 		  is performed at this level. --Frotz 911130 BUG

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int	PDC_rawgetch( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_rawgetch(void)
#else
int	PDC_rawgetch()
#endif
/***********************************************************************/
{
extern	WINDOW*	_getch_win_;
/* extern	WINDOW*	w;*/   /* w defined in wgetch() as static - _getch_win_ */
                        /* is the same window - all references to w changed*/
                        /* to _getch_win_ - marked with @@ */

	int	c=0;
	int	oldc=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_rawgetch() - called\n");
#endif

	if (_getch_win_ == (WINDOW *)NULL)   /* @@ */
		return( -1 );

	if (_getch_win_->_nodelay && !PDC_breakout()) /* @@ */
		return( -1 );

#ifdef UNIX
	c = getchar();
	return(c);
#endif

#if defined(XCURSES)
	c = XCurses_rawgetch();
	return(c);
#else
	while (1)		/* loop to get valid char */
	{
		c = PDC_get_bios_key();
		oldc = c;
		/*
		 * Return the key if it is not a special key.
		 */
		if ((c = PDC_validchar(c)) >= 0)
		{		/* get & check next char */
			return( c );
		}
		if (_getch_win_->_use_keypad)
			return( oldc );
	}
#endif
}

/*man-start*********************************************************************

  PDC_set_ctrl_break()	- Enables/Disables the host OS BREAK key check.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Enables/Disables the host OS BREAK key check. If the supplied setting
 	is TRUE, this enables CTRL/C and CTRL/BREAK to abort the process.
 	If FALSE, CTRL/C and CTRL/BREAK are ignored.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_set_ctrl_break( bool setting );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_set_ctrl_break(bool setting)
#else
int	PDC_set_ctrl_break(setting)
bool setting;
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_ctrl_break() - called\n");
#endif

#ifdef	FLEXOS
	retcode = s_get(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	if (retcode < 0L)
		return( ERR );

	vir.vc_kbmode = ((vir.vc_kbmode & ~0x01) & (setting) ? 0x01 : 0x00);

	retcode = s_set(T_VIRCON, 0L, (char *) &vir, (long) sizeof(vir));
	return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef	DOS
#  ifdef	NDP
	if( setting )
		_ignore_breaks();
	else
		_watch_breaks();
#  else
	regs.h.ah = 0x33;
	regs.h.al = 0x00;
	regs.h.dl = (unsigned char) (setting ? 1 : 0);
	int86(0x21, &regs, &regs);
#  endif
	return( OK );
#endif

#ifdef	OS2
#  if defined (CURSES__32BIT__) || defined (CSET2) || defined(TC)
	if (setting) {
		signal (SIGINT, SIG_DFL);
		signal (SIGBREAK, SIG_DFL);
	} else {
		signal (SIGINT, SIG_IGN);
		signal (SIGBREAK, SIG_IGN);
        }
	return( OK );
#  else
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
	return( OK );
#  endif	
#endif

#ifdef UNIX
/* INCOMPLETE */
#endif
}

/*man-start*********************************************************************

  PDC_sysgetch()	- Return a character using default system routines.

  PDCurses Description:
 	This is a private PDCurses function.

 	Gets a character without normal ^S, ^Q, ^P and ^C interpretation
 	and returns it.  If keypad mode is active for the designated
 	window, function key translation will be performed. Otherwise,
 	function keys are ignored. If nodelay mode is active in the
 	window, then sysgetch() returns -1 if no character is
 	available.

  PDCurses Return Value:
 	This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
 	No errors are defined for this routine.

  Portability:
 	PDCurses	int	PDC_sysgetch( void );

**man-end**********************************************************************/

#ifdef UNIX
static bool alarmed;
#endif

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_sysgetch(void)
#else
int	PDC_sysgetch()
#endif
/***********************************************************************/
{
extern	WINDOW*	_getch_win_;
/* extern	WINDOW*	w;*/   /* w defined in wgetch() as static - _getch_win_ */
                        /* is the same window - all references to w changed*/
                        /* to _getch_win_ - marked with @@ */

	int c=0;

#ifdef UNIX
	void (*oldsig)();
	void sigalrm();
	char keybuf[10];
	char *bufp=keybuf;
	int val=0;
	register int i=0;
#define MATCH_FALSE 0
#define MATCH_PARTIAL 1
#define MATCH_TRUE 2
	char match=0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_sysgetch() - called\n");
#endif

	if (_getch_win_ == (WINDOW *)NULL)  /* @@ */
		return (-1);

	if (_getch_win_->_nodelay && !PDC_breakout())
		return (-1);

#ifdef UNIX
/* INCOMPLETE */
	oldsig = signal(SIGALRM,sigalrm);
	alarmed = FALSE;
	while(1)
		{
		c = getchar();
		if (c != EOF)
			{
			*(bufp+i++) = c;
			*(bufp+i) = '\0';
			}
		if (alarmed)
			break;
		val = find_key(keybuf,i);
		if (val == MATCH_FALSE)
			{
			alarm(0);
			signal(SIGALRM,oldsig);
			return(keybuf[0]);
			}
		if (val != MATCH_PARTIAL)
			{
			alarm(0);
			signal(SIGALRM,oldsig);
			return(val);
			}
		alarm(1);
		}
	alarm(0);
	signal(SIGALRM,oldsig);
/* INCOMPLETE */
	return(c);
#else
	while (1)
	{
		c = PDC_get_bios_key();
#if	defined (DOS) || defined (OS2)
		/*
		 * Return the key if it is not a special key.
		 */
		if ((unsigned int)c < 256)
			return(c);
#endif
		if ((c = PDC_validchar(c)) >= 0)
		{
			return (c);		/* get & check next char */
		}
	}
#endif
}

#ifdef UNIX
static void sigalrm()
{
	alarmed = TRUE;
	signal(SIGALRM,sigalrm);
}
static int find_key(char *seq,int len)
{
	register int i=0;

	for (i=0;i<SP->number_keys;i++)
		{
		if (strcmp(seq,SP->key_seq[i]) == 0)
			return(SP->key_num[i]);
		if (memcmp(seq,SP->key_seq[i],len) == 0)
			return(MATCH_PARTIAL);
		}
	return(MATCH_FALSE);
}
#endif

/*man-start*********************************************************************

  PDC_validchar()	- validate/translate passed character
  
  PDCurses Description:
  	This is a private PDCurses function.
  
  	Checks that 'c' is a valid character, and if so returns it,
  	with function key translation applied if 'w' has keypad mode
  	set.  If char is invalid, returns -1.
  
  PDCurses Return Value:
  	This function returns -1 if the passed character is invalid, or
  	the WINDOW* 'w' is NULL, or 'w's keypad is not active.
  
  	This function returns 0 under Flexos if the passed character
  	is 0x0300.  (-:Forget why.  --Frotz	911130:-)
  
  	Otherwise, this function returns the PDCurses equivalent of the
  	passed character.  See the function key and key macros in
  	<curses.h>
  
  PDCurses Errors:
  	There are no errors defined for this routine.
  
  Portability:
  	PDCurses	int	PDC_validchar( int c );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
int	PDC_validchar(int c)
#else
int	PDC_validchar(c)
int c;
#endif
/***********************************************************************/
{
extern	WINDOW*	_getch_win_;
/* extern	WINDOW*	w;*/   /* w defined in wgetch() as static - _getch_win_ */
                        /* is the same window - all references to w changed*/
                        /* to _getch_win_ - marked with @@ */

	int *scanp=NULL;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_validchar() - called\n");
#endif

	if (_getch_win_ == (WINDOW *)NULL)
		return (-1);	/* bad window pointer	  */

#ifdef	FLEXOS
	if (c == 0x0300)
		return (0);	/* special case, ^@ = NULL	  */
#endif

	if ((unsigned int)c < 256)	return (c);  /* normal character */
	if (!(_getch_win_->_use_keypad))	return (-1); /* skip if keys if !keypad mode */

#if	defined (DOS) || defined (OS2)
	/*
	* Under DOS, extended keys are in the upper byte.  Shift down for a
	* comparison.
	*/
	c = (c >> 8) & 0xFF;
#endif

#if defined(XCURSES)
/* Nothing to do for Xcurses */
#else
	scanp = kptab;
	while (*scanp > 0)	/* search for value		 */
	{			/* (stops on table entry 0x100) */
		if (*scanp++ == c)
		{
			return (*scanp);	/* found, return it */
		}
		scanp++;
	}
	return( -1 );		/* not found, invalid */
#endif
}

/*man-start*********************************************************************

  PDC_get_key_modifiers()	- Returns the keyboard modifier(s) at time of last getch()

  PDCurses Description:
 	This is a private PDCurses routine.

 	Returns the keyboard modifiers effective at the time of the last getch()
 	call only if PDC_save_key_modifiers(TRUE) has been called before the
 	getch();
 	Use the macros; PDC_KEY_MODIFIER_* to determine which modifier(s)
 	were set.

  PDCurses Return Value:
 	This function returns the modifiers.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_get_key_modifiers( void );

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
unsigned long	PDC_get_key_modifiers(void)
#else
unsigned long	PDC_get_key_modifiers()
#endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_key_modifiers() - called\n");
#endif
	return(pdc_key_modifiers);
}
