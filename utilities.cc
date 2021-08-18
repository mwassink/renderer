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
    Gluint v, f, program;
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

void addVerticesToPosition(Vertex* vertices, u32* indices, u32 position) {
    
}
