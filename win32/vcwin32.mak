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
# This makefile requires the following environment variables with values like:
# PDCURSES_SRCDIR=c:\pdcurses
#
################################################################################
VER = 26
VERDOT=2.6

PDCURSES_HOME		=$(PDCURSES_SRCDIR)
################################################################################
# Nothing below here should required changing.
################################################################################

PDCURSES_CURSES_H		=$(PDCURSES_HOME)\curses.h
PDCURSES_CURSPRIV_H		=$(PDCURSES_HOME)\curspriv.h
PDCURSES_HEADERS		=$(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		=$(PDCURSES_HOME)\panel.h

srcdir		= $(PDCURSES_HOME)\pdcurses
osdir		= $(PDCURSES_HOME)\win32
pandir		= $(PDCURSES_HOME)\panel
demodir		= $(PDCURSES_HOME)\demos

CC		= cl.exe -nologo

!ifdef DEBUG
CFLAGS  = -Z7 -DPDCDEBUG
LDFLAGS = -debug -pdb:none
!else
CFLAGS  =  -Ox
LDFLAGS = 
!endif

SHL_LD=link $(LDFLAGS) /NOLOGO /DLL /OUT:curses.dll /DEF:$(osdir)\curses.def

CPPFLAGS	= -I$(PDCURSES_HOME) -I.

CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS) -DPDC_STATIC_BUILD
DLL_CCFLAGS		= -c $(CFLAGS) $(CPPFLAGS) -MT -DPDC_THREAD_BUILD

LINK		= link.exe -nologo

CCLIBS		= user32.lib
# may need to add msvcrt.lib for VC 2.x, VC 5.0 doesn't want it
#CCLIBS		= msvcrt.lib user32.lib
STARTUP		=

LIBEXE		= lib -nologo

LIBCURSES	= pdcurses.lib
DLLCURSES	= curses.lib
CURSESDLL = curses.dll
LIBPANEL	= panel.lib

PDCLIBS	= $(LIBCURSES) $(LIBPANEL) $(CURSESDLL)
DEMOS	=testcurs.exe newdemo.exe xmas.exe tuidemo.exe firework.exe
DLL_DEMOS	=testcurs_dll.exe newdemo_dll.exe xmas_dll.exe tuidemo_dll.exe firework_dll.exe

################################################################################
all:	$(PDCLIBS) $(DEMOS) $(DLL_DEMOS)

clean:
	-del *.obj
	-del curses.lib
	-del panel.lib
	-del *.exe

demos:	$(DEMOS)
#------------------------------------------------------------------------

LIBOBJS =     \
addch.obj     \
addchstr.obj  \
addstr.obj    \
attr.obj      \
beep.obj      \
bkgd.obj      \
border.obj    \
clear.obj     \
color.obj     \
delch.obj     \
deleteln.obj  \
getch.obj     \
getstr.obj    \
getyx.obj     \
inch.obj      \
inchstr.obj   \
initscr.obj   \
inopts.obj    \
insch.obj     \
insstr.obj    \
instr.obj     \
kernel.obj    \
mouse.obj     \
move.obj      \
outopts.obj   \
overlay.obj   \
pad.obj       \
printw.obj    \
refresh.obj   \
scanw.obj     \
scroll.obj    \
slk.obj       \
termattr.obj  \
terminfo.obj  \
touch.obj     \
util.obj      \
window.obj

PDCOBJS =     \
pdcclip.obj   \
pdcdebug.obj  \
pdcdisp.obj   \
pdcgetsc.obj  \
pdckbd.obj    \
pdcprint.obj  \
pdcscrn.obj   \
pdcsetsc.obj  \
pdcutil.obj   \
pdcwin.obj

PANOBJS =     \
panel.obj

LIBDLLS =     \
addch.dll.obj     \
addchstr.dll.obj  \
addstr.dll.obj    \
attr.dll.obj      \
beep.dll.obj      \
bkgd.dll.obj      \
border.dll.obj    \
clear.dll.obj     \
color.dll.obj     \
delch.dll.obj     \
deleteln.dll.obj  \
getch.dll.obj     \
getstr.dll.obj    \
getyx.dll.obj     \
inch.dll.obj      \
inchstr.dll.obj   \
initscr.dll.obj   \
inopts.dll.obj    \
insch.dll.obj     \
insstr.dll.obj    \
instr.dll.obj     \
kernel.dll.obj    \
mouse.dll.obj     \
move.dll.obj      \
outopts.dll.obj   \
overlay.dll.obj   \
pad.dll.obj       \
printw.dll.obj    \
refresh.dll.obj   \
scanw.dll.obj     \
scroll.dll.obj    \
slk.dll.obj       \
termattr.dll.obj  \
terminfo.dll.obj  \
touch.dll.obj     \
util.dll.obj      \
window.dll.obj

PDCDLLS =     \
pdcclip.dll.obj   \
pdcdebug.dll.obj  \
pdcdisp.dll.obj   \
pdcgetsc.dll.obj  \
pdckbd.dll.obj    \
pdcprint.dll.obj  \
pdcscrn.dll.obj   \
pdcsetsc.dll.obj  \
pdcutil.dll.obj   \
pdcwin.dll.obj

