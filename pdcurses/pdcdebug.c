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
#define  CURSES_LIBRARY 1
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <curses.h>

#include <string.h>
#include <sys/types.h>
#include <time.h>


#undef   PDC_debug

#if defined(HAVE_STDARG_H) && defined(HAVE_PROTO)
# include <stdarg.h>
# define HAVE_STDARG_H_HAVE_PROTO
#else
# include <varargs.h>
#endif

#ifdef PDCDEBUG
char *rcsid_PDCdebug  = "$Id: pdcdebug.c,v 1.2 2002/06/23 04:13:02 mark Exp $";
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
   PDCurses void PDC_debug( char *,... );

**man-end**********************************************************************/

#ifdef HAVE_STDARG_H_HAVE_PROTO
void  PDC_CDECL   PDC_debug( char *fmt, ... )
#else
void  PDC_CDECL   PDC_debug(fmt,va_alist)
char *fmt;
va_dcl
#endif
{
   va_list args;
   FILE *dbfp=NULL;
   char buffer[256];

   /*
    * open debug log file append
    */
   buffer[0] = '\0';
   if (!trace_on)
      return; 
   dbfp = fopen("trace","a");
   if (dbfp == NULL)
   {
      fprintf( stderr, "PDC_debug(): Unable to open debug log file\n" );
      return;
   }

#ifdef HAVE_STDARG_H_HAVE_PROTO
   va_start(args, fmt);
#else
   va_start(args);
#endif

   fprintf( dbfp, "At: %8.8d - %d secs ", clock(), clock() / CLOCKS_PER_SEC );
   vsprintf(buffer,fmt,args);
   fputs(buffer,dbfp);
   va_end(args);
   fclose(dbfp);
   return;
}
