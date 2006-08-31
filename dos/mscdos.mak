################################################################################
#
# NMAKE Makefile for PDCurses library - Microsoft C for DOS
#
# Usage: nmake -f [path\]dosmsc.mak [DEBUG=1] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|panel.lib|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, and/or edit the
# lines below; for example, "set PDCURSES_SRCDIR=c:\pdcurses".
#
################################################################################
PDCURSES_HOME	= $(PDCURSES_SRCDIR)
MODEL		= L       # one of L, S, M, T, H, C
SIZE		= LARGE    # one of LARGE, SMALL, MEDIUM, TINY, HUGE, COMPACT
################################################################################
# Nothing below here should require changing.
################################################################################

O = obj

!include $(PDCURSES_HOME)\version.mif
!include $(PDCURSES_HOME)\libobjs.mif

osdir		= $(PDCURSES_HOME)\dos

PDCURSES_DOS_H	= $(osdir)\pdcdos.h

CC		= cl

!ifdef DEBUG
CFLAGS		= /Od /Zi /DPDCDEBUG
LDFLAGS		= /CO /NOE /SE:160 
!else
CFLAGS		= /Ox
LDFLAGS		= /NOE /SE:160
!endif

CPPFLAGS	= -I$(PDCURSES_HOME)

BUILD		= $(CC) /J /nologo /c /D$(SIZE) /A$(MODEL) $(CFLAGS) $(CPPFLAGS)

LINK		= link

CCLIBS		=

LIBEXE		= lib

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

DEMOOBJS = $(DEMOS:.exe=.obj) tui.obj

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(PANOBJS) : $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)
$(DEMOS) : $(LIBCURSES)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @$(osdir)\mscdos.lrf

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) $@ -+$(PANOBJS);

{$(srcdir)\}.c{}.obj:
	$(BUILD) $<

{$(osdir)\}.c{}.obj:
	$(BUILD) $<

{$(pandir)\}.c{}.obj:
	$(BUILD) $<

{$(demodir)\}.c{}.obj:
	$(BUILD) $<

#------------------------------------------------------------------------

firework.exe: firework.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

newdemo.exe: newdemo.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

ptest.exe: ptest.obj $(LIBPANEL)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBPANEL)+$(LIBCURSES);

rain.exe: rain.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

testcurs.exe: testcurs.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

tuidemo.exe: tuidemo.obj tui.obj
	$(LINK) $(LDFLAGS) $*.obj+tui.obj,$*,,$(LIBCURSES);

worm.exe: worm.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

xmas.exe: xmas.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

#------------------------------------------------------------------------

ptest.obj: $(demodir)\ptest.c $(PANEL_HEADER)
	$(BUILD) $(demodir)\ptest.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h
	$(BUILD) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c
	$(BUILD) -I$(demodir) $(demodir)\tuidemo.c

#------------------------------------------------------------------------

PLATFORM1 = Microsoft C
PLATFORM2 = Microsoft C for DOS
ARCNAME = pdc$(VER)msc

!include $(PDCURSES_HOME)\makedist.mif
