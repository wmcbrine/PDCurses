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

PDCURSES_WIN_H	= $(osdir)\pdcwin.h

CC		= lcc

#CFLAGS		= -c -g3 -A -ansic
CFLAGS		= -c -O -A -ansic

CPPFLAGS	= -I$(PDCURSES_HOME) #-DPDC_WIDE -DPDC_FORCE_UTF8

LINK		= lcclnk

DEFFILE		= $(osdir)\curses_lcc.def
#DEFFILE		= $(osdir)\cursesw_lcc.def
SHL_LD		= lcclnk -DLL $(DEFFILE)

LIBEXE		= lcclib

LIBCURSES	= pdcurses.lib
CURSESDLL	= pdcurses.dll
LIBPANEL	= panel.lib

# For a static build:

BUILD		= $(CC) $(CFLAGS) $(CPPFLAGS)
PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

# For a DLL build:

#BUILD		= $(CC) $(CFLAGS) $(CPPFLAGS) -DPDC_DLL_BUILD
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
border.obj clear.obj color.obj delch.obj deleteln.obj deprec.obj getch.obj \
getstr.obj getyx.obj inch.obj inchstr.obj initscr.obj inopts.obj \
insch.obj insstr.obj instr.obj kernel.obj keyname.obj mouse.obj move.obj \
outopts.obj overlay.obj pad.obj printw.obj refresh.obj scanw.obj \
scr_dump.obj scroll.obj slk.obj termattr.obj terminfo.obj touch.obj util.obj \
window.obj debug.obj

PDCOBJS = pdcclip.obj pdcdisp.obj pdcgetsc.obj pdckbd.obj pdcscrn.obj \
pdcsetsc.obj pdcutil.obj

PANOBJS = panel.obj

DEMOOBJS = testcurs.obj newdemo.obj xmas.obj tuidemo.obj tui.obj \
firework.obj ptest.obj rain.obj worm.obj

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_WIN_H)
$(PANOBJS) ptest.obj: $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)

$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(DEMOS) : $(LIBCURSES)

# Remove the next two lines for a DLL build:
$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) /out:$@ $(LIBOBJS) $(PDCOBJS)

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) /out:$@ $(PANOBJS)

$(CURSESDLL) : $(LIBOBJS) $(PDCOBJS) $(DEFFILE)
	$(SHL_LD) -o $(CURSESDLL) $(LIBOBJS) $(PDCOBJS)

addch.obj: $(srcdir)\addch.c
	$(BUILD) $(srcdir)\$*.c

addchstr.obj: $(srcdir)\addchstr.c
	$(BUILD) $(srcdir)\$*.c

addstr.obj: $(srcdir)\addstr.c
	$(BUILD) $(srcdir)\$*.c

attr.obj: $(srcdir)\attr.c
	$(BUILD) $(srcdir)\$*.c

beep.obj: $(srcdir)\beep.c
	$(BUILD) $(srcdir)\$*.c

bkgd.obj: $(srcdir)\bkgd.c
	$(BUILD) $(srcdir)\$*.c

border.obj: $(srcdir)\border.c
	$(BUILD) $(srcdir)\$*.c

clear.obj: $(srcdir)\clear.c
	$(BUILD) $(srcdir)\$*.c

color.obj: $(srcdir)\color.c
	$(BUILD) $(srcdir)\$*.c

delch.obj: $(srcdir)\delch.c
	$(BUILD) $(srcdir)\$*.c

deleteln.obj: $(srcdir)\deleteln.c
	$(BUILD) $(srcdir)\$*.c

deprec.obj: $(srcdir)\deprec.c
	$(BUILD) $(srcdir)\$*.c

getch.obj: $(srcdir)\getch.c
	$(BUILD) $(srcdir)\$*.c

getstr.obj: $(srcdir)\getstr.c
	$(BUILD) $(srcdir)\$*.c

getyx.obj: $(srcdir)\getyx.c
	$(BUILD) $(srcdir)\$*.c

inch.obj: $(srcdir)\inch.c
	$(BUILD) $(srcdir)\$*.c

inchstr.obj: $(srcdir)\inchstr.c
	$(BUILD) $(srcdir)\$*.c

initscr.obj: $(srcdir)\initscr.c
	$(BUILD) $(srcdir)\$*.c

