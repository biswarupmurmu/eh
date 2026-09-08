#include "editor.h"
#include "terminal.h"
#include <stdbool.h>

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
