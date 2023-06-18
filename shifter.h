#ifndef INC_SHIFTER_H
#define INC_SHIFTER_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct Context {
    size_t shiftCount;
    bool isNegShift;
    FILE *currFile;
    FILE *tempFile;
    unsigned char tempByte;
    unsigned char lastByte;
    bool isEnd;
} Context;

// files funcs
void openFiles(Context *ctx, char **argv);
void rewriteFile(Context *ctx, char **argv);

//utils and business-logic funcs
void getTempByte(Context *ctx);
unsigned int getShiftValue(char *str);
void circularShift(char* buff, size_t size, Context *ctx, size_t count);
void shiftBites(Context *ctx);

// debug funcs
void readBitesFromFile(FILE *file);
void readBitesFromByte(char byte);


#endif //INC_SHIFTER_H
