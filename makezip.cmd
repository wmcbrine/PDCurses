/**/
Trace o
Parse Upper Arg target plat verdot .
If Wordpos(target,'DJG VCP EMX VC LCC MING CYGNUS 16W 32W WCC ICC MSC BCC') = 0 Then Call Usage
If Wordpos(plat,'DOS OS2 W32') = 0 Then Call Usage
If Countstr('.',verdot) = 0 Then Call Usage
curses_home = Value('PDCURSES_SRCDIRN',,'ENVIRONMENT')
If curses_home = '' Then Abort('PDCURSES_SRCDIRN is not set!' )
Parse Value 'dos\go32 dos\emx os2\emx vc' With src.djg src.vcp src.emx src.w32
Parse Value 'libregna.a regina.a regina.a regina.lib' With lib.djg lib.vcp lib.emx lib.w32
Parse Value 'rexx.exe rexx.exe rexx.exe regina.exe' With exe.djg exe.vcp exe.emx exe.w32
Parse Value 'rex rex rexx rexx' With ext.djg ext.vcp ext.emx ext.w32
version = Changestr('.',verdot,'')
If Stream('tmp','C','QUERY EXISTS') = '' Then  'mkdir tmp'
rc = Chdir('tmp')
If rc \= 0 Then Abort( 'Cannot change dir to "tmp"' )
'del *.*'
Select
   When plat = 'DOS' Then
      Do
         Select
            When target = 'DJG' Then
               Do
                  'copy  ..\pdcurses.a .'
                  'copy  ..\panel.a .'
                  zipfile = 'pdc'version'djg.zip'
                  Call makefileid zipfile verdot version 'DOS DJGPP 2.0+'
               End
            When target = 'VCP' Then
               Do
                  'copy  ..\pdcurses.a .'
                  'copy  ..\panel.a .'
                  zipfile = 'pdc'version'vcp.zip'
                  Call makefileid zipfile verdot version 'DOS EMX 0.9d+'
               End
            When target = '16W' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'16w.zip'
                  Call makefileid zipfile verdot version 'DOS Watcom 10.6+ 16bit'
               End
            When target = '32W' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'32w.zip'
                  Call makefileid zipfile verdot version 'DOS Watcom 10.6+ 32bit'
               End
            When target = 'MSC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'msc.zip'
                  Call makefileid zipfile verdot version 'DOS Microsoft C 6.0'
               End
            When target = 'BCC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'bcc.zip'
                  Call makefileid zipfile verdot version 'DOS Borland C++ 3.0+'
               End
            Otherwise Call Abort 'Invalid platform:' plat 'for DOS'
         End
      End
   When plat = 'OS2' Then
      Do
         Select
            When target = 'ICC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'_icc_os2.zip'
                  Call makefileid zipfile verdot version 'OS/2 C Set/2'
               End
            When target = 'EMX' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\pdcurses.a .'
                  'copy  ..\panel.lib .'
                  'copy  ..\panel.a .'
                  'copy  ..\curses.lib .'
                  'copy  ..\curses.a .'
                  'copy  ..\curses.dll .'
                  zipfile = 'pdc'version'_emx_os2.zip'
                  Call makefileid zipfile verdot version 'OS/2 EMX 09d+'
               End
            When target = 'WCC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'_wcc_os2.zip'
                  Call makefileid zipfile verdot version 'OS/2 Watcom C++ 10.6+'
               End
            When target = 'BCC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'_bcc_os2.zip'
                  Call makefileid zipfile verdot version 'OS/2 Borland C++ 1.0'
               End
            Otherwise Call Abort 'Invalid platform:' plat 'for OS/2'
         End
      End
   When plat = 'W32' Then
      Do
         Select
            When target = 'VC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  'copy  ..\curses.dll .'
                  'copy  ..\curses.lib .'
                  zipfile = 'pdc'version'_vc_w32.zip'
                  Call makefileid zipfile verdot version 'Win32 Visual C++ 5.0+'
               End
            When target = 'LCC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'_lcc_w32.zip'
                  Call makefileid zipfile verdot version 'Win32 LCCWin32'
               End
            When target = 'MING' Then
               Do
                  'copy  ..\pdcurses.a .'
                  'copy  ..\panel.a .'
                  zipfile = 'pdc'version'_ming_w32.zip'
                  Call makefileid zipfile verdot version 'Win32 MingW32'
               End
            When target = 'WCC' Then
               Do
                  'copy  ..\pdcurses.lib .'
                  'copy  ..\panel.lib .'
                  zipfile = 'pdc'version'_wcc_w32.zip'
                  Call makefileid zipfile verdot version 'Win32 Watcom C++ 10.6+'
               End
            When target = 'CYGNUS' Then
               Do
                  'copy  ..\pdcurses.a .'
                  'copy  ..\panel.a .'
                  zipfile = 'pdc'version'_cyg_w32.zip'
                  Call makefileid zipfile verdot version 'Win32 Cygnus GCC'
               End
            Otherwise Call Abort 'Invalid platform:' plat 'for Win32'
         End
      End
End
'copy' curses_home || '\curses.h .'
'copy' curses_home || '\panel.h .'
'copy' curses_home || '\maintain.er .'
'copy' curses_home || '\README .'
'copy' curses_home || '\INSTALL .'
'copy' curses_home || '\TODO .'
'copy' curses_home || '\pdc64.gif .'
'copy' curses_home || '\readme.?? .'

'zip' zipfile '*'
Return 0

usage: Procedure
Say 'Usage: makezip target platform verdot'
Say 'where: target is one of emx, djg, vcp, w32'
Say '       plat is one of dos, os2, w32'
Say '       verdot is 2.4'
Exit 0

abort: Procedure
Parse Arg msg
Say msg
Exit 1

makefileid: Procedure
fn = 'file_id.diz'
Parse Arg zipfile verdot version desc
Call Stream fn,'C','OPEN WRITE REPLACE'
Call Lineout fn,'컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴'
Call Lineout fn,'     PDCurses - Public Domain Curses'
Call Lineout fn,Left(' Version:' verdot, 15) Right(zipfile,25)
Call Lineout fn,'------------------------------------------'
Call Lineout fn,' Public Domain Curses library for'
Call Lineout fn,' '|| desc
Call Lineout fn,' Source available in pdcurs'version'.zip'
Call Lineout fn,' Public Domain.'
Call Lineout fn,'컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴'
Call Stream fn,'C','CLOSE'
Return 0
