################################################################################
#
# GNU MAKE Makefile for PDCurses library - WIN32 MingW32 GCC
#
# Usage: make -f [path\]gccwin32.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|panel.a|testcurs.exe...]
#
################################################################################
#
# Change these for your environment...
#
################################################################################
PDCURSES_HOME		=f:/curses
CC_HOME		=f:/mingw32
################################################################################
# Nothing below here should required changing.
################################################################################

PDCURSES_CURSES_H		=$(PDCURSES_HOME)/curses.h
PDCURSES_CURSPRIV_H		=$(PDCURSES_HOME)/curspriv.h
PDCURSES_HEADERS		=$(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		=$(PDCURSES_HOME)/panel.h

CCLIBDIR		=$(CC_HOME)/lib
CCINCDIR		=$(CC_HOME)/include

srcdir		= $(PDCURSES_HOME)/pdcurses
osdir		= $(PDCURSES_HOME)/win32
pandir		= $(PDCURSES_HOME)/panel
demodir		= $(PDCURSES_HOME)/demos

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -c -g -Wall -DPDCDEBUG -D_NO_OLDNAMES
	LDFLAGS = -g
else
	CFLAGS  = -c -O -Wall -D_NO_OLDNAMES
	LDFLAGS =
endif

CPPFLAGS	= -I$(PDCURSES_HOME) -I$(CCINCDIR)

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc
COFF2EXE	=coff2exe $*

CCLIBS		= -L$(CCLIBDIR) -lkernel32 -luser32

LIBEXE		= ar
LIBFLAGS		=rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS	= $(LIBCURSES) $(LIBPANEL)
DEMOS	=testcurs.exe newdemo.exe xmas.exe tuidemo.exe firework.exe

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.o
	-del curses.lib
	-del panel.lib
	-del *.exe

demos:	$(DEMOS)

#------------------------------------------------------------------------

LIBOBJS =     \
addch.o     \
addchstr.o  \
addstr.o    \
attr.o      \
beep.o      \
bkgd.o      \
border.o    \
clear.o     \
color.o     \
delch.o     \
deleteln.o  \
getch.o     \
getstr.o    \
getyx.o     \
inch.o      \
inchstr.o   \
initscr.o   \
inopts.o    \
insch.o     \
insstr.o    \
instr.o     \
kernel.o    \
mouse.o     \
move.o      \
outopts.o   \
overlay.o   \
pad.o       \
printw.o    \
refresh.o   \
scanw.o     \
scroll.o    \
slk.o       \
termattr.o  \
terminfo.o  \
touch.o     \
util.o      \
window.o

PDCOBJS =   \
pdcclip.o   \
pdcdebug.o  \
pdcdisp.o   \
pdcgetsc.o  \
pdckbd.o    \
pdcprint.o  \
pdcscrn.o   \
pdcsetsc.o  \
pdcutil.o   \
pdcwin.o

PANOBJS =     \
panel.o

pdcurses.a : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ *.o
	$(LIBEXE) d $@ $(PANOBJS) firework.o newdemo.o ptest.o testcurs.o tui.o tuidemo.o xmas.o

panel.a : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

addch.o: $(srcdir)/addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/addch.c

addchstr.o: $(srcdir)/addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/addchstr.c

addstr.o: $(srcdir)/addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/addstr.c

attr.o: $(srcdir)/attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/attr.c

beep.o: $(srcdir)/beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/beep.c

bkgd.o: $(srcdir)/bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/bkgd.c

border.o: $(srcdir)/border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/border.c

clear.o: $(srcdir)/clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/clear.c

color.o: $(srcdir)/color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/color.c

delch.o: $(srcdir)/delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/delch.c

deleteln.o: $(srcdir)/deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/deleteln.c

getch.o: $(srcdir)/getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/getch.c

getstr.o: $(srcdir)/getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/getstr.c

getyx.o: $(srcdir)/getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/getyx.c

inch.o: $(srcdir)/inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/inch.c

inchstr.o: $(srcdir)/inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/inchstr.c

initscr.o: $(srcdir)/initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/initscr.c

inopts.o: $(srcdir)/inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/inopts.c

insch.o: $(srcdir)/insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/insch.c

insstr.o: $(srcdir)/insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/insstr.c

instr.o: $(srcdir)/instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/instr.c

kernel.o: $(srcdir)/kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/kernel.c

mouse.o: $(srcdir)/mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/mouse.c

move.o: $(srcdir)/move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/move.c

outopts.o: $(srcdir)/outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/outopts.c

overlay.o: $(srcdir)/overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/overlay.c

pad.o: $(srcdir)/pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/pad.c

printw.o: $(srcdir)/printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/printw.c

refresh.o: $(srcdir)/refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/refresh.c

scanw.o: $(srcdir)/scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/scanw.c

scroll.o: $(srcdir)/scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/scroll.c

slk.o: $(srcdir)/slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/slk.c

termattr.o: $(srcdir)/termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/termattr.c

terminfo.o: $(srcdir)/terminfo.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/terminfo.c

touch.o: $(srcdir)/touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/touch.c

util.o: $(srcdir)/util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/util.c

window.o: $(srcdir)/window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/window.c


pdcclip.o: $(osdir)/pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdcclip.c

pdcdebug.o: $(srcdir)/pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/pdcdebug.c

pdcdisp.o: $(osdir)/pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdcdisp.c

pdcgetsc.o: $(osdir)/pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdcgetsc.c

pdckbd.o: $(osdir)/pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdckbd.c

pdcprint.o: $(osdir)/pdcprint.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdcprint.c

pdcscrn.o: $(osdir)/pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdcscrn.c

pdcsetsc.o: $(osdir)/pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdcsetsc.c

pdcutil.o: $(srcdir)/pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/pdcutil.c

pdcwin.o: $(srcdir)/pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)/pdcwin.c

#------------------------------------------------------------------------

panel.o: $(pandir)/panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(CCFLAGS) -o$@ $(pandir)/panel.c

#------------------------------------------------------------------------

firework.exe:	firework.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o firework.exe firework.o $(LIBCURSES) $(CCLIBS)

newdemo.exe:	newdemo.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o newdemo.exe newdemo.o $(LIBCURSES) $(CCLIBS)

ptest.exe:	ptest.o $(LIBCURSES) $(LIBPANEL)
	$(LINK) $(LDFLAGS) -o ptest.exe ptest.o $(LIBCURSES) $(LIBPANEL) $(CCLIBS)

testcurs.exe:	testcurs.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o testcurs.exe testcurs.o $(LIBCURSES) $(CCLIBS)

tuidemo.exe:	tuidemo.o tui.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o tuidemo.exe tuidemo.o tui.o $(LIBCURSES) $(CCLIBS)

xmas.exe:	xmas.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o xmas.exe xmas.o $(LIBCURSES) $(CCLIBS)


firework.o: $(demodir)/firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/firework.c

newdemo.o: $(demodir)/newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/newdemo.c

ptest.o: $(demodir)/ptest.c $(PDCURSES_HOME)/panel.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/ptest.c

testcurs.o: $(demodir)/testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/testcurs.c

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)/tui.c

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)/tuidemo.c

xmas.o: $(demodir)/xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/xmas.c
