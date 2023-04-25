@nmake -fmakefile.vc WIDE=Y UTF8=Y DLL=Y 
@move *.dll ..\..\bin\%ARCH%
@move *.lib ..\..\lib\%ARCH%
@del *.dll *.lib *.exp *.res *.obj

@nmake -fmakefile.vc WIDE=Y UTF8=Y
@move *.lib ..\..\lib\%ARCH%\pdcurses_s.lib
@del *.dll *.lib *.exp *.res *.obj
