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
#define	INCLUDE_WINDOWS_H
#include <curses.h>

#undef PDC_usleep

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#ifdef HAVE_LIMITS_H
#  include <limits.h>
#endif

#ifdef STDC_HEADERS
#  include <string.h>
#endif

#if defined(HAVE_STDARG_H) && defined(HAVE_PROTO)
#  include <stdarg.h>
#  define HAVE_STDARG_H_HAVE_PROTO
#else
#  include <varargs.h>
#endif

#ifdef PDCDEBUG
char *rcsid_PDCutil  = "$Id: pdcutil.c,v 1.1 2001/01/10 08:27:22 mark Exp $";
#endif

/*man-start*********************************************************************

  PDC_memmove()	- Replacement memmove() for lacking compliers.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Move a portion of memory taking consideration of overlapping memory
 	areas.

  PDCurses Return Value:
 	This function returns NULL if no temporary memory could be allocated.

  Portability:
 	PDCurses	int PDC_memmove(void *s1,const void *s2,size_t n);

**man-end**********************************************************************/

/***********************************************************************/
#ifdef HAVE_PROTO
void *PDC_memmove (void *s1, const void *s2, size_t n)
#else
void *PDC_memmove (s1,s2,n)
void *s1;
void *s2;
size_t n;
#endif
/***********************************************************************/
{
  char *dd=NULL;
#ifdef HAVE_PROTO
  const char *ss=NULL;
#else
  char *ss=NULL;
#endif

  dd = (char *)s1;
#ifdef HAVE_PROTO
  ss = (const char *)s2;
#else
  ss = (char *)s2;
#endif
  if (dd > ss && dd < ss + n)
    {
      dd += n;
      ss += n;
      while (n--)
        *--dd = *--ss;
    }
  else
    while (n--)
      *dd++ = *ss++;
  return s1;
}

/***********************************************************************/
#ifdef HAVE_PROTO
void PDC_beep (void)
#else
void PDC_beep ()
#endif
/***********************************************************************/
{
#ifdef UNIX
	if (bell != NULL)
		putp(bell);
#endif

#if defined (XCURSES)
	XCursesInstruct(CURSES_BELL);
#endif

#if defined(DOS) 
	PDC_putctty( (chtype)'\007', 0 );
#endif

#if defined(OS2)
#   if defined(EMXVIDEO)
	putchar('\007');
#   else
	DosBeep( 1380, 100 );
#   endif
#endif

#if defined(WIN32)
/*	MessageBeep(MB_OK); */
	MessageBeep(0XFFFFFFFF);
#endif

  return;
}

/*man-start*********************************************************************

  PDC_usleep()	- waits for specified number of microseconds

  PDCurses Description:
 	This routine is intended to provide a mechanism to wait the
 	specified number of microseconds. It is provided for those
 	platforms that do not have their own usleep() function.

  Portability:
 	PDCurses	void PDC_usleep( clock_t );

  Acknowledgement
 	PDC_usleep() was written by John Steele  (jsteele@netcom.com)
 	and hacked savagely by Mark Hessling

**man-end**********************************************************************/


/***********************************************************************/
#if defined(HAVE_USLEEP)
# ifdef HAVE_PROTO
void	PDC_usleep(long wait)
# else
void	PDC_usleep(wait)
long wait;
# endif
/***********************************************************************/
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_usleep() - called\n");
#endif
	usleep(wait);
	return;
}
/***********************************************************************/
#elif defined(HAVE_POLL)
#   include <poll.h>
#   ifdef HAVE_PROTO
void	PDC_usleep(long wait)
#   else
void	PDC_usleep(wait)
long wait;
#   endif
/***********************************************************************/
{
	struct pollfd fd;
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_usleep() - called\n");
#endif
	poll(&fd,0L,min(1L,wait/1000));
	return;
}
/***********************************************************************/
# else
#  ifdef HAVE_PROTO
void	PDC_usleep(long wait)
#  else
void	PDC_usleep(wait)
long wait;
#  endif
/***********************************************************************/
{
#ifndef WIN32
	clock_t goal;
#endif
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_usleep() - called\n");
# endif

#if defined(WIN32)
	Sleep(wait);
#else
	goal = (clock_t)wait + clock();
	while (goal > clock())
	;
#endif
	return;
}
#endif

#ifndef HAVE_VSSCANF
/*
 * Do not compile this module unless required.  This is due to the
 * requirement of some compilers; at least Borland C++ 3.0, of having
 * to link with math libraries due to the use of floats in the code.
 *
 * This module is based on vsscanf.c and input.c from emx 0.8f library
 * source which is Copyright (c) 1990-1992 by Eberhard Mattes.
 * Eberhard Mattes has kindly agreed to allow this module to be incorporated
 * into PDCurses.
 */

