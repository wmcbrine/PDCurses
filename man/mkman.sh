echo Definitions and Variables \(curses.h\) > MANUAL.md
echo ==================================== >> MANUAL.md
./manext.py ../curses.h >> MANUAL.md
echo Functions >> MANUAL.md
echo ========= >> MANUAL.md
./manext.py ../pdcurses/*.c >> MANUAL.md
./manext.py ../x11/*.c >> MANUAL.md
