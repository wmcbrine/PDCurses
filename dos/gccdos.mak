# GNU MAKE (3.79.1) Makefile for PDCurses library - DOS DJGPP V2.0+
#
# Usage: make -f [path\]gccdos.mak [DEBUG=Y] [CROSS=[build]] [target]
#
# where "target" can be any of:
# [all|libs|demos|dist|pdcurses.a|testcurs.exe...]
#
# and "build" any installed cross-compiler (gcc + binutils),
#  CROSS=Y defaults to i586-pc-msdosdjgpp build
# Note: when cross-compiling from GNU/Linux set CROSS=Y even for make clean

O = o

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/version.mif
include $(PDCURSES_SRCDIR)/libobjs.mif

osdir		= $(PDCURSES_SRCDIR)/dos

PDCURSES_DOS_H	= $(osdir)/pdcdos.h

ifndef CROSS
	COPY	= copy
	DEL		= del
else
	COPY	= cp
	DEL		= rm -rf
endif

ifeq ($(CROSS),Y)
	PREFIX	= i586-pc-msdosdjgpp-
else
	ifdef CROSS
		PREFIX	= $(CROSS)-
	endif
endif

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CFLAGS += -I$(PDCURSES_SRCDIR)


CC		= $(PREFIX)gcc
LINK		= $(PREFIX)gcc

LIBEXE		= $(PREFIX)ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a

.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(LIBCURSES)

clean:
	-$(DEL) *.o
	-$(DEL) *.a
	-$(DEL) *.exe

demos:	$(DEMOS)
	$(PREFIX)strip *.exe

# note: we always want the system specific objects to be first
#       as these are most likely to raise compiler issues 
$(LIBCURSES) : $(PDCOBJS) $(LIBOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $?
	-$(COPY) $(LIBCURSES) panel.a

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_DOS_H)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
panel.o : $(PANEL_HEADER)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) $<

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe \
ptest.exe: %.exe: $(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $< $(LIBCURSES)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o$@ tuidemo.o tui.o $(LIBCURSES)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

PLATFORM1 = DJGPP 2.03
PLATFORM2 = DJGPP 2.03 for DOS
ARCNAME = pdc$(VER)djg

include $(PDCURSES_SRCDIR)/makedist.mif
