################################################################################
#
# GNU MAKE Makefile for PDCurses library - OS/2 emx 0.9c
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
# Change these for your environment...
#
################################################################################
PDCURSES_HOME		=$(PDCURSES_SRCDIR)
CC_HOME		=c:/emx
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
osdir		= $(PDCURSES_HOME)/os2
pandir		= $(PDCURSES_HOME)/panel
demodir		= $(PDCURSES_HOME)/demos

CC		= gcc

ifeq ($(EMXVIDEO),Y)
	EMXVID = -DEMXVIDEO
	VIDLIB = -lvideo
	BINDFLAGS = -acm
	DLLTARGET = 
	DISTTARGETS = pdcurses.a pdcurses.lib panel.a panel.lib
else
	EMXVID =
	VIDLIB =
	BINDFLAGS = 
	DLLTARGET = curses.dll
	DISTTARGETS = curses.a curses.dll curses.lib pdcurses.a pdcurses.lib panel.a panel.lib
endif

ifeq ($(DEBUG),Y)
	CFLAGS  = -c -g -Wall -fomit-frame-pointer -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  = -c -O3 -Wall -fomit-frame-pointer
	LDFLAGS =
endif

DLLFLAGS = -Zdll -Zcrtdll -Zomf
DLLCURSES = curses.lib

CPPFLAGS	= -I$(PDCURSES_HOME) -I$(CCINCDIR) $(EMXVID)

CCFLAGS		= $(CFLAGS) $(CPPFLAGS)

LINK		= gcc
EMXBIND	=emxbind 
EMXOMF	=emxomf

CCLIBS		=$(VIDLIB)

LIBEXE		= ar
LIBFLAGS		=rcv

LIBCURSES	= pdcurses.a
LIBPANEL	= panel.a

PDCLIBS	= $(LIBCURSES) $(DLLTARGET) $(LIBPANEL)
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

DLLOBJS =     \
addch.dlo     \
addchstr.dlo  \
addstr.dlo    \
attr.dlo      \
beep.dlo      \
bkgd.dlo      \
border.dlo    \
clear.dlo     \
color.dlo     \
delch.dlo     \
deleteln.dlo  \
getch.dlo     \
getstr.dlo    \
getyx.dlo     \
inch.dlo      \
inchstr.dlo   \
initscr.dlo   \
inopts.dlo    \
insch.dlo     \
insstr.dlo    \
instr.dlo     \
kernel.dlo    \
mouse.dlo     \
move.dlo      \
outopts.dlo   \
overlay.dlo   \
pad.dlo       \
printw.dlo    \
refresh.dlo   \
scanw.dlo     \
scroll.dlo    \
slk.dlo       \
termattr.dlo  \
terminfo.dlo  \
touch.dlo     \
util.dlo      \
window.dlo

PDCOBJS =   \
pdcclip.o   \
pdcdebug.o  \
pdcdisp.o   \
pdcgetsc.o  \
pdckbd.o    \
pdcprint.o  \
pdcscrn.o   \
pdcsetsc.o  \
pdcutil.o   \
pdcwin.o

PDCDLOS =   \
pdcclip.dlo   \
pdcdebug.dlo  \
pdcdisp.dlo   \
pdcgetsc.dlo  \
pdckbd.dlo    \
pdcprint.dlo  \
pdcscrn.dlo   \
pdcsetsc.dlo  \
pdcutil.dlo   \
pdcwin.dlo

PANOBJS =     \
panel.o

pdcurses.a : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ *.o
	$(LIBEXE) d $@ $(PANOBJS) firework.o newdemo.o ptest.o testcurs.o tui.o tuidemo.o xmas.o
	$(EMXOMF) -o pdcurses.lib pdcurses.a

curses.dll : $(DLLOBJS) $(PDCDLOS)
	$(LINK) $(DLLFLAGS) -o curses.dll $(DLLOBJS) $(PDCDLOS) $(osdir)\pdcurses.def
	lxlite curses.dll
	emximp -o curses.lib $(osdir)\pdcurses.def
	emximp -o curses.a curses.lib

