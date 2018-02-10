# GNU MAKE Makefile for PDCurses library - OS/2 emx 0.9c+
#
# Usage: make [-f path\Makefile] [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|testcurs.exe...]

O = o
E = .exe
RM = del

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/version.mif
include $(PDCURSES_SRCDIR)/libobjs.mif

osdir		= $(PDCURSES_SRCDIR)/os2

PDCURSES_OS2_H	= $(osdir)/pdcos2.h

CC		= gcc

CFLAGS = -I$(PDCURSES_SRCDIR) -c -Wall

ifeq ($(DEBUG),Y)
	CFLAGS  += -g -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  += -O2
	LDFLAGS =
endif

LINK		= gcc
EMXBIND		= emxbind
EMXOMF		= emxomf

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES = pdcurses.a
LIBDEPS = $(LIBOBJS) $(PDCOBJS)
PDCLIBS = $(LIBCURSES)
EXEPOST = $(EMXBIND) $*
TUIPOST = $(EMXBIND) tuidemo
CLEAN = *.a testcurs ozdemo xmas tuidemo firework ptest rain worm

.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(PDCLIBS)

clean:
	-$(RM) *.o
	-$(RM) *.exe
	-$(RM) $(CLEAN)

demos:	$(DEMOS)

DEMOOBJS = testcurs.o ozdemo.o xmas.o tui.o tuidemo.o firework.o \
ptest.o rain.o worm.o

$(LIBCURSES) : $(LIBDEPS)
	$(LIBEXE) $(LIBFLAGS) $@ $?

$(LIBOBJS) $(PDCOBJS) $(DEMOOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_OS2_H)
$(DEMOS) : $(LIBCURSES)
panel.o ptest.o: $(PANEL_HEADER)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) -o$@ $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) -o$@ $<

firework.exe ozdemo.exe rain.exe testcurs.exe worm.exe xmas.exe \
ptest.exe: %.exe: %.o
	$(LINK) $(LDFLAGS) -o $* $< $(LIBCURSES)
	$(EXEPOST)

tuidemo.exe:	tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o tuidemo tuidemo.o tui.o $(LIBCURSES)
	$(TUIPOST)

firework.o ozdemo.o ptest.o rain.o testcurs.o worm.o xmas.o: %.o: \
$(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $<

tui.o: $(demodir)\tui.c $(demodir)\tui.h
	$(CC) $(CFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)\tuidemo.c
	$(CC) $(CFLAGS) -I$(demodir) -o $@ $<

PLATFORM1 = EMX OS/2
PLATFORM2 = EMX 0.9d for OS/2
ARCNAME = pdc$(VER)_emx_os2

include $(PDCURSES_SRCDIR)/makedist.mif
