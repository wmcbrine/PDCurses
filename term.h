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

/* $Id: term.h,v 1.5 2006/02/08 17:40:32 wmcbrine Exp $ */

/* PDCurses doesn't operate with terminfo, but we need these functions for 
   compatibility, to allow some things (notably, interface libraries for 
   other languages) to be compiled. Anyone who tries to actually _use_ 
   them will be disappointed, since they only return ERR. */

#include <curses.h>

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
extern "C"
{
#endif

#ifndef __PDCURSES_TERM_H__
#define __PDCURSES_TERM_H__ 1

typedef void TERMINAL;

#if defined(PDC_DLL_BUILD)
# if !defined(CURSES_LIBRARY)
__declspec(dllimport)  TERMINAL *cur_term;
# else
__declspec(dllexport) extern TERMINAL *cur_term;
# endif
#else
extern TERMINAL *cur_term;
#endif

int	del_curterm(TERMINAL *);
int	putp(const char *);
int	restartterm(char *, int, int *);
TERMINAL *set_curterm(TERMINAL *);
int	setterm(char *);
int	setupterm(char *, int, int *);
int	tgetent(char *, const char *);
int	tgetflag(char *);
int	tgetnum(char *);
char   *tgetstr(char *, char **);
char   *tgoto(char *, int, int);
int	tigetflag(char *);
int	tigetnum(char *);
char   *tigetstr(char *);
char   *tparm(char *,long, long, long, long, long, 
				long, long, long, long);
int	tputs(const char *, int, int (*)(int));

#endif /* __PDCURSES_TERM_H__ */

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
}
#endif
