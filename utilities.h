#ifndef UTILITIES_H
#define UTILITIES_H
#include "types.h"
typedef unsigned char u8;

char* readFile(const char* name, int* size);
int checkFailure(int shader);
bool fileExists(const char* str);
u8* loadBitmap(const char* fileName, u32* w, u32* h, u32* bpp);
Mesh loadMesh(const char* objPrefix, Texture texRequest);
u32 hash432(u32 a, u32 b, u32 c, u32 d);
Vertex constructVertex(Array<Vector3>* coords, Array<Vector3>* normals, Array<UV>* uvcoords, u32 p, u32 t, u32 n  );
void setupTexture(const char*);
void addBasicTexturedVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, u32 positionNorm, u32 positionUV, glTriangleNames* names );
void addVerticesToShader(VertexLarge* verts, u32* indices, int numVerts, int numIndices, u32 posCoord, u32 posNorm, u32 posTangent, u32 posUV, u32 posHandedness, glTriangleNames* ids);
int setupBitmapTexture(const char* textureString, u32* w, u32* h, u32* bpp);
int setShaders(const char*, const char*);
int32 clampRangei(int32 min, int32 max, int32 in);
f32 clampNormal(f32 in);
Vector3* loadNormals(const char* f, u32* w, u32* h);
void addMeshTangents(Mesh* mesh);
void writeOutNormalMapBMP(const char* target, u32 w, u32 h, Vector3* normals);
f32* convertBitmapHeightmap(const char* b, u32* w, u32* h, f32 m);
void normalMap(f32* heightMap, Vector3* normalMap, int32 height, int32 width );
void buildNormalMap(const char* hFile, const char* n);

#pragma pack(push, 1)

struct BMPFileHeader {
    // bitmap file header
    u16 fileType;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmapOffset;
};

struct BitmapHeader
{
    
    // bitmapheaderCORE
    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bitsPerPixel; // wraps up the core of the header
    u32 compression;
    u32 imageSize;
    u32 yRes;
    u32 xRes;
    u32 numColorsPalette;
    u32 mostImpColor;
    
    
};

struct BitmapCore {
    // bitmapheaderCORE
    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bitsPerPixel;
};

#pragma pack(pop)

#endif

