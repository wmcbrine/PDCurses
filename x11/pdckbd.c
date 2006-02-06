/**************************************************************************
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
**************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>

#ifdef PDCDEBUG
const char *rcsid_PDCkbd =
	"$Id: pdckbd.c,v 1.16 2006/02/06 03:13:31 wmcbrine Exp $";
#endif

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
	PDCurses  int PDC_get_input_fd(void);

**man-end**********************************************************************/

unsigned long PDC_get_input_fd(void)
{
	PDC_LOG(("PDC_get_input_fd() - called\n"));

	return XCurses_get_input_fd();
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
	PDCurses  bool PDC_check_bios_key(void);

**man-end**********************************************************************/

bool PDC_check_bios_key(void)
{
	PDC_LOG(("PDC_check_bios_key() - called\n"));

	return XCurses_kbhit();
}         

/*man-start*********************************************************************

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

**man-end**********************************************************************/

bool PDC_get_ctrl_break(void)
{
	PDC_LOG(("PDC_get_ctrl_break() - called\n"));

	return FALSE;
}

/*man-start*********************************************************************

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

**man-end**********************************************************************/

int PDC_rawgetch(void)
{
	extern WINDOW *_getch_win_;

	PDC_LOG(("PDC_rawgetch() - called\n"));

	if (_getch_win_ == (WINDOW *)NULL)
		return -1;

	if ((SP->delaytenths || _getch_win_->_delayms || _getch_win_->_nodelay)
	    && !PDC_breakout())
		return -1;

	return XCurses_rawgetch(0);
}

/*man-start*********************************************************************

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

**man-end**********************************************************************/

int PDC_set_ctrl_break(bool setting)
{
	PDC_LOG(("PDC_set_ctrl_break() - called\n"));

	return OK;
}

/*man-start*********************************************************************

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

**man-end**********************************************************************/

unsigned long PDC_get_key_modifiers(void)
{
	PDC_LOG(("PDC_get_key_modifiers() - called\n"));

	return XCurses_get_key_modifiers();
}
