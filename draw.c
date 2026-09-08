#include "editor.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

void drawBuffer(Editor *editor) {
    Array original_buffer = *editor->buffer;
    Array render_buffer;
    initArray(&render_buffer, sizeof(char));

    // clear screen \x1b[2J
    // move mouse to top left \x1b[H
    addToArray(&render_buffer, "\x1b[2J\x1b[H", 0, 7);

    // original_buffer.ptr holds the pointer of the array
    // containing Array pointers
    Array **lines = (Array **)original_buffer.ptr;

    int lines_to_render = original_buffer.len;
    if (lines_to_render > editor->window_row) {
        lines_to_render = editor->window_row;
    }

    int i = editor->render_start_line;
    while (lines_to_render > 0) {
        ////////
        if (i == editor->buffer_line) {
            addToArray(&render_buffer, "\x1b[32m", render_buffer.len, 5);
        }
        ////////
        addToArray(&render_buffer, lines[i]->ptr, render_buffer.len,
                   lines[i]->len);
        ////////
        if (i == editor->buffer_line) {
            addToArray(&render_buffer, "\x1b[0m", render_buffer.len, 4);
        }
        //////
        i += 1;
        lines_to_render -= 1;
    }

    // cursor
    char cursor_pos[32];
    snprintf(cursor_pos, sizeof(cursor_pos), "\x1b[%d;%dH",
             editor->cursor_row + 1, editor->cursor_col + 1);
    addToArray(&render_buffer, &cursor_pos, render_buffer.len,
               strlen(cursor_pos));

    write(STDOUT_FILENO, render_buffer.ptr, render_buffer.len);

    freeArray(&render_buffer);
}

void addBorderedText(Array *render_buffer, char *text, int leftPadding) {
    char padding[leftPadding];
    memset(padding, 32, leftPadding);

    int textLen = strlen(text);
    addToArray(render_buffer, padding, render_buffer->len, leftPadding);
    addToArray(render_buffer, "┌", render_buffer->len, strlen("┌"));
    for (int i = 0; i < textLen + 2; i++) {
        addToArray(render_buffer, "─", render_buffer->len, strlen("─"));
    }
    addToArray(render_buffer, "┐\r\n", render_buffer->len, strlen("┐\r\n"));
    addToArray(render_buffer, padding, render_buffer->len, leftPadding);
    addToArray(render_buffer, "│ ", render_buffer->len, strlen("│ "));
    addToArray(render_buffer, text, render_buffer->len, textLen);
    addToArray(render_buffer, " │\r\n", render_buffer->len, strlen(" │\r\n"));
    addToArray(render_buffer, padding, render_buffer->len, leftPadding);
    addToArray(render_buffer, "└", render_buffer->len, strlen("└"));
    for (int i = 0; i < textLen + 2; i++) {
        addToArray(render_buffer, "─", render_buffer->len, strlen("─"));
    }
    addToArray(render_buffer, "┘\r\n", render_buffer->len, strlen("┘\r\n"));
}

void drawSave(Editor *editor) {
    Array render_buffer;
    initArray(&render_buffer, sizeof(char));

    int w = editor->window_col;
    int h = editor->window_row;
    int filenameWindow = 40;

    char emptyBuffer[w * (h / 2)];
    memset(emptyBuffer, 32, (h / 2) * w);

    // clear screen \x1b[2J
    // move cursor to top left \x1b[H
    addToArray(&render_buffer, "\x1b[2J\x1b[H", 0, 7);
    addToArray(&render_buffer, emptyBuffer, render_buffer.len,
               ((h / 2) - 4) * w);

    // addToArray(&render_buffer, emptyBuffer, render_buffer.len,
    //            (w - filenameWindow) / 2);
    // addToArray(&render_buffer, "\x1b[44m", render_buffer.len, 5);
    char *filename = editor->filename;
    addBorderedText(&render_buffer, filename, (w - strlen(filename)) / 2);
    // addToArray(&render_buffer, filename, render_buffer.len,
    // strlen(filename));
    addToArray(&render_buffer, emptyBuffer, render_buffer.len,
               filenameWindow - strlen(filename));
    // addToArray(&render_buffer, "\x1b[0m", render_buffer.len, 4);

    char cursor_pos[32];
    snprintf(cursor_pos, sizeof(cursor_pos), "\x1b[%d;%dH",
             editor->cursor_row + 1, editor->cursor_col + 1);
    addToArray(&render_buffer, &cursor_pos, render_buffer.len,
               strlen(cursor_pos));

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
