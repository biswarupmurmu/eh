#include "array.h"
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

    char *filename = editor->filename;

    addBorderedText(&render_buffer, filename, w);
    addCenteredText(&render_buffer, "Enter to \033[32msave\033[0m", w, 9);
    addCenteredText(&render_buffer, "Esc to \033[34mreturn\033[0m to editing", w,
                    9);
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
