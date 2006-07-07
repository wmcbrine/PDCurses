################################################################################
#
# GNU MAKE Makefile for PDCurses library - WIN32 Cygnus GCC
#
# Usage: make -f [path\]gccwin32.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|panel.a|testcurs.exe...]
#
################################################################################
#
# First, set the environment variable PDCURSES_SRCDIR, or edit the line
# below; for example, "export PDCURSES_SRCDIR=c:/pdcurses".
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
osdir		= $(PDCURSES_HOME)/win32
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

CPPFLAGS	= -I$(PDCURSES_HOME) #-DUNICODE

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)
DEMOS		= testcurs.exe newdemo.exe xmas.exe tuidemo.exe \
firework.exe ptest.exe rain.exe worm.exe

SHELL		= /bin/bash

################################################################################
all:	$(PDCLIBS) $(DEMOS)
	strip *.exe

clean:
	-rm -f *.o
	-rm -f *.a
	-rm -f *.exe

demos:	$(DEMOS)

#------------------------------------------------------------------------

LIBOBJS = addch.o addchstr.o addstr.o attr.o beep.o bkgd.o border.o \
clear.o color.o delch.o deleteln.o getch.o getstr.o getyx.o inch.o \
inchstr.o initscr.o inopts.o insch.o insstr.o instr.o kernel.o mouse.o \
move.o outopts.o overlay.o pad.o printw.o refresh.o scanw.o scr_dump.o \
scroll.o slk.o termattr.o terminfo.o touch.o util.o window.o

PDCOBJS = pdcclip.o pdcdebug.o pdcdisp.o pdcgetsc.o pdckbd.o \
pdckey.o pdcscrn.o pdcsetsc.o pdcutil.o pdcwin.o

PANOBJS = panel.o


pdcurses.a : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

panel.a : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)


addch.o: $(srcdir)/addch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

addchstr.o: $(srcdir)/addchstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

addstr.o: $(srcdir)/addstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

attr.o: $(srcdir)/attr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

beep.o: $(srcdir)/beep.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

bkgd.o: $(srcdir)/bkgd.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

border.o: $(srcdir)/border.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

clear.o: $(srcdir)/clear.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

color.o: $(srcdir)/color.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

delch.o: $(srcdir)/delch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

deleteln.o: $(srcdir)/deleteln.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

getch.o: $(srcdir)/getch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

getstr.o: $(srcdir)/getstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

getyx.o: $(srcdir)/getyx.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

inch.o: $(srcdir)/inch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

inchstr.o: $(srcdir)/inchstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

initscr.o: $(srcdir)/initscr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

inopts.o: $(srcdir)/inopts.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

insch.o: $(srcdir)/insch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

insstr.o: $(srcdir)/insstr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

instr.o: $(srcdir)/instr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

kernel.o: $(srcdir)/kernel.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

mouse.o: $(srcdir)/mouse.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

move.o: $(srcdir)/move.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

outopts.o: $(srcdir)/outopts.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

overlay.o: $(srcdir)/overlay.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pad.o: $(srcdir)/pad.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

printw.o: $(srcdir)/printw.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

refresh.o: $(srcdir)/refresh.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

scanw.o: $(srcdir)/scanw.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

scr_dump.o: $(srcdir)/scr_dump.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

scroll.o: $(srcdir)/scroll.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

slk.o: $(srcdir)/slk.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

termattr.o: $(srcdir)/termattr.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

terminfo.o: $(srcdir)/terminfo.c $(PDCURSES_HEADERS) $(TERM_HEADER)
	$(CC) -c $(CCFLAGS) -o$@ $<

touch.o: $(srcdir)/touch.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

util.o: $(srcdir)/util.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

window.o: $(srcdir)/window.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<


pdcclip.o: $(osdir)/pdcclip.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdcdebug.o: $(srcdir)/pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdcdisp.o: $(osdir)/pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdcgetsc.o: $(osdir)/pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdckbd.o: $(osdir)/pdckbd.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdckey.o: $(srcdir)/pdckey.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdcscrn.o: $(osdir)/pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdcsetsc.o: $(osdir)/pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdcutil.o: $(osdir)/pdcutil.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

pdcwin.o: $(srcdir)/pdcwin.c $(PDCURSES_HEADERS)
	$(CC) -c $(CCFLAGS) -o$@ $<

#------------------------------------------------------------------------

panel.o: $(pandir)/panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) -c $(CCFLAGS) -o$@ $<

#------------------------------------------------------------------------

firework.exe: $(demodir)/firework.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)

newdemo.exe: $(demodir)/newdemo.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)

ptest.exe: $(demodir)/ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H) $(LIBCURSES) $(LIBPANEL)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES) $(LIBPANEL)

rain.exe: $(demodir)/rain.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)

testcurs.exe: $(demodir)/testcurs.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)

tuidemo.exe: tuidemo.o tui.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o tuidemo.exe tuidemo.o tui.o $(LIBCURSES)

worm.exe: $(demodir)/worm.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)

xmas.exe: $(demodir)/xmas.c $(PDCURSES_CURSES_H) $(LIBCURSES)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)


tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o $@ $<


dist: $(PDCLIBS)
	echo PDCurses $(VERDOT) for Cygnus Win32 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo Cygnus GCC for Win32. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_cyg_w32 \
	$(PDCURSES_HOME)/README $(PDCURSES_HOME)/HISTORY \
	$(PDCURSES_HOME)/maintain.er \
	$(PDCURSES_HOME)/curses.h $(PDCURSES_HOME)/curspriv.h \
	$(PDCURSES_HOME)/panel.h $(PDCURSES_HOME)/term.h \
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	rm file_id.diz
