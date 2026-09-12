#include "array.h"
#include <stdlib.h>
#include <string.h>

void initArray(Array *array, int element_size) {
    if (!array || element_size <= 0) {
        return;
    }
    array->len = 0;
    array->element_size = element_size;
    array->ptr = NULL;
}

void freeArray(Array *array) {
    // freeing the malloc'ed dynamic array pointer
    if (!array)
        return;
    array->len = 0;
    free(array->ptr);
	array->ptr = NULL;
}

void removeFromArray(Array *array, int index, int length) {
    if (!array || index < 0 || length < 0 || index + length > array->len) {
        return;
    }

    // move things to the left
    // delete 'length' number of elements starting from 'index'

    // destination: array.ptr + index * array.element_size
    // src: array.ptr + (index + length) * array.element_size

    char *ptr = array->ptr;
    memmove(ptr + index * array->element_size,
            ptr + (index + length) * array->element_size,
            (array->len - (index + length)) * array->element_size);

    array->len = array->len - length;

    memset(ptr + (array->len * array->element_size), 0, array->element_size);
}

void addToArray(Array *array, void *new_array, int index, int new_array_len) {
    if (!array || !new_array || index < 0 || index > array->len) {
        return;
    }

    int new_len = array->len + new_array_len;
    int element_size = array->element_size;

    // adding 1 for null terminator for string, just in case
    // pointer typer is char, as char is exactly 1 byte
    // so it will be easier to move byte by byte
    char *newPtr = realloc(array->ptr, (new_len + 1) * element_size);
    if (newPtr == NULL) {
        return;
    }

    // move everything from index to the right to make room for the new array
    // move start from: newPtr+(index * element_size)
    // move destination: newPtr + ((index + new_array_len) * element_size)
    // how much to move: all the elements from the index
    // if index = 0, move all elements
    // if index = 1, move array.len - 1 elements
    if (index < array->len) {
        memmove(newPtr + (index + new_array_len) * element_size,
                newPtr + index * element_size,
                (array->len - index) * element_size);
    }

    memcpy(newPtr + index * element_size, new_array,
           new_array_len * element_size);

    array->ptr = newPtr;
    array->len = new_len;

    memset(newPtr + (new_len * element_size), 0, element_size);
}
