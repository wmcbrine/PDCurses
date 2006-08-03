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

!include $(PDCURSES_HOME)\version
!include $(PDCURSES_HOME)\libobjs

osdir		= $(PDCURSES_HOME)/win32

CC		= cl.exe -nologo

!ifdef DEBUG
CFLAGS		= -Z7 -DPDCDEBUG
LDFLAGS		= -debug -pdb:none
!else
CFLAGS		=  -Ox
LDFLAGS		=
!endif

SHL_LD = link $(LDFLAGS) /NOLOGO /DLL /OUT:curses.dll /DEF:$(osdir)\curses.def

CPPFLAGS	= -I$(PDCURSES_HOME) #-DPDC_WIDE

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS)
DLL_CCFLAGS	= -c $(CFLAGS) $(CPPFLAGS) -Fo$@ -DPDC_DLL_BUILD #-MT -DPDC_THREAD_BUILD

LINK		= link.exe -nologo

CCLIBS		= user32.lib
# may need to add msvcrt.lib for VC 2.x, VC 5.0 doesn't want it
#CCLIBS		= msvcrt.lib user32.lib

LIBEXE		= lib -nologo

LIBCURSES	= pdcurses.lib
DLLCURSES	= curses.lib
CURSESDLL	= curses.dll
LIBPANEL	= panel.lib

PDCLIBS		= $(LIBCURSES) $(LIBPANEL) #$(CURSESDLL)
DLL_DEMOS	= testcurs_dll.exe newdemo_dll.exe xmas_dll.exe \
tuidemo_dll.exe firework_dll.exe

################################################################################
all:	$(PDCLIBS) $(DEMOS) #$(DLL_DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe
	-del *.dlo
	-del *.dll
	-del *.exp
	-del *.res

demos:	$(DEMOS)
#------------------------------------------------------------------------

LIBDLLS = addch.dlo addchstr.dlo addstr.dlo attr.dlo beep.dlo bkgd.dlo \
border.dlo clear.dlo color.dlo delch.dlo deleteln.dlo getch.dlo \
getstr.dlo getyx.dlo inch.dlo inchstr.dlo initscr.dlo inopts.dlo \
insch.dlo insstr.dlo instr.dlo kernel.dlo mouse.dlo move.dlo outopts.dlo \
overlay.dlo pad.dlo printw.dlo refresh.dlo scanw.dlo scr_dump.dlo \
scroll.dlo slk.dlo termattr.dlo terminfo.dlo touch.dlo util.dlo \
window.dlo pdcdebug.dlo pdcwin.dlo

PDCDLLS = pdcclip.dlo pdcdisp.dlo pdcgetsc.dlo pdckbd.dlo pdcscrn.dlo \
pdcsetsc.dlo pdcutil.dlo

PANDLLS = panel.dlo

$(LIBOBJS) $(PDCOBJS) $(LIBDLLS) $(PDCDLLS) : $(PDCURSES_HEADERS)
terminfo.obj: $(TERM_HEADER)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) -out:$@ $(LIBOBJS) $(PDCOBJS)

$(CURSESDLL) : $(LIBDLLS) $(PDCDLLS) $(osdir)\curses.def pdcurses.obj
	$(SHL_LD) $(LIBDLLS) $(PDCDLLS) pdcurses.obj $(CCLIBS)

pdcurses.res pdcurses.obj: $(osdir)\pdcurses.rc $(osdir)\pdcurses.ico
	rc /r /fopdcurses.res $(osdir)\pdcurses.rc
	cvtres /MACHINE:IX86 /NOLOGO /OUT:pdcurses.obj pdcurses.res

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) -out:$@ $(PANOBJS)

{$(srcdir)\}.c{}.obj::
	$(CC) $(CCFLAGS) $<

{$(osdir)\}.c{}.obj::
	$(CC) $(CCFLAGS) $<

addch.dlo: $(srcdir)\addch.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\addch.c

addchstr.dlo: $(srcdir)\addchstr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\addchstr.c

addstr.dlo: $(srcdir)\addstr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\addstr.c

attr.dlo: $(srcdir)\attr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\attr.c

beep.dlo: $(srcdir)\beep.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\beep.c

bkgd.dlo: $(srcdir)\bkgd.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\bkgd.c

border.dlo: $(srcdir)\border.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\border.c

clear.dlo: $(srcdir)\clear.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\clear.c

color.dlo: $(srcdir)\color.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\color.c

delch.dlo: $(srcdir)\delch.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\delch.c

deleteln.dlo: $(srcdir)\deleteln.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\deleteln.c

getch.dlo: $(srcdir)\getch.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\getch.c

getstr.dlo: $(srcdir)\getstr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\getstr.c

getyx.dlo: $(srcdir)\getyx.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\getyx.c

inch.dlo: $(srcdir)\inch.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\inch.c

inchstr.dlo: $(srcdir)\inchstr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\inchstr.c

initscr.dlo: $(srcdir)\initscr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\initscr.c

inopts.dlo: $(srcdir)\inopts.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\inopts.c

insch.dlo: $(srcdir)\insch.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\insch.c

insstr.dlo: $(srcdir)\insstr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\insstr.c

instr.dlo: $(srcdir)\instr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\instr.c

kernel.dlo: $(srcdir)\kernel.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\kernel.c

mouse.dlo: $(srcdir)\mouse.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\mouse.c

