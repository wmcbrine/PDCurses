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
 *
 */
#ifndef _RXDEF_H
#define _RXDEF_H

#if defined(__OS2__)
# if defined(USE_REGINA)
#  include <rexxsaa.h>
# elif defined(USE_REXXTRANS)
#  include "rexxtrans.h"
# else
#  if !defined(__EMX__)
#   include <rexxsaa.h>
#  endif
# endif
#endif

#if defined(UNIX)
# if defined(USE_REGINA)
#  include <rexxsaa.h>
# endif
# if defined(USE_REXXIMC)
#  include <rexxsaa.h>
# endif
# if defined(USE_REXX6000)
#  include <rexxsaa.h>
# endif
# if defined(USE_OREXX)
#  include <rexx.h>
# endif
# if defined(USE_REXXTRANS)
#  include <rexxtrans.h>
# endif
# if defined(USE_UNIREXX)
#  include <rexxsaa.h>
#  if defined(HAVE_PROTO) && !defined(ORXXPrototype)
#   define ORXXPrototype
#  endif
# endif
#endif

#if defined(DOS)
# if defined(USE_REGINA)
#  include <rexxsaa.h>
# endif
#endif

#if defined(MSWIN)
# if !defined(NOREXX)
typedef signed short int SHORT;
typedef char CHAR;
typedef CHAR *PCH;
#  include <windows.h>
#  include <wrexx.h>
#  define RXTHE_PSZ        LPCSTR
#  define ULONG            DWORD
#  define PUSHORT          LPWORD
#  define RXTHE_PFN        FARPROC
#  define RXTHE_PUCHAR     LPBYTE
#  define RXTHE_PRXSUBCOM  FARPROC
#  define RXTHE_PRXEXIT    FARPROC
# endif
#endif

#if defined(WIN32)
# if defined(USE_REGINA)
#  include <rexxsaa.h>
# endif
# if defined(USE_OREXX)
#  include <rexx.h>
# endif
# if defined(USE_QUERCUS)
#  include <windows.h>
#  include <wrexx32.h>
# endif
# if defined(USE_WINREXX)
#  include <windows.h>
#  include <rexxapi.h>
# endif
# if defined(USE_UNIREXX)
#  include <windows.h>
#  include <rexxsaa.h>
#  if defined(HAVE_PROTO) && !defined(ORXXPrototype)
#   define ORXXPrototype
#  endif
# endif
# if defined(USE_REXXTRANS)
#  include <rexxtrans.h>
# endif
#endif

#if !defined(RXAUTOBUFLEN)
# define RXAUTOBUFLEN 256L
#endif

/*
 * The following abbreviations are used as prefixes for the following
 * #defines:
 *
 * RFH   - RexxFunctionHandler
 * RRFE  - RexxRegisterFuntionExe
 * RRFD  - RexxRegisterFuntionDll
 * RDF   - RexxDeregisterFuntion
 * RQF   - RexxQueryFuntion
 *
 * REH   - RexxExitHandler
 * RREE  - RexxRegisterExitExe
 * RRED  - RexxRegisterExitDll
 * RDE   - RexxDeregisterExit
 * RQE   - RexxQueryExit
 *
 * RSH   - RexxSubcomHandler
 * RRSE  - RexxRegisterSubcomExe
 * RRSD  - RexxRegisterSubcomDll
 * RDS   - RexxDeregisterSubcom
 *
 * RS    - RexxStart
 */

