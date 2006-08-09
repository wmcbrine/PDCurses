###############################################################################
#
# Makefile for PDCurses library - WIN32 LCC-Win32
#
# Usage: make -f lccwin32.mak [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|panel.lib|testcurs.exe...]
#
###############################################################################
#
# Edit the line below for your environment.
#
###############################################################################
PDCURSES_HOME	= \pdcurses
###############################################################################
# Nothing below here should require changing.
###############################################################################
PDCURSES_CURSES_H	= $(PDCURSES_HOME)\curses.h
PDCURSES_CURSPRIV_H	= $(PDCURSES_HOME)\curspriv.h
PDCURSES_HEADERS	= $(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)

PANEL_HEADER	= $(PDCURSES_HOME)\panel.h
TERM_HEADER	= $(PDCURSES_HOME)\term.h

srcdir		= $(PDCURSES_HOME)\pdcurses
osdir		= $(PDCURSES_HOME)\win32
pandir		= $(PDCURSES_HOME)\panel
demodir		= $(PDCURSES_HOME)\demos

CC		= lcc

#CFLAGS		= -c -g3 -A -ansic
CFLAGS		= -c -O -A -ansic

CPPFLAGS	= -I$(PDCURSES_HOME) #-DPDC_WIDE

LINK		= lcclnk

DEFFILE		= $(osdir)\curses_lcc.def
SHL_LD		= lcclnk -DLL $(DEFFILE)

LIBEXE		= lcclib

LIBCURSES	= pdcurses.lib
CURSESDLL	= pdcurses.dll
LIBPANEL	= panel.lib

# For a static build:

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)
PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

# For a DLL build:

#CCFLAGS		= $(CFLAGS) $(CPPFLAGS) -DPDC_DLL_BUILD
#PDCLIBS		= $(CURSESDLL) $(LIBPANEL)

DEMOS		= testcurs.exe newdemo.exe xmas.exe tuidemo.exe \
firework.exe ptest.exe rain.exe worm.exe

###############################################################################

all:    $(PDCLIBS) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.dll
	-del *.exp
	-del *.exe

#------------------------------------------------------------------------

LIBOBJS = addch.obj addchstr.obj addstr.obj attr.obj beep.obj bkgd.obj \
border.obj clear.obj color.obj delch.obj deleteln.obj getch.obj \
getstr.obj getyx.obj inch.obj inchstr.obj initscr.obj inopts.obj \
insch.obj insstr.obj instr.obj kernel.obj mouse.obj move.obj outopts.obj \
overlay.obj pad.obj printw.obj refresh.obj scanw.obj scr_dump.obj \
scroll.obj slk.obj termattr.obj terminfo.obj touch.obj util.obj \
window.obj pdcdebug.obj pdcwin.obj

PDCOBJS = pdcclip.obj pdcdisp.obj pdcgetsc.obj pdckbd.obj pdcscrn.obj \
pdcsetsc.obj pdcutil.obj

PANOBJS = panel.obj

DEMOOBJS = testcurs.obj newdemo.obj xmas.obj tuidemo.obj tui.obj \
firework.obj ptest.obj rain.obj worm.obj

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(PANOBJS) ptest.obj: $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)

$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(DEMOS) : $(LIBCURSES)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) /out:$@ $(LIBOBJS) $(PDCOBJS)

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) /out:$@ $(PANOBJS)

$(CURSESDLL) : $(LIBOBJS) $(PDCOBJS) $(DEFFILE)
	$(SHL_LD) -o $(CURSESDLL) $(LIBOBJS) $(PDCOBJS)

addch.obj: $(srcdir)\addch.c
	$(CC) $(CCFLAGS) $(srcdir)\addch.c

addchstr.obj: $(srcdir)\addchstr.c
	$(CC) $(CCFLAGS) $(srcdir)\addchstr.c

addstr.obj: $(srcdir)\addstr.c
	$(CC) $(CCFLAGS) $(srcdir)\addstr.c

