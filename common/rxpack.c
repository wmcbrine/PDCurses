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

static char RCSid[] = "$Id: rxpack.c,v 1.36 2003/04/13 01:48:59 mark Exp $";

#include "rxpack.h"

#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
static RexxExitHandler RxExitHandlerForSayTraceRedirection;
#endif

#if defined(USE_REXX6000)
static LONG RxSubcomHandler( RSH_ARG0_TYPE, RSH_ARG1_TYPE, RSH_ARG2_TYPE );
#else
static RexxSubcomHandler RxSubcomHandler;
#endif

#if !defined(HAVE_STRERROR)
/*
 * This function and the following description borrowed from Regina 0.08a
 *
 * Sigh! This must probably be done this way, although it's incredibly 
 * backwards. Some versions of gcc comes with a complete set of ANSI C
 * include files, which contains the definition of strerror(). However,
 * that function does not exist in the default libraries of SunOS. 
 * To circumvent that problem, strerror() is #define'd to get_sys_errlist() 
 * in config.h, and here follows the definition of that function. 
 * Originally, strerror() was #defined to sys_errlist[x], but that does
 * not work if string.h contains a declaration of the (non-existing) 
 * function strerror(). 
 *
 * So, this is a mismatch between the include files and the library, and
 * it should not create problems for Regina. However, the _user_ will not
 * encounter any problems until he compiles Regina, so we'll have to 
 * clean up after a buggy installation of the C compiler!
 */
char *rxpackage_get_sys_errlist( int num )
{
   extern char *sys_errlist[] ;
   return sys_errlist[num] ;
}
#endif


/*-----------------------------------------------------------------------------
 * Compares buffers for equality ignoring case
 *----------------------------------------------------------------------------*/
int memcmpi

#ifdef HAVE_PROTO
   (char *buf1, char *buf2, int len)
#else
   (buf1,buf2,len)
   char    *buf1;
   char    *buf2;
   int     len;
#endif
{
   register short i=0;
   char c1=0,c2=0;
   for(i=0;i<len;i++)
   {
      if (isupper(*buf1))
         c1 = tolower(*buf1);
      else
         c1 = *buf1;
      if (isupper(*buf2))
         c2 = tolower(*buf2);
      else
         c2 = *buf2;
      if (c1 != c2)
         return(c1-c2);
      ++buf1;
      ++buf2;
   }
 return(0);
}
/*-----------------------------------------------------------------------------
 * Uppercases the supplied string.
 *----------------------------------------------------------------------------*/
char *make_upper

#ifdef HAVE_PROTO
   (char *str)
#else
   (str)
   char    *str;
#endif

{
   char *save_str=str;
   while(*str)
   {
      if (islower(*str))
         *str = toupper(*str);
      ++str;
   }
   return(save_str);
}

/*-----------------------------------------------------------------------------
 * Allocate memory for a char * based on an RXSTRING
 *----------------------------------------------------------------------------*/
char *AllocString

#ifdef HAVE_PROTO
   (char *buf, int bufsize)
#else
   (buf, bufsize)
   char    *buf;
   int   bufsize;
#endif
{
   char *tempstr=NULL;

   tempstr = (char *)malloc(sizeof(char)*(bufsize+1));
   return tempstr;
}


/*-----------------------------------------------------------------------------
 * Copy a non terminated character array to the nominated buffer (truncate
 * if necessary) and null terminate.
 *----------------------------------------------------------------------------*/
char *MkAsciz

#ifdef HAVE_PROTO
   (char *buf, int bufsize, char *str, int len)
#else
   (buf, bufsize, str, len)
   char    *buf;
   int  bufsize;
   char    *str;
   int  len;
#endif

{
   bufsize--;     /* Make room for terminating byte */
   if (len > bufsize)
      len = bufsize;
   (void)memcpy(buf, str, len);
   buf[len] = '\0';
   return buf;
}

/*-----------------------------------------------------------------------------
 * Check number of parameters
 *----------------------------------------------------------------------------*/
int my_checkparam(RxPackageGlobalDataDef *RxPackageGlobalData, char *name, int argc, int mini, int maxi)
{
   if ( argc < mini )
   {
      RxDisplayError( RxPackageGlobalData,
                      name,
                      "*ERROR* Not enough parameters in call to \"%s\". Minimum %d\n",
                      name, mini);
      return 1;
   }
   if ( argc > maxi
   &&   maxi != 0 )
   {
      RxDisplayError( RxPackageGlobalData,
                      name, 
                      "*ERROR* Too many parameters in call to \"%s\". Maximum %d\n",
                      name, maxi);
      return 1;
   }
   return 0;
}

/*-----------------------------------------------------------------------------
 * Create a REXX variable of the specified name and bind the value to it.
 *----------------------------------------------------------------------------*/
int SetRexxVariable

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name, int namelen, char *value, int valuelen )
#else
   ( RxPackageGlobalData, name, namelen, value, valuelen )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char    *name;
   int     namelen;
   char    *value;
   int     valuelen;
#endif

{
   ULONG  rc=0L;
   SHVBLOCK       shv;

   /*
    * Uppercase the variable before we do anything. That way all debugging
    * output reflects the "real" name we are setting.
    */
   ( void )make_upper( name );

   InternalTrace( RxPackageGlobalData, "SetRexxVariable", "\"%s\",%d,\"%s\",%d", name, namelen, value, valuelen );

   if ( RxPackageGlobalData->RxRunFlags & MODE_DEBUG)
   {
      char buf1[50], buf2[50];

      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
         "*DEBUG* Setting variable \"%s\" to \"%s\".\n",
         MkAsciz( buf1, sizeof( buf1 ), name, namelen ),
         MkAsciz( buf2, sizeof( buf2 ), value, valuelen ) );
   }
   shv.shvnext = ( SHVBLOCK* )NULL;
   shv.shvcode = RXSHV_SET;
   MAKERXSTRING( shv.shvname, name, ( ULONG )namelen );
   MAKERXSTRING( shv.shvvalue, value, ( ULONG )valuelen );
   shv.shvnamelen = shv.shvname.strlength;
   shv.shvvaluelen = shv.shvvalue.strlength;
   rc = RexxVariablePool( &shv );
   if ( rc == RXSHV_OK
   ||   rc == RXSHV_NEWV )
      return( 0 );
   else
   {
      if ( RxPackageGlobalData->RxRunFlags & MODE_DEBUG)
      {
         char buf1[50], buf2[50];
         char *err;
         switch( rc )
         {
            case RXSHV_TRUNC: err = "Name of Value truncated"; break;
            case RXSHV_BADN : err = "Invalid variable name"; break;
            case RXSHV_MEMFL: err = "Memory problem; probably none"; break;
            case RXSHV_BADF : err = "Invalid function code"; break;
            case RXSHV_NOAVL: err = "Interface not available"; break;
            default:     err = "Unknown error with RexxVariablePool()"; break;
         }

         (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
            "*DEBUG* Error Setting variable \"%s\" to \"%s\". %s.\n",
            MkAsciz( buf1, sizeof( buf1 ), name, namelen ),
            MkAsciz( buf2, sizeof( buf2 ), value, valuelen ),
            err );
      }
      return( 1 );
   }
}

/*-----------------------------------------------------------------------------
 * This function gets a REXX variable and returns it in an RXSTRING
 *----------------------------------------------------------------------------*/
RXSTRING *GetRexxVariable

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name, RXSTRING *value, int suffix )
#else
   ( RxPackageGlobalData, name, value, suffix )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char *name;
   RXSTRING *value;
   int suffix;
