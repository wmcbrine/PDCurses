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
firework.exe ptest.exe rain.exe worm.exe

################################################################################
all:	$(PDCLIBS) $(DEMOS)

clean:
	-del *.o
	-del *.dlo
	-del *.a
	-del *.lib
	-del *.dll
	-del *.exe
	-del testcurs newdemo xmas tuidemo firework ptest rain worm

demos:	$(DEMOS)

#------------------------------------------------------------------------

LIBOBJS = addch.o addchstr.o addstr.o attr.o beep.o bkgd.o border.o \
clear.o color.o delch.o deleteln.o getch.o getstr.o getyx.o inch.o \
inchstr.o initscr.o inopts.o insch.o insstr.o instr.o kernel.o mouse.o \
move.o outopts.o overlay.o pad.o printw.o refresh.o scanw.o scr_dump.o \
scroll.o slk.o termattr.o terminfo.o touch.o util.o window.o

DLLOBJS = addch.dlo addchstr.dlo addstr.dlo attr.dlo beep.dlo bkgd.dlo \
border.dlo clear.dlo color.dlo delch.dlo deleteln.dlo getch.dlo \
getstr.dlo getyx.dlo inch.dlo inchstr.dlo initscr.dlo inopts.dlo \
insch.dlo insstr.dlo instr.dlo kernel.dlo mouse.dlo move.dlo outopts.dlo \
overlay.dlo pad.dlo printw.dlo refresh.dlo scanw.dlo scr_dump.dlo \
scroll.dlo slk.dlo termattr.dlo terminfo.dlo touch.dlo util.dlo \
window.dlo

PDCOBJS = pdcclip.o pdcdebug.o pdcdisp.o pdcgetsc.o pdckbd.o \
pdckey.o pdcscrn.o pdcsetsc.o pdcutil.o pdcwin.o

PDCDLOS = pdcclip.dlo pdcdebug.dlo pdcdisp.dlo pdcgetsc.dlo pdckbd.dlo \
pdckey.dlo pdcscrn.dlo pdcsetsc.dlo pdcutil.dlo pdcwin.dlo

PANOBJS = panel.o


pdcurses.a : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(LIBOBJS) $(PDCOBJS)

pdcurses.lib : pdcurses.a
	$(EMXOMF) -o pdcurses.lib pdcurses.a

curses.dll : $(DLLOBJS) $(PDCDLOS)
	$(LINK) $(DLLFLAGS) -o curses.dll $(DLLOBJS) $(PDCDLOS) \
$(osdir)\pdcurses.def
#	lxlite curses.dll
	emximp -o curses.lib $(osdir)\pdcurses.def
	emximp -o curses.a curses.lib

panel.a : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)

panel.lib : panel.a
	$(EMXOMF) -o panel.lib panel.a


addch.o: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

addchstr.o: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

addstr.o: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

attr.o: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

beep.o: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

bkgd.o: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

border.o: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

clear.o: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

color.o: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

delch.o: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

deleteln.o: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

getch.o: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

getstr.o: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

getyx.o: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

inch.o: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

inchstr.o: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

initscr.o: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

inopts.o: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

insch.o: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

insstr.o: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

instr.o: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

kernel.o: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

mouse.o: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

move.o: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

outopts.o: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

overlay.o: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pad.o: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

printw.o: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

refresh.o: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

scanw.o: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

scr_dump.o: $(srcdir)\scr_dump.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

scroll.o: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

slk.o: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

termattr.o: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

terminfo.o: $(srcdir)\terminfo.c $(PDCURSES_HEADERS) $(TERM_HEADER)
	$(CC) $(CCFLAGS) -o$@ $<

touch.o: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

util.o: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

window.o: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<


pdcclip.o: $(osdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdcdebug.o: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdcdisp.o: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdcgetsc.o: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdckbd.o: $(osdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdckey.o: $(srcdir)\pdckey.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdcscrn.o: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdcsetsc.o: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdcutil.o: $(osdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

pdcwin.o: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $<

addch.dlo: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

addchstr.dlo: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

addstr.dlo: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

attr.dlo: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

beep.dlo: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

bkgd.dlo: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

border.dlo: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

clear.dlo: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

color.dlo: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

delch.dlo: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

deleteln.dlo: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

getch.dlo: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

getstr.dlo: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

getyx.dlo: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

inch.dlo: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

inchstr.dlo: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

initscr.dlo: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

inopts.dlo: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

insch.dlo: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

insstr.dlo: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

instr.dlo: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

kernel.dlo: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

mouse.dlo: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

move.dlo: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

outopts.dlo: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

overlay.dlo: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pad.dlo: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

printw.dlo: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

refresh.dlo: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

scanw.dlo: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

scr_dump.dlo: $(srcdir)\scr_dump.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

scroll.dlo: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

slk.dlo: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

termattr.dlo: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

terminfo.dlo: $(srcdir)\terminfo.c $(PDCURSES_HEADERS) $(TERM_HEADER)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

touch.dlo: (srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

util.dlo: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

window.dlo: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<


pdcclip.dlo: $(osdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdcdebug.dlo: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdcdisp.dlo: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdcgetsc.dlo: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdckbd.dlo: $(osdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdckey.dlo: $(srcdir)\pdckey.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdcscrn.dlo: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdcsetsc.dlo: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdcutil.dlo: $(osdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

pdcwin.dlo: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $<

#------------------------------------------------------------------------

panel.o: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(CCFLAGS) -o$@ $<

#------------------------------------------------------------------------

firework.exe:	firework.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o firework firework.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) firework $(BINDFLAGS)

newdemo.exe:	newdemo.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o newdemo newdemo.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) newdemo $(BINDFLAGS)

ptest.exe:	ptest.o $(LIBCURSES) $(LIBPANEL)
	$(LINK) $(LDFLAGS) -o ptest ptest.o $(LIBCURSES) $(LIBPANEL) $(CCLIBS)
	$(EMXBIND) ptest $(BINDFLAGS)

rain.exe:	rain.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o rain rain.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) rain $(BINDFLAGS)

testcurs.exe:	testcurs.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o testcurs testcurs.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) testcurs $(BINDFLAGS)

tuidemo.exe:	tuidemo.o tui.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o tuidemo tuidemo.o tui.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) tuidemo $(BINDFLAGS)

worm.exe:	worm.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o worm worm.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) worm $(BINDFLAGS)

xmas.exe:	xmas.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o xmas xmas.o $(LIBCURSES) $(CCLIBS)
	$(EMXBIND) xmas $(BINDFLAGS)


testcurs_dyn.exe:	testcurs.obj curses.dll
	$(LINK) $(LDFLAGS) $(DLLFLAGS) -o testcurs_dyn testcurs.obj $(DLLCURSES) $(CCLIBS)
	$(EMXBIND) testcurs_dyn $(BINDFLAGS)

firework.o: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $<

newdemo.o: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $<

ptest.o: $(demodir)\ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $<

rain.o: $(demodir)\rain.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $<

testcurs.o: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $<

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Zomf -o$@ $<

tui.o: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $<

worm.o: $(demodir)\worm.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $<

xmas.o: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $<

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
