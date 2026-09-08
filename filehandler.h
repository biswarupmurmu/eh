#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "array.h"

void writeBufferToFile(Array *buffer, char *filename);
void readFileToBuffer(Array *buffer, char *filename);

#endif
