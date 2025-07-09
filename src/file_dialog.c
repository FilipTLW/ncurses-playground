#include "file_dialog.h"
#include <ncurses.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define FILE_DIALOG_WIDTH 50
#define FILE_DIALOG_HEIGHT 8

#define FILE_DIALOG_MAX_FILENAME_LENGTH (FILE_DIALOG_WIDTH - 6)

WINDOW *open_file_dialog() {
    WINDOW* dialog;

    dialog = newwin(FILE_DIALOG_HEIGHT, FILE_DIALOG_WIDTH, LINES / 2 - FILE_DIALOG_HEIGHT / 2, COLS / 2 - FILE_DIALOG_WIDTH / 2);
    box(dialog, 0, 0);
    wmove(dialog, 2, 2);
    wprintw(dialog, "Enter file name: ");

    wrefresh(dialog);

    return dialog;
}

char *get_file_name(WINDOW *dialog) {
    char *user_input;
    int current_char, cursor_offset;
    size_t length;

    user_input = (char *)calloc(FILE_DIALOG_MAX_FILENAME_LENGTH, sizeof(char));
    wmove(dialog, 4, 3);
    wrefresh(dialog);

    cursor_offset = 0;
    length = 0;
    while ((current_char = getch()) != 10) {
        switch (current_char) {
            case KEY_LEFT:
                if (cursor_offset > 0) cursor_offset--;
                break;
            case KEY_RIGHT:
                if (cursor_offset < length) cursor_offset++;
                break;
            case KEY_BACKSPACE:
                if (cursor_offset == 0) break;
                if (cursor_offset != length) {
                    memmove(user_input + sizeof(char) * (cursor_offset - 1), user_input + sizeof(char) * cursor_offset, length - cursor_offset);
                }
                user_input[length - 1] = '\0';
                cursor_offset--;
                length--;
                break;
            default:
                if (isprint(current_char)) {
                    if (length >= FILE_DIALOG_MAX_FILENAME_LENGTH) break;
                    if (cursor_offset != length) {
                        memmove(user_input + sizeof(char) * (cursor_offset + 1), user_input + sizeof(char) * cursor_offset, length - cursor_offset);
                    }

                    user_input[cursor_offset] = current_char;
                    cursor_offset++;
                    length++;
                }
                break;
        }
        wmove(dialog, 4, 3);
        wprintw(dialog, "%s", user_input);
        wclrtoeol(dialog);
        box(dialog, 0, 0);

        wmove(dialog, 4, 3 + cursor_offset);
        wrefresh(dialog);
    }

    return user_input;
}
