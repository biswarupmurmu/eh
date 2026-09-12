#include "array.h"
#include "draw.h"
#include "editor.h"
#include "filehandler.h"
#include "keypresshandler.h"
#include <string.h>
#include <unistd.h>

void handleArgs(int argc, char *argv[], Editor *editor) {
    if (argc == 1) {
        initBuffer(editor);
        return;
    }
    char *filename = argv[1];
    readFileToBuffer(editor->buffer, filename);
    freeArray(editor->filename);
    addToArray(editor->filename, filename, editor->filename->len,
               strlen(filename));
}

int main(int argc, char *argv[]) {
    Editor editor;
    Array buffer;
    initArray(&buffer, sizeof(Array *));
    editor.buffer = &buffer;
    initEditor(&editor);
    handleArgs(argc, argv, &editor);

    drawEditor(&editor);
    char c;
    while (1) {
        int n = read(STDIN_FILENO, &c, 1);
        if (n == 1) {
            readKeyPress(c, &editor);
            drawEditor(&editor);
        }
    }
    exitEditor(&editor);
    return 0;
}
