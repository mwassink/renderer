#ifndef UTILITIES_H
#define UTILITIES_H
#include "types.h"
typedef unsigned char u8;
#define ASSERT(expr) if (!(expr)) { fatalError("The program encountered an error", "Error");}

char* readFile(const char* name, int* size);
void fatalError(const char* msg, const char* title);
bool fileExists(const char* str);
u8* loadBitmap(const char* fileName, u32* w, u32* h, u32* bpp);
Mesh loadMesh(const char* objPrefix, Texture texRequest);
u32 hash432(u32 a, u32 b, u32 c, u32 d);
Vertex constructVertex(Array<Vector3>* coords, Array<Vector3>* normals, Array<UV>* uvcoords, u32 p, u32 t, u32 n  );
void setupTexture(const char*);
int setupBitmapTexture(const char* textureString, u32* w, u32* h, u32* bpp);
int32 clampRangei(int32 min, int32 max, int32 in);
f32 clampNormal(f32 in);
void writeOutBMP(const char* target, u32 w, u32 h, u8* mem);
void writeNormalMapBitmap(u32 w, u32 h, Vector3* normals, const char* target);
void uplumbf(u32 s, f32 f, const char* n);
u32 BitmapTextureInternal(const char* textureString, u32* width, u32* height, u32* bitsPerPixel);
Mesh ReadModel(const char* file);
void SerializeModel(const char* f, const char* dst);

#ifndef ASSET_BUILD
void uplumbMatrix4(u32 s, Matrix4& m, const char* n);
void uplumbMatrix3(u32 s, Matrix3& m, const char* n);
void uplumbVector4(u32 s, Vector4& v, const char* n);
void uplumbVector3(u32 s, Vector3& v, const char* n);
#endif


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

