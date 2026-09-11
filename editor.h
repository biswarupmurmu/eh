#ifndef EDITOR_H
#define EDITOR_H
#include "array.h"
#include <stdbool.h>

typedef struct {
    Array *buffer;
    char *filename;
    int render_start_line;
    int buffer_line;
    int buffer_char;
    short window_row;
    short window_col;
    int cursor_row;
    int cursor_col;
    bool editing_view;
} Editor;

void initEditor(Editor *editor);
void exitEditor(Editor *editor);

#endif
