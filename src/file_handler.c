#include "file_handler.h"
#include <ncurses.h>
#include <stdlib.h>
#include <ctype.h>

#define FILE_DIALOG_WIDTH 50
#define FILE_DIALOG_HEIGHT 8

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
    char *user_input, shift_character;
    int current_char, shift_pointer, i;
    size_t length;

    user_input = (char *)malloc(sizeof(char) * 32);
    wmove(dialog, 4, 3);
    wrefresh(dialog);

    i = 0;
    length = 0;
    while ((current_char = getch()) != 10) {
        switch (current_char) {
            case KEY_LEFT:
                if (i > 0) i--;
                break;
            case KEY_RIGHT:
                if (i < length) i++;
                break;
            case KEY_BACKSPACE:
                if (i == 0) break;
                // shift the rest to the left
                for (shift_pointer = i - 1; shift_pointer < length; shift_pointer++) {
                    shift_character = user_input[shift_pointer + 1];
                    user_input[shift_pointer] = shift_character;
                    wmove(dialog, 4, 3 + shift_pointer);
                    wprintw(dialog, "%c", shift_character);
                }
                user_input[length - 1] = '\0';
                wmove(dialog, 4, 2 + length);
                wprintw(dialog, " ");
                i--;
                length--;
                break;
            default:
                if (isprint(current_char)) {
                    // shift the rest to the right if user moved cursor to the left
                    if (length != i) {
                        for (shift_pointer = length; shift_pointer >= i; shift_pointer--) {
                            shift_character = shift_pointer >= 0 ? user_input[shift_pointer] : '\0';
                            user_input[shift_pointer + 1] = shift_character;
                            wmove(dialog, 4, 4 + shift_pointer);
                            wprintw(dialog, "%c", shift_character);
                        }
                    }

                    wmove(dialog, 4, 3 + i);
                    wprintw(dialog, "%c", current_char);
                    user_input[i] = current_char;
                    i++;
                    length++;
                }
                break;
        }

        wmove(dialog, 4, 3 + i);
        wrefresh(dialog);
    }

    return user_input;
}
