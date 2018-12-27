cat intro.md > PDCurses.md
echo PDCurses Definitions and Variables >> PDCurses.md
echo ================================== >> PDCurses.md
./manext.py ../curses.h >> PDCurses.md
echo PDCurses Functions >> PDCurses.md
echo ================== >> PDCurses.md
./manext.py ../pdcurses/*.c >> PDCurses.md
./manext.py ../x11/*.c >> PDCurses.md
