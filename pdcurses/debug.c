/* Public Domain Curses */

#include <curspriv.h>

/*man-start**************************************************************

debug
-----

### Synopsis

    void traceon(void);
    void traceoff(void);
    void PDC_debug(const char *, ...);

### Description

   traceon() and traceoff() toggle the recording of debugging
   information to the file "trace". Although not standard, similar
   functions are in some other curses implementations.

   PDC_debug() is the function that writes to the file, based on
   whether traceon() has been called. It's used from the PDC_LOG()
   macro.

### Portability
                             X/Open    BSD    SYS V
    traceon                     -       -       -
    traceoff                    -       -       -
    PDC_debug                   -       -       -

**man-end****************************************************************/

#include <string.h>
#include <sys/types.h>
#include <time.h>

FILE *pdc_dbfp = NULL;

void PDC_debug(const char *fmt, ...)
{
    va_list args;
    char hms[9];
    time_t now;

    if (!pdc_dbfp)
        return; 

    time(&now);
    strftime(hms, 9, "%H:%M:%S", localtime(&now));
    fprintf(pdc_dbfp, "At: %8.8ld - %s ", (long) clock(), hms);

    va_start(args, fmt);
    vfprintf(pdc_dbfp, fmt, args);
    va_end(args);
}

void traceon(void)
{
    if (pdc_dbfp)
        fclose(pdc_dbfp);

    /* open debug log file append */
    pdc_dbfp = fopen("trace", "a");
    if (!pdc_dbfp)
    {
        fprintf(stderr,
            "PDC_debug(): Unable to open debug log file\n");
        return;
    }

    PDC_LOG(("traceon() - called\n"));
}

void traceoff(void)
{
    if (!pdc_dbfp)
        return;

    PDC_LOG(("traceoff() - called\n"));

    fclose(pdc_dbfp);
    pdc_dbfp = NULL;
}
