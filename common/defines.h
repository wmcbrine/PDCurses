/*
 * Copyright (C) 1999-2002 Mark Hessling <M.Hessling@qut.edu.au>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * For platforms that don't use autoconf, put these here...
 */

#if defined(DOS) && defined(__EMX__)
# define HAVE_ASSERT_H        1
# define HAVE_GETOPT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_IO_H            1
# define HAVE_LIMITS_H        1
# define HAVE_MALLOC_H        1
# define HAVE_SETJMP_H        1
# define HAVE_SIGNAL_H        1
# define HAVE_STDARG_H        1
# define HAVE_STDLIB_H        1
# define HAVE_SYS_STAT_H      1
# define HAVE_SYS_TYPES_H     1
# define HAVE_STRING_H        1
# define HAVE_UNISTD_H        1
# define HAVE__FULLPATH       1
# define HAVE_GETOPT          1
# define HAVE_PUTENV          1
# define HAVE_GETTIMEOFDAY    1
# define HAVE_FTRUNCATE       1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_STRERROR        1
# define HAVE_VSPRINTF        1
# define TIME_WITH_SYS_TIME   1
#endif

#if (defined(OS2) || defined(__OS2__)) && defined(__EMX__)
# define HAVE_ASSERT_H        1
# define HAVE_GETOPT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_FTRUNCATE       1
# define HAVE_GETOPT          1
# define HAVE_GETTIMEOFDAY    1
# define HAVE_IO_H            1
# define HAVE_LIMITS_H        1
# define HAVE_MALLOC_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PUTENV          1
# define HAVE_SETJMP_H        1
# define HAVE_SIGNAL_H        1
# define HAVE_STDARG_H        1
# define HAVE_STDLIB_H        1
# define HAVE_STRERROR        1
# define HAVE_STRING_H        1
# define HAVE_SYS_STAT_H      1
# define HAVE_SYS_TYPES_H     1
# define HAVE_TIME_H          1
# define HAVE_UNISTD_H        1
# define HAVE_VSPRINTF        1
# define HAVE__FULLPATH       1
# define TIME_WITH_SYS_TIME   1
#endif

#if (defined(OS2) || defined(__OS2__)) && defined(__IBMC__)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_FTRUNCATE       1
# define HAVE_GETTIMEOFDAY    1
# define HAVE_IO_H            1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PUTENV          1
# define HAVE_MEMORY_H        1
# define HAVE_STRING_H        1
# define HAVE_SETJMP_H        1
# define HAVE_ERRNO_H         1
# define HAVE_STDARG_H        1
# define HAVE_STDLIB_H        1
# define HAVE_STRERROR        1
# define HAVE_SYS_STAT_H      1
# define HAVE_VSPRINTF        1
# define HAVE__FULLPATH       1
# define TIME_WITH_SYS_TIME   1
#endif

#if (defined(OS2) || defined(__OS2__)) && defined(__WATCOMC__)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_IO_H            1
# define HAVE_MALLOC_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PUTENV          1
# define HAVE_SETJMP_H        1
# define HAVE_STDARG_H        1
# define HAVE_STRERROR        1
# define HAVE_STRING_H        1
# define HAVE_PROCESS_H       1
# define HAVE_SYS_STAT_H      1
# define HAVE_UNISTD_H        1
# define HAVE_VSPRINTF        1
# define HAVE__FULLPATH       1
#endif

#if defined(WIN32) && defined(__WATCOMC__)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_FTIME           1
# define HAVE_IO_H            1
# define HAVE_MALLOC_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_SETENV          1
# define HAVE_SETJMP_H        1
# define HAVE_STDARG_H        1
# define HAVE_STRERROR        1
# define HAVE_SYS_STAT_H      1
# define HAVE_UNISTD_H        1
# define HAVE_VSPRINTF        1
# define HAVE__FULLPATH       1
# define HAVE__SPLITPATH2     1
/* # define HAVE_FTRUNCATE      1*/
#endif

#if defined(WIN32) && defined(_MSC_VER)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_FTIME           1
# define HAVE_FTRUNCATE       1
# define HAVE_IO_H            1
# define HAVE_LIMITS_H        1
# define HAVE_MALLOC_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_SETJMP_H        1
# define HAVE_SIGNAL_H        1
# define HAVE_STDARG_H        1
# define HAVE_STDLIB_H        1
# define HAVE_STRERROR        1
# define HAVE_STRING_H        1
# define HAVE_SYS_STAT_H      1
# define HAVE_VFPRINTF        1
# define HAVE__FULLPATH       1
# define HAVE__SPLITPATH      1
#endif