PANDLLS =     \
panel.dll.obj


pdcurses.lib : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) -out:$@ @<<
	$(LIBOBJS) $(PDCOBJS)
<<

curses.dll : $(LIBDLLS) $(PDCDLLS) $(osdir)\curses.def pdcurses.obj
	$(SHL_LD) @<<
	$(LIBDLLS) $(PDCDLLS) pdcurses.obj $(CCLIBS)
<<

pdcurses.res pdcurses.obj: $(osdir)\pdcurses.rc $(osdir)\pdcurses.ico
	copy $(osdir)\pdcurses.ico
	rc /r /fopdcurses.res $(osdir)\pdcurses.rc
	cvtres /MACHINE:IX86 /NOLOGO /OUT:pdcurses.obj pdcurses.res

panel.lib : $(PANOBJS)
	$(LIBEXE) -out:$@ $(PANOBJS)


addch.obj: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\addch.c

addchstr.obj: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\addchstr.c

addstr.obj: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\addstr.c

attr.obj: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\attr.c

beep.obj: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\beep.c

bkgd.obj: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\bkgd.c

border.obj: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\border.c

clear.obj: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\clear.c

color.obj: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\color.c

delch.obj: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\delch.c

deleteln.obj: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\deleteln.c

getch.obj: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\getch.c

getstr.obj: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\getstr.c

getyx.obj: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\getyx.c

inch.obj: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\inch.c

inchstr.obj: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\inchstr.c

initscr.obj: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\initscr.c

inopts.obj: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\inopts.c

insch.obj: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\insch.c

insstr.obj: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\insstr.c

instr.obj: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\instr.c

kernel.obj: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\kernel.c

mouse.obj: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\mouse.c

move.obj: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\move.c

outopts.obj: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\outopts.c

overlay.obj: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\overlay.c

pad.obj: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\pad.c

printw.obj: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\printw.c

refresh.obj: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\refresh.c

scanw.obj: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\scanw.c

scroll.obj: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\scroll.c

slk.obj: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\slk.c

termattr.obj: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\termattr.c

terminfo.obj: $(srcdir)\terminfo.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\terminfo.c

touch.obj: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\touch.c

util.obj: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\util.c

window.obj: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\window.c


