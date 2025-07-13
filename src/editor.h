#ifndef EDITOR_H
#define EDITOR_H

#include <stdlib.h>

typedef struct {
    char *content;
    size_t length;
    size_t alloc_length;
} LINE;

void editor_launch(char *filename);

#endif
