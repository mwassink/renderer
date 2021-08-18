#include "types.h"
#include <stdio.h>
#include "glwrangler.h"
#include <gl/gl.h>


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


void loadMesh(const char* objPrefix, Mesh* mesh ) {
    char fullFileName[300]; //this is the entire file name
    
}


char* readFile(const char* name) {
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
    char* vertexShaderSrc = 0; char* fragmentShaderSrc = 0;
    GLuint v, f, program;
    if (vertexFile) {
        vertexShaderSrc = readFile(vertexFile);
        v = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(v, 1, &vertexShaderSrc, NULL);
        glCompileShader(v);
        if (checkFailure(v)) {
            return -1;
        }
    }
    if (fragmentFile) {
        fragmentShaderSrc = readFile(fragmentFile);
        f = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(f, 1, &fragmentShaderSrc, NULL  );
        glCompileShader(f);
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

void addBasicVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices u32 positionCoord, u32 positionNorm, u32 positionUV) {
    GLuint ebo, vao, vbo;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices* sizeof(u32), indices
                 , GL_STATIC_DRAW);

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao);
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertices);

    glEnableVertexAttribArray(positionCoord);
    glEnableVertexAttribArray(positionNorm);
    glEnableVertexAttribArray(positionUV);
    glVertexAttribPointer(positionCoord, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, coord)));
    glVertexAttribPointer(positionNorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, norm)));
    glVertexAttribPointer(positionUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, u)));
    
}
