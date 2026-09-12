#include "array.h"
#include "draw.h"
#include "editor.h"
#include "filehandler.h"
#include "keypresshandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printHelp() {
    printf("Usage: eh [filename]\n");
    printf("If filename is not given a new file will be created\n");
	printf("To save the file Ctrl+s\n");
	printf("To exit the program Ctrl+c\n");
    printf("  -h, --help    Show this help message and exit\n");
}

void printVersion() { printf("eh version 0.0.1\n"); }

void handleArgs(int argc, char *argv[], Editor *editor) {
    if (argc == 1) {
        initBuffer(editor->buffer);
        initEditor(editor);
        return;
    }
    // maybe i will ignore all other args except for the first one
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printHelp();
        exit(EXIT_SUCCESS);
    }

    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        printVersion();
        exit(EXIT_SUCCESS);
    }

    initEditor(editor);

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
