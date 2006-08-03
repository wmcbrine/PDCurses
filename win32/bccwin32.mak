################################################################################
#
# Borland MAKE Makefile for PDCurses library - Win32 BC++ 4.0+
#
# Usage: make -f [path\]bccwin32.mak [-DDEBUG] [target]
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
PDCURSES_HOME	= $(PDCURSES_SRCDIR)
################################################################################
# Nothing below here should require changing.
################################################################################

O = obj

!include $(PDCURSES_HOME)\version
!include $(PDCURSES_HOME)\libobjs

PDCURSES_CURSES_H	= $(PDCURSES_HOME)\curses.h
PDCURSES_CURSPRIV_H	= $(PDCURSES_HOME)\curspriv.h
PDCURSES_HEADERS	= $(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		= $(PDCURSES_HOME)\panel.h
TERM_HEADER		= $(PDCURSES_HOME)\term.h

srcdir		= $(PDCURSES_HOME)\pdcurses
osdir		= $(PDCURSES_HOME)\win32
pandir		= $(PDCURSES_HOME)\panel
demodir		= $(PDCURSES_HOME)\demos

CC		= bcc32 -q

!if $d(DEBUG)
CFLAGS		= -N -v -y -DPDCDEBUG
!else
CFLAGS		= -O -w-par
!endif

CPPFLAGS	= -I$(PDCURSES_HOME) #-DPDC_WIDE

CCFLAGS		= -c -Tpe -w32 $(CFLAGS) $(CPPFLAGS)

LIBEXE		= tlib /C /E /0

LIBCURSES	= pdcurses.lib
LIBPANEL	= panel.lib

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.map
	-del *.tds
	-del *.exe

demos:	$(DEMOS)

#------------------------------------------------------------------------

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PANOBJS) : $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	-del $@
	$(LIBEXE) $@ \
+addch.obj +addchstr.obj +addstr.obj +attr.obj +beep.obj +bkgd.obj \
+border.obj +clear.obj +color.obj +delch.obj +deleteln.obj +getch.obj \
+getstr.obj +getyx.obj +inch.obj +inchstr.obj +initscr.obj +inopts.obj \
+insch.obj +insstr.obj +instr.obj +kernel.obj +mouse.obj +move.obj \
+outopts.obj +overlay.obj +pad.obj +printw.obj +refresh.obj +scanw.obj \
+scr_dump.obj +scroll.obj +slk.obj +termattr.obj +terminfo.obj \
+touch.obj +util.obj +window.obj +pdcdebug.obj +pdcclip.obj +pdcdisp.obj \
+pdcgetsc.obj +pdckbd.obj +pdcscrn.obj +pdcsetsc.obj +pdcutil.obj \
+pdcwin.obj ,lib.map

$(LIBPANEL) : $(PANOBJS)
	-del $@
	$(LIBEXE) $@ +$(PANOBJS)

{$(srcdir)\}.c.obj:
	$(CC) $(CCFLAGS) $<

{$(osdir)\}.c.obj:
	$(CC) $(CCFLAGS) $<

{$(pandir)\}.c.obj:
	$(CC) $(CCFLAGS) $<

.c.obj:
	$(CC) $(CCFLAGS) $<

#------------------------------------------------------------------------

firework.exe:	firework.obj $(LIBCURSES)
	$(CC) -e$@ $**

newdemo.exe:	newdemo.obj $(LIBCURSES)
	$(CC) -e$@ $**

ptest.exe:	ptest.obj $(LIBCURSES) $(LIBPANEL)
	$(CC) -e$@ $**

rain.exe:	rain.obj $(LIBCURSES)
	$(CC) -e$@ $**

testcurs.exe:	testcurs.obj $(LIBCURSES)
	$(CC) -e$@ $**

tuidemo.exe:	tuidemo.obj tui.obj $(LIBCURSES)
	$(CC) -e$@ $**

worm.exe:	worm.obj $(LIBCURSES)
	$(CC) -e$@ $**

xmas.exe:	xmas.obj $(LIBCURSES)
	$(CC) -e$@ $**


firework.obj: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\firework.c

newdemo.obj: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\newdemo.c

ptest.obj: $(demodir)\ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\ptest.c

rain.obj: $(demodir)\rain.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\rain.c

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\testcurs.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o$@ $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o$@ $(demodir)\tuidemo.c

worm.obj: $(demodir)\worm.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\worm.c

xmas.obj: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\xmas.c

dist: $(PDCLIBS)
	echo PDCurses $(VERDOT) for Borland C++ Win32 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Borland C/C++ 5.5 for Win32. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_bcc_w32 \
	$(PDCURSES_HOME)\README $(PDCURSES_HOME)\HISTORY \
	$(PDCURSES_HOME)\maintain.er \
	$(PDCURSES_HOME)\curses.h $(PDCURSES_HOME)\curspriv.h \
	$(PDCURSES_HOME)\panel.h $(PDCURSES_HOME)\term.h \
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	del file_id.diz
