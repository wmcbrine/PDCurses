################################################################################
#
# Borland MAKE Makefile for PDCurses library - Win32 BC++ 4.0+
#
# Usage: make -f [path\]bccwin32.mak [DEBUG=] [WIDE=] [UTF8=] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]
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

LIBEXE		= tlib /C /E /0 /a

LIBCURSES	= pdcurses.lib

################################################################################
all:	$(LIBCURSES) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.tds
	-del *.exe

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	-del $@
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)
	-copy $(LIBCURSES) panel.lib

.autodepend

{$(srcdir)\}.c.obj:
	$(BUILD) $<

{$(osdir)\}.c.obj:
	$(BUILD) $<

{$(demodir)\}.c.obj:
	$(BUILD) $<

.c.obj:
	$(BUILD) $<

.obj.exe:
	$(CC) -e$@ $** $(LIBCURSES)

#------------------------------------------------------------------------

tuidemo.exe:	tuidemo.obj tui.obj $(LIBCURSES)
	$(CC) -e$@ $**

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(BUILD) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(BUILD) -I$(demodir) $(demodir)\tuidemo.c

PLATFORM1 = Borland C++ Win32
PLATFORM2 = Borland C/C++ 5.5 for Win32
ARCNAME = pdc$(VER)_bcc_w32

!include $(PDCURSES_HOME)\makedist.mif