/*man-start*********************************************************************

  PDC_vsscanf()	- Internal routine to parse and format an input buffer.

  PDCurses Description:
 	This is a private PDCurses routine.

 	Scan a series of input fields. Each field is formatted according to
 	a supplied format string and the formatted input is stored in the
 	variable number of addresses passed.

  PDCurses Return Value:
 	This function returns the number of input fields or EOF on error.

  PDCurses Errors:
 	If the supplied data is invalid or an incorrect number of arguments
 	are passed, EOF is returned as an error.

  Portability:
 	PDCurses	int PDC_vsscanf(char *buf,const char *fmt,va_list arg_ptr);

**man-end**********************************************************************/

#define WHITE(x) ((x) == ' ' || (x) == '\t' || (x) == '\n')

#define NEXT(x) \
        do { \
            x = *buf++; \
            if (x == '\0') \
               return (count == 0 ? EOF : count); \
            ++chars; \
           } while (0)

#define UNGETC(x) \
        do { \
            --buf; *buf = x; --chars; \
           } while (0)

/***********************************************************************/
#ifdef HAVE_STDARG_H_HAVE_PROTO
int PDC_vsscanf ( char *buf, const char *fmt, va_list arg_ptr)
#else
int PDC_vsscanf (buf,fmt,va_alist)
char *buf;
char *fmt;
va_dcl
#endif
/***********************************************************************/
{
#if !defined(HAVE_STDARG_H_HAVE_PROTO)
	va_list arg_ptr;
#endif
	int count, chars, c, width, radix, d, i;
	int *int_ptr;
	long *long_ptr;
	short *short_ptr;
	char *char_ptr;
	unsigned char f;
	char neg, assign, ok, size;
	unsigned long n;
	char map[256], end;
	double dx, dd, *dbl_ptr;
	float *flt_ptr;
	int exp;
	char eneg;

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_vsscanf() - called\n");
#endif

#if 0
# ifdef HAVE_STDARG_H_HAVE_PROTO
	va_start(arg_ptr, fmt);
# else
	va_start(arg_ptr);
# endif
#else
# if !defined(HAVE_STDARG_H_HAVE_PROTO)
	va_start(arg_ptr);
# endif
#endif

  count = 0; chars = 0; c = 0;
  while ((f = *fmt) != 0)
    {
      if (WHITE (f))
        {
          do
            {
              ++fmt; f = *fmt;
            } while (WHITE (f));
          do
            {
              c = *buf++;
              if (c == '\0')
                {
                  if (f == 0 || count != 0)
                    return (count);
                  else
                    return (EOF);
                }
              else
                ++chars;
            } while (WHITE (c));
          UNGETC (c);
        }
      else if (f != '%')
        {
          NEXT (c);
          if (c != f)
            return (count);
          ++fmt;
        }
      else
        {
          assign = TRUE; width = INT_MAX;
          char_ptr = NULL;
          ++fmt;
          if (*fmt == '*')
            {
              assign = FALSE;
              ++fmt;
            }
          if (isdigit (*fmt))
            {
              width = 0;
              while (isdigit (*fmt))
                width = width * 10 + (*fmt++ - '0');
              if (width == 0) width = INT_MAX;
            }
          size = 0;
          if (*fmt == 'h' || *fmt == 'l')
            size = *fmt++;
          f = *fmt;
          switch (f)
            {
            case 'c':
              if (width == INT_MAX)
                width = 1;
              if (assign)
                char_ptr = va_arg (arg_ptr, char *);
              while (width > 0)
                {
                  --width;
                  NEXT (c);
                  if (assign)
                    {
                      *char_ptr++ = (char)c;
                      ++count;
                    }
                }
              break;
            case '[':
              (void)memset (map, 0, 256);
              end = 0;
              ++fmt;
              if (*fmt == '^')
                {
                  ++fmt; end = 1;
                }
              i = 0;
              for (;;)
                {
                  f = (unsigned char)*fmt;
                  switch (f)
                    {
                    case 0:
                      --fmt;       /* avoid skipping past 0 */
                      NEXT (c);
                      goto string;
                    case ']':
                      if (i > 0)
                        {
                          NEXT (c);
                          goto string;
                        }
                      /* no break */
                    default:
                      if (fmt[1] == '-' && fmt[2] != 0 &&
                          f < (unsigned char)fmt[2])
                        {
                          (void)memset (map+f, 1, (unsigned char)fmt[2]-f);
                          fmt += 2;
                        }
                      else
                        map[f] = 1;
                      break;
                    }
                  ++fmt; ++i;
                }
            case 's':
              (void)memset (map, 0, 256);
              map[' '] = 1;
              map['\n'] = 1;
              map['\r'] = 1;
              map['\t'] = 1;
              end = 1;
              do
                {
                  NEXT (c);
                } while (WHITE (c));
string:
              if (assign)
                char_ptr = va_arg (arg_ptr, char *);
              while (width > 0 && map[(unsigned char)c] != end)
                {
                  --width;
                  if (assign)
                    *char_ptr++ = (char)c;
                  c = *buf++;
                  if (c == '\0')
                    break;
                  else
                    ++chars;
                }
              if (assign)
                {
                  *char_ptr = 0;
                  ++count;
                }
              if (c == '\0')
                return (count);
              else
                UNGETC (c);
              break;
            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
              neg = ok = FALSE; dx = 0.0;
              do
                {
                  NEXT (c);
                } while (WHITE (c));
              if (c == '+')
                {
                  NEXT (c); --width;
                }
              else if (c == '-')
                {
                  neg = TRUE; NEXT (c); --width;
                }
              while (width > 0 && isdigit (c))
                {
                  --width;
                  dx = dx * 10.0 + (double)(c - '0');
                  ok = TRUE;
                  c = *buf++;
                  if (c == '\0')
                    break;
                  else
                    ++chars;
                }
              if (width > 0 && c == '.')
                {
                  --width;
                  dd = 10.0; NEXT (c);
                  while (width > 0 && isdigit (c))
                    {
                      --width;
                      dx += (double)(c - '0') / dd;
                      dd *= 10.0;
                      ok = TRUE;
                      c = *buf++;
                      if (c == '\0')
                        break;
                      else
                        ++chars;
                    }
                }
              if (!ok)
                return (count);
              if (width > 0 && (c == 'e' || c == 'E'))
                {
                  eneg = FALSE; exp = 0; NEXT (c); --width;
                  if (width > 0 && c == '+')
                    {
                      NEXT (c); --width;
                    }
                  else if (width > 0 && c == '-')
                    {
                      eneg = TRUE; NEXT (c); --width;
                    }
                  if (!(width > 0 && isdigit (c)))
                    {
                      UNGETC (c);
                      return (count);
                    }
                  while (width > 0 && isdigit (c))
                    {
                      --width;
                      exp = exp * 10 + (c - '0');
                      c = *buf++;
                      if (c == '\0')
                        break;
                      else
                        ++chars;
                    }
                  if (eneg) exp = -exp;
                  while (exp > 0)
                    {
                      dx *= 10.0;
                      --exp;
                    }
                  while (exp < 0)
                    {
                      dx /= 10.0;
                      ++exp;
                    }
                }
              if (assign)
                {
                  if (neg) dx = -dx;
                  if (size == 'l')
                    {
                      dbl_ptr = va_arg (arg_ptr, double *);
                      *dbl_ptr = dx;
                    }
                  else
                    {
                      flt_ptr = va_arg (arg_ptr, float *);
                      *flt_ptr = (float)dx;
                    }
                  ++count;
                }
              if (c == '\0')
                return (count);
              else
                UNGETC (c);
              break;
            case 'i':
              neg = FALSE; radix = 10;
              do
                {
                  NEXT (c);
                } while (WHITE (c));
              if (!(width > 0 && c == '0'))
                goto scan_complete_number;
              NEXT (c); --width;
              if (width > 0 && (c == 'x' || c == 'X'))
                {
                  NEXT (c); radix = 16; --width;
                }
              else if (width > 0 && (c >= '0' && c <= '7'))
                radix = 8;
              goto scan_unsigned_number;
            case 'd':
            case 'u':
            case 'o':
            case 'x':
            case 'X':
              do
                {
                  NEXT (c);
                } while (WHITE (c));
              switch (f)
                {
                case 'o':           radix = 8; break;
                case 'x': case 'X': radix = 16; break;
                default:            radix = 10; break;
                }
scan_complete_number:
              neg = FALSE;
              if (width > 0 && c == '+')
                {
                  NEXT (c); --width;
                }
              else if (width > 0 && c == '-' && radix == 10)
                {
                  neg = TRUE; NEXT (c); --width;
                }
scan_unsigned_number:
              n = 0; ok = FALSE;
              while (width > 0)
                {
                  --width;
                  if (isdigit (c))
                    d = c - '0';
                  else if (isupper (c))
                    d = c - 'A' + 10;
                  else if (islower (c))
                    d = c - 'a' + 10;
                  else
                    break;
                  if (d < 0 || d >= radix)
                    break;
                  ok = TRUE;
                  n = n * radix + d;
                  c = *buf++;
                  if (c == '\0')
                    break;
                  else
                    ++chars;
                }
              if (!ok)
                return (count);
              if (assign)
                {
                  if (neg) n = -n;
                  switch(size)
                     {
                      case 'h':
                              short_ptr = va_arg (arg_ptr, short *);
                              *short_ptr = (short)n;
                              break;
                      case 'l':
                              long_ptr = va_arg (arg_ptr, long *);
                              *long_ptr = (long)n;
                              break;
                      default:
                              int_ptr = va_arg (arg_ptr, int *);
                              *int_ptr = (int)n;
                     }
                  ++count;
                }
              if (c == '\0')
                return (count);
              else
                UNGETC (c);
              break;
            case 'n':
              if (assign)
                {
                  int_ptr = va_arg (arg_ptr, int *);
                  *int_ptr = chars;
                  ++count;
                }
              break;
            default:
              if (f == 0)                 /* % at end of string */
                return (count);
              NEXT (c);
              if (c != f)
                return (count);
              break;
            }
          ++fmt;
        }
    }
  return (count);
}
#endif
