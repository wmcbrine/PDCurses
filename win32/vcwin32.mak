################################################################################
#
# Visual C++ NMakefile for PDCurses library - Win32 VC++ 2.0+
#
# Usage: nmake -f [path\]vcwin32.mak [DEBUG=] [DLL=] [WIDE=] [UTF8=] [target]
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

PDCURSES_WIN_H	= $(osdir)\pdcwin.h

CC		= cl.exe -nologo

!ifdef DEBUG
CFLAGS		= -Z7 -DPDCDEBUG
LDFLAGS		= -debug -pdb:none
!else
CFLAGS		= -O1
LDFLAGS		=
!endif

!ifdef WIDE
DEFFILE		= $(osdir)\cursesw.def
WIDEOPT		= -DPDC_WIDE
!else
DEFFILE		= $(osdir)\curses.def
!endif

!ifdef UTF8
UTF8OPT		= -DPDC_FORCE_UTF8
!endif

SHL_LD = link $(LDFLAGS) /NOLOGO /DLL /OUT:pdcurses.dll /DEF:$(DEFFILE)

LINK		= link.exe -nologo

CCLIBS		= user32.lib advapi32.lib
# may need to add msvcrt.lib for VC 2.x, VC 5.0 doesn't want it
#CCLIBS		= msvcrt.lib user32.lib advapi32.lib

LIBEXE		= lib -nologo

LIBCURSES	= pdcurses.lib
CURSESDLL	= pdcurses.dll
LIBPANEL	= panel.lib

!ifdef DLL
DLLOPT		= -DPDC_DLL_BUILD
PDCLIBS		= $(CURSESDLL) $(LIBPANEL)
!else
PDCLIBS		= $(LIBCURSES) $(LIBPANEL)
!endif

BUILD		= $(CC) -I$(PDCURSES_HOME) -c $(CFLAGS) $(DLLOPT) \
$(WIDEOPT) $(UTF8OPT)

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
$(PDCOBJS) : $(PDCURSES_WIN_H)
$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(PANOBJS) : $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)
$(DEMOS) : $(LIBCURSES)

!ifndef DLL
$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) -out:$@ $(LIBOBJS) $(PDCOBJS)
!endif

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
