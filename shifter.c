#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

static char Usage[30];

typedef struct Context {
    unsigned int shiftCount;
    bool isNeg;
    FILE *currFile;
    FILE *tempFile;
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

void openFiles(Context *ctx, char **argv) {
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
    ctx->currFile = file, ctx->tempFile = tmpFile;
}

void readBites(FILE *file){ //debug func for read bites;
    int byte;
    while ((byte = fgetc(file)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            printf("%d", (byte >> i) & 1);
        }
        printf(" ");
    }
    printf("\n\n\n");
};


void ruleCheck(Context *ctx, int argc, char **argv) {
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
    uint byteShift = ctx->shiftCount / 8;
    uint bitShift = ctx->shiftCount % 8;
    if (ctx->isNeg == true) {
        byteShift = (size - byteShift) % size;
    }
    char* temp = (char*)malloc(size);
    memcpy(temp, buff, size);
    for (size_t i = 0; i < size; i++) {
        uint index = (i + byteShift) % size;
        char byte = temp[index];
        char shifted_byte;
        if (ctx->isNeg == true) {
            shifted_byte = (byte << bitShift) | (byte >> (8 - bitShift));
        } else {
            shifted_byte = (byte >> bitShift) | (byte << (8 - bitShift));
        }
        buff[i] = shifted_byte;
    }
    writeToFile(ctx->tempFile, buff);
    free(temp);
}


int main(int argc, char **argv) {
    Context ctx;
    sprintf(Usage, "Usage: %s file.any shiftCount", argv[0]);
    ruleCheck(&ctx, argc, argv);
    char buff[2048] = {0}, temp[1] = {0};
    size_t bRead = 0;
    while  ((bRead = fread(buff, 1, 2048, ctx.currFile)) > 0) {
        circularShift(buff, bRead - 1, &ctx);
    }
    fclose((ctx.currFile));
    fclose(ctx.tempFile);
    ctx.currFile = fopen(argv[1], "r");
    if (ctx.currFile == NULL) {
        fprintf(stderr, "No such file or directory.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    ctx.tempFile = fopen("temp.file", "r");
    if (ctx.tempFile == NULL) {
        fprintf(stderr, "Failed to create temp file.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    readBites(ctx.currFile);
    readBites(ctx.tempFile);
    fclose((ctx.currFile));
    fclose(ctx.tempFile);
//    rename(argv[1], "")
    remove("temp.file");
}

