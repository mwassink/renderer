#ifndef ASSET_BUILD
#include <windows.h>
#include <gl/gl.h>
#include <intrin.h>
#include "glwrangler.h"
#endif

#include <string.h>
#include <stdio.h>
#include "types.h"
#include "utilities.h"
#define MAXU32 0xFFFFFFFF

#define NORMALMAPPED "#define NORMALMAPPING 1\n";
#define SPOTLIGHT "define SPOTLIGHT 1\n";
#define POINTLIGHT "define POINTLIGHT 1\n"
#define TEXTURED "define TEXTURED 1\n"

#define VERSIONSTRING "version 450 core\n"
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__clang__)
#include <x86intrin.h>
#else 
#error No valid compiler available for build!
#end
#endif

#define VERTEX_BASIC 0
#define VERTEX_NORMAL 0x010100

#ifndef ASSET_BUILD
void uplumbMatrix4(u32 s, Matrix4& m, const char* n) {
    GLint loc = glGetUniformLocation(s, n);
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, (f32*)&m.data[0]);
}
void uplumbMatrix3(u32 s, Matrix3& m, const char* n) {
    GLint loc = glGetUniformLocation(s, n);
    if (loc != -1)
        glUniformMatrix3fv(loc, 1, GL_FALSE, (f32*)&m.data[0]);
}
void uplumbVector4(u32 s, Vector4& v, const char* n) {
    GLint loc = glGetUniformLocation(s, n);
    if (loc != -1)
        glUniform4fv(loc, 1, (f32*)&v.data[0]);
}
void uplumbVector3(u32 s, Vector3& v, const char* n) {
    GLint loc = glGetUniformLocation(s, n);
    if (loc != -1)
        glUniform3fv(loc, 1, (f32*)&v.data[0]);
}

void uplumbf(u32 s, f32 f, const char* n) {
    GLint loc = glGetUniformLocation(s, n);
    glUniform1f(loc, f);
}
#endif 
int length(const char* str) {
    const char* init = str;
    while ( *str) str++;
    return str - init;
}

// Do not overlap these
int slowCopy(const void* src, void* trg, int maxLen, bool str = true) {
    char* s = (char*)src;
    char *d = (char*) trg;
    int initMax = maxLen;
    if (str) {
        while (*s && initMax--) *d++ = *s++;
    }
    else {
        while (initMax--) *d++ = *s++;
    }
    
    return maxLen - initMax;
}

int countOccurrences(const char* s, char ch, char delim = 0) {
    int ctr = 0;
    while (*s && *s != delim) ctr += (*s++ == ch);
    return ctr;
}

int checkDoubleSlash(const char* s) {
    while (s[0] && s[1]) {
        if (s[0] == s[1] && s[1] == '/')
            return true;
    }
    return false;
}
bool operator==(const TripleKeyVal& l, const TripleKeyVal& r) {
    // compare the keys for equality
    for (int i = 0; i < 3; ++i) {
        if (l.arr[i] != r.arr[i])
            return false;
    }
    return true;
}

bool fileExists(const char* str) {
    FILE* fp = fopen(str, "rb");
    if (!fp) {
        return false;
    }
    fclose(fp);
    return true;
}

// (TODO) make this better
int countTrianglesOccurences(const char* f) {
    int sz, ctr = 0;
    char* ptr = readFile(f, &sz);
    if (!ptr) return -1;
    for (int i = 0; i < sz; ++i) {
        ctr += (ptr[i] == 'f');
    }
    free(ptr);
    return ctr;   
}

