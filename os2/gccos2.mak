################################################################################
#
# GNU MAKE Makefile for PDCurses library - OS/2 emx 0.9c+
#
# Usage: make -f [path\]gccos2.mak [DEBUG=Y] [EMXVIDEO=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|panel.a|testcurs.exe...]
#
# The EMXVIDEO option compiles with the emx video library, which
# enables a PDCurses program to run under OS/2 and DOS.
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

osdir		= $(PDCURSES_HOME)/os2

CC		= gcc

ifeq ($(EMXVIDEO),Y)
	EMXVID = -DEMXVIDEO
	VIDLIB = -lvideo
	BINDFLAGS = -acm
	DLLTARGET = 
else
	EMXVID =
	VIDLIB =
	BINDFLAGS = 
	DLLTARGET = curses.dll
endif

ifeq ($(DEBUG),Y)
	CFLAGS  = -c -g -Wall -fomit-frame-pointer -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -c -O2 -Wall -fomit-frame-pointer
	LDFLAGS =
endif

DLLFLAGS 	= -Zdll -Zcrtdll -Zomf
DLLCURSES	= curses.lib

CPPFLAGS	= -I$(PDCURSES_HOME) $(EMXVID)

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc
EMXBIND		= emxbind 
EMXOMF		= emxomf

CCLIBS		= $(VIDLIB)

LIBEXE		= ar
LIBFLAGS	= rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS		= $(LIBCURSES) $(LIBPANEL) #$(DLLTARGET) pdcurses.lib panel.lib

################################################################################
.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(PDCLIBS)

clean:
	-del *.o
	-del *.a
	-del *.exe
	-del *.lib
	-del *.dlo
	-del *.dll
	-del testcurs newdemo xmas tuidemo firework ptest rain worm
	-del *.obj

demos:	$(DEMOS) #testcurs_dyn.exe

#------------------------------------------------------------------------

DLLOBJS = $(LIBOBJS:.o=.dlo)
PDCDLOS = $(PDCOBJS:.o=.dlo)

DEMOOBJS = testcurs.o newdemo.o xmas.o tui.o tuidemo.o firework.o \
ptest.o rain.o worm.o

#------------------------------------------------------------------------

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

pdcurses.lib: pdcurses.a
	$(EMXOMF) -o pdcurses.lib pdcurses.a

curses.dll: $(DLLOBJS) $(PDCDLOS)
	$(LINK) $(DLLFLAGS) -o curses.dll $(DLLOBJS) $(PDCDLOS) \
$(osdir)\pdcurses.def
#	lxlite curses.dll
	emximp -o curses.lib $(osdir)\pdcurses.def
	emximp -o curses.a curses.lib

$(LIBPANEL) : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

panel.lib: panel.a
	$(EMXOMF) -o panel.lib panel.a

$(LIBOBJS) $(DLLOBJS) $(PDCOBJS) $(PDCDLOS) $(PANOBJS) $(DEMOOBJS) : \
$(PDCURSES_HEADERS)

$(PANOBJS) ptest.o: $(PANEL_HEADER)

$(DEMOS) : $(LIBCURSES)

terminfo.o terminfo.dlo: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CCFLAGS) $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CCFLAGS) $<

$(DLLOBJS) : %.dlo: $(srcdir)/%.c
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

$(PDCDLOS) : %.dlo: $(osdir)/%.c
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

$(PANOBJS) : %.o: $(pandir)/%.c
	$(CC) -c $(CCFLAGS) $<

#------------------------------------------------------------------------

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe: %.exe: %.o
	$(LINK) $(LDFLAGS) -o $* $< $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) $* $(BINDFLAGS)

ptest.exe:	ptest.o $(LIBPANEL)
	$(LINK) $(LDFLAGS) -o ptest ptest.o $(LIBCURSES) $(LIBPANEL) $(CCLIBS)
	$(EMXBIND) ptest $(BINDFLAGS)

tuidemo.exe:	tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o tuidemo tuidemo.o tui.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) tuidemo $(BINDFLAGS)

testcurs_dyn.exe:	testcurs.obj curses.dll
	$(LINK) $(LDFLAGS) -Zomf -Zcrtdll -o $@ testcurs.obj \
$(DLLCURSES) $(CCLIBS)

firework.o newdemo.o ptest.o rain.o testcurs.o worm.o xmas.o: %.o: \
$(demodir)/%.c
	$(CC) $(CCFLAGS) -o$@ $<

testcurs.obj: $(demodir)\testcurs.c
	$(CC) $(CCFLAGS) -Zomf -o$@ $<

tui.o: $(demodir)\tui.c $(demodir)\tui.h
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)\tuidemo.c
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $<

PLATFORM1 = EMX OS/2
PLATFORM2 = EMX 0.9d for OS/2
ARCNAME = pdc$(VER)_emx_os2

include $(PDCURSES_HOME)/makedist.mif
