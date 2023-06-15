#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

static char Usage[30];

typedef struct Context {
    unsigned int shiftCount;
    bool isNegShift;
    FILE *currFile;
    FILE *tempFile;
    char tempByte;
} Context;

void getTempByte(Context *ctx) {
    if (ctx->isNegShift == true) {
        fseek(ctx->currFile, 0, SEEK_SET);
        fread(&ctx->tempByte, 1, 1, ctx->currFile);
        fseek(ctx->currFile, 0, SEEK_SET);
    } else {
        fseek(ctx->currFile, -1, SEEK_END);
        fread(&ctx->tempByte, 1, 1, ctx->currFile);
        fseek(ctx->currFile, 0, SEEK_SET);
    }
}

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
    FILE* tmpFile = fopen("temp.file", "wb");
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
        ctx->isNegShift = true;
        (argv[2])++;
    }
    uint val = getShiftValue(&argv[2][0]);
    if (val > 8) {
        fprintf(stderr, "shiftCount value must be between -8 and 8.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    ctx->shiftCount=val;
}

//size_t count
void circularShift(char* buff, size_t size, Context *ctx, size_t count) {
    uint byteShift = ctx->shiftCount / 8;
    uint bitShift = ctx->shiftCount % 8;
    if (ctx->isNegShift == true) {
        byteShift = (size - byteShift) % size;
    }
    char* temp = (char*)malloc(size);
    memcpy(temp, buff, size);
    for (size_t i = 0; i < size; i++) {
        uint index = (i + byteShift) % size;
        char byte = temp[index];
        char shifted_byte;
        if (ctx->isNegShift == false) {
            shifted_byte =  (byte >> bitShift) | temp[(i + size - 1) % size] << (8 - bitShift);
        } else {
            shifted_byte = (byte << bitShift) | temp[(i + 1) % size] >> (8 - bitShift);
        }
        buff[i] = shifted_byte;
    }
//    if (ctx->isNegShift == false && count == 0) {
//        buff[0] |= ctx->tempByte << (8 - bitShift);
//    }
    writeToFile(ctx->tempFile, buff);
    free(temp);
}


int main(int argc, char **argv) {
    Context ctx;
    sprintf(Usage, "Usage: %s file.any shiftCount", argv[0]);
    ruleCheck(&ctx, argc, argv);
    char buff[2048] = {0};
    openFiles(&ctx, argv);
    getTempByte(&ctx);
    size_t bRead = 0, count = 0;
    while  ((bRead = fread(buff, 1, 2048, ctx.currFile)) > 0) {
        circularShift(buff, bRead - 1, &ctx, count);
        count++;
    }
    fclose(ctx.tempFile);
    fseek(ctx.currFile, 0, SEEK_SET);
    ctx.tempFile = fopen("temp.file", "r");
    readBites(ctx.currFile);
    readBites(ctx.tempFile);

//    rename(argv[1], "")
    remove("temp.file");
}

