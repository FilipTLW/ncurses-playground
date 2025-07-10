#include "editor.h"
#include "string.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_BLOCK_LENGTH 255
#define LINE_PADDING 32
#define LINE_BLOCK_BLOCK_LENGTH 16

void create_line(LINE *line, char *content, size_t length) {
    line->content = calloc(length + LINE_PADDING, sizeof(char));
    line->length = length;
    memmove(line->content, content, length);
}

void editor_process(char *filename, LINE *lines, size_t line_count, size_t length) {
    int ln, col, virt_col, ch;
    move(0, 0);
    for (ln = 0; ln < line_count; ln++) {
        for (col = 0; col < lines[ln].length; col++) {
            addch(lines[ln].content[col]);
        }
        addch('\n');
    }

    ln = 0, col = 0, virt_col = 0;
    move(0, 0);
    refresh();
    while ((ch = getch()) != ('x'&037)) {
        switch (ch) {
            case KEY_LEFT:
                if (col > 0) col--;
                virt_col = col;
                break;
            case KEY_RIGHT:
                if (col < lines[ln].length) col++;
                virt_col = col;
                break;
            case KEY_UP:
                if (ln > 0) ln--;
                if (virt_col >= lines[ln].length) col = lines[ln].length;
                else col = virt_col;
                break;
            case KEY_DOWN:
                if (ln < line_count) ln++;
                if (virt_col >= lines[ln].length) col = lines[ln].length;
                else col = virt_col;
                break;
        }
        move(ln, col);
        refresh();
    }

}

void editor_launch(char *filename) {
    FILE *file;
    char *block;
    LINE *line_block;
    int ch, ln;
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
                line_block = realloc(line_block, full_line_block_length + sizeof(LINE) * LINE_BLOCK_BLOCK_LENGTH);
                full_line_block_length += LINE_BLOCK_BLOCK_LENGTH;
            }
            create_line(line_block + line_block_length, block, block_length);
            free(block);
            block = NULL;
            line_block_length++;
        } else {
            if (block_length == full_block_length) {
                block = realloc(block, full_block_length + sizeof(char) * LINE_BLOCK_LENGTH);
                full_block_length += LINE_BLOCK_LENGTH;
            }
            block[block_length] = ch;
            block_length++;
        }
        length++;
    }
    free(block);
    fclose(file);

    editor_process(filename, line_block, line_block_length, length);

    for (ln = 0; ln < line_block_length; ln++) {
        free(line_block[ln].content);
    }
    free(line_block);

    refresh();
}
