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

#define CURSES_LIBRARY 1
#include <curses.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#undef PDC_debug

#ifdef PDCDEBUG
const char *rcsid_PDCdebug =
	"$Id: pdcdebug.c,v 1.16 2006/02/22 06:09:47 wmcbrine Exp $";
#endif

bool trace_on = FALSE;

/*man-start*********************************************************************

  PDC_debug()  - Write debugging info to log file.

  PDCurses Description:
	This is a private PDCurses routine.

  PDCurses Return Value:
	No return value.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses void PDC_debug(char *, ...);

**man-end**********************************************************************/

void PDC_debug(char *fmt, ...)
{
	va_list args;
	FILE *dbfp;
	char hms[9];
	time_t now;

	if (!trace_on)
		return; 

	/* open debug log file append */

	dbfp = fopen("trace", "a");
	if (dbfp == NULL)
	{
		fprintf(stderr,
			"PDC_debug(): Unable to open debug log file\n");
		return;
	}

	time(&now);
	strftime(hms, 9, "%H:%M:%S", localtime(&now));
	fprintf(dbfp, "At: %8.8ld - %s ", (long) clock(), hms);

	va_start(args, fmt);
	vfprintf(dbfp, fmt, args);
	va_end(args);

	fclose(dbfp);
}
