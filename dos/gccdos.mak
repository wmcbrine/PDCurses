################################################################################
#
# GNU MAKE (3.7.3) Makefile for PDCurses library - DOS DJGPP V2.0
#
# Usage: make -f [path\]gccdos.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|panel.a|testcurs.exe...]
#
################################################################################
#
# Change these for your environment...
#
################################################################################
PDCURSES_HOME		=c:/curses
PDC_HOME		=c:\curses
CC_HOME		=c:/djgpp
################################################################################
# Nothing below here should required changing.
################################################################################
VER=26
VERDOT=2.6

PDCURSES_CURSES_H		=$(PDCURSES_HOME)/curses.h
PDCURSES_CURSPRIV_H		=$(PDCURSES_HOME)/curspriv.h
PDCURSES_HEADERS		=$(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		=$(PDCURSES_HOME)/panel.h

CCLIBDIR		=$(CC_HOME)/lib
CCINCDIR		=$(CC_HOME)/include

srcdir		= $(PDCURSES_HOME)/pdcurses
osdir		= $(PDCURSES_HOME)/dos
pandir		= $(PDCURSES_HOME)/panel
demodir		= $(PDCURSES_HOME)/demos

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -c -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -c -O -Wall
	LDFLAGS =
endif

CPPFLAGS	= -I$(PDCURSES_HOME) -I$(CCINCDIR) -D_NAIVE_DOS_REGS

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc
COFF2EXE	=coff2exe

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
pdcgo32.o   \
pdckbd.o    \
pdcprint.o  \
pdcscrn.o   \
pdcsetsc.o  \
pdcutil.o   \
pdcwin.o

PANOBJS =     \
panel.o

pdcurses.a : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

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

pdcgo32.o: $(osdir)/pdcgo32.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)/pdcgo32.c

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
	$(LINK) $(LDFLAGS) -o firework firework.o $(LIBCURSES)
	$(COFF2EXE) firework
	strip $@

newdemo.exe:	newdemo.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o newdemo newdemo.o $(LIBCURSES)
	$(COFF2EXE) newdemo
	strip $@

ptest.exe:	ptest.o $(LIBCURSES) $(LIBPANEL)
	$(LINK) $(LDFLAGS) -o ptest ptest.o $(LIBCURSES) $(LIBPANEL)
	$(COFF2EXE) ptest
	strip $@

testcurs.exe:	testcurs.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o testcurs testcurs.o $(LIBCURSES)
	$(COFF2EXE) testcurs
	strip $@

tuidemo.exe:	tuidemo.o tui.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o tuidemo tuidemo.o tui.o $(LIBCURSES)
	$(COFF2EXE) tuidemo
	strip $@

xmas.exe:	xmas.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o xmas xmas.o $(LIBCURSES)
	$(COFF2EXE) xmas
	strip $@


firework.o: $(demodir)/firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/firework.c

newdemo.o: $(demodir)/newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/newdemo.c

ptest.o: $(demodir)/ptest.c $(PDCURSES_HOME)\panel.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/ptest.c

testcurs.o: $(demodir)/testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/testcurs.c

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)/tui.c

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)/tuidemo.c

xmas.o: $(demodir)/xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/xmas.c

dist: pdcurses.a panel.a
	-mkdir tmp
	cd tmp
	copy $(PDC_HOME)\README README
	copy $(PDC_HOME)\readme.?? .
	copy $(PDC_HOME)\curses.h .
	copy $(PDC_HOME)\curspriv.h .
	copy $(PDC_HOME)\maintain.er .
	copy ..\pdcurses.a .
	copy ..\panel.a .
	echo 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴 > file_id.diz
	echo      PDCurses - Public Domain Curses >> file_id.diz
	echo  Version $(VERDOT) for DJGPP 2.0 PDC$(VER)DJG.ZIP >> file_id.diz
	echo  ------------------------------------------ >> file_id.diz
	echo  Public Domain Curses library for >> file_id.diz
	echo  DJGPP 2.0. >> file_id.diz
	echo  Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo  Public Domain. >> file_id.diz
	echo 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴 >> file_id.diz
	zip pdc$(VER)djg *.*
	cd ..