#endif
{
   static SHVBLOCK shv;
   char variable_name[350];
   ULONG rc = 0L;

   InternalTrace( RxPackageGlobalData, "GetRexxVariable", "%s,%x,%d", name, value, suffix );

   shv.shvnext=NULL;                                /* only one block */
   shv.shvcode = RXSHV_FETCH;
   /*
    * This calls the RexxVariablePool() function for each value. This is
    * not the most efficient way of doing this.
    */
   if ( suffix == -1)
      strcpy( variable_name, name );
   else
      sprintf( variable_name, "%s%-d", name, suffix );
   ( void )make_upper( variable_name );
   /*
    * Now (attempt to) get the REXX variable
    * Set shv.shvvalue to NULL to force interpreter to allocate memory.
    */
   MAKERXSTRING( shv.shvname, variable_name, strlen( variable_name ) );
   shv.shvvalue.strptr = NULL;
   shv.shvvalue.strlength = 0;
   /*
    * One or both of these is needed, too <sigh>
    */
   shv.shvnamelen = strlen( variable_name );
   shv.shvvaluelen = 0;
   rc = RexxVariablePool( &shv );              /* Set the REXX variable */
   if ( rc == RXSHV_OK )
   {
      value->strptr = ( RXSTRING_STRPTR_TYPE )malloc( ( sizeof( char )*shv.shvvalue.strlength ) + 1 );
      if ( value->strptr != NULL )
      {
         value->strlength = shv.shvvalue.strlength;
         memcpy( value->strptr, shv.shvvalue.strptr, value->strlength );
         *( value->strptr + value->strlength ) = '\0';
      }
#if defined(REXXFREEMEMORY)
      RexxFreeMemory( shv.shvvalue.strptr );
#elif defined(WIN32) && (defined(USE_OREXX) || defined(USE_WINREXX) || defined(USE_QUERCUS))
      GlobalFree( shv.shvvalue.strptr );
#elif defined(USE_OS2REXX)
      DosFreeMem( shv.shvvalue.strptr );
#else
      free( shv.shvvalue.strptr );
#endif
   }
   else
      value = NULL;
   return( value );
}

/*-----------------------------------------------------------------------------
 * This function gets a REXX variable as a number and returns it
 * Returns NULL if the variable is not set; an invalid number returns 0
 *----------------------------------------------------------------------------*/
int *GetRexxVariableInteger

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name, int *value, int suffix )
#else
   ( RxPackageGlobalData, name, value, suffix )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char *name;
   int *value;
   int suffix;
#endif
{
   static SHVBLOCK shv;
   char variable_name[350];
   ULONG rc = 0L;

   InternalTrace( RxPackageGlobalData, "GetRexxVariableNumber", "%s,%x,%d", name, value, suffix );

   shv.shvnext=NULL;                                /* only one block */
   shv.shvcode = RXSHV_FETCH;
   /*
    * This calls the RexxVariablePool() function for each value. This is
    * not the most efficient way of doing this.
    */
   if ( suffix == -1)
      strcpy( variable_name, name );
   else
      sprintf( variable_name, "%s%-d", name, suffix );
   ( void )make_upper( variable_name );
   /*
    * Now (attempt to) get the REXX variable
    * Set shv.shvvalue to NULL to force interpreter to allocate memory.
    */
   MAKERXSTRING( shv.shvname, variable_name, strlen( variable_name ) );
   shv.shvvalue.strptr = NULL;
   shv.shvvalue.strlength = 0;
   /*
    * One or both of these is needed, too <sigh>
    */
   shv.shvnamelen = strlen( variable_name );
   shv.shvvaluelen = 0;
   rc = RexxVariablePool( &shv );              /* Get the REXX variable */
   if ( rc == RXSHV_OK )
   {
      if ( RxStrToInt( RxPackageGlobalData, &shv.shvvalue, value ) == -1 )
         value = NULL;
#if defined(REXXFREEMEMORY)
      RexxFreeMemory( shv.shvvalue.strptr );
#elif defined(WIN32) && (defined(USE_OREXX) || defined(USE_WINREXX) || defined(USE_QUERCUS))
      GlobalFree( shv.shvvalue.strptr );
#elif defined(USE_OS2REXX)
      DosFreeMem( shv.shvvalue.strptr );
#else
      free( shv.shvvalue.strptr );
#endif
   }
   else
      value = NULL;
   return( value );
}

/*-----------------------------------------------------------------------------
 * Drop a REXX variable of the specified name
 *----------------------------------------------------------------------------*/
int DropRexxVariable

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name, int namelen )
#else
   ( RxPackageGlobalData, name, namelen )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char    *name;
   int     namelen;
#endif

{
   ULONG  rc=0L;
   SHVBLOCK       shv;

   /*
    * Uppercase the variable before we do anything. That way all debugging
    * output reflects the "real" name we are setting.
    */
   ( void )make_upper( name );

   InternalTrace( RxPackageGlobalData, "DropRexxVariable", "\"%s\",%d", name, namelen );

   if ( RxPackageGlobalData->RxRunFlags & MODE_DEBUG)
   {
      char buf1[50];

      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
         "*DEBUG* Dropping variable \"%s\".\n",
         MkAsciz( buf1, sizeof( buf1 ), name, namelen ) );
   }
   shv.shvnext = ( SHVBLOCK* )NULL;
   shv.shvcode = RXSHV_DROPV;
   MAKERXSTRING( shv.shvname, name, ( ULONG )namelen );
   shv.shvnamelen = shv.shvname.strlength;
   rc = RexxVariablePool( &shv );
   if ( rc == RXSHV_OK )
      return( 0 );
   else
   {
      if ( RxPackageGlobalData->RxRunFlags & MODE_DEBUG)
      {
         char buf1[50];
         char *err;
         switch( rc )
         {
            case RXSHV_TRUNC: err = "Name of Value truncated"; break;
            case RXSHV_BADN : err = "Invalid variable name"; break;
            case RXSHV_MEMFL: err = "Memory problem; probably none"; break;
            case RXSHV_BADF : err = "Invalid function code"; break;
            case RXSHV_NOAVL: err = "Interface not available"; break;
            default:     err = "Unknown error with RexxVariablePool()"; break;
         }

         (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
            "*DEBUG* Error Dropping variable \"%s\". %s.\n",
            MkAsciz( buf1, sizeof( buf1 ), name, namelen ),
            err );
      }
      return( 1 );
   }
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to integer. Return 0 if OK and -1 if error.
 * Assumes a string of decimal digits and does not check for overflow!
 *----------------------------------------------------------------------------*/
int StrToInt

#ifdef HAVE_PROTO
   (RXSTRING *ptr, ULONG *result) 
#else
   (ptr, result) 
   RXSTRING *ptr;
   ULONG    *result; 
#endif

