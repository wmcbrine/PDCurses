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
# Change these for your environment...
#
###############################################################################

PDCURSES_HOME		=\curses
CC_HOME=\lcc
###############################################################################

# Nothing below here should required changing.
###############################################################################
PDCURSES_CURSES_H         =$(PDCURSES_HOME)\curses.h
PDCURSES_CURSPRIV_H         =$(PDCURSES_HOME)\curspriv.h
PDCURSES_HEADERS              =$(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)

PANEL_HEADER      =$(PDCURSES_HOME)\panel.h

CCLIBDIR              =$(CC_HOME)\lib
CCINCDIR              =$(CC_HOME)\include

srcdir          = $(PDCURSES_HOME)\pdcurses
dosdir          = $(PDCURSES_HOME)\win32
pandir          = $(PDCURSES_HOME)\panel
demodir= $(PDCURSES_HOME)\demos

CC          = lcc

#CFLAGS  = -c -g3 -D__LCC__
CFLAGS  = -c -O -D__LCC__
LDFLAGS =

CPPFLAGS        = -I$(PDCURSES_HOME) -I$(CCINCDIR)

CCFLAGS= $(CFLAGS) $(CPPFLAGS)

LINK      = lcclnk

CCLIBS          = -L$(CCLIBDIR) -lkernel32 -luser32

LIBEXE          = lcclib
LIBFLAGS        =

LIBCURSES       = pdcurses.lib
LIBPANEL        = panel.lib

PDCLIBS= $(LIBCURSES) $(LIBPANEL)
DEMOS   =testcurs.exe newdemo.exe xmas.exe tuidemo.exe firework.exe

###############################################################################

all:    $(PDCLIBS) $(DEMOS)

clean:
	-del *.obj
	-del pdcurses.lib
	-del panel.lib
	-del *.exe

demos:  $(DEMOS)

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

PDCOBJS =   \
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

pdcurses.lib : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) /out:$@ $(LIBOBJS) $(PDCOBJS)

panel.lib : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

addch.obj: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\addch.c

addchstr.obj: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\addchstr.c

addstr.obj: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\addstr.c

attr.obj: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\attr.c

beep.obj: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\beep.c

bkgd.obj: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\bkgd.c

border.obj: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\border.c

clear.obj: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\clear.c

color.obj: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\color.c

delch.obj: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\delch.c

deleteln.obj: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\deleteln.c

getch.obj: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\getch.c

getstr.obj: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\getstr.c

getyx.obj: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\getyx.c

inch.obj: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\inch.c

inchstr.obj: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\inchstr.c

initscr.obj: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\initscr.c

inopts.obj: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\inopts.c

insch.obj: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\insch.c

insstr.obj: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\insstr.c

instr.obj: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\instr.c

kernel.obj: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\kernel.c

mouse.obj: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\mouse.c

move.obj: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\move.c

outopts.obj: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\outopts.c

overlay.obj: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\overlay.c

pad.obj: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pad.c

printw.obj: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\printw.c

refresh.obj: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\refresh.c

scanw.obj: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\scanw.c

scroll.obj: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\scroll.c

slk.obj: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\slk.c

termattr.obj: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\termattr.c

terminfo.obj: $(srcdir)\terminfo.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\terminfo.c

touch.obj: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\touch.c

util.obj: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\util.c

window.obj: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\window.c

pdcclip.obj: $(dosdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(dosdir)\pdcclip.c

pdcdebug.obj: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pdcdebug.c

pdcdisp.obj: $(dosdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(dosdir)\pdcdisp.c

pdcgetsc.obj: $(dosdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(dosdir)\pdcgetsc.c

pdckbd.obj: $(dosdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(dosdir)\pdckbd.c

pdcprint.obj: $(dosdir)\pdcprint.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(dosdir)\pdcprint.c

pdcscrn.obj: $(dosdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(dosdir)\pdcscrn.c

pdcsetsc.obj: $(dosdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(dosdir)\pdcsetsc.c

pdcutil.obj: $(srcdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pdcutil.c

pdcwin.obj: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pdcwin.c

#------------------------------------------------------------------------

panel.obj: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(CCFLAGS) -o$@ $(pandir)\panel.c

#------------------------------------------------------------------------

firework.exe:   firework.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o firework.exe firework.obj $(LIBCURSES) $(CCLIBS)

newdemo.exe:    newdemo.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o newdemo.exe newdemo.obj $(LIBCURSES) $(CCLIBS)

ptest.exe:      ptest.obj $(LIBCURSES) $(LIBPANEL)
	$(LINK) $(LDFLAGS) -o ptest.exe ptest.obj $(LIBCURSES) $(LIBPANEL) $(CCLIBS)

testcurs.exe:   testcurs.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o testcurs.exe testcurs.obj $(LIBCURSES) $(CCLIBS)

tuidemo.exe:    tuidemo.obj tui.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o tuidemo.exe tuidemo.obj tui.obj $(LIBCURSES) $(CCLIBS)

xmas.exe:       xmas.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o xmas.exe xmas.obj $(LIBCURSES) $(CCLIBS)


firework.obj: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\firework.c

newdemo.obj: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\newdemo.c

ptest.obj: $(demodir)\ptest.c $(PDCURSES_HOME)\panel.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\ptest.c

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\testcurs.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)\tuidemo.c

xmas.obj: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\xmas.c