inopts.obj: $(srcdir)\inopts.c
	$(BUILD) $(srcdir)\$*.c

insch.obj: $(srcdir)\insch.c
	$(BUILD) $(srcdir)\$*.c

insstr.obj: $(srcdir)\insstr.c
	$(BUILD) $(srcdir)\$*.c

instr.obj: $(srcdir)\instr.c
	$(BUILD) $(srcdir)\$*.c

kernel.obj: $(srcdir)\kernel.c
	$(BUILD) $(srcdir)\$*.c

keyname.obj: $(srcdir)\keyname.c
	$(BUILD) $(srcdir)\$*.c

mouse.obj: $(srcdir)\mouse.c
	$(BUILD) $(srcdir)\$*.c

move.obj: $(srcdir)\move.c
	$(BUILD) $(srcdir)\$*.c

outopts.obj: $(srcdir)\outopts.c
	$(BUILD) $(srcdir)\$*.c

overlay.obj: $(srcdir)\overlay.c
	$(BUILD) $(srcdir)\$*.c

pad.obj: $(srcdir)\pad.c
	$(BUILD) $(srcdir)\$*.c

printw.obj: $(srcdir)\printw.c
	$(BUILD) $(srcdir)\$*.c

refresh.obj: $(srcdir)\refresh.c
	$(BUILD) $(srcdir)\$*.c

scanw.obj: $(srcdir)\scanw.c
	$(BUILD) $(srcdir)\$*.c

scr_dump.obj: $(srcdir)\scr_dump.c
	$(BUILD) $(srcdir)\$*.c

scroll.obj: $(srcdir)\scroll.c
	$(BUILD) $(srcdir)\$*.c

slk.obj: $(srcdir)\slk.c
	$(BUILD) $(srcdir)\$*.c

termattr.obj: $(srcdir)\termattr.c
	$(BUILD) $(srcdir)\$*.c

terminfo.obj: $(srcdir)\terminfo.c
	$(BUILD) $(srcdir)\$*.c

touch.obj: $(srcdir)\touch.c
	$(BUILD) $(srcdir)\$*.c

util.obj: $(srcdir)\util.c
	$(BUILD) $(srcdir)\$*.c

window.obj: $(srcdir)\window.c
	$(BUILD) $(srcdir)\$*.c

debug.obj: $(srcdir)\debug.c
	$(BUILD) $(srcdir)\$*.c


pdcclip.obj: $(osdir)\pdcclip.c
	$(BUILD) $(osdir)\$*.c

pdcdisp.obj: $(osdir)\pdcdisp.c
	$(BUILD) $(osdir)\$*.c

pdcgetsc.obj: $(osdir)\pdcgetsc.c
	$(BUILD) $(osdir)\$*.c

pdckbd.obj: $(osdir)\pdckbd.c
	$(BUILD) $(osdir)\$*.c

pdcscrn.obj: $(osdir)\pdcscrn.c
	$(BUILD) $(osdir)\$*.c

pdcsetsc.obj: $(osdir)\pdcsetsc.c
	$(BUILD) $(osdir)\$*.c

pdcutil.obj: $(osdir)\pdcutil.c
	$(BUILD) $(osdir)\$*.c

#------------------------------------------------------------------------

panel.obj: $(pandir)\panel.c
	$(BUILD) $(pandir)\$*.c

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
	$(BUILD) $(demodir)\$*.c

newdemo.obj: $(demodir)\newdemo.c
	$(BUILD) $(demodir)\$*.c

ptest.obj: $(demodir)\ptest.c
	$(BUILD) $(demodir)\$*.c

rain.obj: $(demodir)\rain.c
	$(BUILD) $(demodir)\$*.c

testcurs.obj: $(demodir)\testcurs.c
	$(BUILD) $(demodir)\$*.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h
	$(BUILD) -I$(demodir) $(demodir)\$*.c

tuidemo.obj: $(demodir)\tuidemo.c
	$(BUILD) -I$(demodir) $(demodir)\$*.c

worm.obj: $(demodir)\worm.c
	$(BUILD) $(demodir)\$*.c

xmas.obj: $(demodir)\xmas.c
	$(BUILD) $(demodir)\$*.c