{
   int    i=0;
   char   *p=NULL;
   ULONG  sum=0L;

   p = (char *)ptr->strptr;
   for (i = ptr->strlength; i; i--)
   {
      if (isdigit(*p))
         sum = sum * 10 + (*p++ - '0');
      else
         return -1;
   }
   *result = sum;
   return 0;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to integer. Return 0 if OK and -1 if error.
 * Assumes a string of decimal digits and does not check for overflow!
 *----------------------------------------------------------------------------*/
int StrToNumber

#ifdef HAVE_PROTO
   (RXSTRING *ptr, LONG *result) 
#else
   (ptr, result) 
   RXSTRING *ptr;
   LONG     *result;
#endif

{
   unsigned int i=0;
   char   *p=NULL;
   LONG   sum=0L;
   int    neg=0;

   p = (char *)ptr->strptr;
   for (i = ptr->strlength; i; i--)
   {
      if (isdigit(*p))
         sum = sum * 10 + (*p - '0');
      else if ( i == ptr->strlength && *p == '-' )
         neg = 1;
      else if ( i == ptr->strlength && *p == '+' )
         ;
      else
         return -1;
      p++;
   }
   if ( neg )
      sum *= -1;
   *result = sum;
   return 0;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to a boolean. Input can be ON, Yes, 1 or OFF No, 0
 *----------------------------------------------------------------------------*/
int StrToBool

#ifdef HAVE_PROTO
   (RXSTRING *ptr, ULONG *result) 
#else
   (ptr, result) 
   RXSTRING *ptr;
   ULONG    *result; 
#endif

{
   char   *p=(char *)ptr->strptr;
   int    len=ptr->strlength;

   if (memcmp(p,"YES",len) == 0
   ||  memcmp(p,"yes",len) == 0
   ||  memcmp(p,"Y",len) == 0
   ||  memcmp(p,"y",len) == 0
   ||  memcmp(p,"ON",len) == 0
   ||  memcmp(p,"on",len) == 0
   ||  memcmp(p,"1",len) == 0)
   {
      *result = 1;
      return(0);
   }
   if (memcmp(p,"NO",len) == 0
   ||  memcmp(p,"no",len) == 0
   ||  memcmp(p,"N",len) == 0
   ||  memcmp(p,"n",len) == 0
   ||  memcmp(p,"OFF",len) == 0
   ||  memcmp(p,"off",len) == 0
   ||  memcmp(p,"0",len) == 0)
   {
      *result = 0;
      return(0);
   }
   return (-1);
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to signed integer. Return 0 if OK and -1 if error.
 * Assumes a string of decimal digits with or without signs and does not check for overflow!
 *----------------------------------------------------------------------------*/
int RxStrToInt

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, int *result )
#else
   ( RxPackageGlobalData, ptr, result )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   int      *result;
#endif

{
   unsigned int i=0;
   char   *p=NULL;
   int    sum=0;
   int    neg=0;

   p = (char *)ptr->strptr;
   for (i = ptr->strlength; i; i--)
   {
      if (isdigit(*p))
         sum = sum * 10 + (*p - '0');
      else if ( i == ptr->strlength && *p == '-' )
         neg = 1;
      else if ( i == ptr->strlength && *p == '+' )
         ;
      else
         return -1;
      p++;
   }
   if ( neg )
      sum *= -1;
   *result = sum;
   DEBUGDUMP(fprintf(stderr,"%s-%d: In RxStrToInt() Input string is \"%s\" Result is %d\n",__FILE__,__LINE__,ptr->strptr, sum);)
   return 0;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to unsigned integer. Return 0 if OK and -1 if error.
 * Assumes a string of decimal digits and no signs and does not check for overflow!
 *----------------------------------------------------------------------------*/
int RxStrToUInt

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, unsigned int *result )
#else
   ( RxPackageGlobalData, ptr, result )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING     *ptr;
   unsigned int *result;
#endif

{
   int    i=0;
   char   *p=NULL;
   unsigned int sum=0;

   p = (char *)ptr->strptr;
   for (i = ptr->strlength; i; i--)
   {
      if (isdigit(*p))
         sum = sum * 10 + (*p++ - '0');
      else
         return -1;
   }
   *result = sum;
   return 0;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to signed long. Return 0 if OK and -1 if error.
 * Assumes a string of decimal digits with or without signs and does not check for overflow!
 *----------------------------------------------------------------------------*/
int RxStrToLong

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, long *result )
#else
   ( RxPackageGlobalData, ptr, result )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   long     *result;
#endif

{
   unsigned int i=0;
   char   *p=NULL;
   long   sum=0L;
   int    neg=0;

   p = (char *)ptr->strptr;
   for (i = ptr->strlength; i; i--)
   {
      if (isdigit(*p))
         sum = sum * 10 + (*p - '0');
      else if ( i == ptr->strlength && *p == '-' )
         neg = 1;
      else if ( i == ptr->strlength && *p == '+' )
         ;
      else
         return -1;
      p++;
   }
   if ( neg )
      sum *= -1;
   *result = sum;
   return 0;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to unsigned long. Return 0 if OK and -1 if error.
 * Assumes a string of decimal digits and no signs and does not check for overflow!
 *----------------------------------------------------------------------------*/
int RxStrToULong

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, unsigned long *result )
#else
   ( RxPackageGlobalData, ptr, result )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING      *ptr;
   unsigned long *result;
#endif

{
   int    i=0;
   char   *p=NULL;
   unsigned long sum=0L;

   p = (char *)ptr->strptr;
   for (i = ptr->strlength; i; i--)
   {
      if (isdigit(*p))
         sum = sum * 10 + (*p++ - '0');
      else
         return -1;
   }
   *result = sum;
   return 0;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING to double. Return 0 if OK and -1 if error.
 * Assumes a string of decimal digits with or without signs and does not check for overflow!
 *----------------------------------------------------------------------------*/
int RxStrToDouble

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, double *result )
#else
   ( RxPackageGlobalData, ptr, result )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   double   *result;
#endif

{
   char *endptr;
   double sum;

   sum = (double)strtod( ptr->strptr, &endptr );
   if ( sum == 0
   &&   errno != 0 )
      return -1;
   if ( sum == 0 
   &&   (char *)endptr == (char *)ptr->strptr )
      return -1;
   *result = sum;
   return 0;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING containing a stem name to an array of char pointers
 * Allocates memory for the char pointers, NOT the strings. The pointers
 * need to be freed using RxFreeCharArray()
 * Returns the number of items in the array
 *----------------------------------------------------------------------------*/
int RxStemToCharArray

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, char ***retval )
#else
   ( RxPackageGlobalData, ptr, retval )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   char     ***retval;
#endif

{
   int      len=ptr->strlength;
   int      num_items,i;
   RXSTRING value;
   char **ret, **tmp;

   /*
    * Validate that 'ptr' is a stem name.
    */
   if ( ptr->strptr[len-1] != '.' )
      return -1;
   /*
    * Get the number of items in the array
    */
   if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, &num_items, 0 ) == NULL )
      return -1;
   /*
    * Allocate num_items char *
    */
   if ( num_items )
   {
      if ( ( ret = tmp = (char **)malloc( num_items * sizeof(char *) ) ) == NULL )
         return -1;
      /*
       * Get each stem value, and set the equivalent entry in the allocated
       * char **
       */
      for ( i = 0; i < num_items; i++, tmp++ )
      {
         if ( GetRexxVariable( RxPackageGlobalData, ptr->strptr, &value, i+1 ) == NULL )
            return -1;
         *tmp = value.strptr;
      }
      *retval = ret;
   }
   return num_items;
}

/*-----------------------------------------------------------------------------
 * Frees memory allocated by RxStemToCharArray()
 *----------------------------------------------------------------------------*/
void RxFreeCharArray

#ifdef HAVE_PROTO
   (char **ptr, int num_args)
#else
   (ptr, num_args)
   char     **ptr;
   int      num_args;
#endif

{
   int      i;

   /*
    * Validate that 'ptr' is valid.
    */
   if ( ptr == NULL )
      return;
#if 0
   /*
    * Free each item...
    * NO WAY. These are passed as arguments from the Rexx interpreter
    * and cannot be freed by us.
    */
   for ( i = 0; i < num_args; i++ )
   {
      free( *ptr );
   }
#endif
   /*
    * Free the ptr
    */
   free( ptr );
   return;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING containing a stem name to an array of ULONGs
 * Allocates memory for ULONGs which needs to be freed by the caller
 * using RxFreeULongArray()
 * Returns the number of items in the array
 *----------------------------------------------------------------------------*/
int RxStemToULongArray

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, unsigned long **retval )
#else
   ( RxPackageGlobalData, ptr, retval )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING      *ptr;
   unsigned long **retval;
#endif

{
   int      len=ptr->strlength;
   int      num_items,i;
   unsigned long value;
   unsigned long *ret,*tmp;

   /*
    * Validate that 'ptr' is a stem name.
    */
   if ( ptr->strptr[len-1] != '.' )
      return -1;
   /*
    * Get the number of items in the array
    */
   if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, &num_items, 0 ) == NULL )
      return -1;
   /*
    * Allocate num_items unsigned longs
    */
   if ( num_items )
   {
      if ( ( ret = tmp = (unsigned long *)malloc( num_items * sizeof(unsigned long) ) ) == NULL )
         return -1;
      /*
       * Get each stem value, and set the equivalent entry in the allocated
       * ULONG *
       */
      for ( i = 0; i < num_items; i++, tmp++ )
      {
         if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, (int *)&value, i+1 ) == NULL )
            return -1;
         *tmp = (unsigned long)value;
      }
      *retval = ret;
   }
   return num_items;
}

/*-----------------------------------------------------------------------------
 * Frees memory allocated by RxStemToULongArray()
 *----------------------------------------------------------------------------*/
void RxFreeULongArray

#ifdef HAVE_PROTO
   (unsigned long *ptr)
#else
   (ptr)
   unsigned long *ptr;
#endif

{
   /*
    * Validate that 'ptr' is valid.
    */
   if ( ptr == NULL )
      return;
   /*
    * Free the ptr
    */
   free( ptr );
   return;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING containing a stem name to an array of LONGs
 * Allocates memory for LONGs which needs to be freed by the caller
 * using RxFreeLongArray()
 * Returns the number of items in the array
 *----------------------------------------------------------------------------*/
int RxStemToLongArray

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, long **retval )
#else
   ( RxPackageGlobalData, ptr, retval )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   long     **retval;
#endif

