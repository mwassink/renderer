#ifndef UTILITIES_H
#define UTILITIES_H
#include "types.h"
typedef unsigned char u8;

char* readFile(const char* name, int* size);
int checkFailure(int shader);
bool fileExists(const char* str);
u8* loadBitmap(const char* fileName);
Mesh loadMesh(const char* objPrefix);
u32 hash432(u32 a, u32 b, u32 c, u32 d);
Vertex constructVertex(Array<TripleF>* coords, Array<TripleF>* normals, Array<UV>* uvcoords, u32 p, u32 t, u32 n  );

#pragma pack(push, 1)

struct BMPFileHeader {
    // bitmap file header
    u16 fileType;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmapOffset;
};

#pragma pack(pop)



#endif

