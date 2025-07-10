#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <ncurses.h>

WINDOW *open_file_dialog();
char *get_file_name(WINDOW *dialog);

#endif