{
   int      len=ptr->strlength;
   int      num_items,i;
   long     value;
   long     *ret,*tmp;

   /*
    * Validate that 'ptr' is a stem name.
    */
   if ( ptr->strptr[len-1] != '.' )
      return -1;
   /*
    * Get the number of items in the array
    */
   if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, &num_items, 0 ) == NULL )
      return -1;
   /*
    * Allocate num_items unsigned longs
    */
   if ( num_items )
   {
      if ( ( ret = tmp = (long *)malloc( num_items * sizeof(long) ) ) == NULL )
         return -1;
      /*
       * Get each stem value, and set the equivalent entry in the allocated
       * LONG *
       */
      for ( i = 0; i < num_items; i++, tmp++ )
      {
         if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, (int *)&value, i+1 ) == NULL )
            return -1;
         *tmp = (long)value;
      }
      *retval = ret;
   }
   return num_items;
}

/*-----------------------------------------------------------------------------
 * Frees memory allocated by RxStemToLongArray()
 *----------------------------------------------------------------------------*/
void RxFreeLongArray

#ifdef HAVE_PROTO
   (long *ptr)
#else
   (ptr)
   long *ptr;
#endif

{
   /*
    * Validate that 'ptr' is valid.
    */
   if ( ptr == NULL )
      return;
   /*
    * Free the ptr
    */
   free( ptr );
   return;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING containing a stem name to an array of INTs
 * Allocates memory for INTs which needs to be freed by the caller
 * using RxFreeIntArray()
 * Returns the number of items in the array
 *----------------------------------------------------------------------------*/
int RxStemToIntArray

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, int **retval )
#else
   ( RxPackageGlobalData, ptr, retval )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   int      **retval;
#endif

{
   int      len=ptr->strlength;
   int      num_items,i;
   int      value;
   int      *ret,*tmp;

   /*
    * Validate that 'ptr' is a stem name.
    */
   if ( ptr->strptr[len-1] != '.' )
      return -1;
   /*
    * Get the number of items in the array
    */
   if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, &num_items, 0 ) == NULL )
      return -1;
   /*
    * Allocate num_items unsigned longs
    */
   if ( num_items )
   {
      if ( ( ret = tmp = (int *)malloc( num_items * sizeof(int) ) ) == NULL )
         return -1;
      /*
       * Get each stem value, and set the equivalent entry in the allocated
       * INT *
       */
      for ( i = 0; i < num_items; i++, tmp++ )
      {
         if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, (int *)&value, i+1 ) == NULL )
            return -1;
         *tmp = (int)value;
      }
      *retval = ret;
   }
   return num_items;
}

/*-----------------------------------------------------------------------------
 * Frees memory allocated by RxStemToIntArray()
 *----------------------------------------------------------------------------*/
void RxFreeIntArray

#ifdef HAVE_PROTO
   (int *ptr)
#else
   (ptr)
   int *ptr;
#endif

{
   /*
    * Validate that 'ptr' is valid.
    */
   if ( ptr == NULL )
      return;
   /*
    * Free the ptr
    */
   free( ptr );
   return;
}

/*-----------------------------------------------------------------------------
 * Converts a RXSTRING containing a stem name to an array of UINTs
 * Allocates memory for UINTs which needs to be freed by the caller
 * using RxFreeUIntArray()
 * Returns the number of items in the array
 *----------------------------------------------------------------------------*/
int RxStemToUIntArray

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, unsigned int **retval )
#else
   ( RxPackageGlobalData, ptr, retval )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   unsigned int **retval;
#endif

{
   int      len=ptr->strlength;
   int      num_items,i;
   unsigned int value;
   unsigned int *ret,*tmp;

   /*
    * Validate that 'ptr' is a stem name.
    */
   if ( ptr->strptr[len-1] != '.' )
      return -1;
   /*
    * Get the number of items in the array
    */
   if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, &num_items, 0 ) == NULL )
      return -1;
   /*
    * Allocate num_items unsigned longs
    */
   if ( num_items )
   {
      if ( ( ret = tmp = (unsigned int *)malloc( num_items * sizeof(unsigned int) ) ) == NULL )
         return -1;
      /*
       * Get each stem value, and set the equivalent entry in the allocated
       * UNSIGNED INT *
       */
      for ( i = 0; i < num_items; i++, tmp++ )
      {
         if ( GetRexxVariableInteger( RxPackageGlobalData, ptr->strptr, (int *)&value, i+1 ) == NULL )
            return -1;
         *tmp = (unsigned int)value;
      }
      *retval = ret;
   }
   return num_items;
}

/*-----------------------------------------------------------------------------
 * Frees memory allocated by RxStemToUIntArray()
 *----------------------------------------------------------------------------*/
void RxFreeUIntArray

#ifdef HAVE_PROTO
   (unsigned int *ptr)
#else
   (ptr)
   unsigned int *ptr;
#endif

{
   /*
    * Validate that 'ptr' is valid.
    */
   if ( ptr == NULL )
      return;
   /*
    * Free the ptr
    */
   free( ptr );
   return;
}

/*-----------------------------------------------------------------------------
 * Sets the variable specified to the number value supplied.
 *----------------------------------------------------------------------------*/
int RxNumberToVariable

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *ptr, ULONG number )
#else
   ( RxPackageGlobalData, ptr, number )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *ptr;
   ULONG number;
#endif

{
   int  len;
   char buf[50];

   /*
    * Convert the number to a string...
    */
   len = sprintf( buf, "%ld", number );
   /*
    * Set the Rexx variable if valid
    */
   if ( ptr
   &&   ptr->strlength
   &&   ptr->strptr )
   {
      if ( SetRexxVariable( RxPackageGlobalData, ptr->strptr, ptr->strlength, buf, len ) == 1 )
         return -1;
   }
   return 0;
}


/*-----------------------------------------------------------------------------
 * This is called when in VERBOSE mode. It prints function name & arg values.
 *----------------------------------------------------------------------------*/
RxPackageGlobalDataDef *FunctionPrologue

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, PackageInitialiser *RxPackageInitialiser, char *name, ULONG argc, RXSTRING *argv )
#else
   ( RxPackageGlobalData, RxPackageInitialiser, name, argc, argv )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   PackageInitialiser *RxPackageInitialiser;
   char *name;
   ULONG argc;
   RXSTRING *argv;
#endif

{
   ULONG i = 0L;
   char buf[61];
   RxPackageGlobalDataDef *GlobalData;
   int rc;

   if ( RxPackageGlobalData == NULL )
   {
      DEBUGDUMP(fprintf(stderr,"%s-%d: In FunctionPrologue() for %s RxPackageGlobalData is NULL\n",__FILE__,__LINE__,name);)
      GlobalData = InitRxPackage( RxPackageGlobalData, RxPackageInitialiser, &rc );
   }
   else
   {
      GlobalData = RxPackageGlobalData;
   }

   if ( GlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( GlobalData->RxTraceFilePointer, "++\n" );
      (void)fprintf( GlobalData->RxTraceFilePointer, "++ Call %s%s\n", name, argc ? "" : "()" );
      for (i = 0; i < argc; i++) 
      {
         (void)fprintf( GlobalData->RxTraceFilePointer, "++ %3ld: \"%s\"\n",
            i+1,
            MkAsciz( buf, sizeof(buf), (char *)RXSTRPTR( argv[i] ), (int)RXSTRLEN( argv[i] ) ) );
      }
      fflush( GlobalData->RxTraceFilePointer );
   }
   /*
    * This should be replaced with an integer to make things
    * run faster!!
    */
   if ( strcmp( name, GlobalData->FName ) != 0 )
      strcpy( GlobalData->FName, name );

   return GlobalData;
}

/*-----------------------------------------------------------------------------
 * This is called when in VERBOSE mode. It prints function name & return value.
 *----------------------------------------------------------------------------*/
long FunctionEpilogue

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name, long rc )
#else
   ( RxPackageGlobalData, name, rc )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char *name;
   long rc;
#endif

{
   if ( RxPackageGlobalData == NULL )
   {
      return rc;
   }

   if ( RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer, "++\n" );
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer, "++ Exiting %s() with %ld\n", name, rc );
      fflush( RxPackageGlobalData->RxTraceFilePointer );
   }
   return rc;
}

/*-----------------------------------------------------------------------------
 * This is called when in VERBOSE mode. It prints function name & return value.
 *----------------------------------------------------------------------------*/
void FunctionTrace

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name, ... )
#else
   ( RxPackageGlobalData, name )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char *name;
