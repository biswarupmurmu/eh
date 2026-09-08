#include "array.h"
#include "editor.h"
#include "terminal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

// handlekeypress will return the command or the input char
// based on the inpute need to recalculate the cursor pos
// the brain to process input will be done here
// need to predefine the commands or the inputs the keypress handler will return
//


void readKeyPress(char c, Editor *editor) {
    Array **lines = (Array **)editor->buffer->ptr;
    if (lines == NULL)
        return;
    switch (c) {
    case CTRL_KEY('c'):
        disableRawMode();
        exit(0);
        break;

    case CTRL_KEY('s'):
        // writeBufferToFile(editor->buffer, editor->filename);
        // disableRawMode();
        // exit(0);
        if (editor->editing_view == true) {
            editor->editing_view = false;
        } else {
            editor->editing_view = true;
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
        }

        break;

    case '\x1b': {
        char seq[3];

        // Try to read two more bytes into seq[0] and seq[1].
        // If they aren't there, the user just pressed the ESC key by itself.
        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            break;
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            break;

        // If the sequence starts with '[', it's an arrow key (or page up/down)
        if (seq[0] == '[') {
            switch (seq[1]) {
            case 'A': // Up
                if (editor->buffer_line > 0 && editor->cursor_row == 0) {
                    editor->render_start_line -= 1;
                }
                if (editor->cursor_row > 0) {
                    editor->cursor_row -= 1;
                }

                if (editor->buffer_line > 0) {
                    editor->buffer_line -= 1;
                }

                if (lines[editor->buffer_line]->len < editor->cursor_col) {
                    editor->cursor_col = lines[editor->buffer_line]->len - 1;
                }
                editor->buffer_char = editor->cursor_col;
                break;

            case 'B':
                // Down
                if (editor->buffer_line == editor->buffer->len - 1)
                    break;
                if (editor->buffer_line < editor->buffer->len - 1 &&
                    editor->cursor_row == editor->window_row - 1) {
                    editor->render_start_line += 1;
                }
                editor->buffer_line += 1;
                editor->cursor_row += 1;
                if (editor->buffer_line > editor->buffer->len - 1) {
                    editor->buffer_line -= 1;
                }
                if (editor->cursor_row > editor->window_row - 1) {
                    editor->cursor_row -= 1;
                }

                if (lines[editor->buffer_line]->len < editor->cursor_col) {
                    editor->cursor_col = lines[editor->buffer_line]->len - 1;
                }
                editor->buffer_char = editor->cursor_col;

                break;
            case 'C': // Right
                editor->cursor_col += 1;
                if (editor->cursor_col > lines[editor->buffer_line]->len - 1) {
                    editor->cursor_col = lines[editor->buffer_line]->len - 1;
                }
                editor->buffer_char = editor->cursor_col;

                break;
            case 'D': // Left
                editor->cursor_col -= 1;
                if (editor->cursor_col < 0) {
                    editor->cursor_col = 0;
                }
                editor->buffer_char = editor->cursor_col;
                break;
            }
        }
        break;
    }

    default:
        addToArray(lines[editor->buffer_line], &c, editor->buffer_char, 1);
        editor->cursor_col += 1;
        if (editor->cursor_col > lines[editor->buffer_line]->len - 1) {
            editor->cursor_col = lines[editor->buffer_line]->len - 1;
        }
        editor->buffer_char = editor->cursor_col;

        break;
    };
}
