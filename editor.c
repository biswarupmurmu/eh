#include "editor.h"
#include "array.h"
#include "terminal.h"
#include <stdbool.h>
#include <stdlib.h>

void initEditor(Editor *editor) {
    enableRawMode();
    getWindowSize(&editor->window_row, &editor->window_col);
    editor->cursor_col = 0;
    editor->cursor_row = 0;
    editor->render_start_line = 0;
    editor->buffer_line = 0;
    editor->buffer_char = 0;
    editor->editing_view = true;
    editor->filename = "untitled.txt";
}

void exitEditor(Editor *editor) {
    // free all the memory here
    Array **lines = editor->buffer->ptr;
    for (int i = 0; i < editor->buffer->len; i++) {
        freeArray(lines[i]); // freeing the malloc'ed line
		free(lines[i]); // free the pointer to Array struct itself
		// as it is done with malloc
    }
    freeArray(editor->buffer); // freeing the array of the pointers of the lines
    disableRawMode();
    exit(0);
}