#endif
{
   va_list argptr;
   char *fmt=NULL;

   if ( RxPackageGlobalData == NULL )
   {
      return;
   }

   if ( RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void) fprintf( RxPackageGlobalData->RxTraceFilePointer, ">>\n" );
      va_start( argptr, name );
      fmt = va_arg( argptr, char *);
      if (fmt != NULL)
      {
         (void)fprintf( RxPackageGlobalData->RxTraceFilePointer, ">> " );
         (void)vfprintf( RxPackageGlobalData->RxTraceFilePointer, fmt, argptr );
      }
      (void) fprintf( RxPackageGlobalData->RxTraceFilePointer, ")\n");
      fflush( RxPackageGlobalData->RxTraceFilePointer );
      va_end( argptr );
   }
   return;
}

/*-----------------------------------------------------------------------------
 * This is called when run_flags & MODE_INTERNAL is true
 *----------------------------------------------------------------------------*/
void InternalTrace

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name, ...)
#else
   ( RxPackageGlobalData, name )
    RxPackageGlobalDataDef *RxPackageGlobalData;
    char *name;
#endif

{
   va_list argptr;
   char *fmt=NULL;

   if ( RxPackageGlobalData != NULL
   &&   RxPackageGlobalData->RxRunFlags & MODE_INTERNAL )
   {
      (void) fprintf( RxPackageGlobalData->RxTraceFilePointer, ">>>> Call %s(", name );
      va_start( argptr, name );
      fmt = va_arg( argptr, char *);
      if (fmt != NULL)
      {
         (void)vfprintf( RxPackageGlobalData->RxTraceFilePointer, fmt, argptr );
      }
      (void) fprintf( RxPackageGlobalData->RxTraceFilePointer, ")\n");
      fflush( RxPackageGlobalData->RxTraceFilePointer );
      va_end( argptr );
   }
   return;
}
void RxDisplayError

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RFH_ARG0_TYPE name, ...)
#else
   ( RxPackageGlobalData, name )
    RxPackageGlobalDataDef *RxPackageGlobalData;
    char *name;
#endif

{
   va_list argptr;
   char *fmt=NULL;

   if ( RxPackageGlobalData != NULL )
   {
      (void) fprintf( RxPackageGlobalData->RxTraceFilePointer, ">>>> Calling %s(", name );
      va_start( argptr, name );
      fmt = va_arg( argptr, char *);
      if (fmt != NULL)
      {
         (void)vfprintf( RxPackageGlobalData->RxTraceFilePointer, fmt, argptr );
      }
      (void) fprintf( RxPackageGlobalData->RxTraceFilePointer, ")\n");
      fflush( RxPackageGlobalData->RxTraceFilePointer );
      va_end( argptr );
   }
   return;
}


/*-----------------------------------------------------------------------------
 * This function registers the default subcommand handler...
 *----------------------------------------------------------------------------*/
int RegisterRxSubcom

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RexxSubcomHandler ptr)
#else
   ( RxPackageGlobalData, ptr )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RexxSubcomHandler ptr;
#endif

{
   ULONG rc=0L;

   InternalTrace( RxPackageGlobalData, "RegisterRxSubcom", NULL );

#if defined(USE_REXX6000)
   rc = RexxRegisterSubcom( (RRSE_ARG0_TYPE)RXPACKAGENAME,
                            (RRSE_ARG1_TYPE)(ptr) ? ptr : RxSubcomHandler,
                            (RRSE_ARG2_TYPE)NULL );
#else
   rc = RexxRegisterSubcomExe( (RRSE_ARG0_TYPE)RXPACKAGENAME,
                               (RRSE_ARG1_TYPE)(ptr) ? ptr : RxSubcomHandler,
                               (RRSE_ARG2_TYPE)NULL );
#endif
   if ( rc != RXSUBCOM_OK )
      return 1;
   return 0;
}

/*-----------------------------------------------------------------------------
 * This function registers the package's RXINI exit handler...
 *----------------------------------------------------------------------------*/
int RegisterRxInit

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RexxExitHandler ptr, char *name)
#else
   ( RxPackageGlobalData, ptr, name )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RexxExitHandler ptr;
   char *name;
#endif

{
   ULONG rc=0L;

   InternalTrace( RxPackageGlobalData, "RegisterRxInit", "Name: %s Addr: %ld", name, ptr );

   rc = RexxRegisterExitExe( (RREE_ARG0_TYPE)name,
                             (RREE_ARG1_TYPE)(ptr),
                             (RREE_ARG2_TYPE)NULL );
   InternalTrace( RxPackageGlobalData, "RegisterRxInit", "returning %d", rc );
   if ( rc != RXEXIT_OK )
      return 1;
   return 0;
}

/*-----------------------------------------------------------------------------
 * This function registers the external functions...
 *----------------------------------------------------------------------------*/
int RegisterRxFunctions

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RexxFunction *RxPackageFunctions, char *name )
#else
   ( RxPackageGlobalData,  RxPackageFunctions, name )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RexxFunction *RxPackageFunctions;
   char *name;
#endif

{
   RexxFunction *func=NULL;
   ULONG rc=0L;

   InternalTrace( RxPackageGlobalData, "RegisterRxFunctions", NULL );

   for ( func = RxPackageFunctions; func->InternalName; func++ )
   {
#if defined(DYNAMIC_LIBRARY)
# if !defined(USE_REXX6000)
      if (func->DllLoad)
      {
         rc = RexxRegisterFunctionDll( func->ExternalName,
              name,
              func->InternalName );
         InternalTrace( RxPackageGlobalData, "RegisterRxFunctions","%s-%d: Registered (DLL) %s with rc = %ld",__FILE__,__LINE__,func->ExternalName,rc);
      }
# endif
#else
# if defined(WIN32) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
      (void)RexxDeregisterFunction( func->ExternalName );
# endif
# if defined(USE_REXX6000)
      rc = RexxRegisterFunction( func->ExternalName, func->EntryPoint, NULL );
      InternalTrace( RxPackageGlobalData, "RegisterRxFunctions","%s-%d: Registered (EXE) %s with rc = %d",__FILE__,__LINE__,func->ExternalName,rc);
# else
      rc = RexxRegisterFunctionExe( func->ExternalName, func->EntryPoint );
      InternalTrace( RxPackageGlobalData, "RegisterRxFunctions","%s-%d: Registered (EXE) %s with rc = %d",__FILE__,__LINE__,func->ExternalName,rc);
# endif
#endif
      if (rc != RXFUNC_OK
#ifdef RXFUNC_DUP
      &&  rc != RXFUNC_DUP /* bug in Object Rexx for Linux */
#else
      &&  rc != RXFUNC_DEFINED
#endif
      &&  rc != RXFUNC_NOTREG)
         return 1;
   }
#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
   if ( RexxRegisterExitExe( ( RREE_ARG0_TYPE )RXPACKAGENAME,
                             ( RREE_ARG1_TYPE )RxExitHandlerForSayTraceRedirection,
                             ( RREE_ARG2_TYPE )NULL) != RXEXIT_OK )
      return 1;
#endif
   return 0 ;
}

/*-----------------------------------------------------------------------------
 * This function queries if the supplied external function has been registered
 *----------------------------------------------------------------------------*/
int QueryRxFunction

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *funcname )
#else
   ( RxPackageGlobalData, funcname )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char *funcname;
#endif

{
   InternalTrace( RxPackageGlobalData, "QueryRxFunction", "%s", funcname );

#if defined(USE_REXX6000)
   if ( RexxQueryFunction( funcname, NULL ) == RXFUNC_OK )
#else
   if ( RexxQueryFunction( funcname ) == RXFUNC_OK )
#endif
   {
      DEBUGDUMP(fprintf(stderr,"%s-%d: Query function %s - found\n",__FILE__,__LINE__,funcname);)
      return 1;
   }
   DEBUGDUMP(fprintf(stderr,"%s-%d: Query function %s - not found\n",__FILE__,__LINE__,funcname);)
   return 0;
}

/*-----------------------------------------------------------------------------
 * This function deregisters the external functions...
 *----------------------------------------------------------------------------*/
int DeregisterRxFunctions

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RexxFunction *RxPackageFunctions, int verbose )
#else
   ( RxPackageGlobalData, RxPackageFunctions, verbose )
   RexxFunction *RxPackageFunctions;
   RxPackageGlobalDataDef *RxPackageGlobalData;
   int verbose;