attr.obj: $(srcdir)\attr.c
	$(CC) $(CCFLAGS) $(srcdir)\attr.c

beep.obj: $(srcdir)\beep.c
	$(CC) $(CCFLAGS) $(srcdir)\beep.c

bkgd.obj: $(srcdir)\bkgd.c
	$(CC) $(CCFLAGS) $(srcdir)\bkgd.c

border.obj: $(srcdir)\border.c
	$(CC) $(CCFLAGS) $(srcdir)\border.c

clear.obj: $(srcdir)\clear.c
	$(CC) $(CCFLAGS) $(srcdir)\clear.c

color.obj: $(srcdir)\color.c
	$(CC) $(CCFLAGS) $(srcdir)\color.c

delch.obj: $(srcdir)\delch.c
	$(CC) $(CCFLAGS) $(srcdir)\delch.c

deleteln.obj: $(srcdir)\deleteln.c
	$(CC) $(CCFLAGS) $(srcdir)\deleteln.c

getch.obj: $(srcdir)\getch.c
	$(CC) $(CCFLAGS) $(srcdir)\getch.c

getstr.obj: $(srcdir)\getstr.c
	$(CC) $(CCFLAGS) $(srcdir)\getstr.c

getyx.obj: $(srcdir)\getyx.c
	$(CC) $(CCFLAGS) $(srcdir)\getyx.c

inch.obj: $(srcdir)\inch.c
	$(CC) $(CCFLAGS) $(srcdir)\inch.c

inchstr.obj: $(srcdir)\inchstr.c
	$(CC) $(CCFLAGS) $(srcdir)\inchstr.c

initscr.obj: $(srcdir)\initscr.c
	$(CC) $(CCFLAGS) $(srcdir)\initscr.c

inopts.obj: $(srcdir)\inopts.c
	$(CC) $(CCFLAGS) $(srcdir)\inopts.c

insch.obj: $(srcdir)\insch.c
	$(CC) $(CCFLAGS) $(srcdir)\insch.c

insstr.obj: $(srcdir)\insstr.c
	$(CC) $(CCFLAGS) $(srcdir)\insstr.c

instr.obj: $(srcdir)\instr.c
	$(CC) $(CCFLAGS) $(srcdir)\instr.c

kernel.obj: $(srcdir)\kernel.c
	$(CC) $(CCFLAGS) $(srcdir)\kernel.c

mouse.obj: $(srcdir)\mouse.c
	$(CC) $(CCFLAGS) $(srcdir)\mouse.c

move.obj: $(srcdir)\move.c
	$(CC) $(CCFLAGS) $(srcdir)\move.c

outopts.obj: $(srcdir)\outopts.c
	$(CC) $(CCFLAGS) $(srcdir)\outopts.c

overlay.obj: $(srcdir)\overlay.c
	$(CC) $(CCFLAGS) $(srcdir)\overlay.c

pad.obj: $(srcdir)\pad.c
	$(CC) $(CCFLAGS) $(srcdir)\pad.c

printw.obj: $(srcdir)\printw.c
	$(CC) $(CCFLAGS) $(srcdir)\printw.c

refresh.obj: $(srcdir)\refresh.c
	$(CC) $(CCFLAGS) $(srcdir)\refresh.c

scanw.obj: $(srcdir)\scanw.c
	$(CC) $(CCFLAGS) $(srcdir)\scanw.c

scr_dump.obj: $(srcdir)\scr_dump.c
	$(CC) $(CCFLAGS) $(srcdir)\scr_dump.c

scroll.obj: $(srcdir)\scroll.c
	$(CC) $(CCFLAGS) $(srcdir)\scroll.c

slk.obj: $(srcdir)\slk.c
	$(CC) $(CCFLAGS) $(srcdir)\slk.c

termattr.obj: $(srcdir)\termattr.c
	$(CC) $(CCFLAGS) $(srcdir)\termattr.c