#if defined(USE_REXX6000)
# define RXSTRING_STRPTR_TYPE PUCHAR  /* PRXFUNC - RexxFunctionHandler */
# define RFH_RETURN_TYPE      USHORT
# define RFH_ARG0_TYPE        PSZ
# define RFH_ARG1_TYPE        ULONG
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        PSZ
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       PSZ
# define RRFE_ARG1_TYPE       PRXFUNC
# define RRFD_ARG0_TYPE       PSZ
# define RRFD_ARG1_TYPE       not_supported
# define RRFD_ARG2_TYPE       PSZ
# define RDF_ARG0_TYPE        PSZ
# define REH_RETURN_TYPE      LONG
# define REH_ARG0_TYPE        LONG
# define REH_ARG1_TYPE        LONG
# define REH_ARG2_TYPE        PEXIT
# define RSH_RETURN_TYPE      LONG /* PRXSUBCOM - RexxSubcomHandler */
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        PUSHORT
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       PSZ
# define RRSE_ARG1_TYPE       PRXSUBCOM
# define RRSE_ARG2_TYPE       PUCHAR
# define RRSD_ARG0_TYPE       ?
# define RRSD_ARG1_TYPE       ?
# define RRSD_ARG2_TYPE       ?
# define RRSD_ARG3_TYPE       ?
# define RRSD_ARG4_TYPE       ?
# define RDS_ARG0_TYPE        PSZ
# define RS_ARG0_TYPE         LONG
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         PSZ
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         PSZ
# define RS_ARG5_TYPE         LONG
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         PLONG
# define RS_ARG8_TYPE         PRXSTRING

#elif defined(USE_OS2REXX)
# if defined(__EMX__)
#  define RXSTRING_STRPTR_TYPE PCH
#  define RFH_RETURN_TYPE      ULONG
#  define RFH_ARG0_TYPE        PCSZ
#  define RFH_ARG1_TYPE        ULONG
#  define RFH_ARG2_TYPE        PRXSTRING
#  define RFH_ARG3_TYPE        PCSZ
#  define RFH_ARG4_TYPE        PRXSTRING
#  define RRFE_ARG0_TYPE       PSZ
#  define RRFE_ARG1_TYPE       RexxFunctionHandler *
#  define RRFD_ARG0_TYPE       PSZ
#  define RRFD_ARG1_TYPE       PSZ
#  define RRFD_ARG2_TYPE       PSZ
#  define RDF_ARG0_TYPE        PSZ
#  define REH_RETURN_TYPE      LONG
#  define REH_ARG0_TYPE        LONG
#  define REH_ARG1_TYPE        LONG
#  define REH_ARG2_TYPE        PEXIT
#  define RREE_ARG0_TYPE       PSZ
#  define RREE_ARG1_TYPE       PFN
#  define RREE_ARG2_TYPE       PUCHAR
#  define RRED_ARG0_TYPE       ?
#  define RRED_ARG1_TYPE       ?
#  define RRED_ARG2_TYPE       ?
#  define RRED_ARG3_TYPE       ?
#  define RRED_ARG4_TYPE       ?
#  define RSH_RETURN_TYPE      ULONG
#  define RSH_ARG0_TYPE        PRXSTRING
#  define RSH_ARG1_TYPE        PUSHORT
#  define RSH_ARG2_TYPE        PRXSTRING
#  define RRSE_ARG0_TYPE       PSZ
#  define RRSE_ARG1_TYPE       PFN
#  define RRSE_ARG2_TYPE       PUCHAR
#  define RRSD_ARG0_TYPE       ?
#  define RRSD_ARG1_TYPE       ?
#  define RRSD_ARG2_TYPE       ?
#  define RRSD_ARG3_TYPE       ?
#  define RRSD_ARG4_TYPE       ?
#  define RDE_ARG0_TYPE        PSZ
#  define RDE_ARG1_TYPE        PSZ
#  define RDS_ARG0_TYPE        PSZ
#  define RDS_ARG1_TYPE        PSZ
#  define RS_ARG0_TYPE         LONG
#  define RS_ARG1_TYPE         PRXSTRING
#  define RS_ARG2_TYPE         PSZ
#  define RS_ARG3_TYPE         PRXSTRING
#  define RS_ARG4_TYPE         PSZ
#  define RS_ARG5_TYPE         LONG
#  define RS_ARG6_TYPE         PRXSYSEXIT
#  define RS_ARG7_TYPE         PSHORT
#  define RS_ARG8_TYPE         PRXSTRING
# else
#  define RXSTRING_STRPTR_TYPE PCH
#  define RFH_RETURN_TYPE      ULONG
#  define RFH_ARG0_TYPE        PUCHAR
#  define RFH_ARG1_TYPE        ULONG
#  define RFH_ARG2_TYPE        PRXSTRING
#  define RFH_ARG3_TYPE        PSZ
#  define RFH_ARG4_TYPE        PRXSTRING
#  define RRFE_ARG0_TYPE       PSZ
#  define RRFE_ARG1_TYPE       PFN
#  define RRFD_ARG0_TYPE       PSZ
#  define RRFD_ARG1_TYPE       PSZ
#  define RRFD_ARG2_TYPE       PSZ
#  define RDF_ARG0_TYPE        PSZ
#  define REH_RETURN_TYPE      LONG
#  define REH_ARG0_TYPE        LONG
#  define REH_ARG1_TYPE        LONG
#  define REH_ARG2_TYPE        PEXIT
#  define RREE_ARG0_TYPE       PSZ
#  define RREE_ARG1_TYPE       PFN
#  define RREE_ARG2_TYPE       PUCHAR
#  define RRED_ARG0_TYPE       ?
#  define RRED_ARG1_TYPE       ?
#  define RRED_ARG2_TYPE       ?
#  define RRED_ARG3_TYPE       ?
#  define RRED_ARG4_TYPE       ?
#  define RSH_RETURN_TYPE      ULONG
#  define RSH_ARG0_TYPE        PRXSTRING
#  define RSH_ARG1_TYPE        PUSHORT
#  define RSH_ARG2_TYPE        PRXSTRING
#  define RRSE_ARG0_TYPE       PSZ
#  define RRSE_ARG1_TYPE       PFN
#  define RRSE_ARG2_TYPE       PUCHAR
#  define RRSD_ARG0_TYPE       ?
#  define RRSD_ARG1_TYPE       ?
#  define RRSD_ARG2_TYPE       ?
#  define RRSD_ARG3_TYPE       ?
#  define RRSD_ARG4_TYPE       ?
#  define RDE_ARG0_TYPE        PSZ
#  define RDE_ARG1_TYPE        PSZ
#  define RDS_ARG0_TYPE        PSZ
#  define RDS_ARG1_TYPE        PSZ
#  define RS_ARG0_TYPE         LONG
#  define RS_ARG1_TYPE         PRXSTRING
#  define RS_ARG2_TYPE         PSZ
#  define RS_ARG3_TYPE         PRXSTRING
#  define RS_ARG4_TYPE         PSZ
#  define RS_ARG5_TYPE         LONG
#  define RS_ARG6_TYPE         PRXSYSEXIT
#  define RS_ARG7_TYPE         PSHORT
#  define RS_ARG8_TYPE         PRXSTRING
# endif

