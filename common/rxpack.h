/*
 * Copyright (C) 1998-2001  Mark Hessling <M.Hessling@qut.edu.au>
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# include "defines.h"
#endif

#include <stdio.h>

#ifdef HAVE_CTYPE_H
# include <ctype.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#ifdef HAVE_ASSERT_H
# include <assert.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#ifdef HAVE_IO_H
# include <io.h>
#endif

/*
 * All things that this application may require out of os2.h must be
 * specified here in one place.  This is because EMX includes all Rexx
 * related stuff in os2.h, not in a serperate header file.  This makes
 * it difficult to use another Rexx interpreter with EMX :-(
 */
#if defined(OS2) || defined(__OS2__)
# if !defined(EMXVIDEO)
#  define INCL_VIO
#  define INCL_KBD
# endif
# if defined(USE_OS2REXX)
#  define INCL_RXSHV      /* Shared variable support */
#  define INCL_RXFUNC     /* External functions support */
#  define INCL_RXSYSEXIT  /* System exit routines */
#  include <os2.h>
# else
#  include <os2.h>
#  define INCL_RXSHV      /* Shared variable support */
#  define INCL_RXFUNC     /* External functions support */
#  define INCL_RXSYSEXIT  /* System exit routines */
# endif
#else
# define INCL_RXSHV      /* Shared variable support */
# define INCL_RXFUNC     /* External functions support */
# define INCL_RXSYSEXIT  /* System exit routines */
#endif

/*
 * The following header file is supplied by the application package.
 * It specifies any application-specific header files and #defines
 * the following:
 * RXPACKAGE_MAGIC_NUMBER  12345     - any number
 * RXPACKAGE_DEBUG_VAR     "ENV_VAR" - an env variable enclosed in quotes
 */
#include "apphead.h"

#include "rxdef.h"

#define RETBUFLEN 250

#if defined(DYNAMIC)
# define DYNAMIC_LIBRARY 1
#endif

#ifndef TRUE
# define TRUE   1
# define FALSE  0
#endif

/* 
 * Run time modes 
 */
#define MODE_DEBUG      1
#define MODE_VERBOSE    2
#define MODE_INTERNAL   4

#define REXX_FAIL      1

#define RXSTRCAT(dst,dstlen,src,srclen) (memcpy((dst)+(dstlen),(src),(srclen)),(dstlen)+=(srclen),*((dst)+(dstlen))='\0')
#define RXSTRCPY(dst,dstlen,src,srclen) (memcpy((dst),(src),(srclen)),(dstlen)=(srclen),*((dst)+(dstlen))='\0')

#if !defined(max)
# define max(a,b)        (((a) > (b)) ? (a) : (b))
#endif

#if !defined(min)
# define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif

#if !defined(MAX_PATH)
# if defined(NAME_MAX)
#  define MAX_PATH NAME_MAX
# elif defined(MAXNAMLEN)
#  define MAX_PATH MAXNAMLEN
# else
#  define MAX_PATH 255
# endif
#endif

#ifdef USE_REXX6000
typedef USHORT RexxFunctionHandler(PSZ, ULONG, PRXSTRING, PSZ, PRXSTRING) ;
#endif

/*-----------------------------------------------------------------------------
 * Definition of an external function
 *----------------------------------------------------------------------------*/
typedef struct {
   RRFD_ARG0_TYPE      ExternalName;
   RRFE_ARG1_TYPE      EntryPoint; 
   RRFD_ARG2_TYPE      InternalName;
   int                 DllLoad;
} RexxFunction;

extern char *RxPackageName;

/*
 * The following structure contains all "global" data common to all
 * external function packages.  A similar structure should exists
 * for package-specific data.
 */
typedef struct 
{
   int RxRunFlags;                    /* debug/verbose flags */
   char FName[100];                   /* current function name */
   FILE *RxTraceFilePointer;          /* file pointer for all output */
   char RxTraceFileName[MAX_PATH];    /* filename of output file */
   ULONG RxPackageInitialised;        /* needed ?? */
} RxPackageGlobalDataDef;

#ifdef HAVE_PROTO
void FunctionPrologue( char *, ULONG, RXSTRING * );
void FunctionTrace( char *, ... );
long FunctionEpilogue( char *, long );
void InternalTrace( char *, ... );
void RxDisplayError( RFH_ARG0_TYPE, ... );
int InitRxPackage( RxPackageGlobalDataDef * );
int TermRxPackage( char *, int );
int InitialisePackage( void );
int TerminatePackage( void );
int RegisterRxFunctions( void );
int QueryRxFunction( char * );
int DeregisterRxFunctions( int );
char *make_upper( char * );
char *AllocString( char *, int );
char *MkAsciz( char *, int, char *, int );
int SetRexxVariable( char *, int, char *, int );
RXSTRING *GetRexxVariable(char *, RXSTRING *, int );
int *GetRexxVariableInteger( char *name, int *value, int suffix );
int StrToInt( RXSTRING *, ULONG * );
int StrToNumber( RXSTRING *, LONG * );
int StrToBool( RXSTRING *, ULONG * );
int RxSetTraceFile( char * );
char *RxGetTraceFile( void );
void RxSetRunFlags( int );
int RxGetRunFlags( void );
int RxReturn( RXSTRING * );
int RxReturnString( RXSTRING *, char * );
int RxReturnStringAndFree( RXSTRING *, char *, int );
int RxReturnNumber( RXSTRING *, long );
int RxReturnDouble( RXSTRING *, double );
int RxReturnPointer( RXSTRING *, void * );
int memcmpi( char *, char *, int );
int my_checkparam(const char *, int, int, int);
#else
void FunctionPrologue( );
void FunctionTrace( );
long FunctionEpilogue( );
void InternalTrace( );
void RxDisplayError( );
int InitRxPackage( );
int TermRxPackage( );
int InitialisePackage( );
int TerminatePackage( );
int RegisterRxFunctions( );
int QueryRxFunction( );
int DeregisterRxFunctions( );
char *make_upper ( );
char *AllocString( );
char *MkAsciz( );
int SetRexxVariable( );
RXSTRING *GetRexxVariable( );
int *GetRexxVariableInteger( );
int StrToInt( );
int StrToNumber( );
int StrToBool( );
int RxSetTraceFile( );
char *RxGetTraceFile( );
void RxSetRunFlags( );
int RxGetRunFlags( );
int RxReturn( );
int RxReturnString( );
int RxReturnStringAndFree( );
int RxReturnNumber( );
int RxReturnDouble( );
int memcmpi( );
int my_checkparam( );
#endif

#ifdef DEBUG
# define DEBUGDUMP(x) {x;}
#else
# define DEBUGDUMP(x) {}
#endif
