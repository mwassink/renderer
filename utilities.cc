#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#include <immintrin.h>
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
        v.uv = {.5f, .5f};
    return v;

}
// Should check for a malicious file
Mesh parseObj(const char* f, const char* texture) {
    Array<Vector3> coords, normals;
    Array<UV> tcoords;
    Array<Vertex> verticesList;
    Array<u32> indices;
    int triangles = countTrianglesOccurences(f);
    if (triangles == -1) {
        return Mesh(0, 0, TextureName(0), 0, 0);
    }
    HashTable mappingTable(triangles * 3);

    FILE* fp = fopen(f, "rb");
    char arr[200];
    char type[100];
    char tri1[80]; char tri2[80]; char tri3[80];
    Vector3 tr;
    UV df;
    int debugCtr = 0;
    while (fgets(arr, 100, fp)) {
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
            sscanf(arr, "%s %s %s %s", type, tri1, tri2, tri3);
            parseVertex(tri1, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList );
            parseVertex(tri2, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
            parseVertex(tri3, &mappingTable, &indices, &coords, &normals, &tcoords, &verticesList);
            debugCtr++;
        }
    }
    coords.release();
    normals.release();
    tcoords.release();
    
    Mesh mesh(verticesList.data, indices.data, TextureName(texture), verticesList.sz, indices.sz);
    return mesh;
}


Mesh loadMesh(const char* objFile, const char* textureFile) {

    return parseObj(objFile, textureFile);

}

char*  readFileWindows(const char* name, int* sz) {
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

// Returns 0 on success
int checkFailure(int shader, GLenum status) {
    
    int success = 0;
    char log[512];
    glGetShaderiv(shader, status, &success);
    
    if(!success ){
        glGetShaderInfoLog(shader, 512, NULL, log);
        FILE* fp = fopen("error.log", "w");
        fwrite( log, 1, 512, fp);
        fclose(fp);
        return -1;
    }
    return 0;
}

int checkFailureLink(int shader, GLenum status) {
    int success = 0;
    char log[512];
    glGetProgramiv(shader, status, &success);

    if (!success) {
        glGetProgramInfoLog(shader, 512, NULL, log);
        FILE* fp = fopen("error.log", "w");
        fwrite(log, 1, 512, fp);
        fclose(fp);
        return -1;
    }
    return 0;
}

int setShaders(const char* vertexFile, const char* fragmentFile) {
    int size;
    const char* vertexShaderSrc = 0; const char* fragmentShaderSrc = 0;
    GLuint v, f, program;
    if (vertexFile) {
        vertexShaderSrc = readFile(vertexFile, &size);
        if (!vertexShaderSrc)
            return -1;
        v = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(v, 1, &vertexShaderSrc, NULL);
        glCompileShader(v);
        
        if (checkFailure(v, GL_COMPILE_STATUS)) {
            free((void*)vertexShaderSrc);
            return -1;
        }
        free((void*)vertexShaderSrc);
    }
    if (fragmentFile) {
        fragmentShaderSrc = readFile(fragmentFile, &size);
        if (!fragmentShaderSrc)
            return -1;
        f = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(f, 1, &fragmentShaderSrc, NULL  );
        glCompileShader(f);
        
        if (checkFailure(f, GL_COMPILE_STATUS)) {
            free((void*)fragmentShaderSrc);
            return -1;
        }
        free((void*)fragmentShaderSrc);
    }
    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    if (checkFailureLink(program, GL_LINK_STATUS)) {
        return -1;
    }

    
    
    return program;

}

void addBasicTexturedVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, u32 positionNorm, u32 positionUV, glTriangleNames* names ) {
    
    glGenBuffers(1, &names->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices* sizeof(u32), indices
                 , GL_STATIC_DRAW);

    glGenVertexArrays(1, &names->vao);
    glBindVertexArray(names->vao);
    glGenBuffers(1, &names->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(positionCoord, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, coord)));
    glVertexAttribPointer(positionNorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(positionUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glEnableVertexAttribArray(positionCoord);
    glEnableVertexAttribArray(positionNorm); 
    glEnableVertexAttribArray(positionUV);
    
}

void addThings(f32* verts, int numVerts) {
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * numVerts * 4, verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);
    glEnableVertexAttribArray(0);
}


void setupBitmapTexture(const char* textureString, int width, int height, int mips, GLuint* tex) {

    
    u8* bitmapTexture = loadBitmap(textureString);

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTextureStorage2D(*tex, mips, GL_RGBA32F, width, height   );
    glTextureSubImage2D(*tex, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, bitmapTexture );
    free(bitmapTexture);
    glTextureParameteri(*tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateTextureMipmap(*tex);
    
        
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
