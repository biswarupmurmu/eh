#include "array.h"
#include "editor.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int rowsReqToRender(Array *array, int screenWidth) {
    int line_len = array->len;
    char *text = (char *)array->ptr;

    if (line_len > 0 && text[line_len - 1] == '\n') {
        line_len -= 1;
    }

    if (line_len <= 0) {
        return 1;
    }
    return (line_len / screenWidth) + 1;
}

static void preRenderCalculations(Editor *editor) {
    int buffer_line = editor->buffer_line;
    int start = editor->render_start_line;
    int height = editor->window_row;
    int width = editor->window_col;
    Array **lines = (Array **)editor->buffer->ptr;

    if (buffer_line < start) {
        // if buffer_line line is above the screen
        editor->render_start_line = buffer_line;
    }

    // calculate how many rows are required to render the lines from
    // "render_start_line" to "buffer_line". Excluding the "buffer_line"
    int rowsRendered = 0;
    for (int i = editor->render_start_line; i < editor->buffer_line; i++) {
        rowsRendered += rowsReqToRender(lines[i], editor->window_col);
    }

    // move to the "buffer_line" which is in the next row
    editor->cursor_row = rowsRendered + 1;
    // check if current char position affects the row count
    editor->cursor_row += editor->buffer_char / width;

    // what if the current row is below the screen
    // we have to remove some rows from the top of the scrren
    int rowsToBeRemoved = 0;
    if (editor->cursor_row >= height) {
        // cursor row is below the screen
        rowsToBeRemoved = editor->cursor_row - height;
    }

    while (rowsToBeRemoved > 0 &&
           editor->render_start_line < editor->buffer_line) {
        int r = rowsReqToRender(lines[editor->render_start_line],
                                editor->window_col);
        editor->cursor_row -= r;
        rowsToBeRemoved -= r;
        editor->render_start_line += 1;
    }

    // decide the col
    int char_index = editor->buffer_char;
    // char_index%width
    editor->cursor_col = (char_index % width) + 1;
}

void drawBuffer(Editor *editor) {
    Array render_buffer;
    initArray(&render_buffer, sizeof(char));
    // clear screen \x1b[2J move mouse to top left \x1b[H
    addToArray(&render_buffer, "\x1b[2J\x1b[H", 0, 7);

    Array **lines = editor->buffer->ptr;
    int rows = (int)editor->window_row;

    preRenderCalculations(editor);

    int i = editor->render_start_line;

    while (rows > 0 && i < editor->buffer->len) {
        if (i == editor->buffer_line) {
            addToArray(&render_buffer, "\x1b[32m", render_buffer.len, 5);
        }

        addToArray(&render_buffer, lines[i]->ptr, render_buffer.len,
                   lines[i]->len);

        rows -= rowsReqToRender(lines[i], editor->window_col);

        // this is a workaround to not print the newline character in the last
        // line rendered, as it is causing the first line in the screen go above
        // the screen
        if (rows <= 0) {
            removeFromArray(&render_buffer, render_buffer.len - 1, 1);
        }

        if (i == editor->buffer_line) {
            addToArray(&render_buffer, "\x1b[0m", render_buffer.len, 4);
        }
        i++;
    }

    // draw cursor
    char cursor_pos[32];
    snprintf(cursor_pos, sizeof(cursor_pos), "\x1b[%d;%dH", editor->cursor_row,
             editor->cursor_col);
    addToArray(&render_buffer, cursor_pos, render_buffer.len,
               strlen(cursor_pos));

    write(STDOUT_FILENO, render_buffer.ptr, render_buffer.len);

    freeArray(&render_buffer);
}

// add borderedTextCenter
void addBorderedText(Array *render_buffer, char *text, int screenWidth) {
    int textLen = (int)strlen(text);
    int innerPadding = 1;
    int boxWidth = textLen + innerPadding * 2 + 2; // 2 for the |
    int leftPadding = (screenWidth - boxWidth) / 2;
    char padding[leftPadding];
    memset(padding, 32, leftPadding);

    addToArray(render_buffer, padding, render_buffer->len, leftPadding);
    addToArray(render_buffer, "┌", render_buffer->len, strlen("┌"));
    for (int i = 0; i < textLen + innerPadding * 2; i++) {
        addToArray(render_buffer, "─", render_buffer->len, strlen("─"));
    }
    addToArray(render_buffer, "┐\r\n", render_buffer->len, strlen("┐\r\n"));
    addToArray(render_buffer, padding, render_buffer->len, leftPadding);
    addToArray(render_buffer, "│ ", render_buffer->len, strlen("│ "));
    addToArray(render_buffer, text, render_buffer->len, textLen);
    addToArray(render_buffer, " │\r\n", render_buffer->len, strlen(" │\r\n"));
    addToArray(render_buffer, padding, render_buffer->len, leftPadding);
    addToArray(render_buffer, "└", render_buffer->len, strlen("└"));
    for (int i = 0; i < textLen + innerPadding * 2; i++) {
        addToArray(render_buffer, "─", render_buffer->len, strlen("─"));
    }
    addToArray(render_buffer, "┘\r\n", render_buffer->len, strlen("┘\r\n"));
}

void addCenteredText(Array *buffer, char *message, int window_width,
                     int ignoredChars) {
    // add ewline character
    addToArray(buffer, "\n", buffer->len, 1);
    int leftPadding = (window_width - strlen(message) + ignoredChars) / 2;
    for (int i = 0; i < leftPadding; i++) {
        addToArray(buffer, " ", buffer->len, 1);
    }
    addToArray(buffer, message, buffer->len, strlen(message));
}

void drawSave(Editor *editor) {
    Array render_buffer;
    initArray(&render_buffer, sizeof(char));

    int w = editor->window_col;
    int h = editor->window_row;

    // clear screen \x1b[2J
    // move cursor to top left \x1b[H
    addToArray(&render_buffer, "\x1b[2J\x1b[H", 0, 7);
    for (int i = 0; i < (h / 2) - 5; i++) {
        addToArray(&render_buffer, "\n", render_buffer.len, 1);
    }

    char *filename = editor->filename->ptr;

    addBorderedText(&render_buffer, filename, w);
    addCenteredText(&render_buffer, "Enter to \033[32msave\033[0m", w, 9);
    addCenteredText(&render_buffer, "Esc to \033[34mreturn\033[0m to editing",
                    w, 9);
    addCenteredText(&render_buffer,
                    "Ctrl+C to \033[1;31mquit without\033[0m saving", w, 11);

    // will implement editing filename in this window

    write(STDOUT_FILENO, render_buffer.ptr, render_buffer.len);

    freeArray(&render_buffer);
}

void drawEditor(Editor *editor) {
    if (editor->editing_view == true) {
        drawBuffer(editor);
    } else {
        drawSave(editor);
    }
}
