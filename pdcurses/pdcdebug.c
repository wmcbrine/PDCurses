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

#define CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

#include <string.h>
#include <sys/types.h>
#include <time.h>

#undef PDC_debug

#ifdef PDCDEBUG
char *rcsid_PDCdebug = "$Id: pdcdebug.c,v 1.11 2006/01/28 19:31:00 wmcbrine Exp $";
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

void PDC_CDECL PDC_debug(char *fmt, ...)
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