pdcclip.obj: $(osdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(osdir)\pdcclip.c

pdcdebug.obj: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\pdcdebug.c

pdcdisp.obj: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(osdir)\pdcdisp.c

pdcgetsc.obj: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(osdir)\pdcgetsc.c

pdckbd.obj: $(osdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(osdir)\pdckbd.c

pdcprint.obj: $(osdir)\pdcprint.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(osdir)\pdcprint.c

pdcscrn.obj: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(osdir)\pdcscrn.c

pdcsetsc.obj: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(osdir)\pdcsetsc.c

pdcutil.obj: $(srcdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\pdcutil.c

pdcwin.obj: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -Fo$@ $(srcdir)\pdcwin.c



addch.dll.obj: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\addch.c

addchstr.dll.obj: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\addchstr.c

addstr.dll.obj: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\addstr.c

attr.dll.obj: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\attr.c

beep.dll.obj: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\beep.c

bkgd.dll.obj: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\bkgd.c

border.dll.obj: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\border.c

clear.dll.obj: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\clear.c

color.dll.obj: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\color.c

delch.dll.obj: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\delch.c

deleteln.dll.obj: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\deleteln.c

getch.dll.obj: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\getch.c

getstr.dll.obj: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\getstr.c

getyx.dll.obj: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\getyx.c

inch.dll.obj: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\inch.c

inchstr.dll.obj: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\inchstr.c

initscr.dll.obj: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\initscr.c

inopts.dll.obj: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\inopts.c

insch.dll.obj: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\insch.c

insstr.dll.obj: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\insstr.c

instr.dll.obj: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\instr.c

kernel.dll.obj: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\kernel.c

mouse.dll.obj: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\mouse.c

move.dll.obj: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\move.c

outopts.dll.obj: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\outopts.c

overlay.dll.obj: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\overlay.c

pad.dll.obj: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\pad.c

printw.dll.obj: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\printw.c

refresh.dll.obj: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\refresh.c

scanw.dll.obj: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\scanw.c

scroll.dll.obj: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\scroll.c

slk.dll.obj: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\slk.c

termattr.dll.obj: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\termattr.c

terminfo.dll.obj: $(srcdir)\terminfo.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\terminfo.c

touch.dll.obj: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\touch.c

util.dll.obj: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\util.c

window.dll.obj: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\window.c


pdcclip.dll.obj: $(osdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(osdir)\pdcclip.c

pdcdebug.dll.obj: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\pdcdebug.c

pdcdisp.dll.obj: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(osdir)\pdcdisp.c

pdcgetsc.dll.obj: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(osdir)\pdcgetsc.c

pdckbd.dll.obj: $(osdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(osdir)\pdckbd.c

pdcprint.dll.obj: $(osdir)\pdcprint.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(osdir)\pdcprint.c

pdcscrn.dll.obj: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(osdir)\pdcscrn.c

pdcsetsc.dll.obj: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(osdir)\pdcsetsc.c

pdcutil.dll.obj: $(srcdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\pdcutil.c

pdcwin.dll.obj: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) -LD $(DLL_CCFLAGS) -Fo$@ $(srcdir)\pdcwin.c

#------------------------------------------------------------------------

panel.obj: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(CCFLAGS) -Fo$@ $(pandir)\panel.c

panel.dll.obj: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(DLL_CCFLAGS) -Fo$@ $(pandir)\panel.c

#--- Targets for statically linked demo programs ------------------------

firework.exe:	firework.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

newdemo.exe:	newdemo.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

ptest.exe:	ptest.obj $(LIBCURSES) $(LIBPANEL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ $*.obj $(LIBPANEL) $(LIBCURSES) $(CCLIBS)

testcurs.exe:	testcurs.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

tuidemo.exe:	tuidemo.obj tui.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ tui.obj $*.obj $(LIBCURSES) $(CCLIBS)

xmas.exe:	xmas.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

test.exe:	test.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ $*.obj $(LIBCURSES) $(CCLIBS)

#--- Targets for statically linked demo objects -------------------------

firework.obj: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Fo$@ $(demodir)\firework.c

newdemo.obj: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Fo$@ $(demodir)\newdemo.c

ptest.obj: $(demodir)\ptest.c $(PDCURSES_HOME)\panel.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Fo$@ $(demodir)\ptest.c

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Fo$@ $(demodir)\testcurs.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -Fo$@ $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -Fo$@ $(demodir)\tuidemo.c

xmas.obj: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Fo$@ $(demodir)\xmas.c

test.obj: test.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Fo$@ test.c

#--- Targets for dynamically linked demo programs -----------------------

firework_dll.exe:	firework.dll.obj $(CURSESDLL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ firework.dll.obj $(DLLCURSES) $(CCLIBS)

newdemo_dll.exe:	newdemo.dll.obj $(CURSESDLL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ newdemo.dll.obj $(DLLCURSES) $(CCLIBS)

ptest_dll.exe:	ptest.dll.obj $(CURSESDLL) $(LIBPANEL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ ptest.dll.obj $(LIBPANEL) $(DLLCURSES) $(CCLIBS)

testcurs_dll.exe:	testcurs.dll.obj $(CURSESDLL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ testcurs.dll.obj $(DLLCURSES) $(CCLIBS)

tuidemo_dll.exe:	tuidemo.dll.obj tui.dll.obj $(CURSESDLL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ tui.dll.obj tuidemo.dll.obj $(DLLCURSES) $(CCLIBS)

xmas_dll.exe:	xmas.dll.obj $(CURSESDLL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ xmas.dll.obj $(DLLCURSES) $(CCLIBS)

test_dll.exe:	test.dll.obj $(CURSESDLL)
	$(LINK) $(LDFLAGS) $(STARTUP) -out:$@ test.dll.obj $(DLLCURSES) $(CCLIBS)

#--- Targets for dynamically linked demo objects ------------------------

firework.dll.obj: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -Fo$@ $(demodir)\firework.c

newdemo.dll.obj: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -Fo$@ $(demodir)\newdemo.c

ptest.dll.obj: $(demodir)\ptest.c $(PDCURSES_HOME)\panel.h $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -Fo$@ $(demodir)\ptest.c

testcurs.dll.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -Fo$@ $(demodir)\testcurs.c

tui.dll.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -I$(demodir) -Fo$@ $(demodir)\tui.c

tuidemo.dll.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -I$(demodir) -Fo$@ $(demodir)\tuidemo.c

xmas.dll.obj: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -Fo$@ $(demodir)\xmas.c

test.dll.obj: test.c $(PDCURSES_CURSES_H)
	$(CC) $(DLL_CCFLAGS) -Fo$@ test.c

dist: curses.dll pdcurses.lib panel.lib
	-mkdir tmp
	cd tmp
	copy $(PDCURSES_HOME)\README .
	copy $(PDCURSES_HOME)\readme.?? .
	copy $(PDCURSES_HOME)\curses.h .
	copy $(PDCURSES_HOME)\curspriv.h .
	copy $(PDCURSES_HOME)\maintain.er .
	copy ..\curses.dll .
	copy ..\curses.lib .
	copy ..\pdcurses.lib .
	copy ..\panel.lib .
	echo 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴 > file_id.diz
	echo      PDCurses - Public Domain Curses >> file_id.diz
	echo  Version $(VERDOT)   for VC++        PDC$(VER)_VC_W32.ZIP >> file_id.diz
	echo  ------------------------------------------ >> file_id.diz
	echo  Public Domain Curses library for >> file_id.diz
	echo  Microsoft Visual C/C++ for Win32. >> file_id.diz
	echo  Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo  Public Domain. >> file_id.diz
	echo 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴 >> file_id.diz
	zip pdc$(VER)_vc_w32 *.*
	cd ..