panel.a : $(PANOBJS)
	$(LIBEXE) $(LIBFLAGS) $@ $(PANOBJS)
	$(EMXOMF) -o panel.lib panel.a

addch.o: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\addch.c

addchstr.o: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\addchstr.c

addstr.o: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\addstr.c

attr.o: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\attr.c

beep.o: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\beep.c

bkgd.o: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\bkgd.c

border.o: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\border.c

clear.o: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\clear.c

color.o: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\color.c

delch.o: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\delch.c

deleteln.o: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\deleteln.c

getch.o: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\getch.c

getstr.o: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\getstr.c

getyx.o: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\getyx.c

inch.o: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\inch.c

inchstr.o: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\inchstr.c

initscr.o: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\initscr.c

inopts.o: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\inopts.c

insch.o: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\insch.c

insstr.o: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\insstr.c

instr.o: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\instr.c

kernel.o: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\kernel.c

mouse.o: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\mouse.c

move.o: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\move.c

outopts.o: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\outopts.c

overlay.o: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\overlay.c

pad.o: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pad.c

printw.o: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\printw.c

refresh.o: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\refresh.c

scanw.o: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\scanw.c

scroll.o: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\scroll.c

slk.o: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\slk.c

termattr.o: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\termattr.c

terminfo.o: $(srcdir)\terminfo.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\terminfo.c

touch.o: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\touch.c

util.o: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\util.c

window.o: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\window.c


pdcclip.o: $(osdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)\pdcclip.c

pdcdebug.o: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pdcdebug.c

pdcdisp.o: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)\pdcdisp.c

pdcgetsc.o: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)\pdcgetsc.c

pdckbd.o: $(osdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)\pdckbd.c

pdcprint.o: $(osdir)\pdcprint.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)\pdcprint.c

pdcscrn.o: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)\pdcscrn.c

pdcsetsc.o: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(osdir)\pdcsetsc.c

pdcutil.o: $(srcdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pdcutil.c

pdcwin.o: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) -o$@ $(srcdir)\pdcwin.c

addch.dlo: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\addch.c

addchstr.dlo: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\addchstr.c

addstr.dlo: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\addstr.c

attr.dlo: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\attr.c

beep.dlo: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\beep.c

bkgd.dlo: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\bkgd.c

border.dlo: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\border.c

clear.dlo: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\clear.c

color.dlo: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\color.c

delch.dlo: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\delch.c

deleteln.dlo: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\deleteln.c

getch.dlo: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\getch.c

getstr.dlo: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\getstr.c

getyx.dlo: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\getyx.c

inch.dlo: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\inch.c

inchstr.dlo: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\inchstr.c

initscr.dlo: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\initscr.c

inopts.dlo: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\inopts.c

insch.dlo: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\insch.c

insstr.dlo: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\insstr.c

instr.dlo: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\instr.c

kernel.dlo: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\kernel.c

mouse.dlo: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\mouse.c

move.dlo: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\move.c

outopts.dlo: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\outopts.c

overlay.dlo: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\overlay.c

pad.dlo: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\pad.c

printw.dlo: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\printw.c

refresh.dlo: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\refresh.c

scanw.dlo: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\scanw.c

scroll.dlo: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\scroll.c

slk.dlo: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\slk.c

termattr.dlo: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\termattr.c

terminfo.dlo: $(srcdir)\terminfo.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\terminfo.c

touch.dlo: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\touch.c

util.dlo: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\util.c

window.dlo: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\window.c


pdcclip.dlo: $(osdir)\pdcclip.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(osdir)\pdcclip.c

pdcdebug.dlo: $(srcdir)\pdcdebug.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\pdcdebug.c

pdcdisp.dlo: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(osdir)\pdcdisp.c

pdcgetsc.dlo: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(osdir)\pdcgetsc.c

pdckbd.dlo: $(osdir)\pdckbd.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(osdir)\pdckbd.c

pdcprint.dlo: $(osdir)\pdcprint.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(osdir)\pdcprint.c

pdcscrn.dlo: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(osdir)\pdcscrn.c

pdcsetsc.dlo: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(osdir)\pdcsetsc.c

pdcutil.dlo: $(srcdir)\pdcutil.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\pdcutil.c