move.dlo: $(srcdir)\move.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\move.c

outopts.dlo: $(srcdir)\outopts.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\outopts.c

overlay.dlo: $(srcdir)\overlay.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\overlay.c

pad.dlo: $(srcdir)\pad.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\pad.c

printw.dlo: $(srcdir)\printw.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\printw.c

refresh.dlo: $(srcdir)\refresh.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\refresh.c

scanw.dlo: $(srcdir)\scanw.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\scanw.c

scr_dump.dlo: $(srcdir)\scr_dump.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\scr_dump.c

scroll.dlo: $(srcdir)\scroll.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\scroll.c

slk.dlo: $(srcdir)\slk.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\slk.c

termattr.dlo: $(srcdir)\termattr.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\termattr.c

terminfo.dlo: $(srcdir)\terminfo.c $(TERM_HEADER)
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\terminfo.c

touch.dlo: $(srcdir)\touch.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\touch.c

util.dlo: $(srcdir)\util.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\util.c

window.dlo: $(srcdir)\window.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\window.c


pdcclip.dlo: $(osdir)\pdcclip.c
	$(CC) -LD $(DLL_CCFLAGS) $(osdir)\pdcclip.c

pdcdebug.dlo: $(srcdir)\pdcdebug.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\pdcdebug.c

pdcdisp.dlo: $(osdir)\pdcdisp.c
	$(CC) -LD $(DLL_CCFLAGS) $(osdir)\pdcdisp.c

pdcgetsc.dlo: $(osdir)\pdcgetsc.c
	$(CC) -LD $(DLL_CCFLAGS) $(osdir)\pdcgetsc.c

pdckbd.dlo: $(osdir)\pdckbd.c
	$(CC) -LD $(DLL_CCFLAGS) $(osdir)\pdckbd.c

pdcscrn.dlo: $(osdir)\pdcscrn.c
	$(CC) -LD $(DLL_CCFLAGS) $(osdir)\pdcscrn.c

pdcsetsc.dlo: $(osdir)\pdcsetsc.c
	$(CC) -LD $(DLL_CCFLAGS) $(osdir)\pdcsetsc.c

pdcutil.dlo: $(osdir)\pdcutil.c
	$(CC) -LD $(DLL_CCFLAGS) $(osdir)\pdcutil.c

pdcwin.dlo: $(srcdir)\pdcwin.c
	$(CC) -LD $(DLL_CCFLAGS) $(srcdir)\pdcwin.c

#------------------------------------------------------------------------

panel.obj: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(CCFLAGS) $(pandir)\panel.c

panel.dlo: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(DLL_CCFLAGS) $(pandir)\panel.c

#--- Targets for statically linked demo programs ------------------------

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

#--- Targets for statically linked demo objects -------------------------

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

#--- Targets for dynamically linked demo programs -----------------------

firework_dll.exe:	firework.dlo $(CURSESDLL)
	$(LINK) $(LDFLAGS) -out:$@ firework.dlo $(DLLCURSES) $(CCLIBS)

newdemo_dll.exe:	newdemo.dlo $(CURSESDLL)
	$(LINK) $(LDFLAGS) -out:$@ newdemo.dlo $(DLLCURSES) $(CCLIBS)

ptest_dll.exe:	ptest.dlo $(CURSESDLL) $(LIBPANEL)
	$(LINK) $(LDFLAGS) -out:$@ ptest.dlo $(LIBPANEL) $(DLLCURSES) $(CCLIBS)

testcurs_dll.exe:	testcurs.dlo $(CURSESDLL)
	$(LINK) $(LDFLAGS) -out:$@ testcurs.dlo $(DLLCURSES) $(CCLIBS)

tuidemo_dll.exe:	tuidemo.dlo tui.dlo $(CURSESDLL)
	$(LINK) $(LDFLAGS) -out:$@ tui.dlo tuidemo.dlo $(DLLCURSES) $(CCLIBS)

xmas_dll.exe:	xmas.dlo $(CURSESDLL)
	$(LINK) $(LDFLAGS) -out:$@ xmas.dlo $(DLLCURSES) $(CCLIBS)

test_dll.exe:	test.dlo $(CURSESDLL)
	$(LINK) $(LDFLAGS) -out:$@ test.dlo $(DLLCURSES) $(CCLIBS)

#--- Targets for dynamically linked demo objects ------------------------

firework.dlo: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) $(demodir)\firework.c

newdemo.dlo: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) $(demodir)\newdemo.c

ptest.dlo: $(demodir)\ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) $(demodir)\ptest.c

testcurs.dlo: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) $(demodir)\testcurs.c

tui.dlo: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -I$(demodir) $(demodir)\tui.c

tuidemo.dlo: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -I$(demodir) $(demodir)\tuidemo.c

xmas.dlo: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) $(demodir)\xmas.c

test.dlo: test.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) test.c

dist: $(PDCLIBS)
	echo PDCurses $(VERDOT) for Visual C++ > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Microsoft Visual C/C++ for Win32. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_vc_w32 \
	$(PDCURSES_HOME)\README $(PDCURSES_HOME)\HISTORY \
	$(PDCURSES_HOME)\maintain.er \
	$(PDCURSES_HOME)\curses.h $(PDCURSES_HOME)\curspriv.h \
	$(PDCURSES_HOME)\panel.h $(PDCURSES_HOME)\term.h \
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	del file_id.diz
