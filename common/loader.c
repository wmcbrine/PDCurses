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
 *
 * Mark Hessling  M.Hessling@qut.edu.au  http://www.lightlink.com/hessling/
 */

static char RCSid[] = "$Id: loader.c,v 1.10 2002/08/25 09:01:06 mark Exp $";

#include "rxpack.h"

#ifndef R_OK
# define R_OK 4
#endif

#ifdef HAVE_PROTO
# if !defined(HAVE_GETOPT)
int getopt( int argc, char *argv[], char *optstring );
# endif
#else
# if !defined(HAVE_GETOPT)
int getopt( );
# endif
#endif

/* These are required by the getopt() function */
extern char *optarg;
extern int  optind;

extern PackageInitialiser *GETPACKAGEINITIALISER();
extern PackageTerminator *GETPACKAGETERMINATOR();
extern RexxSubcomHandler *GETPACKAGESUBCOMHANDLER();
extern RexxFunction *GETPACKAGEFUNCTIONS();
extern void PACKAGEUSAGE();

/*-----------------------------------------------------------------------------
 * Checks to see if supplied filename is readable.
 *----------------------------------------------------------------------------*/
static int file_readable

#ifdef HAVE_PROTO
   (char *filename)
#else
   (filename)
   char  *filename;
#endif
{
   if ((access(filename,R_OK)) == (-1))
      return(0);
   else
      return(1);
}


/*-----------------------------------------------------------------------------
 * Processing starts here for stand-alone rexxsql executable...
 *----------------------------------------------------------------------------*/
int main

#if HAVE_PROTO
   (int argc, char *argv[])
#else
   (argc, argv)
   int   argc;
   char  *argv[];
