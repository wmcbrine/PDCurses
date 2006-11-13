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

#include "pdcx11.h"

RCSID("$Id: pdckbd.c,v 1.49 2006/11/13 17:50:08 wmcbrine Exp $");

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
	struct timeval socket_timeout = {0};
	int s;

	XC_LOG(("PDC_check_bios_key() - called\n"));

	/* Is something ready to be read on the socket ? Must be a key. */

	FD_ZERO(&xc_readfds);
	FD_SET(xc_key_sock, &xc_readfds);

	if ((s = select(FD_SETSIZE, (FD_SET_CAST)&xc_readfds, NULL, 
	    NULL, &socket_timeout)) < 0)
		XCursesExitCursesProcess(3, "child - exiting from "
			"PDC_check_bios_key select failed");

	PDC_LOG(("%s:PDC_check_bios_key() - returning %s\n", XCLOGMSG,
		(s == 0) ? "FALSE" : "TRUE"));

	if (s == 0)
		return FALSE;

	return TRUE;
}

int PDC_get_bios_key(void)
{
	unsigned long newkey = 0;
	int key = 0;

	XC_LOG(("PDC_get_bios_key() - called\n"));

	if (XC_read_socket(xc_key_sock, (char *)&newkey,
	    sizeof(unsigned long)) < 0)
		XCursesExitCursesProcess(2, "exiting from PDC_get_bios_key");

	pdc_key_modifiers = (newkey >> 24) & 0xFF;
	key = (int)(newkey & 0x00FFFFFF);

	if (key == KEY_MOUSE)
	{
		if (XC_read_socket(xc_key_sock, (char *)&pdc_mouse_status, 
		    sizeof(MOUSE_STATUS)) < 0)
			XCursesExitCursesProcess(2,
			    "exiting from PDC_get_bios_key");
	}

	PDC_LOG(("%s:PDC_get_bios_key() - key %d returned\n", XCLOGMSG, key));

	SP->key_code = ((unsigned)key >= 256);	/* temporary */

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

	return xc_key_sock;
}

void PDC_set_keyboard_binary(bool on)
{
        PDC_LOG(("PDC_set_keyboard_binary() - called\n"));
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

	while (PDC_check_bios_key())
		PDC_get_bios_key();
}

int PDC_mouse_set(void)
{
	return OK;
}
