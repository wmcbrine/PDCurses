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
/********************************* tuidemo.c ********************************/
/*
 * File   : tuidemo.c
 * Author : P.J. Kunst  (kunst@prl.philips.nl)
 * Date   : 25-02-93
 * Version: 1.02
 *
 * Purpose: This program demonstrates the use of the 'curses' library
 *          for the creation of (simple) menu-operated programs.
 *          In the PD-Curses version, use is made of colors for the
 *          highlighting of subwindows (title bar, status bar etc).
 *          The program was tested using DJGPP 1.09 ('GO32') and
 *          Turbo C (2.0), as well as on a UNIX machine (HP-UX 8.07).
 *
 * Acknowledgement: some ideas were borrowed from Mark Hessling's
 *                  version of the 'testcurs' program.
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tui.h"

#ifdef PDCDEBUG
char *rcsid_tuidemo  = "$Id: tuidemo.c,v 1.1 2001/01/10 08:27:46 mark Exp $";
#endif

#if defined(XCURSES)
	char *XCursesProgramName = "tuidemo";
#  define FNAME   "demos/tui.c"    /* change this if source at other location */
#else
#  define FNAME   "demos\\tui.c"    /* change this if source at other location */
#endif



/**************************** strings entry box ***************************/

#ifdef __STDC__
void address (void)
#else
void address ()
#endif
{
#ifdef __STDC__
  char *fieldname[6] = {"Name","Street","City","State","Country",(char*)0};
#else
  char *fieldname[6];
#endif
  char *fieldbuf[5];
  WINDOW *wbody = bodywin();
  int i, field = 50;

#ifndef __STDC__
  fieldname[0] = "Name";
  fieldname[1] = "Street";
  fieldname[2] = "City";
  fieldname[3] = "State";
  fieldname[4] = "Country";
  fieldname[5] = (char*)0;
#endif
  for (i=0; i<5; i++) fieldbuf[i] = (char *) calloc (field+1, sizeof(char));
  if (getstrings (fieldname, fieldbuf, field) != KEY_ESC)
  {
    for (i=0; fieldname[i]; i++)
      wprintw (wbody, "%10s : %s\n", fieldname[i], fieldbuf[i]);
    wrefresh (wbody);
  }
  for (i=0; i<5; i++) free (fieldbuf[i]);
}

/**************************** string entry box ****************************/

#ifdef __STDC__
char *getfname (char *desc, char *fname, int field)
#else
char *getfname (desc, fname, field)
char *desc;
char *fname;
int field;
#endif
{
  char *fieldname[2];
  char *fieldbuf[1];

  fieldname[0] = desc; fieldname[1] = 0; fieldbuf[0] = fname;
  return (getstrings (fieldname, fieldbuf, field) == KEY_ESC) ? NULL : fname;
}

/**************************** a very simple file browser ******************/

#ifdef __STDC__
void showfile (char *fname)
#else
void showfile (fname)
char *fname;
#endif
{
  int i, bh = bodylen();
  FILE *fp;
  char buf[MAXSTRLEN];
  bool ateof = FALSE;

  statusmsg ("FileBrowser: Hit key to continue, Q to quit");

  if ((fp = fopen (fname, "r")) != NULL)   /* file available ? */
  {
    while (!ateof)
    {
      clsbody ();
      for (i=0; i<bh-1 && !ateof; i++)
      {
        buf[0] = '\0';
        fgets (buf, MAXSTRLEN, fp);
        if (strlen(buf)) bodymsg (buf); else ateof = TRUE;
      }
      switch (waitforkey())
      {
        case 'Q':
        case 'q':
        case 0x1b: /* ESCAPE */
          ateof = TRUE;
          break;

        default:
          break;
      }
    }
    fclose (fp);
  }
  else
  {
    sprintf (buf, "ERROR: file '%s' not found", fname);
    errormsg (buf);
  }
}

/***************************** forward declarations ***********************/

void sub0(), sub1(), sub2(), sub3();
void func1(), func2();
void subfunc1(), subfunc2();
void subsub();

/***************************** menus initialization ***********************/

menu MainMenu[] =
{
 { "Asub",     sub0,     "Go inside first submenu" },
 { "Bsub",     sub1,     "Go inside second submenu" },
 { "Csub",     sub2,     "Go inside third submenu" },
 { "Dsub",     sub3,     "Go inside fourth submenu" },
 { "",         (FUNC)0,  "" }   /* always add this as the last item ! */
};

menu SubMenu0[] =
{
 { "Exit",     DoExit,     "Terminate program" },
 { "",         (FUNC)0,  "" }   /* always add this as the last item ! */
};

menu SubMenu1[] =
{
 { "OneBeep",  func1,    "Sound one beep" },
 { "TwoBeeps", func2,    "Sound two beeps" },
 { "",         (FUNC)0,  "" }   /* always add this as the last item ! */
};

menu SubMenu2[] =
{
 { "Browse",   subfunc1, "Source file lister" },
 { "Input",    subfunc2, "Interactive file lister" },
 { "Address",  address,  "Get address data" },
 { "",         (FUNC)0,  "" }   /* always add this as the last item ! */
};

menu SubMenu3[] =
{
 { "SubSub",   subsub,   "Go inside sub-submenu" },
 { "",         (FUNC)0,  "" }   /* always add this as the last item ! */
};

/***************************** main menu functions ************************/

#ifdef __STDC__
void sub0 (void)
#else
void sub0 ()
#endif
{
  domenu (SubMenu0);
}

#ifdef __STDC__
void sub1 (void)
#else
void sub1 ()
#endif
{
  domenu (SubMenu1);
}

#ifdef __STDC__
void sub2 (void)
#else
void sub2 ()
#endif
{
  domenu (SubMenu2);
}

#ifdef __STDC__
void sub3 (void)
#else
void sub3 ()
#endif
{
  domenu (SubMenu3);
}

/***************************** submenu1 functions *************************/

#ifdef __STDC__
void func1 (void)
#else
void func1 ()
#endif
{
  beep ();
  bodymsg ("One beep! ");
}

#ifdef __STDC__
void func2 (void)
#else
void func2 ()
#endif
{
  beep ();
  bodymsg ("Two beeps! ");
  beep ();
}

/***************************** submenu2 functions *************************/

#ifdef __STDC__
void subfunc1 (void)
#else
void subfunc1 ()
#endif
{
  showfile (FNAME);
}

#ifdef __STDC__
void subfunc2 (void)
#else
void subfunc2 ()
#endif
{
  char fname[MAXSTRLEN];

  strcpy (fname, FNAME);
  if (getfname ("File to browse:", fname, 50)) showfile (fname);
}

/***************************** submenu3 functions *************************/

#ifdef __STDC__
void subsub (void)
#else
void subsub ()
#endif
{
  domenu (SubMenu2);
}

/***************************** start main menu  ***************************/

int main ()
{
  startmenu (MainMenu, "TUI - 'textual user interface' demonstration program");

  return 0;
}
/********************************* tuidemo.c ********************************/
