/*----------------------------------------------------*- Fundamental -*-

Module:		getopt()	[X/OPEN]

File:		getopt.c

Assoc. files:	config.h	(but see below)

Description:	This module implements a command line parser. It 
		returns option flags (specified by the user) and
		associated values (if any) to the calling program.

Notes:		This module began life as a Pascal routine for Oregon
		Software's Pascal-2 compiler, and was later rewritten
		into C.  

Acknowl.:	Some features of the AT&T public domain getopt()
		released to Usenet have been incorporated:

		*  It is now possible to use syscalls for error
		   messages. I disagree slightly with this: the
		   only reason I can think of is to avoid getting
		   the stdio-package in linkage. getopt() is likely
		   to be used in programs where code size is a minor
		   issue. However, I've made it configurable.

Author:		Anders Thulin
		Rydsvagen 288
		S-582 50 Linkoping
		SWEDEN

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

This program/module/routine has been developed as a part of `The Source Code
Project', a personal attempt at producing well-written, portable software
primarily intended to be executed in the ANSI C and POSIX / X/OPEN Unix
environments.

The program/module/routine is hereby placed in the public domain, which
means that it may be freely copied, sold for profit or for fun, praised or
buried as your fancy takes you.

Comments on the portability and adaptability of the program/module/routine
will be appreciated, and, if they result in modifications, acknowledged in
future releases.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Edit history :

Vers  Ed   Date        By                Comments
----  ---  ----------  ----------------  -------------------------------
 1.0    0  1987-12-18  Anders Thulin     Initial version
        1  1988-06-06  Anders Thulin     Got a copy of the AT&T public
					 domain getopt(). Changed
					 a few things to the better.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* #include "config.h" -- included below:  */

/*----------------------------------------------------*- Fundamental -*-

Module:			getopt(3c)

File:			config.h

Associated files:	getopt.c

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*---  Configuration:  ------------------------------------------------

getopt() may write error messages to stderr. 

  USE_STDIO == 0	Use write(2, ...) for error messages
  USE_STDIO == 1	Use fprintf(stderr, ...) for error messages

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define USE_STDIO	1	/* Default is 1 */

/* -- end of "config.h" */

#include <assert.h>
#include <stdio.h>		/* for EOF and (optionally) fprintf() */
#include <string.h>

char   *optarg = (char *) 0;	/* Points to option argument (if any)	*/
int	opterr = 1;		/* Enable error reporting		*/
int	optind = 1;		/* Begin with argv[optind]		*/


#define BUMP(p)	(*++p == '\0' ? (optind++, (char *) 0) : p)

#if USE_STDIO
# define ERROR(pname, s, ch)		\
	do {								\
	  if (opterr != 0) {						\
             fprintf(stderr, "%s: %s -- '%c'\n", pname, (s), (ch));	\
          }								\
        } while (0)
#else
# define ERROR(pname, s, c)	do {					\
			  if (opterr != 0) {			\
#if __STDC__ != 0						\
		            extern int write(int, char *, unsigned); 	\
#else								\
			    extern int write();			\
#endif								\
			    char errbuf[4];			\
			    errbuf[0] = errbuf[2] = '\'';	\
			    errbuf[1] = (c); errbuf[3] = '\0';	\
			    write(2, (pname), strlen(pname));	\
			    write(2, ": " s " -- ", strlen(s)+6); \
			    write(2, errbuf,  strlen(errbuf));	\
			  }					\
			} while (1 == 0)
#endif /* USE_STDIO */


