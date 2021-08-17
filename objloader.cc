#include "types.h"
#include <stdio.h>

#pragma pack(push, 1)
struct BMPFileHeader {
    // bitmap file header
    u16 fileType;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmapOffset;
};
#pragma pack(pop);


bool fileExists(const char* str) {
    FILE* fp = fopen(str, "rb");
    if (!fp) {
        return false;
    }
    return true;
}

u8* loadBitmap(const char* fileName) {
    u32 sz;
    u8* bitMap;
    FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        return 0;
    }
    BMPFileHeader header;
    if (fread(&header, 1, sizeof(BMPFileHeader), fp) != sizeof(BMPFileHeader)) {
        return 0;
    }
    sz = header.fileSize - header.bitmapOffset;
    bitMap = (u8*) malloc(sz);
    fread(bitMap, 1, sz, fp);
    fclose(fp);

    return bitMap;
}

// TODO
void loadMesh(const char* objPrefix, Mesh* mesh ) {
    char fullFileName[300]; //this is the entire file name
    
}


