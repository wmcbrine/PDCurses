################################################################################
#
# Watcom WMAKE Makefile for PDCurses library - Win32 Watcom C/C++ 10.6+
#
# Usage: wmake -f [win32\]wccwin32.mak [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|panel.lib|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, or edit the line
# below; for example, "set PDCURSES_SRCDIR=c:\pdcurses".
#
################################################################################
PDCURSES_HOME		=$(%PDCURSES_SRCDIR)
################################################################################
# Nothing below here should require changing.
################################################################################
VER=27
VERDOT=2.7

PDCURSES_CURSES_H	=$(PDCURSES_HOME)\curses.h
PDCURSES_CURSPRIV_H	=$(PDCURSES_HOME)\curspriv.h
PDCURSES_HEADERS	=$(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		=$(PDCURSES_HOME)\panel.h
TERM_HEADER		=$(PDCURSES_HOME)\term.h

srcdir		= $(PDCURSES_HOME)\pdcurses
osdir		= $(PDCURSES_HOME)\win32
pandir		= $(PDCURSES_HOME)\panel
demodir		= $(PDCURSES_HOME)\demos

CC		= wcc386
TARGET=nt

!ifeq DEBUG Y
CFLAGS  = /d2 /DPDCDEBUG
LDFLAGS = DEBUG WATCOM ALL
!else
CFLAGS  = /oneatx /wcd=302
LDFLAGS =
!endif

CPPFLAGS	= /i=$(PDCURSES_HOME)

CCFLAGS		= /ei /zq /mf /wx $(CFLAGS) $(CPPFLAGS)

LINK		= wlink

LIBEXE		= wlib /q /n /t

LIBCURSES	= pdcurses.lib
LIBPANEL	= panel.lib

PDCLIBS	= $(LIBCURSES) $(LIBPANEL)
DEMOS	=testcurs.exe newdemo.exe xmas.exe tuidemo.exe firework.exe ptest.exe

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean
	-del *.obj
	-del *.lib
	-del *.exe
	-del *.err

demos:	$(DEMOS)

#------------------------------------------------------------------------

LIBOBJS =    &
addch.obj    &
addchstr.obj &
addstr.obj   &
attr.obj     &
beep.obj     &
bkgd.obj     &
border.obj   &
clear.obj    &
color.obj    &
delch.obj    &
deleteln.obj &
getch.obj    &
getstr.obj   &
getyx.obj    &
inch.obj     &
inchstr.obj  &
initscr.obj  &
inopts.obj   &
insch.obj    &
insstr.obj   &
instr.obj    &
kernel.obj   &
mouse.obj    &
move.obj     &
outopts.obj  &
overlay.obj  &
pad.obj      &
printw.obj   &
refresh.obj  &
scanw.obj    &
scr_dump.obj &
scroll.obj   &
slk.obj      &
termattr.obj &
terminfo.obj &
touch.obj    &
util.obj     &
window.obj

PDCOBJS =      &
pdcclip.obj    &
pdcdebug.obj   &
pdcdisp.obj    &
pdcgetsc.obj   &
pdckbd.obj     &
pdcprint.obj   &
pdcscrn.obj    &
pdcsetsc.obj   &
pdcutil.obj    &
pdcwin.obj

PANOBJS =     &
panel.obj

pdcurses.lib : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)

panel.lib : $(PANOBJS)
	echo +$(PANOBJS)   >  lib.rsp
	$(LIBEXE) $@ @lib.rsp
	del lib.rsp

addch.obj: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\addch.c

addchstr.obj: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\addchstr.c

addstr.obj: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\addstr.c

attr.obj: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\attr.c

beep.obj: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\beep.c

bkgd.obj: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\bkgd.c

border.obj: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\border.c

clear.obj: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\clear.c

color.obj: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\color.c

delch.obj: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\delch.c

deleteln.obj: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\deleteln.c

getch.obj: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\getch.c

getstr.obj: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\getstr.c

getyx.obj: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\getyx.c

inch.obj: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\inch.c

inchstr.obj: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\inchstr.c

initscr.obj: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\initscr.c

inopts.obj: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\inopts.c

insch.obj: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\insch.c

insstr.obj: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\insstr.c

instr.obj: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\instr.c

kernel.obj: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\kernel.c

mouse.obj: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\mouse.c

move.obj: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\move.c

outopts.obj: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\outopts.c

overlay.obj: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\overlay.c

pad.obj: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\pad.c

printw.obj: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\printw.c

refresh.obj: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\refresh.c

scanw.obj: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\scanw.c

scr_dump.obj: $(srcdir)\scr_dump.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\scr_dump.c

scroll.obj: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\scroll.c

slk.obj: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\slk.c

termattr.obj: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\termattr.c

