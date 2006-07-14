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
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#include "pdcx11.h"

RCSID("$Id: pdckbd.c,v 1.25 2006/07/14 03:21:16 wmcbrine Exp $");

bool XCurses_kbhit(void)
{
	int s;

	PDC_LOG(("%s:XCurses_kbhit() - called\n", XCLOGMSG));

	/* Is something ready to be read on the socket ? Must be a key. */

	FD_ZERO(&readfds);
	FD_SET(key_sock, &readfds);

	if ((s = select(FD_SETSIZE, (FD_SET_CAST)&readfds, NULL, 
	    NULL, &socket_timeout)) < 0)
		XCursesExitCursesProcess(3,
			"child - exiting from XCurses_kbhit select failed");

	PDC_LOG(("%s:XCurses_kbhit() - returning %s\n", XCLOGMSG,
		(s == 0) ? "FALSE" : "TRUE"));

	if (s == 0)
		return FALSE;

	return TRUE;
}

int XCurses_rawgetch(void)
{
	unsigned long newkey = 0;
	int key = 0;

	PDC_LOG(("%s:XCurses_rawgetch() - called\n", XCLOGMSG));

	while (1)
	{
	    if (read_socket(key_sock, (char *)&newkey,
		sizeof(unsigned long)) < 0)
		    XCursesExitCursesProcess(2, 
			"exiting from XCurses_rawchar");

	    pdc_key_modifier = (newkey >> 24) & 0xFF;
	    key = (int)(newkey & 0x00FFFFFF);

	    if (key == KEY_MOUSE)
	    {
		if (read_socket(key_sock, (char *)&Trapped_Mouse_status, 
		    sizeof(MOUSE_STATUS)) < 0)
			XCursesExitCursesProcess(2,
			    "exiting from XCurses_rawchar");

		/* Check if the mouse has been clicked on a slk area. If 
		   the return value is > 0 (indicating the label number), 
		   return with the KEY_F(key) value. */

		if ((newkey = PDC_mouse_in_slk(TRAPPED_MOUSE_Y_POS,
		    TRAPPED_MOUSE_X_POS)))
		{
		    if (TRAPPED_BUTTON_STATUS(1) & BUTTON_PRESSED)
		    {
			key = KEY_F(newkey);
			break;
		    }
		}

		break;

		MOUSE_LOG(("rawgetch-x: %d y: %d Mouse status: %x\n",
		    MOUSE_X_POS, MOUSE_Y_POS, Mouse_status.changes));
		MOUSE_LOG(("rawgetch-Button1: %x Button2: %x Button3: %x\n",
		    BUTTON_STATUS(1), BUTTON_STATUS(2), BUTTON_STATUS(3)));
	    }
	    else
		break;
	}

	PDC_LOG(("%s:XCurses_rawgetch() - key %d returned\n", XCLOGMSG, key));

	return key;
}

/*man-start**************************************************************

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
	PDCurses  int PDC_get_input_fd(void);

**man-end****************************************************************/

unsigned long PDC_get_input_fd(void)
{
	PDC_LOG(("PDC_get_input_fd() - called\n"));

	return key_sock;
}

/*man-start**************************************************************

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
	PDCurses  bool PDC_check_bios_key(void);

**man-end****************************************************************/

bool PDC_check_bios_key(void)
{
	PDC_LOG(("PDC_check_bios_key() - called\n"));

	return XCurses_kbhit();
}         

/*man-start**************************************************************

  PDC_get_ctrl_break()	- return OS control break state

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the current OS Control Break Check state.

  PDCurses Return Value:
	This function returns TRUE if the Control Break
	Check is enabled otherwise FALSE is returned.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  bool PDC_get_ctrl_break(void);

**man-end****************************************************************/

bool PDC_get_ctrl_break(void)
{
	PDC_LOG(("PDC_get_ctrl_break() - called\n"));

	return FALSE;
}

/*man-start**************************************************************

  PDC_rawgetch()	- Returns the next uninterpreted character
			  (if available).

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
	PDCurses  int PDC_rawgetch(void);

**man-end****************************************************************/

int PDC_rawgetch(void)
{
	PDC_LOG(("PDC_rawgetch() - called\n"));

	if (_getch_win_ == (WINDOW *)NULL)
		return -1;

	if ((SP->delaytenths || _getch_win_->_delayms || _getch_win_->_nodelay)
	    && !PDC_breakout())
		return -1;

	return XCurses_rawgetch();
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

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_set_ctrl_break(bool setting);

**man-end****************************************************************/

int PDC_set_ctrl_break(bool setting)
{
	PDC_LOG(("PDC_set_ctrl_break() - called\n"));

	return OK;
}

/*man-start**************************************************************

  PDC_get_key_modifiers()	- Returns the keyboard modifier(s)
				  at time of last getch()

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the keyboard modifiers effective at the time of the last 
	getch() call only if PDC_save_key_modifiers(TRUE) has been 
	called before the getch(). Use the macros; PDC_KEY_MODIFIER_* to 
	determine which modifier(s) were set.

  PDCurses Return Value:
	This function returns the modifiers.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_get_key_modifiers(void);

**man-end****************************************************************/

unsigned long PDC_get_key_modifiers(void)
{
	PDC_LOG(("PDC_get_key_modifiers() - called\n"));

	return pdc_key_modifier;
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

	while (XCurses_kbhit())
		XCurses_rawgetch();
}
