# Visual C++ & Intel(R) NMakefile for PDCurses library - Win32 VC++ 2.0+
#
# Usage: nmake -f [path\]vcwin32.mak [DEBUG=] [DLL=] [WIDE=] [UTF8=]
#           [ICC=] [CHTYPE_32=] [IX86=] [CHTYPE_16=] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]
#  CHTYPE_## is used to override the default 64-bit chtypes in favor
#  of "traditional" 32- or 16-bit chtypes.
#  IX86 is used to build 32-bit code instead of 64-bit
#  ICC is used to invoke Intel (R) tools icl.exe and xilink.exe,  instead of
#    MS tools cl.exe and link.exe

O = obj

!ifndef PDCURSES_SRCDIR
PDCURSES_SRCDIR = ..
!endif

!include $(PDCURSES_SRCDIR)\version.mif
!include $(PDCURSES_SRCDIR)\libobjs.mif

osdir      = $(PDCURSES_SRCDIR)\win32a

PDCURSES_WIN_H   = $(osdir)\pdcwin.h

!ifdef ICC
CC      = icl.exe -nologo
LINK    = xilink.exe -nologo
!else
CC      = cl.exe -nologo
LINK    = link.exe -nologo
!endif

!ifdef DEBUG
CFLAGS      = -Z7 -DPDCDEBUG -MT -D_CRT_SECURE_NO_WARNINGS
LDFLAGS      = -debug -pdb:none
!else
CFLAGS      = -Ox -MT -W3 -D_CRT_SECURE_NO_WARNINGS
LDFLAGS      =
!endif

!ifdef WIDE
WIDEOPT      = -DPDC_WIDE
!endif

!ifdef UTF8
UTF8OPT      = -DPDC_FORCE_UTF8
!endif

!ifdef CHTYPE_32
CHTYPE_FLAGS= -DCHTYPE_32
!endif

!ifdef CHTYPE_16
CHTYPE_FLAGS= -DCHTYPE_16
!endif

SHL_LD = link $(LDFLAGS) /NOLOGO /DLL /OUT:pdcurses.dll

CCLIBS      = user32.lib gdi32.lib advapi32.lib shell32.lib comdlg32.lib
# may need to add msvcrt.lib for VC 2.x, VC 5.0 doesn't want it
#CCLIBS      = msvcrt.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib

LIBEXE      = lib -nologo

LIBCURSES   = pdcurses.lib
CURSESDLL   = pdcurses.dll

!ifdef DLL
DLLOPT      = -DPDC_DLL_BUILD
PDCLIBS      = $(CURSESDLL)
!else
PDCLIBS      = $(LIBCURSES)
!endif

BUILD      = $(CC) -I$(PDCURSES_SRCDIR) -c $(CFLAGS) $(CHTYPE_FLAGS) $(DLLOPT) \
$(WIDEOPT) $(UTF8OPT)

all:   $(PDCLIBS) $(DEMOS)

clean:
   -del *.obj
   -del *.lib
   -del *.exe
   -del *.dll
   -del *.exp
   -del *.res

DEMOOBJS = $(DEMOS:.exe=.obj) tui.obj

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_WIN_H)
$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(DEMOS) : $(LIBCURSES)
panel.obj : $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)

!ifndef DLL
$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
   $(LIBEXE) -out:$@ $(LIBOBJS) $(PDCOBJS)
   -copy $(LIBCURSES) panel.lib
!endif

$(CURSESDLL) : $(LIBOBJS) $(PDCOBJS) pdcurses.obj
   $(SHL_LD) $(LIBOBJS) $(PDCOBJS) pdcurses.obj $(CCLIBS)
   -copy $(LIBCURSES) panel.lib

pdcurses.res pdcurses.obj: $(osdir)\pdcurses.rc $(osdir)\pdcurses.ico
   rc /r /fopdcurses.res $(osdir)\pdcurses.rc
!ifdef IX86
   cvtres /MACHINE:IX86 /NOLOGO /OUT:pdcurses.obj pdcurses.res
!else
   cvtres /MACHINE:X64 /NOLOGO /OUT:pdcurses.obj pdcurses.res
!endif

{$(srcdir)\}.c{}.obj::
   $(BUILD) $<

{$(osdir)\}.c{}.obj::
   $(BUILD) $<

{$(demodir)\}.c{}.obj::
   $(BUILD) $<

.obj.exe:
   $(LINK) $(LDFLAGS) $< $(LIBCURSES) $(CCLIBS)

tuidemo.exe: tuidemo.obj tui.obj
   $(LINK) $(LDFLAGS) $*.obj tui.obj $(LIBCURSES) $(CCLIBS)

tui.obj: $(demodir)\tui.c $(demodir)\tui.h
   $(BUILD) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c
   $(BUILD) -I$(demodir) $(demodir)\tuidemo.c

PLATFORM1 = Visual C++
PLATFORM2 = Microsoft Visual C/C++ for Win32
ARCNAME = pdc$(VER)_vc_w32

!include $(PDCURSES_SRCDIR)\makedist.mif
