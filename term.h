/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

/* $Id: term.h,v 1.11 2006/03/26 01:48:46 wmcbrine Exp $ */

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

typedef struct
{
	const char *_termname;
} TERMINAL;

#ifdef PDC_DLL_BUILD
# ifndef CURSES_LIBRARY
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