#if defined(WIN32) && defined(__CYGWIN32__)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_FTRUNCATE       1
# define HAVE_GETTIMEOFDAY    1
# define HAVE_GRP_H           1
# define HAVE_IO_H            1
# define HAVE_LIMITS_H        1
# define HAVE_MALLOC_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PROCESS_H       1
# define HAVE_PUTENV          1
# define HAVE_PWD_H           1
# define HAVE_SETJMP_H        1
# define HAVE_SIGNAL_H        1
# define HAVE_STDLIB_H        1
# define HAVE_STRERROR        1
# define HAVE_STRING_H        1
# define HAVE_SYS_STAT_H      1
# define HAVE_SYS_TYPES_H     1
# define HAVE_UNISTD_H        1
# define HAVE_VFPRINTF        1
# define TIME_WITH_SYS_TIME   1
/* # define HAVE_FULLPATH        1 */
#endif

#if defined(WIN32) && defined(__MINGW32__)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_IO_H            1
# define HAVE_LIMITS_H        1
# define HAVE_MALLOC_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PROCESS_H       1
# define HAVE_PUTENV          1
# define HAVE_SETJMP_H        1
# define HAVE_SIGNAL_H        1
# define HAVE_STDLIB_H        1
# define HAVE_STRERROR        1
# define HAVE_STRING_H        1
# define HAVE_SYS_STAT_H      1
# define HAVE_SYS_TYPES_H     1
# define HAVE_UNISTD_H        1
# define HAVE_VFPRINTF        1
# define TIME_WITH_SYS_TIME   1
/* # define HAVE_FTRUNCATE       1 */
/* # define HAVE_FULLPATH        1 */
/* # define HAVE_GETTIMEOFDAY    1 */
#endif

#if defined(WIN32) && defined(__LCC__)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_IO_H            1
# define HAVE_LIMITS_H        1
# define HAVE_MALLOC_H        1
# define HAVE_PROCESS_H       1
# define HAVE_SETJMP_H        1
# define HAVE_SIGNAL_H        1
# define HAVE_STDLIB_H        1
# define HAVE_STRING_H        1
# define HAVE_SYS_STAT_H      1
# define HAVE_SYS_TYPES_H     1
/* # define HAVE_UNISTD_H        1 */
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PUTENV          1
# define HAVE_STRERROR        1
# define HAVE_VFPRINTF        1
# define TIME_WITH_SYS_TIME   1
/* # define HAVE_FTRUNCATE       1 */
/* # define HAVE_FULLPATH        1 */
/* # define HAVE_GETTIMEOFDAY    1 */
#endif

#if (defined(DOS) || defined(MSDOS)) && defined(DJGPP)
# define HAVE_ASSERT_H        1
# define HAVE_CTYPE_H         1
# define HAVE_FTRUNCATE       1
# define HAVE_GETOPT          1
# define HAVE_GETTIMEOFDAY    1
# define HAVE_IO_H            1
# define HAVE_LIMITS_H        1
# define HAVE_MALLOC_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PUTENV          1
# define HAVE_RANDOM          1
# define HAVE_SETJMP_H        1
# define HAVE_STDARG_H        1
# define HAVE_STRERROR        1
# define HAVE_STRING_H        1
# define HAVE_SYS_STAT_H      1
# define HAVE_SYS_TYPES_H     1
# define HAVE_TRUENAME        1
# define HAVE_UNISTD_H        1
# define HAVE_USLEEP          1
# define HAVE_VFPRINTF        1
# define TIME_WITH_SYS_TIME   1
# ifndef _MAX_PATH
#  define _MAX_PATH           PATH_MAX
# endif
#endif

#if defined(_AMIGA)
# define HAVE_ASSERT_H        1
# define HAVE_MEMCPY          1
# define HAVE_MEMMOVE         1
# define HAVE_PUTENV          1
# define HAVE_SETJMP_H        1
# define HAVE_STRERROR        1
# define HAVE_SYS_STAT_H      1
# define HAVE_UNISTD_H        1
# define HAVE_VFPRINTF        1
# define UNIX                 1
#endif
