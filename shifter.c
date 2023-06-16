#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

static char Usage[30];

typedef struct Context {
    size_t shiftCount;
    bool isNegShift;
    FILE *currFile;
    FILE *tempFile;
    char tempByte;
    bool isEnd;
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


void openFiles(Context *ctx, char **argv) {
    FILE* file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "No such file or directory.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    fseek (file, 0, SEEK_END);
    size_t size = ftell(file);
    if (size == 0) {
        fprintf(stderr, "File is empty.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    FILE* tmpFile = fopen("temp.file", "wb");
    if (tmpFile == NULL) {
        fprintf(stderr, "Failed to create temp file.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    ctx->currFile = file, ctx->tempFile = tmpFile;
}

void readBitesFromFile(FILE *file){ //debug func for read bites;
    int byte;
    while ((byte = fgetc(file)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            printf("%d", (byte >> i) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

void readBitesFromByte(char byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
    }
    printf(" ");
    printf("\n");
}

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
    size_t bitShift = ctx->shiftCount;
    char *temp = (char*)calloc(size, 1);
    memcpy(temp, buff, size);
    for (size_t i = 0; i < size; i++) {
        uint index = i;
        unsigned char byte = temp[index];
        unsigned char shifted_byte = 0;
        uint index_neighbor = ctx->isNegShift ? (index + 1) % size : (index + size - 1) % size;
        unsigned char byte_neighbor = temp[index_neighbor];
        if (ctx->isNegShift == true) {
            shifted_byte = (byte << bitShift) | (byte_neighbor >> (8 - bitShift));
            if (i == size - 1 && ctx->isEnd) {
                shifted_byte = byte << bitShift | (ctx->tempByte >> (8 - bitShift));
            }
        } else {
            shifted_byte = (byte >> bitShift) | (byte_neighbor << (8 - bitShift));
            if (i == 0 && count == 0) {
                shifted_byte = byte >> bitShift | (ctx->tempByte << (8 - bitShift));
            }
        }
        buff[index] = shifted_byte;
    }
    fprintf(ctx->tempFile, "%s" , buff);
    ctx->tempByte = temp[ctx->isNegShift ? size - 1 : 0];
    free(temp);
}

int main(int argc, char **argv) {
    Context ctx = {0};
    sprintf(Usage, "Usage: %s file.any shiftCount", argv[0]);
    ruleCheck(&ctx, argc, argv);
    char buff[2048] = {0};
    openFiles(&ctx, argv);
    getTempByte(&ctx);
//    readBitesFromByte(ctx.tempByte);
    size_t bRead = 0, count = 0;
    while  ((bRead = fread(buff, 1, 2048, ctx.currFile)) > 0) {
        if (bRead > 0 && bRead < 2048) {
            ctx.isEnd = true;
        }
        circularShift(buff, bRead, &ctx, count);
        count++;
    }
    fclose(ctx.tempFile);
    fseek(ctx.currFile, 0, SEEK_SET);
    ctx.tempFile = fopen("temp.file", "r");
    readBitesFromFile(ctx.currFile);
    readBitesFromFile(ctx.tempFile);
    fclose(ctx.currFile);
    remove(argv[1]);
    rename("temp.file", argv[1]);
    fprintf(stderr, "Success!.");
    exit(EXIT_SUCCESS);
}

