#include "shifter.h"

static char Usage[30];

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

void rewriteFile(Context *ctx, char **argv) {
    FILE* file = fopen(argv[1], "wb");
    if (file == NULL) {
        fprintf(stderr, "No such file or directory.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    FILE* tmpFile = fopen("temp.file", "rb");
    if (tmpFile == NULL) {
        fprintf(stderr, "Failed to create temp file.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    ctx->currFile = file, ctx->tempFile = tmpFile;
    size_t bRead = 0;
    char buff[4096] = {0};
    while  ((bRead = fread(buff, 1, 4096, ctx->tempFile)) > 0) {
        for (size_t i = 0; i < bRead; i++) {
            fwrite(&buff[i], 1, 1, ctx->currFile);
        }
    }
    fflush(ctx->currFile);
    fclose(ctx->currFile);
    fclose(ctx->tempFile);
}

void getTempByte(Context *ctx) {
    if (ctx->isNegShift == true) {
        fseek(ctx->currFile, 0, SEEK_SET);
        unsigned int result = fread(&ctx->lastByte, 1, 1, ctx->currFile);
        if (result == 0) {
            fprintf(stderr, "reading first byte failed.");
            exit(EXIT_FAILURE);
        }
        fseek(ctx->currFile, 0, SEEK_SET);
    } else {
        fseek(ctx->currFile, -1, SEEK_END);
        unsigned int result = fread(&ctx->tempByte, 1, 1, ctx->currFile);
        if (result == 0) {
            fprintf(stderr, "reading last byte failed.");
            exit(EXIT_FAILURE);
        }
        fseek(ctx->currFile, 0, SEEK_SET);
    }
}

unsigned int getShiftValue(char *str) {
    unsigned int res = 0;
    while (*str && isdigit(*str)) {
        res = res * 10 + (*str - '0');
        (str)++;
    }
    if (!isdigit(*str) && *str != '\0') {
        res = 10;
    }
    return res;
}

void circularShift(char* buff, size_t size, Context *ctx, size_t count) {
    char *temp = (char*)calloc(size, 1);
    memcpy(temp, buff, size);
    ctx->shiftCount %= 8;
    size_t shiftByte = ctx->shiftCount / 8;
    for (size_t index = 0; index < size; index++) {
        index = (index + shiftByte) % size;
        unsigned char byte = temp[index];
        unsigned char shifted_byte = 0;
        unsigned int index_neighbor = ctx->isNegShift ? (index + 1) % size : (index + size - 1) % size;
        unsigned char byte_neighbor = temp[index_neighbor];
        if (ctx->isNegShift == true) {
            if (index == size - 1 && ctx->isEnd == true) {
                shifted_byte = byte << ctx->shiftCount | (ctx->lastByte >> (8 - ctx->shiftCount));
                fwrite(&shifted_byte, 1, 1, ctx->tempFile);
                break;
            }
            if (index == size - 1) {
                ctx->tempByte = byte;
            }
            if (index == 0 && count != 0) {
                shifted_byte = ctx->tempByte << (ctx->shiftCount) | byte >> (8 - ctx->shiftCount);
                fwrite(&shifted_byte, 1, 1, ctx->tempFile);
            }
            if (index != size - 1)  {
                shifted_byte = byte << ctx->shiftCount | byte_neighbor >> (8 - ctx->shiftCount);
                fwrite(&shifted_byte, 1, 1, ctx->tempFile);
            }
        } else {
            shifted_byte = byte >> ctx->shiftCount | (ctx->tempByte << (8 - ctx->shiftCount));
            ctx->tempByte = byte;
            fwrite(&shifted_byte, 1, 1, ctx->tempFile);
        }
    }
    free(temp);
}

void shiftBites(Context *ctx) {
    char buff[4096] = {0};
    size_t bRead = 0, count = 0;
    while  ((bRead = fread(buff, 1, 4096, ctx->currFile)) > 0) {
        if (bRead > 0 && bRead < 4096) {
            ctx->isEnd = true;
        }
        circularShift(buff, bRead, ctx, count);
        count++;
    }
    fflush(ctx->tempFile);
    fclose(ctx->tempFile);
    fclose(ctx->currFile);
}

void readBitesFromFile(FILE *file) { //debug func for read bites;
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
    unsigned int val = getShiftValue(&argv[2][0]);
    if (val > 8) {
        fprintf(stderr, "shiftCount value must be between -8 and 8.\n%s", Usage);
        exit(EXIT_FAILURE);
    }
    ctx->shiftCount=val;
}

int main(int argc, char **argv) {
    Context ctx = {0};
    sprintf(Usage, "Usage: %s file.any shiftCount", argv[0]);
    ruleCheck(&ctx, argc, argv);
    openFiles(&ctx, argv);
    getTempByte(&ctx);
    shiftBites(&ctx);
    rewriteFile(&ctx, argv);
    remove("temp.file");
    fprintf(stderr, "Success!.\n");
    exit(EXIT_SUCCESS);
}