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
#define CURSES_LIBRARY  1
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCgo32  = "$Id: pdcgo32.c,v 1.2 2001/01/10 08:28:41 mark Exp $";
#endif



#include <pc.h>    
#include <sys/movedata.h> /* prototypes of dosmemget() and dosmemput() */

unsigned char getdosmembyte (int offset)
{
 unsigned char b=0;

  dosmemget (offset, sizeof(unsigned char), &b);
  return b;
}

unsigned short getdosmemword (int offset)
{
  unsigned short w=0;

  dosmemget (offset, sizeof(unsigned short), &w);
  return w;
}

void setdosmembyte (int offset, unsigned char b)
{
  dosmemput (&b, sizeof(unsigned char), offset);
}

void setdosmemword (int offset, unsigned short w)
{
  dosmemput (&w, sizeof(unsigned short), offset);
}
