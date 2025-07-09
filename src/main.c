#include <ncurses.h>
#include <stdlib.h>
#include "file_dialog.h"

int main() {
    WINDOW *file_handler_dialog;
    char *file_name;

    initscr();
    noecho();
    keypad(stdscr, 1);
    refresh();
    raw();

    file_handler_dialog = open_file_dialog();
    file_name = get_file_name(file_handler_dialog);
    printw("%s", file_name);
    refresh();

    getch();
    endwin();

    free(file_name);

    return 0;
}
