################################################################################
#
# Borland MAKE Makefile for PDCurses library - OS/2 BC++ 1.0+
#
# Usage: make -f [path\]bccos2.mak [-DDEBUG] [target]
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

!include $(PDCURSES_HOME)\version.mif
!include $(PDCURSES_HOME)\libobjs.mif

osdir		= $(PDCURSES_HOME)\os2

CC		= bcc

!if $d(DEBUG)
CFLAGS		= -N -v -y -DPDCDEBUG 
!else
CFLAGS		= -O 
!endif

CPPFLAGS	= -I$(PDCURSES_HOME)

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS)

LINK		= tlink

LIBEXE		= tlib /C /E

LIBCURSES	= pdcurses.lib
LIBPANEL	= panel.lib

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe

demos:	$(DEMOS)

#------------------------------------------------------------------------

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

.autodepend

{$(srcdir)\}.c.obj:
	$(CC) $(CCFLAGS) $<

{$(osdir)\}.c.obj:
	$(CC) $(CCFLAGS) $<

{$(pandir)\}.c.obj:
	$(CC) $(CCFLAGS) $<

{$(demodir)\}.c.obj:
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

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) $(demodir)\tuidemo.c

PLATFORM1 = Borland C++ OS/2 1.0
PLATFORM2 = Borland C/C++ OS/2 1.0
ARCNAME = pdc$(VER)bcos2

!include $(PDCURSES_HOME)\makedist.mif
