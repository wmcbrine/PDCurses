################################################################################
#
# GNU MAKE (3.79.1) Makefile for PDCurses library - DOS DJGPP V2.0+
#
# Usage: make -f [path\]gccdos.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|libs|demos|dist|pdcurses.a|testcurs.exe...]
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

O = o

include $(PDCURSES_HOME)/version.mif
include $(PDCURSES_HOME)/libobjs.mif

osdir		= $(PDCURSES_HOME)/dos

PDCURSES_DOS_H	= $(osdir)/pdcdos.h

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CFLAGS += -I$(PDCURSES_HOME)

LINK		= gcc

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a

################################################################################
.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(LIBCURSES)

clean:
	-del *.o
	-del $(LIBCURSES)
	-del *.exe

demos:	$(DEMOS)
	strip *.exe

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_DOS_H)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
panel.o : $(PANEL_HEADER)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) $<

#------------------------------------------------------------------------

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe \
ptest.exe: %.exe: $(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $< $(LIBCURSES)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o$@ tuidemo.o tui.o $(LIBCURSES)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CFLAGS) -I$(demodir) -o$@ $<

#------------------------------------------------------------------------

PLATFORM1 = DJGPP 2.03
PLATFORM2 = DJGPP 2.03 for DOS
ARCNAME = pdc$(VER)djg

include $(PDCURSES_HOME)/makedist.mif