#endif

{
   RexxFunction *func=NULL;
   int rc=0;

   InternalTrace( RxPackageGlobalData, "DeregisterRxFunctions", "%d", verbose );

   for ( func = RxPackageFunctions; func->InternalName; func++ )
   {
      rc = RexxDeregisterFunction( func->ExternalName );
      if ( verbose )
         fprintf(stderr,"Deregistering...%s - %d\n",func->ExternalName,rc);
      DEBUGDUMP(fprintf(stderr,"%s-%d: Deregistered %s with rc %d\n",__FILE__,__LINE__,func->ExternalName,rc);)
   }
#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
   RexxDeregisterExit( RXPACKAGENAME, NULL );
#endif
   return 0 ;
}

/*-----------------------------------------------------------------------------
 * This function sets all package constants...
 *----------------------------------------------------------------------------*/
int SetPackageConstants

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RxPackageConstantDef *RxConstants, char *pname, int drop )
#else
   ( RxPackageGlobalData, RxConstants, pname, drop )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RxPackageConstantDef *RxConstants;
   char *pname;
   int drop;
#endif

{
   int varlen=0,vallen,rc;
   char varname[250];
   char *value=NULL;
   char buf[100];
   RxPackageConstantDef *con=NULL;

   InternalTrace( RxPackageGlobalData, "SetPackageConstants", "Name: %s Drop: %d", pname, drop  );

   /*
    * Before we set the new constants, drop the previous stem.
    * This may fail because we don't have a stem yet.
    */
   for ( con=RxConstants; con->name; con++ )
   {
      if ( drop )
      {
         varlen = sprintf( varname, "%s%s.%s%s",
                           RxPackageGlobalData->PreviousConstantPrefix,
                           pname,
                           RxPackageGlobalData->PreviousConstantPrefix,
                           con->name );
         DropRexxVariable( RxPackageGlobalData, varname, varlen );
      }
      varlen = sprintf( varname, "%s%s.%s%s",
                        RxPackageGlobalData->ConstantPrefix,
                        pname,
                        RxPackageGlobalData->ConstantPrefix,
                        con->name );
      switch( con->type )
      {
         case 0:
            vallen = sprintf( buf, "%ld", con->numeric_value );
            value = buf;
            break;
         case 1:
            vallen = strlen( con->text_value );
            value = con->text_value;
            break;
         case 2:
            vallen = sprintf( buf, "%f", con->double_value );
            value = buf;
            break;
         case 3:
            vallen = sprintf( buf, "%c", con->char_value );
            value = buf;
            break;
         default:
            break;
      }
      /*
       * Now set the Rexx variable
       */
      rc = SetRexxVariable( RxPackageGlobalData, varname, varlen, value, vallen );
   }
   return 0;
}

/*-----------------------------------------------------------------------------
 * This function is called to initialise the package
 *----------------------------------------------------------------------------*/
RxPackageGlobalDataDef *InitRxPackage

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *MyGlob, PackageInitialiser *ptr, int *rc )
#else
   ( MyGlob, ptr, rc )
   RxPackageGlobalDataDef *MyGlob;
   PackageInitialiser *ptr;
   int *rc;
#endif

{
   char *env;
   RxPackageGlobalDataDef *RxPackageGlobalData;

   DEBUGDUMP(fprintf(stderr,"%s-%d: Start of InitRxPackage. MyGlob: %lx\n",__FILE__,__LINE__,(ULONG *)MyGlob);)
   if ( MyGlob )
   {
      RxPackageGlobalData = MyGlob;
      RxPackageGlobalData->deallocate = 0;
   }
   else
   {
      if ( ( RxPackageGlobalData = ( RxPackageGlobalDataDef *)malloc( sizeof( RxPackageGlobalDataDef ) ) ) == NULL )
      {
         fprintf( stderr, "Unable to allocate memory for Global Data\n" );
         *rc = 1;
         return NULL;
      }
      memset( RxPackageGlobalData, 0, sizeof( RxPackageGlobalDataDef ) );
      (void)RxSetTraceFile( RxPackageGlobalData, "stderr" );
      (void)RxSetConstantPrefix( RxPackageGlobalData, "!" );
      RxPackageGlobalData->deallocate = 1;
   }

   RxPackageGlobalData->terminated = 0;

   if ( (env = getenv(RXPACKAGE_DEBUG_VAR)) != NULL )
   {
      RxPackageGlobalData->RxRunFlags |= atoi(env);
   }
   /* 
    * Call any package-specific startup code here
    */
   if ( ptr )
   {
      *rc = (*ptr)( RxPackageGlobalData );
   }
   DEBUGDUMP(fprintf(stderr,"%s-%d: End of InitRxPackage with rc = %d\n",__FILE__,__LINE__,*rc);)
   return RxPackageGlobalData;
}


/*-----------------------------------------------------------------------------
 * This function is called to terminate all activity with the package.
 *----------------------------------------------------------------------------*/
int TermRxPackage

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef **RxPackageGlobalData, PackageTerminator *ptr, RexxFunction *RxPackageFunctions, char *progname, int deregfunc )
#else
   ( RxPackageGlobalData, ptr, RxPackageFunctions, progname, deregfunc )
   RxPackageGlobalDataDef **RxPackageGlobalData;
   PackageTerminator *ptr;
   RexxFunction **RxPackageFunctions;
   char *progname;
   int deregfunc;
#endif
{
   int rc=0;
   RxPackageGlobalDataDef *GlobalData = *RxPackageGlobalData;

   InternalTrace( GlobalData, "TermRxPackage", "\"%s\",%d", progname, deregfunc );

   DEBUGDUMP(fprintf(stderr,"%s-%d: Start of TermRxPackage RxPackageGlobalData is %lx\n",__FILE__,__LINE__,(long)GlobalData);)
   /* 
    * De-register all REXX/SQL functions only 
    * if DEBUG value = 99
    * DO NOT DO THIS FOR DYNAMIC LIBRARY
    * AS IT WILL DEREGISTER FOR ALL PROCESSES
    * NOT JUST THE CURRENT ONE.               
    */
   if (deregfunc)
   {
      if ( ( rc = DeregisterRxFunctions( GlobalData, RxPackageFunctions, 0 ) ) != 0 )
         return (int)FunctionEpilogue( GlobalData, "TermRxPackage", (long)rc );
   }
   /* 
    * Call any package-specific termination code here
    */
   if ( ptr )
   {
      DEBUGDUMP(fprintf(stderr,"%s-%d: In TermRxPackage: Calling package terminator function\n",__FILE__,__LINE__);)
      if ( ( rc = (*ptr)( GlobalData ) ) != 0 )
         return (int)FunctionEpilogue( GlobalData, "TermRxPackage", (long)rc );
   }
#if defined(USE_REXX6000)
   rc = RexxDeregisterSubcom( RDS_ARG0_TYPE)RXPACKAGENAME );
#else
   rc = RexxDeregisterSubcom( (RDS_ARG0_TYPE)RXPACKAGENAME,
                              (RDS_ARG1_TYPE)NULL );
#endif

#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
   RexxDeregisterExit( ( RDE_ARG0_TYPE )RXPACKAGENAME,
                       ( RDE_ARG1_TYPE )NULL );
#endif
   DEBUGDUMP(fprintf(stderr,"%s-%d: In TermRxPackage: RxPackageGlobalData is %lx\n",__FILE__,__LINE__,(long)GlobalData);)
   if ( GlobalData )
   {
      DEBUGDUMP(fprintf(stderr,"%s-%d: In TermRxPackage: deallocate is %d, RxTraceFilePointer is %lx stdin is %lx stderr is %lx\n",__FILE__,__LINE__,GlobalData->deallocate,(long)GlobalData->RxTraceFilePointer,(long)stdin,(long)stderr);)
      if ( GlobalData->RxTraceFilePointer
      &&   GlobalData->RxTraceFilePointer != stdin
      &&   GlobalData->RxTraceFilePointer != stderr )
      {
         fclose( GlobalData->RxTraceFilePointer );
         GlobalData->RxTraceFilePointer = NULL;
      }
   }

   GlobalData->terminated = 1;
   (void)FunctionEpilogue( GlobalData, "TermRxPackage", (long)0 );

   if ( GlobalData
   &&   GlobalData->deallocate )
   {
      free( GlobalData );
      *RxPackageGlobalData = NULL;
   }
   DEBUGDUMP(fprintf(stderr,"%s-%d: End of TermRxPackage with rc = 0\n",__FILE__,__LINE__);)
   return 0;
}