#elif defined(USE_REGINA)
/*
 * REGINAVERSION introduced in 2.0, at the same time that
 * PCSZ was introduced.
 */
# ifndef REGINAVERSION
#  define PCSZ PSZ
# endif
# define RXSTRING_STRPTR_TYPE char *
# define RFH_RETURN_TYPE      APIRET APIENTRY
# ifndef REGINAVERSION
#  define RFH_ARG0_TYPE       PUCHAR
# else
#  define RFH_ARG0_TYPE       PCSZ
# endif
# define RFH_ARG1_TYPE        ULONG
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        PCSZ
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       PCSZ
# define RRFE_ARG1_TYPE       PFN
# define RRFD_ARG0_TYPE       PCSZ
# define RRFD_ARG1_TYPE       PCSZ
# define RRFD_ARG2_TYPE       PCSZ
# define RDF_ARG0_TYPE        PCSZ
# define RQF_ARG0_TYPE        PCSZ
# define REH_RETURN_TYPE      LONG APIENTRY
# define REH_ARG0_TYPE        LONG
# define REH_ARG1_TYPE        LONG
# define REH_ARG2_TYPE        PEXIT
# define RREE_ARG0_TYPE       PCSZ
# define RREE_ARG1_TYPE       PFN
# define RREE_ARG2_TYPE       PUCHAR
# define RRED_ARG0_TYPE       PCSZ
# define RRED_ARG1_TYPE       PCSZ
# define RRED_ARG2_TYPE       PCSZ
# define RRED_ARG3_TYPE       PUCHAR
# define RRED_ARG4_TYPE       ULONG
# define RSH_RETURN_TYPE      APIRET APIENTRY
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        PUSHORT
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       PCSZ
# define RRSE_ARG1_TYPE       PFN
# define RRSE_ARG2_TYPE       PUCHAR
# define RRSD_ARG0_TYPE       PCSZ
# define RRSD_ARG1_TYPE       PCSZ
# define RRSD_ARG2_TYPE       PCSZ
# define RRSD_ARG3_TYPE       PUCHAR
# define RRSD_ARG4_TYPE       ULONG
# define RDE_ARG0_TYPE        PCSZ
# define RDE_ARG1_TYPE        PCSZ
# define RQE_ARG0_TYPE        PCSZ
# define RQE_ARG1_TYPE        PCSZ
# define RQE_ARG2_TYPE        PUSHORT
# define RQE_ARG3_TYPE        PUCHAR
# define RDS_ARG0_TYPE        PCSZ
# define RDS_ARG1_TYPE        PCSZ
# define RS_ARG0_TYPE         LONG
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         PCSZ
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         PCSZ
# define RS_ARG5_TYPE         LONG
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         PSHORT
# define RS_ARG8_TYPE         PRXSTRING

