#include "array.h"
#include "editor.h"
#include "filehandler.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

void handleUpArrow(Editor *editor) {
    // the cursor will go to previous row if previous row exists
    // and if previous row is not visible in the screen make it visible
    if (editor->cursor_row == 0 && editor->render_start_line > 0) {
        // cusor stays in the same row but previous line gets visible in that
        // row
        editor->buffer_line -= 1;
        editor->render_start_line -= 1;
    } else if (editor->cursor_row > 0) {
        editor->buffer_line -= 1;
        editor->cursor_row -= 1;
    }

    Array **lines = (Array **)editor->buffer->ptr;
    // update cursor col position
    if (lines[editor->buffer_line]->len < editor->cursor_col) {
        editor->cursor_col = lines[editor->buffer_line]->len - 1;
        editor->buffer_char = editor->cursor_col;
    }
}

void handleDownArrow(Editor *editor) {
    if (editor->cursor_row == editor->window_row - 1 &&
        editor->buffer_line < editor->buffer->len - 1) {
        // if its the bottom row and there are more lines
        editor->buffer_line += 1;
        editor->render_start_line += 1;
    } else if (editor->buffer_line < editor->buffer->len - 1) {
        // there is still one more line left to go
        editor->buffer_line += 1;
        editor->cursor_row += 1;
    }

    Array **lines = (Array **)editor->buffer->ptr;
    // update cursor col position
    if (lines[editor->buffer_line]->len < editor->cursor_col) {
        editor->cursor_col = lines[editor->buffer_line]->len - 1;
        editor->buffer_char = editor->cursor_col;
    }
}

void handleLeftArrow(Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    if (editor->cursor_col == 0 && editor->buffer_line > 0) {
        handleUpArrow(editor);
        editor->cursor_col = lines[editor->buffer_line]->len - 1;
        editor->buffer_char = lines[editor->buffer_line]->len - 1;
    } else if (editor->cursor_col > 0) {
        editor->cursor_col -= 1;
        editor->buffer_char = editor->cursor_col;
    }
}

void handleRightArrow(Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    if (editor->buffer_char == lines[editor->buffer_line]->len - 1 &&
        editor->buffer_line < editor->buffer->len - 1) {
        handleDownArrow(editor);
        editor->buffer_char = 0;
        editor->cursor_col = 0;
    } else if (editor->buffer_char < lines[editor->buffer_line]->len) {
        editor->cursor_col += 1;
        editor->buffer_char = editor->cursor_col;
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
        if (editor->buffer_char > 0) {

            removeFromArray(lines[editor->buffer_line], editor->buffer_char - 1,
                            1);
            editor->buffer_char -= 1;
            editor->cursor_col -= 1;
        } else if (editor->buffer_char == 0 && editor->buffer_line > 0) {
            Array *previousLine = lines[editor->buffer_line - 1];
            int cursor_pos = previousLine->len - 1;
            if (cursor_pos < 0) {
                cursor_pos = 0;
            }
            removeFromArray(previousLine, previousLine->len - 1, 1);
            addToArray(previousLine, lines[editor->buffer_line]->ptr,
                       previousLine->len, lines[editor->buffer_line]->len);

            // remove the current line
            freeArray(lines[editor->buffer_line]);
            free(lines[editor->buffer_line]);
            removeFromArray(editor->buffer, editor->buffer_line, 1);
            handleUpArrow(editor);
            editor->buffer_char = cursor_pos;
            editor->cursor_col = cursor_pos;
        }

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

            handleDownArrow(editor);
            editor->buffer_char = 0;
            editor->cursor_col = 0;
            break;
        } else if (editor->editing_view == false) {
            writeBufferToFile(editor->buffer, editor->filename->ptr);
            exitEditor(editor);
        }
        break;
    default:
        if (c >= 32 && c <= 126) {
            addToArray(lines[editor->buffer_line], &c, editor->buffer_char, 1);
            editor->cursor_col += 1;
            editor->buffer_char = editor->cursor_col;
        }
        break;
    };
}