/*-----------------------------------------------------------------------------
 * This function sets the name of thepackage trace file
 *----------------------------------------------------------------------------*/
int RxSetTraceFile

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name )
#else
   ( RxPackageGlobalData, name )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char *name;
#endif

{
   FILE *fp = NULL;

   InternalTrace( RxPackageGlobalData, "RxSetTraceFile", "%s", name );

   if ( strcmp( "stdin", name ) == 0 )
   {
      RxPackageGlobalData->RxTraceFilePointer = stdin;
      strcpy( RxPackageGlobalData->RxTraceFileName, "stdin" );
   }
   else
   {
      if ( strcmp( "stderr", name ) == 0 )
      {
         RxPackageGlobalData->RxTraceFilePointer = stderr;
         strcpy( RxPackageGlobalData->RxTraceFileName, "stderr" );
      }
      else
      {
         if ( RxPackageGlobalData->RxTraceFilePointer != NULL )
            fclose( RxPackageGlobalData->RxTraceFilePointer );
         fp = fopen( name, "w" );
         if ( fp == NULL )
         {
            (void)fprintf( stderr, "ERROR: Could not open trace file: %s for writing\n", name );
            return( 1 );
         }
         RxPackageGlobalData->RxTraceFilePointer = fp;
         strcpy( RxPackageGlobalData->RxTraceFileName, name );
      }
   }
   return( 0 );
}

/*-----------------------------------------------------------------------------
 * This function obtains the name of thepackage trace file and returns it
 *----------------------------------------------------------------------------*/
char *RxGetTraceFile

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData )
#else
   ( RxPackageGlobalData )
   RxPackageGlobalDataDef *RxPackageGlobalData;
#endif

{
   InternalTrace( RxPackageGlobalData, "RxGetTraceFile", NULL );
   return ( RxPackageGlobalData->RxTraceFileName );
}

/*-----------------------------------------------------------------------------
 * This function sets the constants prefix for the package
 *----------------------------------------------------------------------------*/
int RxSetConstantPrefix

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, char *name )
#else
   ( RxPackageGlobalData, name, setvars )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   char *name;
#endif

{
   InternalTrace( RxPackageGlobalData, "RxSetConstantPrefix", "%s", name );

   if ( ( strlen( name ) + 1 ) > sizeof( RxPackageGlobalData->ConstantPrefix ) )
   {
      (void)fprintf( stderr, "ERROR: Constant prefix is too long. It must be <= %ld\n", sizeof( RxPackageGlobalData->ConstantPrefix ) - 1 );
      return( 1 );
   }
   strcpy( RxPackageGlobalData->PreviousConstantPrefix, RxPackageGlobalData->ConstantPrefix );
   strcpy( RxPackageGlobalData->ConstantPrefix, name );
   return( 0 );
}

/*-----------------------------------------------------------------------------
 * This function obtains the constants prefix for the package and returns it
 *----------------------------------------------------------------------------*/
char *RxGetConstantPrefix

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData )
#else
   ( RxPackageGlobalData )
   RxPackageGlobalDataDef *RxPackageGlobalData;
#endif

{
   InternalTrace( RxPackageGlobalData, "RxGetConstantPrefix", NULL );
   return ( RxPackageGlobalData->ConstantPrefix );
}

/*-----------------------------------------------------------------------------
 * This function sets the run-time flags for the package
 *----------------------------------------------------------------------------*/
void RxSetRunFlags

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, int flags )
#else
   ( RxPackageGlobalData, flags )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   int flags;
#endif

{
   InternalTrace( RxPackageGlobalData, "RxSetRunFlags", "%d", flags );
   RxPackageGlobalData->RxRunFlags = flags;
}

/*-----------------------------------------------------------------------------
 * This function obtains the run-time flags for the package and returns them
 *----------------------------------------------------------------------------*/
int RxGetRunFlags

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData )
#else
   ( RxPackageGlobalData )
   RxPackageGlobalDataDef *RxPackageGlobalData;
#endif

{  
   InternalTrace( RxPackageGlobalData, "RxGetRunFlags", NULL );
   return ( RxPackageGlobalData->RxRunFlags );
}

/*-----------------------------------------------------------------------------
 * This function returns the contents of retstr to the Rexx interpreter
 *----------------------------------------------------------------------------*/
int RxReturn

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *retstr )
#else
   ( RxPackageGlobalData, retstr )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *retstr;
#endif
{
   InternalTrace( RxPackageGlobalData, "RxReturn", "%x,%s", retstr, retstr->strptr );

   if ( RxPackageGlobalData
   &&   RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
          "++ Exit %s with value \"%s\"\n",
          RxPackageGlobalData->FName,
          retstr->strptr );
      fflush( RxPackageGlobalData->RxTraceFilePointer );
   }
   return( 0 );
}

/*-----------------------------------------------------------------------------
 * This function takes a signed integer and returns it to the Rexx interpreter
 *----------------------------------------------------------------------------*/
int RxReturnNumber

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *retstr, long num )
#else
   ( RxPackageGlobalData, retstr, num )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *retstr;
   long num;
#endif
{
   InternalTrace( RxPackageGlobalData, "RxReturnNumber", "%x,%ld", retstr, num );

   retstr->strlength = sprintf( (char *)retstr->strptr, "%ld", num );
   if ( RxPackageGlobalData
   &&   RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
         "++ Exit %s with value \"%ld\"\n",
         RxPackageGlobalData->FName,
         num );
      fflush( RxPackageGlobalData->RxTraceFilePointer );
   }
   return( 0 );
}

/*-----------------------------------------------------------------------------
 * This function takes an unsigned integer and returns it to the Rexx interpreter
 *----------------------------------------------------------------------------*/
int RxReturnUnsignedNumber

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *retstr, ULONG num )
#else
   ( RxPackageGlobalData, retstr, num )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *retstr;
   ULONG num;
#endif
{
   InternalTrace( RxPackageGlobalData, "RxReturnUnsignedNumber", "%x,%lu", retstr, num );

   retstr->strlength = sprintf( (char *)retstr->strptr, "%lu", num );
   if ( RxPackageGlobalData
   &&   RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
         "++ Exit %s with value \"%lu\"\n",
         RxPackageGlobalData->FName,
         num );
      fflush( RxPackageGlobalData->RxTraceFilePointer );
   }
   return( 0 );
}

/*-----------------------------------------------------------------------------
 * This function takes a double number and returns it to the Rexx interpreter
 *----------------------------------------------------------------------------*/
int RxReturnDouble

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *retstr, double num )
#else
   ( RxPackageGlobalData, retstr, num )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *retstr;
   long num;
#endif
{
   InternalTrace( RxPackageGlobalData, "RxReturnDouble", "%x,%f", retstr, num );

   retstr->strlength = sprintf( (char *)retstr->strptr, "%f", num );
   if ( RxPackageGlobalData
   &&   RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
         "++ Exit %s with value \"%f\"\n",
         RxPackageGlobalData->FName,
         num );
      fflush( RxPackageGlobalData->RxTraceFilePointer );
   }
   return( 0 );
}


/*-----------------------------------------------------------------------------
 * This function returns a numeric representation of a pointer.
 * If NULL, returns an empty string.
 *----------------------------------------------------------------------------*/
int RxReturnPointer

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *retstr, void *ptr )
#else
   ( RxPackageGlobalData, retstr, ptr )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *retstr;
   void *ptr;
#endif
{
   InternalTrace( RxPackageGlobalData, "RxReturnPointer", "%x,%x", retstr, ptr );

   if ( ptr )
   {
      retstr->strlength = sprintf( (char *)retstr->strptr, "%ld", (long)ptr );
   }
   else
   {
      strcpy( (char *)retstr->strptr, "" );
      retstr->strlength = 0;
   }
   if ( RxPackageGlobalData
   &&   RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
         "++ Exit %s with value \"%s\"\n",
         RxPackageGlobalData->FName,
         retstr->strptr );
      fflush( RxPackageGlobalData->RxTraceFilePointer );
   }
   return( 0 );
}

/*-----------------------------------------------------------------------------
 * This function takes a string and returns it to the Rexx interpreter
 *----------------------------------------------------------------------------*/