u8* flipBitmap(u32 bpp, u32 w, u32 h, u32 sizeBitmap, u8* bitmap) {
    u8* newBitmap = (u8*)malloc(sizeBitmap);
    u32 stride = 3 * w;
    if (3 * w * h != sizeBitmap) {
        fatalError("Unsupported resource given. Bitmap is not of 24 bit format", "ERROR");
    }
    
    for (int i = 0; i < h; ++i) {
        int rowSrc = (h - i - 1);
        int rowTrg = i;
        //memcpy(newBitmap + rowTrg
        //   * stride, rowSrc * stride + bitmap, stride);
        slowCopy(rowSrc*stride + bitmap, newBitmap + rowTrg
                 *stride, stride, false);
    }
    
    
    free(bitmap);
    
    return newBitmap;
    
    
}

u8* loadBitmap(const char* fileName, u32* width, u32* height, u32* bitsPerPixel) {
    u32 sizeBitmap;
    u8* bitMap;
    FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        return 0;
    }
    BMPFileHeader header;
    BitmapCore headerCore;
    if (fread(&header, 1, sizeof(BMPFileHeader), fp) != sizeof(BMPFileHeader)) {
        return 0;
    }
    sizeBitmap = header.fileSize - header.bitmapOffset;
    bitMap = (u8*)malloc(sizeBitmap);
    
    if (fread(&headerCore, 1, sizeof(BitmapCore), fp) != sizeof(BitmapCore)) {
        return 0;
    }
    *width = headerCore.width;
    *height = headerCore.height;
    *bitsPerPixel = headerCore.bitsPerPixel;
    
    // Read up until the bitmap
    if (fread(bitMap, 1, header.bitmapOffset - sizeof(BitmapCore) - sizeof(BMPFileHeader), fp) != (header.bitmapOffset - sizeof(BitmapCore) - sizeof(BMPFileHeader)))
    {
        free(bitMap);
        return 0;
    }
    fread(bitMap, 1, sizeBitmap, fp);
    bitMap = flipBitmap(*bitsPerPixel, *width, *height, sizeBitmap, bitMap);
    fclose(fp);
    return bitMap;
}

void parseVertex(const char* s, HashTable* indexHashTable, Array<u32>* indices,Array<Vector3>* coords, Array<Vector3>* normals, Array<UV>* uvcoords,
                 Array<Vertex>* vertices) {
    unsigned int p, t, n, occurences;
    occurences = countOccurrences(s, '/', 0);
    if (occurences == 2) {
        if (sscanf(s, "%u/%u/%u", &p, &t, &n ) != 3) {
            t = MAXU32;
            sscanf(s, "%u//%u", &p, &n );
        }
    }
    else if (occurences == 1) {
        n = MAXU32;
        sscanf(s, "%u/%u", &p, &t);
    }
    else {
        t = MAXU32; n = MAXU32;
        sscanf(s, "%u", &p);
    }
    // Put the res in the hash table
    int val = indexHashTable->at(p, t, n, 0);
    if (val == -1) {
        val = indexHashTable->insert(p, t, n, 0);
        vertices->push(constructVertex(coords, normals, uvcoords, p, t, n));
    }
    
    indices->push(val);
    
    
}

Vertex constructVertex(Array<Vector3>* coords, Array<Vector3>* normals, Array<UV>* uvcoords, u32 p, u32 t, u32 n  ) {
    Vertex v;
    v.coord = Vector4((*coords)[p], 1.0f);
    if (n != MAXU32)
        v.normal = (*normals)[n];
    else
        v.normal = {0.0f, 0.0f, 0.0f};
    if (n != MAXU32)
        v.uv = (*uvcoords)[t];
    else
        v.uv = {2.5f, 2.5f};
    return v;
    
}

