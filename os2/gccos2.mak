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

include $(PDCURSES_HOME)/version

PDCURSES_CURSES_H	= $(PDCURSES_HOME)/curses.h
PDCURSES_CURSPRIV_H	= $(PDCURSES_HOME)/curspriv.h
PDCURSES_HEADERS	= $(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)
PANEL_HEADER		= $(PDCURSES_HOME)/panel.h
TERM_HEADER		= $(PDCURSES_HOME)/term.h

srcdir		= $(PDCURSES_HOME)/pdcurses
osdir		= $(PDCURSES_HOME)/os2
pandir		= $(PDCURSES_HOME)/panel
demodir		= $(PDCURSES_HOME)/demos

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
DEMOS		= testcurs.exe newdemo.exe xmas.exe tuidemo.exe \
firework.exe ptest.exe rain.exe worm.exe #testcurs_dyn.exe

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.o
	-del *.a
	-del *.exe
	-del *.lib
	-del *.dlo
	-del *.dll
	-del testcurs newdemo xmas tuidemo firework ptest rain worm

demos:	$(DEMOS)

#------------------------------------------------------------------------

LIBOBJS = addch.o addchstr.o addstr.o attr.o beep.o bkgd.o border.o \
clear.o color.o delch.o deleteln.o getch.o getstr.o getyx.o inch.o \
inchstr.o initscr.o inopts.o insch.o insstr.o instr.o kernel.o mouse.o \
move.o outopts.o overlay.o pad.o printw.o refresh.o scanw.o scr_dump.o \
scroll.o slk.o termattr.o terminfo.o touch.o util.o window.o pdcdebug.o \
pdcwin.o

DLLOBJS = addch.dlo addchstr.dlo addstr.dlo attr.dlo beep.dlo bkgd.dlo \
border.dlo clear.dlo color.dlo delch.dlo deleteln.dlo getch.dlo \
getstr.dlo getyx.dlo inch.dlo inchstr.dlo initscr.dlo inopts.dlo \
insch.dlo insstr.dlo instr.dlo kernel.dlo mouse.dlo move.dlo outopts.dlo \
overlay.dlo pad.dlo printw.dlo refresh.dlo scanw.dlo scr_dump.dlo \
scroll.dlo slk.dlo termattr.dlo terminfo.dlo touch.dlo util.dlo \
window.dlo pdcdebug.dlo pdcwin.dlo

PDCOBJS = pdcclip.o pdcdisp.o pdcgetsc.o pdckbd.o pdcscrn.o pdcsetsc.o \
pdcutil.o

PDCDLOS = pdcclip.dlo pdcdisp.dlo pdcgetsc.dlo pdckbd.dlo pdcscrn.dlo \
pdcsetsc.dlo pdcutil.dlo

PANOBJS = panel.o

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
	$(LINK) $(LDFLAGS) $(DLLFLAGS) -o testcurs_dyn testcurs.obj \
$(DLLCURSES) $(CCLIBS)
	$(EMXBIND) testcurs_dyn $(BINDFLAGS)

firework.o newdemo.o ptest.o rain.o testcurs.o worm.o xmas.o: %.o: \
$(demodir)/%.c
	$(CC) $(CCFLAGS) -o$@ $<

testcurs.obj: $(demodir)\testcurs.c
	$(CC) $(CCFLAGS) -Zomf -o$@ $<

tui.o: $(demodir)\tui.c $(demodir)\tui.h
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)\tuidemo.c
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $<

dist: $(PDCLIBS)
	echo PDCurses $(VERDOT) for EMX OS/2 > file_id.diz
	echo ------------------------------------------ >> file_id.diz
	echo Public Domain Curses library for >> file_id.diz
	echo EMX 0.9d for OS/2. >> file_id.diz
	echo Source available in PDCURS$(VER).ZIP >> file_id.diz
	echo Public Domain. >> file_id.diz
	zip -9jX pdc$(VER)_emx_os2 \
	$(PDCURSES_HOME)\README $(PDCURSES_HOME)\HISTORY \
	$(PDCURSES_HOME)\maintain.er \
	$(PDCURSES_HOME)\curses.h $(PDCURSES_HOME)\curspriv.h \
	$(PDCURSES_HOME)\panel.h $(PDCURSES_HOME)\term.h \
	$(LIBCURSES) $(LIBPANEL) file_id.diz
	del file_id.diz
