#include "editor.h"
#include "string.h"
#include <ncurses.h>
#include <stdio.h>

#define LINE_BLOCK_LENGTH 255
#define LINE_PADDING 32
#define LINE_BLOCK_BLOCK_LENGTH 16

void create_line(LINE *line, char *content, size_t length) {
    line->content = calloc(length + LINE_PADDING, sizeof(char));
    line->length = length;
    memmove(line->content, content, length);
}

void editor_launch(char *filename) {
    FILE *file;
    char *block, *new_block;
    LINE *line_block, *new_line_block;
    int ch, ln, col;
    size_t length, full_block_length, block_length, full_line_block_length, line_block_length;

    clear();

    file = fopen(filename, "r");
    block = NULL;
    line_block = malloc(sizeof(LINE) * LINE_BLOCK_BLOCK_LENGTH);
    full_line_block_length = LINE_BLOCK_BLOCK_LENGTH;
    line_block_length = 0;
    length = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (block == NULL) {
            block = malloc(sizeof(char) * LINE_BLOCK_LENGTH);
            full_block_length = LINE_BLOCK_LENGTH;
            block_length = 0;
        }
        if (ch == '\n') {
            if (line_block_length == full_line_block_length) {
                new_line_block = malloc(full_line_block_length + sizeof(LINE) * LINE_BLOCK_BLOCK_LENGTH);
                memmove(new_line_block, line_block, full_line_block_length);
                free(line_block);
                line_block = new_line_block;
                full_line_block_length += LINE_BLOCK_BLOCK_LENGTH;
            }
            create_line(line_block + line_block_length, block, block_length);
            free(block);
            block = NULL;
            line_block_length++;
        } else {
            if (block_length == full_block_length) {
                new_block = malloc(full_block_length + sizeof(char) * LINE_BLOCK_LENGTH);
                memmove(new_block, block, full_block_length);
                free(block);
                block = new_block;
                full_block_length += LINE_BLOCK_LENGTH;
            }
            block[block_length] = ch;
            block_length++;
        }
        length++;
    }
    fclose(file);

    move(0, 0);
    for (ln = 0; ln < line_block_length; ln++) {
        for (col = 0; col < line_block[ln].length; col++) {
            addch(line_block[ln].content[col]);
        }
        addch('\n');
    }
    refresh();


    while ((ch = getch()) != ('x'&037)) {
    }

    refresh();
}
