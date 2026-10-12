// test the very basics are working
// no refresh() or getch() as this requires a terminal and wouldn't work on CI
// without additional measures.

#include <stdio.h>
#include <stdlib.h>

#include <curses.h>

int main(void){
    WINDOW* w = initscr();
    if(w == NULL){
        fprintf(stderr, "Error initialising ncurses.\n");
        return 77;
    }

    int i = noecho();
    if(i == ERR){
        fprintf(stderr, "Error turning off echo.\n");
        return 77;
    }

    i = printw("Hello, PDCurses!");
    if(i == ERR){
        fprintf(stderr, "Error printing to screen.\n");
        return 77;
    }

    endwin();

    return EXIT_SUCCESS;
}
