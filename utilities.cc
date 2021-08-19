#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#include <immintrin.h>
#include <intrin.h>


#include "types.h"
#include "glwrangler.h"
#include "utilities.h"


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


// Should check for a malicious file that has A > 100 start to it
void parseObj(FILE* fp) {
    Array<TripleF> coords, normals;
    Array<UV> tcoords;
    Array<Vertex> verticesList;
    char arr[100];
    char type[100];
    TripleF tr;
    while (fgets(arr, 100, fp)) {
        sscanf(arr, "%s", type );
        if (!strcmp(type, "v")) {
            
        }
        else if (!strcmp(type, "vt")) {

        }
        else if (!strcmp(type, "vn")) {

        }
        else if (!strcmp(type, "f")) {

        }
    }
}


void loadMesh(const char* objFile, const char* textureFile, Mesh* mesh ) {
    f32 c1, c2, c3;
    int size;
    
    FILE* fp = fopen(objFile, "rb");
    // 1. Collect list of coords, normals, texture coords

}

char* readFile(const char* name, int32* sizePtr) {
    FILE* fp = fopen(name, "r");
    int size;
    char* buf = 0;
    if (!fp) 
        return 0;
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    buf = (char*)malloc(size);
    fseek(fp, 0, SEEK_SET);
    fread(buf, 1, size, fp);
    fclose(fp);
    
    *sizePtr = size;
    return buf;
    
}

// Returns 0 on success
int checkFailure(int shader) {
    
    int success = 0;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if(!success ){
        glGetShaderInfoLog(shader, 512, NULL, log);
        FILE* fp = fopen("error.log", "w");
        fwrite( log, 1, 512, fp);
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
        v = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(v, 1, &vertexShaderSrc, NULL);
        glCompileShader(v);
        free((void*)fragmentShaderSrc);
        if (checkFailure(v)) {
            return -1;
        }
    }
    if (fragmentFile) {
        fragmentShaderSrc = readFile(fragmentFile, &size);
        f = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(f, 1, &fragmentShaderSrc, NULL  );
        glCompileShader(f);
        free((void*)vertexShaderSrc);
        if (checkFailure(f)) {
            return -1;
        }
    }
    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    if (checkFailure(program)) {
        return -1;
    }

    
    
    return 0;

}

void addBasicVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, u32 positionNorm, u32 positionUV) {
    GLuint ebo, vao, vbo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices* sizeof(u32), indices
                 , GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(positionCoord);
    glEnableVertexAttribArray(positionNorm);
    glEnableVertexAttribArray(positionUV);
    glVertexAttribPointer(positionCoord, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, coord)));
    glVertexAttribPointer(positionNorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, norm)));
    glVertexAttribPointer(positionUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, u)));
    
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
}

void HashTable::insert(u32 a, u32 b, u32 c, u32 empty) {
        TripleKeyVal tkv(a, b, c, empty);
        u32 bucketIndex = hash432(a, b, c, empty);
        while (arr[bucketIndex].arr[3] != -1) {
            bucketIndex = ((bucketIndex + 1) % sz);
        }
        arr[bucketIndex].arr[0] = a;
        arr[bucketIndex].arr[1] = b;
        arr[bucketIndex].arr[2] = c;
        arr[bucketIndex].arr[3] = ctr++;
        
        
}

int32 HashTable::at(u32 a, u32 b, u32 c, u32 empty ) {
        TripleKeyVal tkv(a, b, c, empty );
        u32 bucketIndex = hash432(a, b, c, empty);
        while (!(arr[bucketIndex] == tkv )) {
             bucketIndex = ((bucketIndex + 1) % sz);
        }
        return arr[bucketIndex].arr[3];
        
}

void HashTable::release() {
    free(arr);
}
