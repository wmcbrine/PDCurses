/************************************************************************** 
* This file is part of PDCurses. PDCurses is public domain software;
* you may use it for any purpose. This software is provided AS IS with
* NO WARRANTY whatsoever.
*
* If you use PDCurses in an application, an acknowledgement would be
* appreciated, but is not mandatory. If you make corrections or
* enhancements to PDCurses, please forward them to the current
* maintainer for the benefit of other users.
*
* No distribution of modified PDCurses code may be made under the name
* "PDCurses", except by the current maintainer. (Although PDCurses is
* public domain, the name is a trademark.)
*
* See the file maintain.er for details of the current maintainer.
**************************************************************************/

#define	CURSES_LIBRARY 1
#include <curses.h>

#ifdef PDCDEBUG
const char *rcsid_PDCprint =
	"$Id: pdcprint.c,v 1.10 2006/02/16 22:59:49 wmcbrine Exp $";
#endif


/*man-start*********************************************************************

  PDC_print()	- Provides primitive access to the BIOS printer functions

  PDCurses Description:
	This is a private PDCurses routine.

	Implements write/init/read printer services at the BIOS level.

	This provides the basic support that PDCurses needs to dump the
	contents of windows or pads to the printer attached to the BIOS
	printer port.

  PDCurses Return Value:
	See the BIOS INT 0x17 specifications.

  PDCurses Errors:
	See the BIOS INT 0x17 specifications.

  Portability:
	PDCurses  int PDC_print(int cmd, int byte, int port);

**man-end**********************************************************************/

int PDC_print(int cmd, int byte, int port)
{
	PDC_LOG(("PDC_print() - called\n"));

	return OK;
}