terminfo.obj: $(srcdir)\terminfo.c
	$(CC) $(CCFLAGS) $(srcdir)\terminfo.c

touch.obj: $(srcdir)\touch.c
	$(CC) $(CCFLAGS) $(srcdir)\touch.c

util.obj: $(srcdir)\util.c
	$(CC) $(CCFLAGS) $(srcdir)\util.c

window.obj: $(srcdir)\window.c
	$(CC) $(CCFLAGS) $(srcdir)\window.c

pdcdebug.obj: $(srcdir)\pdcdebug.c
	$(CC) $(CCFLAGS) $(srcdir)\pdcdebug.c

pdcwin.obj: $(srcdir)\pdcwin.c
	$(CC) $(CCFLAGS) $(srcdir)\pdcwin.c


pdcclip.obj: $(osdir)\pdcclip.c
	$(CC) $(CCFLAGS) $(osdir)\pdcclip.c

pdcdisp.obj: $(osdir)\pdcdisp.c
	$(CC) $(CCFLAGS) $(osdir)\pdcdisp.c

pdcgetsc.obj: $(osdir)\pdcgetsc.c
	$(CC) $(CCFLAGS) $(osdir)\pdcgetsc.c

pdckbd.obj: $(osdir)\pdckbd.c
	$(CC) $(CCFLAGS) $(osdir)\pdckbd.c

pdcscrn.obj: $(osdir)\pdcscrn.c
	$(CC) $(CCFLAGS) $(osdir)\pdcscrn.c

pdcsetsc.obj: $(osdir)\pdcsetsc.c
	$(CC) $(CCFLAGS) $(osdir)\pdcsetsc.c

pdcutil.obj: $(osdir)\pdcutil.c
	$(CC) $(CCFLAGS) $(osdir)\pdcutil.c

#------------------------------------------------------------------------

panel.obj: $(pandir)\panel.c
	$(CC) $(CCFLAGS) $(pandir)\panel.c

#------------------------------------------------------------------------

firework.exe:   firework.obj
	$(LINK) -o $@ firework.obj $(LIBCURSES)

newdemo.exe:    newdemo.obj
	$(LINK) -o $@ newdemo.obj $(LIBCURSES)

ptest.exe:      ptest.obj $(LIBPANEL)
	$(LINK) -o $@ ptest.obj $(LIBPANEL) $(LIBCURSES)

rain.exe:       rain.obj
	$(LINK) -o $@ rain.obj $(LIBCURSES)

testcurs.exe:   testcurs.obj
	$(LINK) -o $@ testcurs.obj $(LIBCURSES)

tuidemo.exe:    tuidemo.obj tui.obj
	$(LINK) -o $@ tuidemo.obj tui.obj $(LIBCURSES)

worm.exe:       worm.obj
	$(LINK) -o $@ worm.obj $(LIBCURSES)

xmas.exe:       xmas.obj
	$(LINK) -o $@ xmas.obj $(LIBCURSES)


firework.obj: $(demodir)\firework.c
	$(CC) $(CCFLAGS) $(demodir)\firework.c

newdemo.obj: $(demodir)\newdemo.c
	$(CC) $(CCFLAGS) $(demodir)\newdemo.c

ptest.obj: $(demodir)\ptest.c
	$(CC) $(CCFLAGS) $(demodir)\ptest.c

rain.obj: $(demodir)\rain.c
	$(CC) $(CCFLAGS) $(demodir)\rain.c

testcurs.obj: $(demodir)\testcurs.c
	$(CC) $(CCFLAGS) $(demodir)\testcurs.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h
	$(CC) $(CCFLAGS) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c
	$(CC) $(CCFLAGS) -I$(demodir) $(demodir)\tuidemo.c

worm.obj: $(demodir)\worm.c
	$(CC) $(CCFLAGS) $(demodir)\worm.c

xmas.obj: $(demodir)\xmas.c
	$(CC) $(CCFLAGS) $(demodir)\xmas.c
