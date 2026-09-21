#include "array.h"
#include "editor.h"
#include "filehandler.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

// the last index for a line of length n will be n-1
// the last character is a newline character, so n-1

static void updateBufferChar(Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    Array *current = lines[editor->buffer_line];

    // try to be in the same column
    editor->buffer_char = editor->buffer_char % editor->window_col;

    if (current->len == 0) {
        editor->buffer_char = 0;
    } else if (editor->buffer_char > current->len - 1) {
        // check if buffer_char is out of bound
        editor->buffer_char = current->len - 1;
    }
}

void handleUpArrow(Editor *editor) {

    int new_buffer_char = editor->buffer_char - editor->window_col;

    if (new_buffer_char < 0) {
        // need to move to new line
        if (editor->buffer_line > 0) {
            editor->buffer_line -= 1;
            updateBufferChar(editor);
        }
    } else {
        editor->buffer_char = new_buffer_char;
    }
}

static void handleDownArrow(Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    int current_line_len = lines[editor->buffer_line]->len;

    int new_buffer_char = editor->buffer_char + editor->window_col;

    if (new_buffer_char >= current_line_len) {
        // need to move to new line
        if (editor->buffer_line < editor->buffer->len - 1) {
            editor->buffer_line += 1;
            updateBufferChar(editor);
        }
    } else {
        editor->buffer_char = new_buffer_char;
    }
}

void handleLeftArrow(Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;

    int new_buffer_char = editor->buffer_char - 1;
    if (new_buffer_char < 0) {
        handleUpArrow(editor);
        editor->buffer_char = lines[editor->buffer_line]->len - 1;
    } else {
        editor->buffer_char = new_buffer_char;
    }
}

void handleRightArrow(Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    Array *current_line = lines[editor->buffer_line];

    int new_buffer_char = editor->buffer_char + 1;
    if (new_buffer_char >= current_line->len) {
        handleDownArrow(editor);
        editor->buffer_char = 0;
    } else {
        editor->buffer_char = new_buffer_char;
    }
}

static void handleBackspace(Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    if (editor->buffer_char > 0) {
        removeFromArray(lines[editor->buffer_line], editor->buffer_char - 1, 1);
        editor->buffer_char -= 1;

    } else if (editor->buffer_char == 0 && editor->buffer_line > 0) {
        Array *previousLine = lines[editor->buffer_line - 1];
        removeFromArray(previousLine, previousLine->len - 1, 1);
        addToArray(previousLine, lines[editor->buffer_line]->ptr,
                   previousLine->len, lines[editor->buffer_line]->len);

        // remove the current line
        freeArray(lines[editor->buffer_line]);
        free(lines[editor->buffer_line]);
        removeFromArray(editor->buffer, editor->buffer_line, 1);
        handleLeftArrow(editor);
    }
}

static void handleNewLine(Editor *editor) {
    if (editor->editing_view == true) {
        Array **lines = editor->buffer->ptr;
        Array *current_line = lines[editor->buffer_line];

        Array *newline = malloc(sizeof(Array));
        initArray(newline, sizeof(char));

        addToArray(newline, current_line->ptr + editor->buffer_char,
                   newline->len, current_line->len - editor->buffer_char);

        removeFromArray(current_line, editor->buffer_char,
                        current_line->len - editor->buffer_char);
        addToArray(current_line, "\n", current_line->len, strlen("\n"));

        addToArray(editor->buffer, &newline, editor->buffer_line + 1, 1);

        handleRightArrow(editor);
    } else if (editor->editing_view == false) {
        writeBufferToFile(editor->buffer, editor->filename->ptr);
        exitEditor(editor);
    }
}

void readKeyPress(char c, Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    if (lines == NULL)
        return;
    switch (c) {
    case CTRL_KEY('c'):
        if (editor->editing_view == true) {
            editor->editing_view = false;
            break;
        }
        exitEditor(editor);
        break;

    case CTRL_KEY('s'):
        // go to save window
        if (editor->editing_view == true) {
            editor->editing_view = false;
        }
        break;
    case 127:
    case '\b':
        // ASCII 8
        handleBackspace(editor);
        break;

    case 27: {
        char seq[3];

        // Try to read two more bytes into seq[0] and seq[1].
        // If they aren't there, the user just pressed the ESC key by itself.
        if (read(STDIN_FILENO, &seq[0], 1) != 1) {
            if (editor->editing_view == false) {
                editor->editing_view = true;
            }
            break;
        }
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            break;

        // If the sequence starts with '[', it's an arrow key (or page up/down)
        if (seq[0] == '[') {
            if (editor->editing_view == false)
                break;

            switch (seq[1]) {
            case 'A': // Up
                handleUpArrow(editor);
                break;

            case 'B':
                // Down
                handleDownArrow(editor);
                break;
            case 'C': // Right
                handleRightArrow(editor);
                break;
            case 'D': // Left
                handleLeftArrow(editor);
                break;
            }
        }
        break;
    }
    case 10: // \n
        handleNewLine(editor);
        break;
    default:
        if (c >= 32 && c <= 126) {
            addToArray(lines[editor->buffer_line], &c, editor->buffer_char, 1);
            editor->buffer_char += 1;
        }
        break;
    };
}
