################################################################################
#
# Visual C++ NMakefile for PDCurses library - Win32 VC++ 2.0+
#
# Usage: nmake -f [path\]vcwin32.mak [DEBUG=] [target]
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

osdir		= $(PDCURSES_HOME)/win32

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

# For a static build:

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS)
PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

# For a DLL build:

#CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS) -DPDC_DLL_BUILD
#PDCLIBS		= $(CURSESDLL) $(LIBPANEL)

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe
	-del *.dll
	-del *.exp
	-del *.res

demos:	$(DEMOS)
#------------------------------------------------------------------------

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(PANOBJS) : $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)

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
	$(CC) $(CCFLAGS) $<

{$(osdir)\}.c{}.obj::
	$(CC) $(CCFLAGS) $<

{$(pandir)\}.c{}.obj::
	$(CC) $(CCFLAGS) $<

#------------------------------------------------------------------------

firework.exe:	firework.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

newdemo.exe:	newdemo.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

ptest.exe:	ptest.obj $(LIBCURSES) $(LIBPANEL)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBPANEL) $(LIBCURSES) $(CCLIBS)

rain.exe:	rain.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

testcurs.exe:	testcurs.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

tuidemo.exe:	tuidemo.obj tui.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ tui.obj $*.obj $(LIBCURSES) $(CCLIBS)

worm.exe:	worm.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

xmas.exe:	xmas.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

test.exe:	test.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

#------------------------------------------------------------------------

firework.obj: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(demodir)\firework.c

newdemo.obj: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(demodir)\newdemo.c

ptest.obj: $(demodir)\ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(demodir)\ptest.c

rain.obj: $(demodir)\rain.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(demodir)\rain.c

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(demodir)\testcurs.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) $(demodir)\tuidemo.c

worm.obj: $(demodir)\worm.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(demodir)\worm.c

xmas.obj: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) $(demodir)\xmas.c

test.obj: test.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) test.c

#------------------------------------------------------------------------

PLATFORM1 = Visual C++
PLATFORM2 = Microsoft Visual C/C++ for Win32
ARCNAME = pdc$(VER)_vc_w32

!include $(PDCURSES_HOME)\makedist.mif
