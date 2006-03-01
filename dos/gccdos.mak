################################################################################
#
# GNU MAKE (3.79.1) Makefile for PDCurses library - DOS DJGPP V2.0+
#
# Usage: make -f [path\]gccdos.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|libs|demos|dist|pdcurses.a|panel.a|testcurs.exe...]
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

include $(PDCURSES_HOME)/version

PDCURSES_CURSES_H	= $(PDCURSES_HOME)/curses.h
PDCURSES_CURSPRIV_H	= $(PDCURSES_HOME)/curspriv.h
PDCURSES_HEADERS	= $(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		= $(PDCURSES_HOME)/panel.h
TERM_HEADER		= $(PDCURSES_HOME)/term.h

srcdir		= $(PDCURSES_HOME)/pdcurses
osdir		= $(PDCURSES_HOME)/dos
pandir		= $(PDCURSES_HOME)/panel
demodir		= $(PDCURSES_HOME)/demos

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CPPFLAGS	= -I$(PDCURSES_HOME) -D_NAIVE_DOS_REGS -funsigned-char

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)
DEMOS		= testcurs.exe newdemo.exe xmas.exe tuidemo.exe \
firework.exe ptest.exe rain.exe worm.exe

################################################################################
.PHONY: all libs clean demos dist

all:	$(PDCLIBS) $(DEMOS)

libs:	$(PDCLIBS)

clean:
	-del *.o
	-del $(LIBCURSES)
	-del $(LIBPANEL)
	-del *.exe

demos:	$(DEMOS)

#------------------------------------------------------------------------

LIBOBJS = addch.o addchstr.o addstr.o attr.o beep.o bkgd.o border.o \
clear.o color.o delch.o deleteln.o getch.o getstr.o getyx.o inch.o \
inchstr.o initscr.o inopts.o insch.o insstr.o instr.o kernel.o mouse.o \
move.o outopts.o overlay.o pad.o printw.o refresh.o scanw.o scr_dump.o \
scroll.o slk.o termattr.o terminfo.o touch.o util.o window.o

PDCOBJS = pdcclip.o pdcdebug.o pdcdisp.o pdcgetsc.o pdcgo32.o pdckbd.o \
pdcprint.o pdcscrn.o pdcsetsc.o pdcutil.o pdcwin.o

PANOBJS = panel.o

#------------------------------------------------------------------------

pdcurses.a : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

panel.a : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)


addch.o: $(srcdir)/addch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/addch.c

addchstr.o: $(srcdir)/addchstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/addchstr.c

addstr.o: $(srcdir)/addstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/addstr.c

attr.o: $(srcdir)/attr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/attr.c

beep.o: $(srcdir)/beep.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/beep.c

bkgd.o: $(srcdir)/bkgd.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/bkgd.c

border.o: $(srcdir)/border.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/border.c

clear.o: $(srcdir)/clear.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/clear.c

color.o: $(srcdir)/color.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/color.c

delch.o: $(srcdir)/delch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/delch.c

deleteln.o: $(srcdir)/deleteln.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/deleteln.c

getch.o: $(srcdir)/getch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/getch.c

getstr.o: $(srcdir)/getstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/getstr.c

getyx.o: $(srcdir)/getyx.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/getyx.c

inch.o: $(srcdir)/inch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/inch.c

inchstr.o: $(srcdir)/inchstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/inchstr.c

initscr.o: $(srcdir)/initscr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/initscr.c

inopts.o: $(srcdir)/inopts.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/inopts.c

insch.o: $(srcdir)/insch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/insch.c

insstr.o: $(srcdir)/insstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/insstr.c

instr.o: $(srcdir)/instr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/instr.c

kernel.o: $(srcdir)/kernel.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/kernel.c

mouse.o: $(srcdir)/mouse.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/mouse.c

move.o: $(srcdir)/move.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/move.c

outopts.o: $(srcdir)/outopts.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/outopts.c

overlay.o: $(srcdir)/overlay.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/overlay.c

pad.o: $(srcdir)/pad.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/pad.c

printw.o: $(srcdir)/printw.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/printw.c

refresh.o: $(srcdir)/refresh.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/refresh.c

scanw.o: $(srcdir)/scanw.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/scanw.c

scr_dump.o: $(srcdir)/scr_dump.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/scr_dump.c

scroll.o: $(srcdir)/scroll.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/scroll.c

slk.o: $(srcdir)/slk.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/slk.c

termattr.o: $(srcdir)/termattr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/termattr.c

terminfo.o: $(srcdir)/terminfo.c $(PDCURSES_HEADERS) $(TERM_HEADER)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/terminfo.c

touch.o: $(srcdir)/touch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/touch.c

util.o: $(srcdir)/util.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/util.c

window.o: $(srcdir)/window.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/window.c


pdcclip.o: $(osdir)/pdcclip.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdcclip.c

pdcdebug.o: $(srcdir)/pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/pdcdebug.c

pdcdisp.o: $(osdir)/pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdcdisp.c

pdcgetsc.o: $(osdir)/pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdcgetsc.c

pdcgo32.o: $(osdir)/pdcgo32.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdcgo32.c

pdckbd.o: $(osdir)/pdckbd.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdckbd.c

pdcprint.o: $(osdir)/pdcprint.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdcprint.c

pdcscrn.o: $(osdir)/pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdcscrn.c

pdcsetsc.o: $(osdir)/pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(osdir)/pdcsetsc.c

pdcutil.o: $(srcdir)/pdcutil.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/pdcutil.c

pdcwin.o: $(srcdir)/pdcwin.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $(srcdir)/pdcwin.c

#------------------------------------------------------------------------

panel.o: $(pandir)/panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) -c $(CCFLAGS) -o$@ $(pandir)/panel.c

#------------------------------------------------------------------------

firework.exe: $(demodir)/firework.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/firework.c $(LIBCURSES)
	strip $@

newdemo.exe: $(demodir)/newdemo.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/newdemo.c $(LIBCURSES)
	strip $@

ptest.exe: $(demodir)/ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H) $(LIBCURSES) $(LIBPANEL)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/ptest.c $(LIBCURSES) $(LIBPANEL)
	strip $@

rain.exe: $(demodir)/rain.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/rain.c $(LIBCURSES)
	strip $@

testcurs.exe: $(demodir)/testcurs.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/testcurs.c $(LIBCURSES)
	strip $@

tuidemo.exe: tuidemo.o tui.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o$@ tuidemo.o tui.o $(LIBCURSES)
	strip $@

worm.exe: $(demodir)/worm.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/worm.c $(LIBCURSES)
	strip $@

xmas.exe: $(demodir)/xmas.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $(demodir)/xmas.c $(LIBCURSES)
	strip $@


tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o$@ $(demodir)/tui.c

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o$@ $(demodir)/tuidemo.c

#------------------------------------------------------------------------

dist: $(PDCLIBS)
	echo PDCurses $(VERDOT) for DJGPP 2.03 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo DJGPP 2.03 for DOS. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	echo $(PDCURSES_HOME)\README > flist
	echo $(PDCURSES_HOME)\HISTORY >> flist
	echo $(PDCURSES_HOME)\maintain.er >> flist
	echo $(PDCURSES_HOME)\curses.h >> flist
	echo $(PDCURSES_HOME)\curspriv.h >> flist
	echo $(PDCURSES_HOME)\panel.h >> flist
	echo $(PDCURSES_HOME)\term.h >> flist
	echo $(LIBCURSES) >> flist
	echo $(LIBPANEL) >> flist
	echo file_id.diz >> flist
	zip -9jX pdc$(VER)djg -@ <flist
	del flist
	del file_id.diz
