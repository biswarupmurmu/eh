#include "array.h"
#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void writeBufferToFile(Array *buffer, char *filename) {
    // it took me some time to understand this double pointer thing
    // i dont know what i was thinking i made it unnecessarily complex
    // its simple
    // im declaring the variable lines, and its a pointer
    // so one * for that but what is the type of value the pointer is pointing
    // the type is Array *
    // hence two *'s
    // trying to make it even simpler: when you want to declare a pointer to an
    // int int *a; it just means a is a pointer and the it points to an int's
    // address now what if a points to an pointer which points to the address of
    // an int *a is the variable declaration, but i need to tell the type to the
    // compiler the type is int * so it comes down to int **a
    //
    // here buffer->ptr holds the pointer of the memory block in heap, and in
    // the block the pointers of Arrays' are stored so it's Array * *lines ---->
    // Array **lines
    Array **lines = buffer->ptr;
    Array write_buffer;
    initArray(&write_buffer, sizeof(char));

    for (int i = 0; i < buffer->len; i++) {
        addToArray(&write_buffer, lines[i]->ptr, write_buffer.len,
                   lines[i]->len);
    }

    FILE *file_ptr = fopen(filename, "w");
    if (file_ptr == NULL) {
        perror("Error opening file");
    }

    if (fputs(write_buffer.ptr, file_ptr) == EOF) {
        perror("Error saving file.\n");
        fclose(file_ptr);
    }
    freeArray(&write_buffer);

    fclose(file_ptr);
}

void readFileToBuffer(Array *buffer, char *filename) {
    FILE *file_ptr;
    char readbuffer[1025];
    readbuffer[1024] = '\0';

    file_ptr = fopen(filename, "r");
    if (file_ptr == NULL) {
        perror("File open failed");
		initBuffer(buffer);
		return; // assuning the file does not exist
		// need to make the logic better
        // exit(EXIT_FAILURE);
    }

    while (fgets(readbuffer, sizeof(readbuffer) - 1, file_ptr) != NULL) {
        // create a new line
        Array *newline = malloc(sizeof(Array));
        initArray(newline, sizeof(char));

        addToArray(newline, readbuffer, 0, strlen(readbuffer));

        // passing the address of the pointer to be stored
        addToArray(buffer, &newline, buffer->len, 1);
    }
    if (buffer->len == 0) {
        Array *emptyline = malloc(sizeof(Array));
        initArray(emptyline, sizeof(char));
        addToArray(buffer, &emptyline, buffer->len, 1);
    }
    fclose(file_ptr);
}
