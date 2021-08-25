#ifndef UTILITIES_H
#define UTILITIES_H
#include "types.h"
typedef unsigned char u8;

char* readFile(const char* name, int* size);
int checkFailure(int shader);
bool fileExists(const char* str);
u8* loadBitmap(const char* fileName);
Mesh loadMesh(const char* objPrefix, Texture texRequest);
u32 hash432(u32 a, u32 b, u32 c, u32 d);
Vertex constructVertex(Array<Vector3>* coords, Array<Vector3>* normals, Array<UV>* uvcoords, u32 p, u32 t, u32 n  );
void setupTexture(const char*);
void addBasicTexturedVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, u32 positionNorm, u32 positionUV, glTriangleNames* names );
int setupBitmapTexture(const char* textureString, int width, int height, int mips);
int setShaders(const char*, const char*);
int32 clampRangei(int32 min, int32 max, int32 in);
f32 clampNormal(f32 in);

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

