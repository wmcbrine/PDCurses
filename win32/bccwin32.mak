################################################################################
#
# Borland MAKE Makefile for PDCurses library - Win32 BC++ 4.0+
#
# Usage: make -f [path\]bccwin32.mak [DEBUG=] [WIDE=] [UTF8=] [target]
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

osdir		= $(PDCURSES_HOME)\win32

CC		= bcc32 -q

!if $d(DEBUG)
CFLAGS		= -N -v -y -DPDCDEBUG
!else
CFLAGS		= -O
!endif

!if $d(WIDE)
WIDEOPT		= -DPDC_WIDE
!endif

!if $d(UTF8)
UTF8OPT		= -DPDC_FORCE_UTF8
!endif

BUILD		= $(CC) -I$(PDCURSES_HOME) -c -Tpe -w32 $(CFLAGS) -w-par \
$(WIDEOPT) $(UTF8OPT)

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

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	-del $@
	$(LIBEXE) $@ \
+addch.obj +addchstr.obj +addstr.obj +attr.obj +beep.obj +bkgd.obj \
+border.obj +clear.obj +color.obj +delch.obj +deleteln.obj +deprec.obj \
+getch.obj +getstr.obj +getyx.obj +inch.obj +inchstr.obj +initscr.obj \
+inopts.obj +insch.obj +insstr.obj +instr.obj +kernel.obj +keyname.obj \
+mouse.obj +move.obj +outopts.obj +overlay.obj +pad.obj +printw.obj \
+refresh.obj +scanw.obj +scr_dump.obj +scroll.obj +slk.obj +termattr.obj \
+terminfo.obj +touch.obj +util.obj +window.obj +debug.obj +pdcclip.obj \
+pdcdisp.obj +pdcgetsc.obj +pdckbd.obj +pdcscrn.obj +pdcsetsc.obj \
+pdcutil.obj ,lib.map

$(LIBPANEL) : $(PANOBJS)
	-del $@
	$(LIBEXE) $@ +$(PANOBJS)

.autodepend

{$(srcdir)\}.c.obj:
	$(BUILD) $<

{$(osdir)\}.c.obj:
	$(BUILD) $<

{$(pandir)\}.c.obj:
	$(BUILD) $<

{$(demodir)\}.c.obj:
	$(BUILD) $<

.c.obj:
	$(BUILD) $<

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
	$(BUILD) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(BUILD) -I$(demodir) $(demodir)\tuidemo.c

PLATFORM1 = Borland C++ Win32
PLATFORM2 = Borland C/C++ 5.5 for Win32
ARCNAME = pdc$(VER)_bcc_w32

!include $(PDCURSES_HOME)\makedist.mif