#endif
{
   int c=0;
   char *ProgramName=NULL;
   FILE *fp;
   long i=0, ArgCount=0;
   int interactive = FALSE;
   int rc=0;
   RXSTRING retstr;
   CHAR retbuf[RETBUFLEN];
   RXSTRING ArgList;
#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
   RXSYSEXIT ExitList[2];
#endif
   RxPackageGlobalDataDef MyGlob, *RxPackageGlobalData;

   memset( (char *)&MyGlob, 0, sizeof( RxPackageGlobalDataDef ) );

   strcpy( MyGlob.RxTraceFileName, "stderr" );
   MyGlob.RxTraceFilePointer = stderr;
   /* 
    * Get any program options. 
    */
   while ((c = getopt(argc, argv, "Dudivh?f:")) != EOF)
   {
      switch (c) 
      {
         case 'f': 
            strcpy( MyGlob.RxTraceFileName, optarg );
            break;
         case 'v': 
            MyGlob.RxRunFlags |= MODE_VERBOSE;
            break;
         case 'd': 
            MyGlob.RxRunFlags |= MODE_DEBUG;
            break;
         case 'D': 
            MyGlob.RxRunFlags |= MODE_INTERNAL;
            break;
         case 'i': 
            interactive = TRUE; 
            break;
         case 'u':
            DeregisterRxFunctions( &MyGlob, GETPACKAGEFUNCTIONS(), 1 );
            return(0);
            break;
         case 'h':
         default : PACKAGEUSAGE();
      }
   }

   /* 
    * Check if any more arguments are presented    
    */
   if (optind >= argc)
   {
      if (interactive)
      {
         ProgramName = tmpnam(NULL);
         if ((fp = fopen(ProgramName,"w")) == NULL)
         {
            (void)fprintf(stderr, "Could not create temporary file for stdin\n");
            exit(REXX_FAIL);
         }
         for ( ; ; )
         {
            if ((i = getc(stdin)) == EOF)
               break;
            putc(i,fp);
         }
         (void)fclose(fp);
      }
      else
         PACKAGEUSAGE();
   }
   else
   {
      /* 
       * Next argument is the name of the Rexx program...
       */
      ProgramName = argv[optind++];
      /* 
       * ... and must be readable.
       */

      if (!file_readable(ProgramName))
      {
         (void)fprintf(stderr, "Could not read file: %s\n",ProgramName);
         exit(REXX_FAIL);
      }
   }

   /* 
    * Get number of arguments to the Rexx program
    */
   ArgCount = argc - optind;

   /* 
    * Build an array of arguments if any. 
    */
   if (ArgCount) 
   {
      int len=0;

      for ( i = optind; i < argc; i++ )
      {
         len += strlen( (char *)argv[i] );
      }
      if ( ( ArgList.strptr = (RXSTRING_STRPTR_TYPE)malloc( len + 1 + ArgCount) ) == (RXSTRING_STRPTR_TYPE)NULL )
      {
         (void)fprintf( stderr, "%s: out of memory\n", argv[0] );
         exit( REXX_FAIL );
      }
      strcpy( ArgList.strptr, "" );
      for ( i = optind; i < argc; i++ )
      {
         strcat( ArgList.strptr, (RXSTRING_STRPTR_TYPE)argv[optind++] );
         if ( i != argc )
            strcat( ArgList.strptr, (RXSTRING_STRPTR_TYPE)" " );
      }
      ArgList.strlength = ArgCount + len - 1;
   }
   else
   {
      ArgList.strptr = NULL;
      ArgList.strlength = 0;
   }

   /* 
    * Initialise the package interface, but don't set the trace file
    */
   RxPackageGlobalData = InitRxPackage( &MyGlob, GETPACKAGEINITIALISER(), &rc );
   if ( rc != 0 )
      return( rc );
   /* 
    * Register all external functions
    */
   if ( ( rc = RegisterRxFunctions( RxPackageGlobalData, GETPACKAGEFUNCTIONS(), RXPACKAGENAME ) ) != 0 )
      return( rc );
   /* 
    * Register a default subcommand handler to pass commands to the OS
    */
   if ( ( rc = RegisterRxSubcom( RxPackageGlobalData, GETPACKAGESUBCOMHANDLER() ) ) != 0 )
      return( rc );
   FunctionPrologue( RxPackageGlobalData, GETPACKAGEINITIALISER(), RXPACKAGENAME, 0L, NULL );
   /*
    * Set up the system exit for the Say and Trace redirection
    */
#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
   ExitList[0].sysexit_name = RXPACKAGENAME;
   ExitList[0].sysexit_code = RXSIO;
   ExitList[1].sysexit_code = RXENDLST;
#endif

   MAKERXSTRING( retstr, retbuf, sizeof( retbuf ) );
   /*
    * Execute the Rexx script. Use RXCOMMAND mode so that the Rexx program
    * expects the same parameter list if called directly via the Rexx
    * interpreter.
    */
   assert(retstr.strptr);
   assert(ProgramName);
   RexxStart( ( RS_ARG0_TYPE )(ArgCount) ? 1 : 0,
              ( RS_ARG1_TYPE )&ArgList,
              ( RS_ARG2_TYPE )ProgramName,
              ( RS_ARG3_TYPE )NULL,
              ( RS_ARG4_TYPE )RXPACKAGENAME,
              ( RS_ARG5_TYPE )RXCOMMAND,
#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
              ( RS_ARG6_TYPE )ExitList,
#else
              ( RS_ARG6_TYPE )NULL,
#endif
              ( RS_ARG7_TYPE )&rc,
              ( RS_ARG8_TYPE )&retstr);

   if ( !RxPackageGlobalData->terminated )
   {
      rc = FunctionEpilogue( RxPackageGlobalData, RXPACKAGENAME, (ULONG)rc );
      /* 
       * Terminate the package interface.
       */
      (void)TermRxPackage( RxPackageGlobalData, GETPACKAGETERMINATOR(), GETPACKAGEFUNCTIONS(), RXPACKAGENAME, 0 );
      RxPackageGlobalData = NULL;
   }

   if ( ArgList.strptr )
      free(ArgList.strptr);
   /*
    * Return the exit value from the program. This is useful for UNIX/DOS etc.
    * if the value is kept to 0-success, small positive numbers (say < 100)
    * to indicate errors!
    */
   if ( interactive )
      unlink( ProgramName );

   return rc;
}
