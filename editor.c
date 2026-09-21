#include "editor.h"
#include "array.h"
#include "terminal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void initBuffer(Array *buffer) {
    // // initialize the buffer
    Array *emptyline = malloc(sizeof(Array));
    initArray(emptyline, sizeof(char));
    addToArray(emptyline, "", emptyline->len, strlen(""));
    addToArray(buffer, &emptyline, buffer->len, 1);
}

void initEditor(Editor *editor) {
    enableRawMode();
    getWindowSize(&editor->window_row, &editor->window_col);
    editor->cursor_col = 1;
    editor->cursor_row = 1;
    editor->render_start_line = 0;
    editor->buffer_line = 0;
    editor->buffer_char = 0;
    editor->editing_view = true;

    Array *filename = malloc(sizeof(Array));
    initArray(filename, sizeof(char));
    addToArray(filename, "untitled.txt", filename->len, strlen("untitled.txt"));
    editor->filename = filename;
}

void freeMemoryEditor(Editor *editor) {
    // free all the memory here
    Array **lines = editor->buffer->ptr;
    for (int i = 0; i < editor->buffer->len; i++) {
        freeArray(lines[i]); // freeing the malloc'ed line
        free(lines[i]);      // free the pointer to Array struct itself
                             // as it is done with malloc
    }
    freeArray(editor->buffer); // freeing the array of the pointers of the lines
    freeArray(editor->filename);
    free(editor->filename);
}

void exitEditor(Editor *editor) {
    freeMemoryEditor(editor);
    disableRawMode();
    exit(0);
}