#elif defined(USE_REXXIMC)
# define RXSTRING_STRPTR_TYPE char *
# define RFH_RETURN_TYPE      ULONG
# define RFH_ARG0_TYPE        PSZ
# define RFH_ARG1_TYPE        ULONG
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        PSZ
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       PSZ
# define RRFE_ARG1_TYPE       RexxFunctionHandler *
# define RRFD_ARG0_TYPE       PSZ
# define RRFD_ARG1_TYPE       not_supported
# define RRFD_ARG2_TYPE       PSZ
# define RDF_ARG0_TYPE        PSZ
# define REH_RETURN_TYPE      LONG
# define REH_ARG0_TYPE        LONG
# define REH_ARG1_TYPE        LONG
# define REH_ARG2_TYPE        PEXIT
# define RREE_ARG0_TYPE       PSZ
# define RREE_ARG1_TYPE       RexxExitHandler *
# define RREE_ARG2_TYPE       PUCHAR
# define RRED_ARG0_TYPE       not_supported
# define RRED_ARG1_TYPE       not_supported
# define RRED_ARG2_TYPE       not_supported
# define RRED_ARG3_TYPE       not_supported
# define RRED_ARG4_TYPE       not_supported
# define RSH_RETURN_TYPE      ULONG
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        PUSHORT
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       PSZ
# define RRSE_ARG1_TYPE       RexxSubcomHandler *
# define RRSE_ARG2_TYPE       PUCHAR
# define RRSD_ARG0_TYPE       not_supported
# define RRSD_ARG1_TYPE       not_supported
# define RRSD_ARG2_TYPE       not_supported
# define RRSD_ARG3_TYPE       not_supported
# define RRSD_ARG4_TYPE       not_supported
# define RDE_ARG0_TYPE        PSZ
# define RDE_ARG1_TYPE        PSZ
# define RDS_ARG0_TYPE        PSZ
# define RDS_ARG1_TYPE        PSZ
# define RS_ARG0_TYPE         long
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         char *
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         PSZ
# define RS_ARG5_TYPE         long
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         short *
# define RS_ARG8_TYPE         PRXSTRING

