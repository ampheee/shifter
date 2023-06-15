#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

static char Usage[30];

typedef struct Context {
    unsigned int shiftCount;
    bool isNeg;
    FILE *CurrFile;
    FILE *TempFile;
} Context;

uint getShiftValue(char *str) {
    uint res = 0;
    while (*str && isdigit(*str)) {
        res = res * 10 + (*str - '0');
        (str)++;
    }
    if (!isdigit(*str) && *str != '\0') {
        res = 10;
    }
    return res;
}

int writeToFile(FILE *temp, char *buff) {
    fprintf(temp, "%s", buff);
    return 0;
}

void readBites(FILE *file){ //debug func for read bites;
    int byte;
    while ((byte = fgetc(file)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            printf("%d", (byte >> i) & 1);
        }
        printf(" ");
    }
    printf("\n\n\n\n\n");
};


void ruleCheck(int argc, char **argv, Context *ctx) {
    if (argc != 3) {
        fprintf(stderr, argc > 3 ? "Too many args.\n%s" : "Too few args.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    if (argv[2][0] == '-') {
        ctx->isNeg = true;
        (argv[2])++;
    }
    uint val = getShiftValue(&argv[2][0]);
    if (val > 8) {
        fprintf(stderr, "shiftCount value must be between -8 and 8.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    ctx->shiftCount=val;
}

void circularShift(char* buff, size_t size, Context *ctx) {
    uint byte_shift = ctx->shiftCount / 8;
    uint bit_shift = ctx->shiftCount % 8;
    char* temp = (char*)malloc(size);
    memcpy(temp, buff, size);
    for (size_t i = 0; i < size; i++) {
        uint index = (i + byte_shift) % size;
        char byte = temp[index];
        char shifted_byte;
        if (ctx->isNeg == false) {
            shifted_byte = (byte << bit_shift) | (byte >> (8 - bit_shift));
        } else {
            shifted_byte = (byte >> bit_shift) | (byte << (8 - bit_shift));
        }
        buff[i] = shifted_byte;
    }
    writeToFile(ctx->TempFile, buff);
    free(temp);
}


int main(int argc, char **argv) {
    Context ctx;
    sprintf(Usage, "Usage: %s file.any shiftCount", argv[0]);
    ruleCheck(argc, argv, &ctx);
    FILE* file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "No such file or directory.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    FILE* tmpFile = fopen("temp.file", "a");
    if (tmpFile == NULL) {
        fprintf(stderr, "Failed to create temp file.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    ctx.CurrFile = file, ctx.TempFile = tmpFile;
    char buff[2048] = {0};
    size_t bRead = 0;
    while  ((bRead = fread(buff, 1, 2048, file)) > 0) {
        circularShift(buff, bRead, &ctx);
    }
    fclose((ctx.CurrFile));
    fclose(ctx.TempFile);
    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "No such file or directory.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    tmpFile = fopen("temp.file", "r");
    if (tmpFile == NULL) {
        fprintf(stderr, "Failed to create temp file.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    readBites(file);
    readBites(tmpFile);
    fclose((ctx.CurrFile));
    fclose(ctx.TempFile);
}

//
//int RenameFile() {
//
//}

