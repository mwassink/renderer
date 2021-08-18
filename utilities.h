#ifndef UTILITIES_H
#define UTILITIES_H
char* readFile(const char* name);
int checkFailure(int shader);
bool fileExists(const char* str);
u8* loadBitmap(const char* fileName);
void loadMesh(const char* objPrefix, Mesh* mesh );


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



#endif