#elif defined(USE_OREXX)
# define RXSTRING_STRPTR_TYPE PCH
# define RFH_RETURN_TYPE      ULONG APIENTRY
# define RFH_ARG0_TYPE        PUCHAR
# define RFH_ARG1_TYPE        ULONG
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        PSZ
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       PSZ
# define RRFE_ARG1_TYPE       PFN
# define RRFD_ARG0_TYPE       PSZ
# define RRFD_ARG1_TYPE       PSZ
# define RRFD_ARG2_TYPE       PSZ
# define RDF_ARG0_TYPE        PSZ
# define REH_RETURN_TYPE      LONG APIENTRY
# define REH_ARG0_TYPE        LONG
# define REH_ARG1_TYPE        LONG
# define REH_ARG2_TYPE        PEXIT
# define RREE_ARG0_TYPE       PSZ
# define RREE_ARG1_TYPE       PFN
# define RREE_ARG2_TYPE       PUCHAR
# define RRED_ARG0_TYPE       PSZ
# define RRED_ARG1_TYPE       PSZ
# define RRED_ARG2_TYPE       PSZ
# define RRED_ARG3_TYPE       PUCHAR
# define RRED_ARG4_TYPE       ULONG
# define RSH_RETURN_TYPE      APIRET APIENTRY
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        PUSHORT
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       PSZ
# define RRSE_ARG1_TYPE       PFN
# define RRSE_ARG2_TYPE       PUCHAR
# define RRSD_ARG0_TYPE       PSZ
# define RRSD_ARG1_TYPE       PSZ
# define RRSD_ARG2_TYPE       PSZ
# define RRSD_ARG3_TYPE       PUCHAR
# define RRSD_ARG4_TYPE       ULONG
# define RDE_ARG0_TYPE        PSZ
# define RDE_ARG1_TYPE        PSZ
# define RDS_ARG0_TYPE        PSZ
# define RDS_ARG1_TYPE        PSZ
# define RS_ARG0_TYPE         LONG
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         PSZ
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         PSZ
# define RS_ARG5_TYPE         LONG
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         PSHORT
# define RS_ARG8_TYPE         PRXSTRING

#elif defined(USE_QUERCUS)
# define RXSTRING_STRPTR_TYPE LPBYTE
# define RFH_RETURN_TYPE      DWORD APIENTRY
# define RFH_ARG0_TYPE        LPCSTR
# define RFH_ARG1_TYPE        DWORD
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        LPCSTR
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       LPCSTR
# define RRFE_ARG1_TYPE       FARPROC
# define RRFD_ARG0_TYPE       LPCSTR
# define RRFD_ARG1_TYPE       LPCSTR
# define RRFD_ARG2_TYPE       LPCSTR
# define RDF_ARG0_TYPE        LPCSTR
# define REH_RETURN_TYPE      LONG
# define REH_ARG0_TYPE        LONG
# define REH_ARG1_TYPE        LONG
# define REH_ARG2_TYPE        PEXIT
# define RREE_ARG0_TYPE       LPCSTR
# define RREE_ARG1_TYPE       FARPROC
# define RREE_ARG2_TYPE       LPBYTE
# define RRED_ARG0_TYPE       LPCSTR
# define RRED_ARG1_TYPE       LPCSTR
# define RRED_ARG2_TYPE       LPCSTR
# define RRED_ARG3_TYPE       LPBYTE
# define RRED_ARG4_TYPE       UINT
# define RDE_ARG0_TYPE        LPCSTR
# define RDE_ARG1_TYPE        LPCSTR
# define RSH_RETURN_TYPE      DWORD
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        LPWORD
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       LPCSTR
# define RRSE_ARG1_TYPE       FARPROC
# define RRSE_ARG2_TYPE       LPBYTE
# define RRSD_ARG0_TYPE       LPCSTR
# define RRSD_ARG1_TYPE       LPCSTR
# define RRSD_ARG2_TYPE       LPCSTR
# define RRSD_ARG3_TYPE       LPBYTE
# define RRSD_ARG4_TYPE       UINT
# define RDS_ARG0_TYPE        LPCSTR
# define RDS_ARG1_TYPE        LPCSTR
# define RS_ARG0_TYPE         int
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         LPCSTR
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         LPCSTR
# define RS_ARG5_TYPE         int
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         LPSHORT
# define RS_ARG8_TYPE         PRXSTRING

