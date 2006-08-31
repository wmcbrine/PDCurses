################################################################################
#
# Visual C++ NMakefile for PDCurses library - Win32 VC++ 2.0+
#
# Usage: nmake -f [path\]vcwin32.mak [DEBUG=] [DLLBUILD=] [target]
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

CC		= cl.exe -nologo

!ifdef DEBUG
CFLAGS		= -Z7 -DPDCDEBUG
LDFLAGS		= -debug -pdb:none
!else
CFLAGS		=  -Ox
LDFLAGS		=
!endif

SHL_LD = link $(LDFLAGS) /NOLOGO /DLL /OUT:pdcurses.dll /DEF:$(osdir)\curses.def

CPPFLAGS	= -I$(PDCURSES_HOME) #-DPDC_WIDE

LINK		= link.exe -nologo

CCLIBS		= user32.lib
# may need to add msvcrt.lib for VC 2.x, VC 5.0 doesn't want it
#CCLIBS		= msvcrt.lib user32.lib

LIBEXE		= lib -nologo

LIBCURSES	= pdcurses.lib
CURSESDLL	= pdcurses.dll
LIBPANEL	= panel.lib

!ifdef DLLBUILD
BUILD		= $(CC) -c $(CFLAGS) $(CPPFLAGS) -DPDC_DLL_BUILD
PDCLIBS		= $(CURSESDLL) $(LIBPANEL)
!else
BUILD		= $(CC) -c $(CFLAGS) $(CPPFLAGS)
PDCLIBS		= $(LIBCURSES) $(LIBPANEL)
!endif

################################################################################

all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe
	-del *.dll
	-del *.exp
	-del *.res

#------------------------------------------------------------------------

DEMOOBJS = $(DEMOS:.exe=.obj) tui.obj

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(PANOBJS) : $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)
$(DEMOS) : $(LIBCURSES)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) -out:$@ $(LIBOBJS) $(PDCOBJS)

$(CURSESDLL) : $(LIBOBJS) $(PDCOBJS) $(osdir)\curses.def pdcurses.obj
	$(SHL_LD) $(LIBOBJS) $(PDCOBJS) pdcurses.obj $(CCLIBS)

pdcurses.res pdcurses.obj: $(osdir)\pdcurses.rc $(osdir)\pdcurses.ico
	rc /r /fopdcurses.res $(osdir)\pdcurses.rc
	cvtres /MACHINE:IX86 /NOLOGO /OUT:pdcurses.obj pdcurses.res

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) -out:$@ $(PANOBJS)

{$(srcdir)\}.c{}.obj::
	$(BUILD) $<

{$(osdir)\}.c{}.obj::
	$(BUILD) $<

{$(pandir)\}.c{}.obj::
	$(BUILD) $<

{$(demodir)\}.c{}.obj::
	$(BUILD) $<

.obj.exe:
	$(LINK) $(LDFLAGS) $< $(LIBCURSES) $(CCLIBS)

#------------------------------------------------------------------------

ptest.exe: ptest.obj $(LIBPANEL)
	$(LINK) $(LDFLAGS) $*.obj $(LIBPANEL) $(LIBCURSES) $(CCLIBS)

tuidemo.exe: tuidemo.obj tui.obj
	$(LINK) $(LDFLAGS) $*.obj tui.obj $(LIBCURSES) $(CCLIBS)

#------------------------------------------------------------------------

ptest.obj: $(demodir)\ptest.c $(PANEL_HEADER)
	$(BUILD) $(demodir)\ptest.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h
	$(BUILD) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c
	$(BUILD) -I$(demodir) $(demodir)\tuidemo.c

#------------------------------------------------------------------------

PLATFORM1 = Visual C++
PLATFORM2 = Microsoft Visual C/C++ for Win32
ARCNAME = pdc$(VER)_vc_w32

!include $(PDCURSES_HOME)\makedist.mif