terminfo.obj: $(srcdir)\terminfo.c $(PDCURSES_HEADERS) $(TERM_HEADER)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\terminfo.c

touch.obj: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\touch.c

util.obj: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\util.c

window.obj: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\window.c


pdcclip.obj: $(osdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(osdir)\pdcclip.c

pdcdebug.obj: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\pdcdebug.c

pdcdisp.obj: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(osdir)\pdcdisp.c

pdcgetsc.obj: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(osdir)\pdcgetsc.c

pdckbd.obj: $(osdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(osdir)\pdckbd.c

pdcprint.obj: $(osdir)\pdcprint.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(osdir)\pdcprint.c

pdcscrn.obj: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(osdir)\pdcscrn.c

pdcsetsc.obj: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(osdir)\pdcsetsc.c

pdcutil.obj: $(srcdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\pdcutil.c

pdcwin.obj: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) /fo=$@ $(srcdir)\pdcwin.c

#------------------------------------------------------------------------

panel.obj: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(CCFLAGS) /fo=$@ $(pandir)\panel.c

#------------------------------------------------------------------------

firework.exe:	firework.obj $(LIBCURSES)
	echo option quiet         >  demos.lnk
	echo system $(TARGET)     >> demos.lnk
	echo name firework.exe    >> demos.lnk
	echo file firework.obj    >> demos.lnk
	echo library $(LIBCURSES) >> demos.lnk
	$(LINK) $(LDFLAGS) @demos.lnk
	del demos.lnk

newdemo.exe:	newdemo.obj $(LIBCURSES)
	echo option quiet         >  demos.lnk
	echo system $(TARGET)     >> demos.lnk
	echo name newdemo.exe     >> demos.lnk
	echo file newdemo.obj     >> demos.lnk
	echo library $(LIBCURSES) >> demos.lnk
	$(LINK) $(LDFLAGS) @demos.lnk
	del demos.lnk

ptest.exe:	ptest.obj $(LIBCURSES) $(LIBPANEL)
	echo option quiet         >  demos.lnk
	echo system $(TARGET)     >> demos.lnk
	echo name ptest.exe       >> demos.lnk
	echo file ptest.obj       >> demos.lnk
	echo library $(LIBCURSES) >> demos.lnk
	echo library $(LIBPANEL)  >> demos.lnk
	$(LINK) $(LDFLAGS) @demos.lnk
	del demos.lnk

testcurs.exe:	testcurs.obj $(LIBCURSES)
	echo option quiet         >  demos.lnk
	echo system $(TARGET)     >> demos.lnk
	echo name testcurs.exe    >> demos.lnk
	echo file testcurs.obj    >> demos.lnk
	echo library $(LIBCURSES) >> demos.lnk
	$(LINK) $(LDFLAGS) @demos.lnk
	del demos.lnk

tuidemo.exe:	tuidemo.obj tui.obj $(LIBCURSES)
	echo option quiet         >  demos.lnk
	echo system $(TARGET)     >> demos.lnk
	echo name tuidemo.exe     >> demos.lnk
	echo file tuidemo.obj     >> demos.lnk
	echo file tui.obj         >> demos.lnk
	echo library $(LIBCURSES) >> demos.lnk
	$(LINK) $(LDFLAGS) @demos.lnk
	del demos.lnk

xmas.exe:	xmas.obj $(LIBCURSES)
	echo option quiet         >  demos.lnk
	echo system $(TARGET)     >> demos.lnk
	echo name xmas.exe        >> demos.lnk
	echo file xmas.obj        >> demos.lnk
	echo library $(LIBCURSES) >> demos.lnk
	$(LINK) $(LDFLAGS) @demos.lnk
	del demos.lnk


firework.obj: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) /fo=$@ $(demodir)\firework.c

newdemo.obj: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) /fo=$@ $(demodir)\newdemo.c

ptest.obj: $(demodir)\ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) /fo=$@ $(demodir)\ptest.c

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) /fo=$@ $(demodir)\testcurs.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) /fo=$@ $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) /fo=$@ $(demodir)\tuidemo.c

xmas.obj: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) /fo=$@ $(demodir)\xmas.c

dist: .symbolic $(PDCLIBS)
	echo PDCurses $(VERDOT) for Watcom C++ Win32 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Open Watcom 1.3 for Win32. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_wcc_w32 &
	$(PDCURSES_HOME)\README $(PDCURSES_HOME)\readme.$(VER) &
	$(PDCURSES_HOME)\maintain.er &
	$(PDCURSES_HOME)\curses.h $(PDCURSES_HOME)\curspriv.h &
	$(PDCURSES_HOME)\panel.h $(PDCURSES_HOME)\term.h &
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	del file_id.diz