#elif defined(USE_WINREXX)
# define RXSTRING_STRPTR_TYPE LPBYTE
# define RFH_RETURN_TYPE      int APIENTRY
# define RFH_ARG0_TYPE        LPCSTR
# define RFH_ARG1_TYPE        UINT
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        LPCSTR
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       LPCSTR
# define RRFE_ARG1_TYPE       FARPROC
# define RRFD_ARG0_TYPE       LPCSTR
# define RRFD_ARG1_TYPE       LPCSTR
# define RRFD_ARG2_TYPE       LPCSTR
# define RDF_ARG0_TYPE        LPCSTR
# define REH_RETURN_TYPE      int APIENTRY
# define REH_ARG0_TYPE        int
# define REH_ARG1_TYPE        int
# define REH_ARG2_TYPE        PEXIT
# define RREE_ARG0_TYPE       LPCSTR
# define RREE_ARG1_TYPE       FARPROC
# define RREE_ARG2_TYPE       LPBYTE
# define RRED_ARG0_TYPE       LPCSTR
# define RRED_ARG1_TYPE       LPCSTR
# define RRED_ARG2_TYPE       LPCSTR
# define RRED_ARG3_TYPE       LPBYTE
# define RRED_ARG4_TYPE       UINT
# define RDE_ARG0_TYPE        LPCSTR
# define RDE_ARG1_TYPE        LPCSTR
# define RSH_RETURN_TYPE      int APIENTRY
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        LPWORD
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       LPCSTR
# define RRSE_ARG1_TYPE       FARPROC
# define RRSE_ARG2_TYPE       LPBYTE
# define RRSD_ARG0_TYPE       LPCSTR
# define RRSD_ARG1_TYPE       LPCSTR
# define RRSD_ARG2_TYPE       LPCSTR
# define RRSD_ARG3_TYPE       LPBYTE
# define RRSD_ARG4_TYPE       UINT
# define RDS_ARG0_TYPE        LPCSTR
# define RDS_ARG1_TYPE        LPCSTR
# define RS_ARG0_TYPE         int
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         LPCSTR
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         LPCSTR
# define RS_ARG5_TYPE         int
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         LPSHORT
# define RS_ARG8_TYPE         PRXSTRING

#elif defined(USE_UNIREXX)
# define RXSTRING_STRPTR_TYPE PCH
#ifdef WIN32
#  define RFH_RETURN_TYPE     ULONG ORXXCDecl ORXXLoadDS
# else
#  define RFH_RETURN_TYPE     ULONG
#endif
# define RFH_ARG0_TYPE        PUCHAR
# define RFH_ARG1_TYPE        ULONG
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        PSZ
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       PSZ
# define RRFE_ARG1_TYPE       PFN
# define RRFD_ARG0_TYPE       PSZ
# define RRFD_ARG1_TYPE       PSZ
# define RRFD_ARG2_TYPE       PSZ
# define RDF_ARG0_TYPE        PSZ
# ifdef WIN32
#  define REH_RETURN_TYPE     LONG ORXXCDecl ORXXLoadDS
# else
#  define REH_RETURN_TYPE     LONG
#endif
# define REH_ARG0_TYPE        LONG
# define REH_ARG1_TYPE        LONG
# define REH_ARG2_TYPE        PEXIT
# define RREE_ARG0_TYPE       PSZ
# define RREE_ARG1_TYPE       PFN
# define RREE_ARG2_TYPE       PUCHAR
# define RRED_ARG0_TYPE       PSZ
# define RRED_ARG1_TYPE       PSZ
# define RRED_ARG2_TYPE       PSZ
# define RRED_ARG3_TYPE       PUCHAR
# define RRED_ARG4_TYPE       ULONG
# ifdef WIN32
#  define RSH_RETURN_TYPE     ULONG ORXXCDecl ORXXLoadDS
# else
#  define RSH_RETURN_TYPE     ULONG
#endif
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        PUSHORT
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       PSZ
# define RRSE_ARG1_TYPE       PFN
# define RRSE_ARG2_TYPE       PUCHAR
# define RRSD_ARG0_TYPE       PSZ
# define RRSD_ARG1_TYPE       PSZ
# define RRSD_ARG2_TYPE       PSZ
# define RRSD_ARG3_TYPE       PUCHAR
# define RRSD_ARG4_TYPE       ULONG
# define RDE_ARG0_TYPE        PSZ
# define RDE_ARG1_TYPE        PSZ
# define RDS_ARG0_TYPE        PSZ
# define RDS_ARG1_TYPE        PSZ
# define RS_ARG0_TYPE         LONG
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         PSZ
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         PSZ
# define RS_ARG5_TYPE         LONG
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         PSHORT
# define RS_ARG8_TYPE         PRXSTRING