/*----------------------------------------------------------------------

Routine:	getopt

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int getopt(argc, argv, optstring)
int   argc;
char *argv[];
char *optstring;
{
  static char *ap = (char *) 0;
  char  *vp;
  char   ch;

  optarg = (char *) 0;		/* Presume no-argument option		*/

  if (optind >= argc) {		/* Reached end of argv[]		*/
    return EOF;
  }

 /*
  *  ap points into argv[optind], and is used to keep track of how far
  *  we have parsed the current option string.  Remember that an argv[]
  *  string may contain several separate option letters (e.g.  the line
  *  'foo -abc -x -y' contains five option letters, but only three 
  *  arguments).
  *
  *  If ap == 0, we have reached a new string.  Check that it *is* a
  *  legal option, that is:
  *
  *  +  check that the first character is '-'
  *  +  check that the string isn't "-"
  *  +  check that the string isn't "--"
  *
  */

  if (ap == (char *) 0) {
    ap = argv[optind];

    if (*ap != '-') {		/* Doesn't begin with '-' -- not an option */
      ap = (char *) 0;
      return EOF;
    }

    ap++;			/* Examine second character */

    if (*ap == '\0') {		/* String was "-" -- not an option */
      ap = (char *) 0;
      return EOF;
    }

    if (*ap == '-' && *(ap+1) == '\0') {	/* String is "--" -- n.a.o. */
      ap = (char *) 0;
      optind++;
      return EOF;
    }
  }

 /*
  *  At this point we know that the string appears to be an option string.
  *  Check that the current character really is one of those specified in
  *  'optstring' (remember, ':' is NOT a permissible option character, as
  *  it is used in optstring to specify that the option takes an argument).
  *
  *  Also note that strchr() treats the terminating NUL character as a part
  *  of the string -- hence strchr() always finds NUL in optstring.  The
  *  routine relies on ch never taking the value NUL.
  *
  */

  ch = *ap;
  assert(ch != '\0');

  if (ch == ':' || (vp = strchr(optstring, ch)) == (char *) 0) {
    ERROR(argv[0], "illegal option", ch);
    ap = BUMP(ap);
    return '?';
  }

 /*
  *  ch is a permissible option letter, and vp points to it in optstring.
  *  If it takes an argument, that is it is followed by ':' in optstring,
  *  extract the argument, and set optarg to point to it.  If not, just
  *  return the option letter.
  *
  */

  if (*++vp == ':') {
    optarg = BUMP(ap);
    if (optarg == (char *) 0) {	 /* option value might be in next argument */
      if (optind >= argc) {	 /* No, it wasn't */
        ERROR(argv[0], "option requires an argument", ch);
        ch = '?';
      }
      else {
        optarg = argv[optind];
      }
    }
    optind++;
    ap = (char *) 0;
  }
  else {
    ap = BUMP(ap);
  }

  return ch;
}
    
#ifdef TEST

/*----------------------------------------------------------------------

Routine:	main()

Description:	This routine provides confidence testing of the
		getopt() routine.  It is rather	crude at present.  A
		better way of doing things would be to build argument
		structures in memory, sending them to getopt(), and
		check that the returned values are as expected.  

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int main(argc, argv)
int	argc;
char   *argv[];
{
  int	c;
  char *opts = "abcd:g0%";

  extern char *optarg;
  extern int   optind;

  fprintf(stderr, "optstring = '%s'\n", opts);

  while ( (c = getopt(argc, argv, opts)) != EOF) {
    switch(c) {
      default :
        fprintf(stderr, "getopt() returned unknown character (0x%x)\n", c);
        break;

      case '?' :
        fprintf(stderr, "getopt() returned '?' -- error detected\n");
        break;

      case 'a' :
      case 'b' :
      case 'c' :
      case 'g' :
      case '0' :
      case '%' :
        fprintf(stderr, "getopt() returned option letter %c\n", c);
        break;

      case 'd' :
        fprintf(stderr, "getopt() returned option letter %c\n", c);
        fprintf(stderr, "option value is '%s'\n", optarg);
        break;
   
    }
  }

  fprintf(stderr, "end of option list detected\n");
  if (optind < argc) {
    fprintf(stderr, "remaining arguments are:\n");
    for ( ; optind <argc; optind++) {
      fprintf(stderr, "argv[%d] = '%s'\n", optind, argv[optind]);
    }
  }

  return 0;
}
#endif
