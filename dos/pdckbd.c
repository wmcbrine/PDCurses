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
                          
#ifdef GO32
# include <signal.h>
#else
# include <conio.h>
#endif

#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>


#ifdef PDCDEBUG
char *rcsid_PDCkbd  = "$Id: pdckbd.c,v 1.4 2002/09/01 08:13:29 mark Exp $";
#endif

/*******************************************************************************
*	Table for key code translation of function keys in keypad mode
*	These values are for strict IBM keyboard compatibles only
*******************************************************************************/


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

 /* Shifted Keypad		 */
 0xb0, KEY_SHOME, 0xb1, KEY_SUP,   0xb2, KEY_SPREVIOUS,
 0xb3, KEY_SLEFT, 0xb4, KEY_SRIGHT,
 0xb5, KEY_SEND,  0xb6, KEY_SDOWN, 0xb7, KEY_SNEXT,
 0xb8, KEY_SIC,   0xb9, KEY_SDC,

 /* Shft-Function Keys	 */
 0x54, KEY_F(13), 0x55, KEY_F(14), 0x56, KEY_F(15), 0x57, KEY_F(16),
 0x58, KEY_F(17), 0x59, KEY_F(18), 0x5a, KEY_F(19), 0x5b, KEY_F(20),
 0x5c, KEY_F(21), 0x5d, KEY_F(22),

 /* Ctl-Function Keys	 */
 0x5e, KEY_F(25), 0x5f, KEY_F(26), 0x60, KEY_F(27), 0x61, KEY_F(28),
 0x62, KEY_F(29), 0x63, KEY_F(30), 0x64, KEY_F(31), 0x65, KEY_F(32),
 0x66, KEY_F(33), 0x67, KEY_F(34),

 /* Alt-Function Keys	 */
 0x68, KEY_F(37), 0x69, KEY_F(38), 0x6a, KEY_F(39), 0x6b, KEY_F(40),
 0x6c, KEY_F(41), 0x6d, KEY_F(42), 0x6e, KEY_F(43), 0x6f, KEY_F(44),
 0x70, KEY_F(45), 0x71, KEY_F(46),

 /* Control-Keypad		 */
 0x77, CTL_HOME, 0x84, CTL_PGUP,
 0x73, CTL_LEFT, 0x74, CTL_RIGHT,
 0x75, CTL_END, 0x76, CTL_PGDN,

 /* Alt-Numbers		 */
 0x78, ALT_1, 0x79, ALT_2, 0x7a, ALT_3, 0x7b, ALT_4,
 0x7c, ALT_5, 0x7d, ALT_6, 0x7e, ALT_7, 0x7f, ALT_8,
 0x80, ALT_9, 0x81, ALT_0,

 /* Extended codes		 */
 0x85, KEY_F(11), 0x86, KEY_F(12), 0x87, KEY_F(23), 0x88, KEY_F(24),
 0x89, KEY_F(35), 0x8a, KEY_F(36), 0x8b, KEY_F(47), 0x8c, KEY_F(48),
 0x03, 0, /* NULL */

#if defined(NUMKEYPAD)
 0xff, (int)'/',     0x0d, (int)'\n',
 0xfa, (int)'*',     0xfd, (int)'-',      0xfb, (int)'+',
#else
 0xff, PADSLASH,     0x0d, PADENTER,      
 0xfa, PADSTAR,      0xfd, PADMINUS,      0xfb, PADPLUS,
#endif

 0x0a, CTL_PADENTER,
 0xa6, ALT_PADENTER, 0x53, (int)'.',      0xfc, CTL_ENTER,
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

static unsigned long pdc_key_modifiers=0L;
MOUSE_STATUS Trapped_Mouse_status;

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
unsigned long PDC_get_input_fd(void)
#else
unsigned long PDC_get_input_fd()
#endif
/***********************************************************************/
{

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_input_fd() - called\n");
#endif

	return (unsigned long)fileno (stdin);
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

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_check_bios_key() - called\n");
#endif

	return(kbhit());
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
	int ascii=0,scan=0;
	static unsigned char keyboard_function=0xFF;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_bios_key() - called\n");
#endif

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
	pdc_key_modifiers = 0;
	if (SP->save_key_modifiers)
		{
		regs.h.ah = 0x02;             /* get shift status for all keyboards */
		int86(0x16, &regs, &regs);
		if (regs.h.al & 0x01
		||  regs.h.al & 0x02)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_SHIFT;
		if (regs.h.al & 0x04)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_CONTROL;
		if (regs.h.al & 0x08)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_ALT;
		if (regs.h.al & 0x20)
			pdc_key_modifiers |= PDC_KEY_MODIFIER_NUMLOCK;
		}

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
	if (ascii == 0xe0 && scan == 0x47 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift Home */
		return ((int) (0xb0 << 8));
	if (ascii == 0xe0 && scan == 0x48 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift Up */
		return ((int) (0xb1 << 8));
	if (ascii == 0xe0 && scan == 0x49 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift PgUp */
		return ((int) (0xb2 << 8));
	if (ascii == 0xe0 && scan == 0x4b && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift Left */
		return ((int) (0xb3 << 8));
	if (ascii == 0xe0 && scan == 0x4d && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift Right */
		return ((int) (0xb4 << 8));
	if (ascii == 0xe0 && scan == 0x4f && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift End */
		return ((int) (0xb5 << 8));
	if (ascii == 0xe0 && scan == 0x50 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift Down */
		return ((int) (0xb6 << 8));
	if (ascii == 0xe0 && scan == 0x51 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift PgDn */
		return ((int) (0xb7 << 8));
	if (ascii == 0xe0 && scan == 0x52 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift Ins */
		return ((int) (0xb8 << 8));
	if (ascii == 0xe0 && scan == 0x53 && pdc_key_modifiers & PDC_KEY_MODIFIER_SHIFT) /* Shift Del */
		return ((int) (0xb9 << 8));
	if (ascii == 0x00 || ascii == 0xe0)
		return ((int) (scan << 8));
	return ((int) (ascii));
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

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_ctrl_break() - called\n");
#endif

	regs.h.ah = 0x33;
	regs.h.al = 0x00;
	int86(0x21, &regs, &regs);
	return ((bool) regs.h.dl);
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

#ifdef	NDP
	if( setting )
		_ignore_breaks();
	else
		_watch_breaks();
#else
# ifdef GO32
	(void*)signal(SIGINT,(setting ? SIG_DFL : SIG_IGN));
/*	__djgpp_set_ctrl_c(setting);*/
	setcbrk(setting);
# else
	regs.h.ah = 0x33;
	regs.h.al = 0x00;
	regs.h.dl = (unsigned char) (setting ? 1 : 0);
	int86(0x21, &regs, &regs);
# endif
#endif
	return( OK );
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

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_sysgetch() - called\n");
#endif

	if (_getch_win_ == (WINDOW *)NULL)  /* @@ */
		return (-1);

	if (_getch_win_->_nodelay && !PDC_breakout())
		return (-1);

	while (1)
	{
		c = PDC_get_bios_key();
		/*
		 * Return the key if it is not a special key.
		 */
		if ((unsigned int)c < 256)
			return(c);
		if ((c = PDC_validchar(c)) >= 0)
			return (c);		/* get & check next char */
	}
}

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

	if ((unsigned int)c < 256)	return (c);  /* normal character */
	if (!(_getch_win_->_use_keypad))	return (-1); /* skip if keys if !keypad mode */

	/*
	* Under DOS, extended keys are in the upper byte.  Shift down for a
	* comparison.
	*/
	c = (c >> 8) & 0xFF;

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
