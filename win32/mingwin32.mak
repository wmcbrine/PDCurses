################################################################################
#
# GNU MAKE Makefile for PDCurses library - WIN32 MinGW GCC
#
# Usage: make -f [path\]gccwin32.mak [-DDEBUG] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|panel.a|testcurs.exe...]
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

osdir		= $(PDCURSES_HOME)/win32

CC		= gcc

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -O2 -Wall
	LDFLAGS =
endif

CPPFLAGS	= -I$(PDCURSES_HOME) -DPDC_WIDE

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS		= $(LIBCURSES) $(LIBPANEL)

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.o
	-del *.a
	-del *.exe

demos:	$(DEMOS)
	strip *.exe

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

$(LIBOBJS) $(PDCOBJS) $(PANOBJS) : $(PDCURSES_HEADERS)
$(PANOBJS) : $(PANEL_HEADER)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CCFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CCFLAGS) $<

$(PANOBJS) : %.o: $(pandir)/%.c
	$(CC) -c $(CCFLAGS) $<

#------------------------------------------------------------------------

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe: \
%.exe: $(demodir)/%.c
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES)

ptest.exe: $(demodir)/ptest.c $(PANEL_HEADER) $(LIBPANEL)
	$(CC) $(CCFLAGS) -o$@ $< $(LIBCURSES) $(LIBPANEL)

tuidemo.exe: tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o tuidemo.exe tuidemo.o tui.o $(LIBCURSES)

tui.o: $(demodir)/tui.c $(demodir)/tui.h $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)/tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) -c $(CCFLAGS) -I$(demodir) -o $@ $<

#------------------------------------------------------------------------

PLATFORM1 = MinGW Win32
PLATFORM2 = MinGW for Win32
ARCNAME = pdc$(VER)_ming_w32

include $(PDCURSES_HOME)/makedist.mif
