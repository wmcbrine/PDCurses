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

#define	CURSES_LIBRARY	1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCkbd  = "$Id: pdckbd.c,v 1.3 2002/06/23 10:52:48 mark Exp $";
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

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_breakout() - called\n");
#endif

	if (c_ungind)
		return (TRUE);			/* ungotten char */
	if (c_pindex > c_gindex)
		return (TRUE);			/* buffered char */

	return((bool)PDC_check_bios_key());
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
	return(XCurses_kbhit());
}         

/*man-start*********************************************************************

  PDC_get_ctrl_break()	- return OS control break state

  PDCurses Description:
 	This is a private PDCurses routine.

 	Returns the current OS Control Break Check state.

  PDCurses Return Value:
 	DOS:
 		This function returns TRUE if the Control Break
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
	return(FALSE);
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

	int	c=0;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_rawgetch() - called\n");
#endif

	if (_getch_win_ == (WINDOW *)NULL)   /* @@ */
		return( -1 );

	if (_getch_win_->_nodelay && !PDC_breakout()) /* @@ */
		return( -1 );

	c = XCurses_rawgetch( SP->delaytenths );
	return(c);
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
	return(OK);
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
	return(XCurses_get_key_modifiers());
}