#elif defined(USE_REXXTRANS)
# define RXSTRING_STRPTR_TYPE char *
# define RFH_RETURN_TYPE      APIRET APIENTRY
# define RFH_ARG0_TYPE        PUCHAR
# define RFH_ARG1_TYPE        ULONG
# define RFH_ARG2_TYPE        PRXSTRING
# define RFH_ARG3_TYPE        PSZ
# define RFH_ARG4_TYPE        PRXSTRING
# define RRFE_ARG0_TYPE       PSZ
# define RRFE_ARG1_TYPE       PFN
# define RRFD_ARG0_TYPE       PSZ
# define RRFD_ARG1_TYPE       PSZ
# define RRFD_ARG2_TYPE       PSZ
# define RDF_ARG0_TYPE        PSZ
# define REH_RETURN_TYPE      LONG APIENTRY
# define REH_ARG0_TYPE        LONG
# define REH_ARG1_TYPE        LONG
# define REH_ARG2_TYPE        PEXIT
# define RREE_ARG0_TYPE       PSZ
# define RREE_ARG1_TYPE       PFN
# define RREE_ARG2_TYPE       PUCHAR
# define RRED_ARG0_TYPE       PSZ
# define RRED_ARG1_TYPE       PSZ
# define RRED_ARG2_TYPE       PSZ
# define RRED_ARG3_TYPE       PUCHAR
# define RRED_ARG4_TYPE       ULONG
# define RSH_RETURN_TYPE      APIRET APIENTRY
# define RSH_ARG0_TYPE        PRXSTRING
# define RSH_ARG1_TYPE        PUSHORT
# define RSH_ARG2_TYPE        PRXSTRING
# define RRSE_ARG0_TYPE       PSZ
# define RRSE_ARG1_TYPE       PFN
# define RRSE_ARG2_TYPE       PUCHAR
# define RRSD_ARG0_TYPE       PSZ
# define RRSD_ARG1_TYPE       PSZ
# define RRSD_ARG2_TYPE       PSZ
# define RRSD_ARG3_TYPE       PUCHAR
# define RRSD_ARG4_TYPE       ULONG
# define RDE_ARG0_TYPE        PSZ
# define RDE_ARG1_TYPE        PSZ
# define RDS_ARG0_TYPE        PSZ
# define RDS_ARG1_TYPE        PSZ
# define RS_ARG0_TYPE         LONG
# define RS_ARG1_TYPE         PRXSTRING
# define RS_ARG2_TYPE         PSZ
# define RS_ARG3_TYPE         PRXSTRING
# define RS_ARG4_TYPE         PSZ
# define RS_ARG5_TYPE         LONG
# define RS_ARG6_TYPE         PRXSYSEXIT
# define RS_ARG7_TYPE         PSHORT
# define RS_ARG8_TYPE         PRXSTRING

#endif

#endif /* !_RXDEF_H */
