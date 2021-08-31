#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#include <intrin.h>


#include "types.h"
#include "glwrangler.h"
#include "utilities.h"
#define MAXU32 0xFFFFFFFF


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
    
    
    Mesh mesh(verticesList.data, indices.data, texRequest, verticesList.sz, indices.sz);
    return mesh;
}



Mesh loadMesh(const char* objFile,  Texture textureRequest) {

    Mesh mesh = parseObj(objFile, textureRequest);
    return mesh;
}

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
    id = setupBitmapTexture(fileName, &width, &height, &bpp);
    if (bpp != 24) {
        ASSERT(0);
    }
}

// Make sure when getting the bitmap it is a multiple of 4
void writeOutNormalMapBMP(const char* target, u32 w, u32 h, Vector3* normals) {
    BMPFileHeader fileHeader = {};
    BitmapHeader header = {};

    FILE* fp = fopen(target, "wb");

    u8* mem = (u8*)malloc(w * h * 3);
    u8* tmp = mem;
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
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Vector3& normal = *normals;
            u8 r = (normal[0] + 1.0f)*127.0f;
            u8 g = (normal[1] + 1.0f)*127.0f;
            u8 b = (normal[2] + 1.0f)*127.0f;
            tmp[0] = b; tmp[1] = g; tmp[2] = r;
            tmp += 3;
            normals++;
        }
    }
    fwrite(mem, 1, w * h * 3, fp);
    fclose(fp);
}


