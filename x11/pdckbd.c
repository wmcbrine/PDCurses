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

RCSID("$Id: pdckbd.c,v 1.33 2006/07/30 23:57:04 wmcbrine Exp $");

MOUSE_STATUS Trapped_Mouse_status;

#define TRAPPED_MOUSE_X_POS	  (Trapped_Mouse_status.x)
#define TRAPPED_MOUSE_Y_POS	  (Trapped_Mouse_status.y)
#define TRAPPED_BUTTON_STATUS(x)  (Trapped_Mouse_status.button[(x) - 1])

static bool PDC_kbhit(void)
{
	int s;

	PDC_LOG(("%s:PDC_kbhit() - called\n", XCLOGMSG));

	/* Is something ready to be read on the socket ? Must be a key. */

	FD_ZERO(&readfds);
	FD_SET(XC_key_sock, &readfds);

	if ((s = select(FD_SETSIZE, (FD_SET_CAST)&readfds, NULL, 
	    NULL, &socket_timeout)) < 0)
		XCursesExitCursesProcess(3,
			"child - exiting from PDC_kbhit select failed");

	PDC_LOG(("%s:PDC_kbhit() - returning %s\n", XCLOGMSG,
		(s == 0) ? "FALSE" : "TRUE"));

	if (s == 0)
		return FALSE;

	return TRUE;
}

int PDC_get_bios_key(void)
{
	unsigned long newkey = 0;
	int key = 0;

	PDC_LOG(("%s:PDC_get_bios_key() - called\n", XCLOGMSG));

	while (1)
	{
	    if (XC_read_socket(XC_key_sock, (char *)&newkey,
		sizeof(unsigned long)) < 0)
		    XCursesExitCursesProcess(2, 
			"exiting from PDC_get_bios_key");

	    pdc_key_modifiers = (newkey >> 24) & 0xFF;
	    key = (int)(newkey & 0x00FFFFFF);

	    if (key == KEY_MOUSE)
	    {
		if (XC_read_socket(XC_key_sock, (char *)&Trapped_Mouse_status, 
		    sizeof(MOUSE_STATUS)) < 0)
			XCursesExitCursesProcess(2,
			    "exiting from PDC_get_bios_key");

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

	PDC_LOG(("%s:PDC_get_bios_key() - key %d returned\n", XCLOGMSG, key));

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

  Portability:
	PDCurses  int PDC_get_input_fd(void);

**man-end****************************************************************/

unsigned long PDC_get_input_fd(void)
{
	PDC_LOG(("PDC_get_input_fd() - called\n"));

	return XC_key_sock;
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
	PDC_LOG(("PDC_check_bios_key() - called\n"));

	return PDC_kbhit();
}         

/*man-start**************************************************************

  PDC_get_ctrl_break()	- return OS control break state

  PDCurses Description:
	This is a private PDCurses routine.

	Returns the current OS Control Break Check state.

  PDCurses Return Value:
	This function returns TRUE if the Control Break
	Check is enabled otherwise FALSE is returned.

  Portability:
	PDCurses  bool PDC_get_ctrl_break(void);

**man-end****************************************************************/

bool PDC_get_ctrl_break(void)
{
	PDC_LOG(("PDC_get_ctrl_break() - called\n"));

	return FALSE;
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
	PDC_LOG(("PDC_set_ctrl_break() - called\n"));

	return OK;
}

/*man-start**************************************************************

  PDC_validchar() - validate/translate passed character

  PDCurses Description:
	This is a private PDCurses function.

	Checks that 'c' is a valid character, and if so returns it,
	with function key translation applied if 'w' has keypad mode
	set.  If char is invalid, returns -1.

  PDCurses Return Value:
	This function returns -1 if the passed character is invalid, or
	the WINDOW *_getch_win_ is NULL, or _getch_win_'s keypad is not 
	active.

	Otherwise, this function returns the PDCurses equivalent of the
	passed character.  See the function key and key macros in
	<curses.h>.

  Portability:
	PDCurses  int PDC_validchar(int c);

**man-end****************************************************************/

int PDC_validchar(int c)
{
	PDC_LOG(("PDC_validchar() - called\n"));

	/* skip special keys if !keypad mode */

	if ((_getch_win_ == (WINDOW *)NULL) ||
	    ((unsigned int)c >= 256 && !_getch_win_->_use_keypad))
		c = -1;

	PDC_LOG(("PDC_validchar() - returned: %x\n", c));

	return c;
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

	while (PDC_kbhit())
		PDC_get_bios_key();
}
