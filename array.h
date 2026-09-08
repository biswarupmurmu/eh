#ifndef ARRAY_H
#define ARRAY_H

typedef struct {
    int len;
    int element_size;
    void *ptr;
} Array;

void initArray(Array *array, int element_size);
void freeArray(Array *array);
void removeFromArray(Array *array, int index, int length);
void addToArray(Array *array, void *new_array, int index, int new_array_len);

#endif