pdcwin.dlo: $(srcdir)\pdcwin.c $(PDCURSES_HEADERS)
	$(CC) $(CCFLAGS) $(DLLFLAGS) -o$@ $(srcdir)\pdcwin.c

#------------------------------------------------------------------------

panel.o: $(pandir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(CC) $(CCFLAGS) -o$@ $(pandir)\panel.c

#------------------------------------------------------------------------

firework.exe:	firework.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o firework firework.o $(LIBCURSES) -L$(CCLIBDIR) $(CCLIBS)
	$(EMXBIND) firework $(BINDFLAGS)

newdemo.exe:	newdemo.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o newdemo newdemo.o $(LIBCURSES) -L$(CCLIBDIR) $(CCLIBS)
	$(EMXBIND) newdemo $(BINDFLAGS)

ptest.exe:	ptest.o $(LIBCURSES) $(LIBPANEL)
	$(LINK) $(LDFLAGS) -o ptest ptest.o $(LIBCURSES) $(LIBPANEL) -L$(CCLIBDIR) $(CCLIBS)
	$(EMXBIND) ptest $(BINDFLAGS)

testcurs.exe:	testcurs.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o testcurs testcurs.o $(LIBCURSES) -L$(CCLIBDIR) $(CCLIBS)
	$(EMXBIND) testcurs $(BINDFLAGS)

tuidemo.exe:	tuidemo.o tui.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o tuidemo tuidemo.o tui.o $(LIBCURSES) -L$(CCLIBDIR) $(CCLIBS)
	$(EMXBIND) tuidemo $(BINDFLAGS)

xmas.exe:	xmas.o $(LIBCURSES)
	$(LINK) $(LDFLAGS) -o xmas xmas.o $(LIBCURSES) -L$(CCLIBDIR) $(CCLIBS)
	$(EMXBIND) xmas $(BINDFLAGS)


testcurs_dyn.exe:	testcurs.obj curses.dll
	$(LINK) $(LDFLAGS) $(DLLFLAGS) -o testcurs_dyn testcurs.obj $(DLLCURSES) -L$(CCLIBDIR) $(CCLIBS)
	$(EMXBIND) testcurs_dyn $(BINDFLAGS)

firework.o: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\firework.c

newdemo.o: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\newdemo.c

ptest.o: $(demodir)\ptest.c $(PDCURSES_HOME)\panel.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\ptest.c

testcurs.o: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\testcurs.c

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -Zomf -o$@ $(demodir)\testcurs.c

tui.o: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)\tui.c
                               
tuidemo.o: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -I$(demodir) -o $@ $(demodir)\tuidemo.c

xmas.o: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(CC) $(CCFLAGS) -o$@ $(demodir)\xmas.c

dist: $(DISTTARGETS)
	-mkdir tmp
	copy $(PDCURSES_HOME)\README tmp\README
	copy $(PDCURSES_HOME)\readme.?? tmp
	copy $(PDCURSES_HOME)\curses.h tmp
	copy $(PDCURSES_HOME)\curspriv.h tmp
	copy $(PDCURSES_HOME)\maintain.er tmp
	-copy curses.dll tmp
	-copy curses.lib tmp
	-copy curses.a tmp
	copy pdcurses.a tmp
	copy pdcurses.lib tmp
	copy panel.a tmp
	copy panel.lib tmp
	echo 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴 > tmp\file_id.diz
	echo      PDCurses - Public Domain Curses >> tmp\file_id.diz
	echo  Version $(VERDOT)   for EMX OS/2   PDC$(VER)_EMX_OS2.ZIP >> tmp\file_id.diz
	echo  ------------------------------------------ >> tmp\file_id.diz
	echo  Public Domain Curses library for >> tmp\file_id.diz
	echo  EMX 0.9c for OS/2. >> tmp\file_id.diz
	echo  Source available in PDCURS$(VER).ZIP >> tmp\file_id.diz
	echo  Public Domain. >> tmp\file_id.diz
	echo 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴 >> tmp\file_id.diz
	rexx 'cd tmp'; 'zip pdc$(VER)_emx_os2 "*.*"'; return 0
