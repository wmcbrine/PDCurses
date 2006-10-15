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

#include <curspriv.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

RCSID("$Id: pdcdebug.c,v 1.24 2006/10/15 02:42:25 wmcbrine Exp $");

bool pdc_trace_on = FALSE;

/*man-start**************************************************************

  PDC_debug()  - Write debugging info to log file.

  PDCurses Description:
	This is a private PDCurses routine.

  Portability:
	PDCurses void PDC_debug(const char *, ...);

**man-end****************************************************************/

void PDC_debug(const char *fmt, ...)
{
	va_list args;
	FILE *dbfp;
	char hms[9];
	time_t now;

	if (!pdc_trace_on)
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