Mesh parseObj(const char* f, Texture texRequest) {
    UV dummyUV;
    Vector3 dummyVec;
    Array<Vector3> coords, normals;
    Array<UV> tcoords;
    Array<Vertex> verticesList;
    Array<u32> indices;
    coords.push(dummyVec);
    normals.push(dummyVec);
    tcoords.push(dummyUV);
    
    
    // is an estimate
    int triangles = countTrianglesOccurences(f);
    if (triangles == -1) {
        return Mesh(0, 0, Texture(), 0, 0);
    }
    HashTable mappingTable(triangles * 4);
    
    FILE* fp = fopen(f, "rb");
    
    if (!fp) {
        fatalError("unable to read in obj file","Invalid file name"); 
    }
    char arr[200];
    char type[100];
    char tri1[80]; char tri2[80]; char tri3[80]; char tri4[80];
    Vector3 tr;
    UV df;
    
    while (fgets(arr, 100, fp) != NULL) {
        sscanf(arr, "%s", type );
        if (!strcmp(type, "v")) {
            sscanf(arr, "%s %f %f %f", type, &tr.x, &tr.y, &tr.z);
            coords.push(tr);
        }
        else if (!strcmp(type, "vt")) {
            sscanf(arr, "%s %f %f", type, &df.u, &df.v);
            tcoords.push(df);
        }
        else if (!strcmp(type, "vn")) {
            sscanf(arr, "%s %f %f %f", type, &tr.x, &tr.y, &tr.z);
            normals.push(tr);
        }
        else if (!strcmp(type, "f")) {
            if(sscanf(arr, "%s %s %s %s %s", type, tri1, tri2, tri3, tri4) == 5) {
                // 1, 2, 3 gives a properly wound triangle
                parseVertex(tri1, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList );
                parseVertex(tri2, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
                parseVertex(tri3, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
                // 1, 3, 4 should?
                parseVertex(tri1, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList );
                parseVertex(tri3, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
                parseVertex(tri4, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
            }
            
            parseVertex(tri1, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList );
            parseVertex(tri2, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
            parseVertex(tri3, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
            
        }
    }
    coords.release();
    normals.release();
    tcoords.release();
    
    fclose(fp);
    Mesh mesh(verticesList.data, indices.data, texRequest, verticesList.sz, indices.sz);
    return mesh;
}



Mesh loadMesh(const char* objFile,  Texture textureRequest) {
    
    Mesh mesh = parseObj(objFile, textureRequest);
    return mesh;
}

#ifndef ASSET_BUILD
char* readFileWindows(const char* name, int* sz) {
    char* mem;
    HANDLE hFile;
    union _LARGE_INTEGER size;
    
    
    DWORD  read;
    
    
    hFile = CreateFileA(name, GENERIC_READ, 0, 0, OPEN_EXISTING,0 , 0 );
    if (!GetFileSizeEx(hFile, &size )) {
        printf("Failure getting file size for %s!\n", name);
        exit(1);
    }
    mem = (char*)VirtualAlloc(NULL, size.QuadPart + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
    if (!ReadFile(hFile, mem, size.QuadPart, &read, 0)) {
        printf("Failure reading %s!\n", name);
        exit(1);
    }
    *sz = read;
    return mem;
    
    
    
    
}
#endif

char* readFile(const char* name, int32* sizePtr) {
    FILE* fp = fopen(name, "rb");
    int size;
    char* buf = 0;
    if (!fp) 
        return 0;
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    buf = (char*)malloc(size+1);
    memset(buf, 0, size+1);
    fseek(fp, 0, SEEK_SET);
    fread(buf, 1, size, fp);
    fclose(fp);
    
    *sizePtr = size;
    return buf;
    
}




u32 hash432(u32 a, u32 b, u32 c, u32 d) {
    unsigned char seed[16] {
        0x0b , 0x6a , 0xf8 , 0xf4 , 0x7b , 0x85 , 0x5b , 0xc1 , 0x38 , 0x6b , 0x97 , 0x07 , 0x09 , 0x00 , 0x58 , 0x7b
    };
    
    __m128i val = _mm_set_epi32(a, b, c, d);
    __m128i seedmm = _mm_loadu_si128((__m128i*) seed);
    __m128i res = _mm_aesdec_si128(val, seedmm);
    res = _mm_aesdec_si128(res, seedmm);
    res = _mm_aesdec_si128(res, seedmm);
    
    // Take the least significant bits
    return (_mm_extract_epi32(res, 0));
    
    
    
}

HashTable::HashTable(int sz) {
    TripleKeyVal tkv(0, 0, 0, -1);
    ctr = 0;
    arr = (TripleKeyVal*)malloc(sizeof(TripleKeyVal)*sz); // this will be aligned fine
    for (int i = 0; i < sz; ++i) {
        arr[i] = tkv;
    }
    this->sz = sz;
}

int32 HashTable::insert(u32 a, u32 b, u32 c, u32 empty) {
    TripleKeyVal tkv(a, b, c, empty);
    u32 bucketIndex = hash432(a, b, c, empty) % sz;
    while (arr[bucketIndex].arr[3] != -1) {
        bucketIndex = ((bucketIndex + 1) % sz);
    }
    arr[bucketIndex].arr[0] = a;
    arr[bucketIndex].arr[1] = b;
    arr[bucketIndex].arr[2] = c;
    arr[bucketIndex].arr[3] = ctr;
    return ctr++;
    
    
    
}

int32 HashTable::at(u32 a, u32 b, u32 c, u32 empty ) {
    TripleKeyVal tkv(a, b, c, empty );
    u32 bucketIndex = hash432(a, b, c, empty) % sz;
    // If we find an empty index or our bucket we stop
    while ((!(arr[bucketIndex] == tkv )) && arr[bucketIndex].arr[3] != -1) {
        bucketIndex = ((bucketIndex + 1) % sz);
    }
    return arr[bucketIndex].arr[3];
    
}

void HashTable::release() {
    free(arr);
}

int32 clampRangei(int32 min, int32 max, int32 in) {
    if (in < min) return min;
    if (in > max) return max;
    return in;
}

f32 clampNormal(f32 in) {
    if (in < -1.0f) return -1.0f;
    if (in > 1.0f) return 1.0f;
    return in;
}


void Texture::activate() {
    
    u32 bpp;
#if defined(ASSET_BUILD)
    id = -1;
#else
    id = BitmapTextureInternal(fileName, &width, &height, &bpp);
#endif
    if (bpp != 24) {
        ASSERT(0);
    }
}


// Make sure when getting the bitmap it is a multiple of 4
void writeOutBMP(const char* target, u32 w, u32 h, u8* mem) {
    BMPFileHeader fileHeader = {};
    BitmapHeader header = {};
    
    FILE* fp = fopen(target, "wb+");

    if (!fp) {
        
        char msg[1000] = { 0 };
        snprintf(msg, 1000, "Unable to open one of the files for writing %s with error: %s. Could another program be using it?", target, strerror(errno));
        fatalError(msg, "Error");
    }
    
    
    fileHeader.fileType = 0x4D42;
    fileHeader.fileSize = sizeof(fileHeader) + sizeof(header) + w*h*3;
    fileHeader.bitmapOffset = sizeof(fileHeader) + sizeof(header);
    fileHeader.reserved1 = 0; fileHeader.reserved2 = 0;
    
    header.size = sizeof(header);
    header.width = w;
    header.height = h;
    header.planes = 1;
    header.bitsPerPixel = 24;
    header.compression = 0;
    header.imageSize = h * w * 3;
    header.xRes = 1; //???
    header.yRes = 1; //???
    header.numColorsPalette = 0;
    
    fwrite(&fileHeader, 1, sizeof(fileHeader), fp);
    fwrite(&header, 1, sizeof(header), fp);
    fwrite(mem, 1, w * h * 3, fp);
    fclose(fp);
}


void writeNormalMapBitmap(u32 w, u32 h, Vector3* normals, const char* target) {
    u8* mem = (u8*)malloc(w * h * 3);
    u8* tmp = mem;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Vector3& normal = *normals;
            u8 r = (normal[0] + 1.0f) * 127.0f;
            u8 g = (normal[1] + 1.0f) * 127.0f;
            u8 b = (normal[2] + 1.0f) * 127.0f;
            tmp[0] = b; tmp[1] = g; tmp[2] = r;
            tmp += 3;
            normals++;
        }
    }
    writeOutBMP(target, w, h, mem);
}

// Deletes the original shader, creates a new shader
char* AlterShaderArgs(char** argList, char* shader, int lenShader ) {    
    int spaceRequired = length(VERSIONSTRING);
    for (int i = 0; argList[i] != 0; ++i) {
        spaceRequired += length(argList[i]);
    }
    char* newShader = (char*)malloc(lenShader + spaceRequired);
    
    int acc = slowCopy(VERSIONSTRING, newShader, length(VERSIONSTRING));
    for (int i = 0; argList[i] != 0; ++i) {
        acc += slowCopy(argList[i], newShader + acc, length(argList[i]));
    }
    slowCopy(shader, newShader + acc, lenShader);
    free(shader);
    return newShader;
}



#ifndef ASSET_BUILD
// TODO... I do not like the GL code making its way into here....
u32 BitmapTextureInternal(const char* textureString, u32* width, u32* height, u32* bitsPerPixel) {
    
    GLuint tex;
    GLint err;
    u32 mips = 0;
    f32 borderColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    
    
    u8* bitmapTexture = loadBitmap(textureString , width, height, bitsPerPixel);
    
    u32 wc = *width;
    while (wc >>= 1) mips++;
    
    if (*width != *height) mips = 1;
    
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTextureStorage2D(tex, mips, GL_RGB8, *width, *height);
    glTextureSubImage2D(tex, 0, 0, 0, *width, *height, GL_BGR, GL_UNSIGNED_BYTE, bitmapTexture );
#if 0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    free(bitmapTexture);
    if (mips != 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
        glGenerateTextureMipmap(tex);
        err = glGetError();
    }
    return tex;
}
#endif
// File format: 4 bytes (type), 4 bytes (vertices size), 4  bytes (indices size)
// little endian. No big endian
void SerializeModel(const char* f, const char* dst, bool normalMapped) {
    int texReq = 0;
    Mesh m = parseObj(f, 0);
    
    
    FILE* filePointer = fopen(dst, "wb+");
    if (!filePointer){
        fatalError("Unable to open file for writing", dst);
    }
    u32 type = VERTEX_BASIC;
    if (normalMapped) {
        type = VERTEX_NORMAL;
        MeshTangentsInternal(&m);
    }
    fwrite(&type, sizeof(u32), 1, filePointer);
    fwrite(&m.numVertices, sizeof(u32), 1, filePointer);
    fwrite(&m.numIndices, sizeof(u32), 1, filePointer);
    if (normalMapped) {
        fwrite(m.normalVertices, sizeof(VertexLarge), m.numVertices, filePointer);
    } else {
        fwrite(m.vertices, sizeof(Vertex), m.numVertices, filePointer);
    }
    
    fwrite(m.triangles, sizeof(u32), m.numIndices, filePointer);
    fclose(filePointer);
    
    
}

Mesh BinaryMesh(const char* file) {
    FILE* filePointer = fopen(file, "rb");
    if (!filePointer) {
        fatalError("Unable to open one of the files for reading!", file);
    }
    u32 numIndices, numVertices, type;
    VertexLarge* normalVertices = 0;
    Vertex* basicVertices = 0;
    
    fread(&type, sizeof(u32), 1, filePointer);
    fread(&numVertices, sizeof(u32), 1, filePointer);
    fread(&numIndices, sizeof(u32), 1, filePointer);
    u32* indices = (u32*) malloc(sizeof(u32) * numIndices);
    if (type == VERTEX_BASIC) {
        basicVertices= (Vertex*) malloc(sizeof(Vertex) * numVertices);
        fread(basicVertices, sizeof(Vertex), numVertices, filePointer);
    } else if (type == VERTEX_NORMAL) {
        normalVertices =  (VertexLarge*) malloc(sizeof(VertexLarge) * numVertices);
        fread(normalVertices, sizeof(VertexLarge), numVertices, filePointer);
    } else {
        fatalError("Unknown file type for serialized mesh!", "File Error");
    }
    fread(indices, sizeof(u32), numIndices, filePointer);
    
    Texture tex;
    Mesh m(basicVertices, indices, tex, numVertices, numIndices);
    if (type == VERTEX_NORMAL) {
        m.normalVertices = normalVertices;
    }

    fclose(filePointer);
    return m;
    
    
    
}


void MeshTangentsInternal(Mesh* mesh) {
    VertexLarge* normalVerts = (VertexLarge*)malloc(sizeof(VertexLarge)*mesh->numVertices);
    Vertex* verts = mesh->vertices;
    u32* indexList = mesh->triangles;
    Vector3* bitangents = (Vector3*)malloc(sizeof(Vector3)*mesh->numVertices);
    for (int i = 0; i < mesh->numVertices; ++i) {
        normalVerts[i].coord = verts[i].coord;
        normalVerts[i].normal  = verts[i].normal;
        normalVerts[i].tangent = Vector3(0.0f, 0.0f, 0.0f);
        normalVerts[i].uv = verts[i].uv;
        normalVerts[i].handedness = 1.0f;
        bitangents[i] = Vector3(0.0f, 0.0f, 0.0f);
    }
    for (int i = 0; i < mesh->numIndices/3; ++i) {
        u32 i0 = mesh->triangles[i*3], i1 = mesh->triangles[i*3+1], i2 = mesh->triangles[i*3+2];
        Vector3 p0 = verts[i0].coord.v3();
        Vector3 p1 = verts[i1].coord.v3();
        Vector3 p2 = verts[i2].coord.v3();
        
        UV uv0 = verts[i0].uv;
        UV uv1 = verts[i1].uv;
        UV uv2 = verts[i2].uv;
        
        Vector3 q1 = p1 - p0;
        Vector3 q2 = p2 - p0;
        f32 s1 = uv1.u - uv0.u, s2 = uv2.u - uv0.u;
        f32 t1 = uv1.v - uv0.v, t2 = uv2.v - uv0.v;
        
        f32 adjScale = 1/(s1*t2 - s2*t1);
        Matrix3 tsmat  = Matrix3(t2, -t1, 0, -s2, s1, 0, 0, 0,0);
        //tsmat:
        // t2 -t1 0
        // -s2 s1 0
        // 0   0  0
        Matrix3 qmat = Matrix3(q1, q2, Vector3(0,0,0));
        qmat = qmat.transpose();
        // <- q1 ->
        // <- q2 ->
        // 0  0  0
        Matrix3 res = tsmat * qmat;
        res *= adjScale;
        
        res = res.transpose();
        
        Vector3 t = res[0];
        Vector3 b = res[1];
        
        normalVerts[i0].tangent = normalVerts[i0].tangent + t;
        bitangents[i0] = bitangents[i0] + b;
        normalVerts[i1].tangent = normalVerts[i1].tangent + t;
        bitangents[i1] = bitangents[i1] + b;
        normalVerts[i2].tangent = normalVerts[i2].tangent + t;
        bitangents[i2] = bitangents[i2] + b;
    }
    for (int i = 0; i < mesh->numVertices; ++i) {
        Vector3 t = normalVerts[i].tangent;
        Vector3& n = normalVerts[i].normal;
        normalVerts[i].tangent = t - dot(normalVerts[i].normal, t)*normalVerts[i].normal;
        normalVerts[i].tangent.normalize();
        if (dot(cross(t, bitangents[i]), n) < 0.0f) {
            normalVerts[i].handedness = -1.0f;
        }
    }
    
    mesh->normalVertices = normalVerts;
    free(verts);
    free(bitangents);
    mesh->vertices = 0;
    
    
    
}


