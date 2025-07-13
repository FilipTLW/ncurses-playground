#include "editor.h"
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_BLOCK_LENGTH 255
#define LINE_PADDING 32
#define LINE_BLOCK_BLOCK_LENGTH 16

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void create_line(LINE *line, char *content, size_t length) {
    line->content = calloc(length + LINE_PADDING, sizeof(char));
    line->length = length;
    line->alloc_length = length + LINE_PADDING;
    memmove(line->content, content, length);
}

char *rpad(char *dest, const char *src, const char pad, const size_t length) {
    memset(dest, pad, length);
    dest[length] = '\0';
    memcpy(dest, src, strlen(src));
    return dest;
}

void editor_process(char *filename, LINE *lines, size_t line_count, size_t alloc_line_count, size_t length) {
    int ln, col, virt_col, x, y, ch, line_offset, col_offset;
    char lnum[16], lnumtmp[16];

    ln = 0, col = 0, virt_col = 0, line_offset = 0, col_offset = log10(line_count) + 2;

    move(0, 0);
    for (y = line_offset; y < MIN(line_count, LINES + line_offset); y++) {
        sprintf(lnumtmp, "%d", y + 1);
        rpad(lnum, lnumtmp, ' ', col_offset);
        printw("%s", lnum);
        for (x = 0; x < lines[y].length; x++) {
            addch(lines[y].content[x]);
        }
        addch('\n');
    }

    move(0, col_offset);
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
                if (ln < line_offset) line_offset--;
                break;
            case KEY_DOWN:
                if (ln < line_count - 1) ln++;
                if (virt_col >= lines[ln].length) col = lines[ln].length;
                else col = virt_col;
                if (ln >= LINES + line_offset) line_offset++;
                break;
            case KEY_BACKSPACE:
                if (col == 0) break;
                if (col != lines[ln].length) {
                    memmove(lines[ln].content + sizeof(char) * (col - 1), lines[ln].content + sizeof(char) * col, lines[ln].length - col);
                }
                lines[ln].content[lines[ln].length] = '\0';
                virt_col = --col;
                lines[ln].length--;
                break;
            case KEY_DC:
                if (col == lines[ln].length) break;
                memmove(lines[ln].content + sizeof(char) * col, lines[ln].content + sizeof(char) * (col + 1), lines[ln].length - col);
                lines[ln].content[lines[ln].length] = '\0';
                lines[ln].length--;
                break;
            case '\n':
                if (line_count == alloc_line_count) {
                    lines = realloc(lines, sizeof(LINE) * (alloc_line_count + LINE_BLOCK_BLOCK_LENGTH));
                    alloc_line_count += LINE_BLOCK_BLOCK_LENGTH;
                }
                memmove(lines + ln + 2, lines + ln + 1, sizeof(LINE) * (line_count - ln - 1));
                create_line(lines + ln + 1, lines[ln].content + sizeof(char) * col, lines[ln].length - col);
                memset(lines[ln].content + sizeof(char) * col, '\0', lines[ln].length - col);
                lines[ln].length -= lines[ln].length - col;
                line_count++;
                ln++;
                virt_col = col = 0;
                col_offset = log10(line_count) + 2;
                if (ln >= LINES + line_offset) line_offset++;
                break;
            default:
                if (isprint(ch)) {
                    if (lines[ln].length == lines[ln].alloc_length) {
                        lines[ln].content = realloc(lines[ln].content, sizeof(char) * (lines[ln].alloc_length + LINE_PADDING));
                        lines[ln].alloc_length = lines[ln].alloc_length + LINE_PADDING;
                    }
                    memmove(lines[ln].content + sizeof(char) * (col + 1), lines[ln].content + sizeof(char) * col, lines[ln].length - col);
                    lines[ln].content[col] = ch;
                    virt_col = ++col;
                    lines[ln].length++;
                }
                break;
        }

        move(0, 0);
        clrtobot();
        for (y = line_offset; y < MIN(line_count, LINES + line_offset); y++) {
            sprintf(lnumtmp, "%d", y + 1);
            rpad(lnum, lnumtmp, ' ', col_offset);
            printw("%s", lnum);
            for (x = 0; x < lines[y].length; x++) {
                addch(lines[y].content[x]);
            }
            addch('\n');
        }
        y = 0;
        for (x = line_offset; x < ln; x++) {
            y += lines[x].length / (COLS - col_offset) + 1;
        }

        move(y + (col + col_offset) / COLS, (col + col_offset) % COLS);
        refresh();
    }

    for (y = 0; y < line_count; y++) {
        free(lines[y].content);
    }
    free(lines);
}

void editor_launch(char *filename) {
    FILE *file;
    char *block;
    LINE *line_block;
    int ch;
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
                line_block = realloc(line_block, sizeof(LINE) * (LINE_BLOCK_BLOCK_LENGTH + full_line_block_length));
                full_line_block_length += LINE_BLOCK_BLOCK_LENGTH;
            }
            create_line(line_block + line_block_length, block, block_length);
            free(block);
            block = NULL;
            line_block_length++;
        } else {
            if (block_length == full_block_length) {
                block = realloc(block, sizeof(char) * (LINE_BLOCK_LENGTH + full_block_length));
                full_block_length += LINE_BLOCK_LENGTH;
            }
            block[block_length] = ch;
            block_length++;
        }
        length++;
    }
    if (block != NULL) free(block);
    fclose(file);

    editor_process(filename, line_block, line_block_length, full_line_block_length, length);

    refresh();
}