int RxReturnString

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *retstr, char *str )
#else
   ( RxPackageGlobalData, retstr, str )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *retstr;
   char *str;
#endif
{
   return( RxReturnStringAndFree( RxPackageGlobalData, retstr, str, 0 ) );
}

/*-----------------------------------------------------------------------------
 * This function takes a copy of a string, frees it, and returns the copy to the Rexx interpreter
 *----------------------------------------------------------------------------*/
int RxReturnStringAndFree

#ifdef HAVE_PROTO
   ( RxPackageGlobalDataDef *RxPackageGlobalData, RXSTRING *retstr, char *str, int freeit )
#else
   ( RxPackageGlobalData, retstr, str )
   RxPackageGlobalDataDef *RxPackageGlobalData;
   RXSTRING *retstr;
   char *str;
#endif
{
   int len = 0;
   char *ret = NULL;

   if ( str )
      len = strlen( str );
   InternalTrace( RxPackageGlobalData, "RxReturnStringAndFree", "%x,\"%s\" Length: %d Free: %d", retstr, str, len, freeit );

   if ( len > RXAUTOBUFLEN )
   {
#if defined(REXXFREEMEMORY)
      ret = (char *)RexxAllocateMemory( len + 1 );
#elif defined(WIN32) && ( defined(USE_WINREXX) || defined(USE_QUERCUS) || defined(USE_OREXX) )
      ret = ( char * )GlobalLock( GlobalAlloc ( GMEM_FIXED, len + 1 ) );
#elif defined(USE_OS2REXX)
      if ( ( BOOL )DosAllocMem( (void **)&ret, len + 1, fPERM|PAG_COMMIT ) )
         ret = ( char * )NULL;
#else
      ret = ( char * )malloc( len + 1 );
#endif
      if ( ret == ( char * )NULL )
      {
         fprintf( stderr, "Unable to allocate %d bytes for return string \"%s\"\n", len, str );
         return( 1 );
      }
      retstr->strptr = (RXSTRING_STRPTR_TYPE)ret;
   }
   memcpy( retstr->strptr, str, len );
   retstr->strlength = len;
   if ( RxPackageGlobalData
   &&   RxPackageGlobalData->RxRunFlags & MODE_VERBOSE )
   {
      (void)fprintf( RxPackageGlobalData->RxTraceFilePointer,
         "++ Exit %s with value \"%s\"\n",
         RxPackageGlobalData->FName,
         str );
      fflush( RxPackageGlobalData->RxTraceFilePointer );
   }
   if ( freeit ) free( str );
   return( 0 );
}

#if !defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
static REH_RETURN_TYPE RxExitHandlerForSayTraceRedirection

#if defined(HAVE_PROTO)
   ( REH_ARG0_TYPE ExitNumber, REH_ARG1_TYPE Subfunction, REH_ARG2_TYPE ParmBlock )
#else
   ( ExitNumber, Subfunction, ParmBlock )
   REH_ARG0_TYPE ExitNumber;    /* code defining the exit function    */
   REH_ARG1_TYPE Subfunction;   /* code defining the exit subfunction */
   REH_ARG2_TYPE ParmBlock;     /* function dependent control block   */
#endif
{
   long i = 0;
   int rc = 0;

   switch( Subfunction )
   {
      case RXSIOSAY:
      {
         RXSIOSAY_PARM *say_parm = (RXSIOSAY_PARM *)ParmBlock;
         if ( say_parm->rxsio_string.strptr != NULL )
         {
            for( i = 0; i < (long)say_parm->rxsio_string.strlength; i++ )
               fputc( ( char )say_parm->rxsio_string.strptr[i], 
                  stdout );
         }
         fputc( '\n', stdout );
         rc = RXEXIT_HANDLED;
         break;
      }
      case RXSIOTRC:
      {
         RXSIOTRC_PARM *trc_parm = (RXSIOTRC_PARM *)ParmBlock;
         if ( trc_parm->rxsio_string.strptr != NULL )
         {
            for( i = 0; i < (long)trc_parm->rxsio_string.strlength; i++ )
               fputc( ( char )trc_parm->rxsio_string.strptr[i], 
                  stderr );
         }
         fputc( '\n', stderr );
         rc = RXEXIT_HANDLED;
         break;
      }
      case RXSIOTRD:
      {
         RXSIOTRD_PARM *trd_parm = (RXSIOTRD_PARM *)ParmBlock;
         int i = 0, ch = 0;
         do
         {
            if ( i < 256 )
               trd_parm->rxsiotrd_retc.strptr[i++] = ch = getc( stdin ) ;
         } while( ch != '\012' && (ch != EOF ) ) ;
         trd_parm->rxsiotrd_retc.strlength = i - 1;
         rc = RXEXIT_HANDLED;
         break;
      }
      case RXSIODTR:
      {
         RXSIODTR_PARM *dtr_parm = (RXSIODTR_PARM *)ParmBlock;
         int i = 0, ch = 0;
         do
         {
            if ( i < 256 )
               dtr_parm->rxsiodtr_retc.strptr[i++] = ch = getc( stdin ) ;
         } while( ch != '\012' && (ch != EOF ) ) ;
         dtr_parm->rxsiodtr_retc.strlength = i - 1;
         rc = RXEXIT_HANDLED;
         break;
      }
      default:
         rc = RXEXIT_NOT_HANDLED;
         break;
   }
   return rc;
}
#endif

/*-----------------------------------------------------------------------------
 * This is the default subcommand handler for the package. It passes all
 * subcommands to the shell for execution.
 *----------------------------------------------------------------------------*/
static RSH_RETURN_TYPE RxSubcomHandler

#if defined(HAVE_PROTO)
   (
   RSH_ARG0_TYPE Command,    /* Command string passed from the caller    */
   RSH_ARG1_TYPE Flags,      /* pointer to short for return of flags     */
   RSH_ARG2_TYPE Retstr)     /* pointer to RXSTRING for RC return        */
#else
   ( Command, Flags, Retstr )
   RSH_ARG0_TYPE Command;    /* Command string passed from the caller    */
   RSH_ARG1_TYPE Flags;      /* pointer to short for return of flags     */
   RSH_ARG2_TYPE Retstr;     /* pointer to RXSTRING for RC return        */
#endif
{
   RSH_RETURN_TYPE rcode=0;
   int rc=0;
   char *buf;

   buf = malloc( Command->strlength + 1 );
   if ( buf == NULL )
   {
      *Flags = RXSUBCOM_ERROR;             /* raise an error condition   */
      sprintf(Retstr->strptr, "%d", RXSUBCOM_NOEMEM);   /* format return code string  */
                                           /* and set the correct length */
      Retstr->strlength = strlen(Retstr->strptr);
   }
   else
   {
      memcpy( buf, Command->strptr, Command->strlength );
      buf[Command->strlength] = '\0';
      rc = system( buf );
      free( buf );
      if (rc < 0)
         *Flags = RXSUBCOM_ERROR;             /* raise an error condition   */
      else
         *Flags = RXSUBCOM_OK;                /* not found is not an error  */
   
      sprintf(Retstr->strptr, "%d", rc); /* format return code string  */
                                              /* and set the correct length */
      Retstr->strlength = strlen(Retstr->strptr);
   }
   return rcode;                              /* processing completed       */
}

/*
 * Both WinRexx and Personal Rexx load and unload this DLL before and
 * after each call to an external function. If this DLL is not resident
 * a new copy of the DLL is loaded together with its data.  What happens
 * then is any data set by one call to an external function is lost
 * and there is no persistence of data.  This code ensures that when the
 * DLL starts, it stays resident, and when the DLL finishes it is
 * unloaded.
 */
#if defined(DYNAMIC_LIBRARY) && (defined(USE_WINREXX) || defined(USE_QUERCUS))
BOOL WINAPI DllMain( HINSTANCE hDLL, DWORD dwReason, LPVOID pReserved)
{
   switch( dwReason)
   {
      case DLL_PROCESS_ATTACH:
         LoadLibrary( RXPACKAGENAME );
         break;
      case DLL_PROCESS_DETACH:
         FreeLibrary( GetModuleHandle( RXPACKAGENAME ) );
         break;
      case DLL_THREAD_ATTACH:
         break;
      case DLL_THREAD_DETACH:
         break;
   }
   return(TRUE);
}
#endif

