echo PDCurses Definitions and Variables > MANUAL.md
echo ================================== >> MANUAL.md
./manext.py ../curses.h >> MANUAL.md
echo PDCurses Functions >> MANUAL.md
echo ================== >> MANUAL.md
./manext.py ../pdcurses/*.c >> MANUAL.md
./manext.py ../x11/*.c >> MANUAL.md
