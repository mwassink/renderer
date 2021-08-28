#ifndef TYPES_H
#define TYPES_H
#include <stdlib.h>
#include <stdint.h>
#include "vecmath.h"
typedef float f32;
typedef double f64;
typedef int s32;
typedef unsigned int u32;
typedef short int s16;
typedef short unsigned int u16;
typedef unsigned char u8;
typedef int32_t int32;




struct TripleKeyVal {
    int32 arr[4];
    TripleKeyVal() {}
    TripleKeyVal(int32 a, int32 b, int32 c, int32 d ){ 
        arr[0] = a;
        arr[1] = b;
        arr[2] = c;
        arr[3] = d; 
    }
};

struct RGB {
    union {
        struct {
            f32 x, y, z;
        };
        struct {
            f32 r, g, b;
        };
    };
    
};

struct RGBA {
    union {
        struct {
            f32 x, y, z, w;
        };
        struct {
            f32 r, g, b, a;
        };
    };
};

struct UV {
    f32 u, v;
};

struct glTriangleNames {
    u32 ebo, vao, vbo;
};

template <typename Type>
struct Array {
    // can this be limited to plain old data types?
    Type* data;
    int sz, cap;
    
    void push(const Type& in) {
        if (sz == cap) {
            data = (Type*)realloc(data, cap*1.5*sizeof(Type));
            cap *= 1.5;
        }
        data[sz++] = in;
    }
    
    void pop() {
        sz--;
    }

    Type& operator[](int index) {
        return data[index];
    }
    
    Array(int sz_in) {
        data = (Type*) malloc(sz_in*sizeof(Type));
        sz = 0;
        cap = sz;
        
    }

    Array() {
        data = (Type* ) malloc(20*sizeof(Type));
        sz = 0;
        cap = 20;
    }

    void release() {
        free(data);
    }
};



// Meant for hashing indices for the meshes
struct HashTable {
    TripleKeyVal* arr;
    int ctr;
    int sz;
    HashTable(int sz);
    int32 insert(u32 a, u32 b, u32 c, u32 empty);
    int32 at(u32 a, u32 b, u32 c, u32 empty );
    void release();
    
};



struct Texture {
    const char* fileName;
    int id;
    u32 width;
    u32 height;
    Texture(const char* fileName) : fileName(fileName){
        activate();
    }
    Texture () {
        id = -1;
    }
    void activate(); 
};

struct Vertex {
    
    Vector4 coord;
    Vector3 normal;
    UV uv;
};

struct VertexLarge {
    Vector4 coord;
    Vector3 normal;
    Vector3 tangent;
    UV uv;
    f32 handedness;
};

struct PlainVertex {
    Vector3 coord;
    Vector3 norm;
};

struct Mesh {
    Mesh() {};
    Vertex* vertices;
    VertexLarge* normalVertices = 0;
    u32* triangles;
    Texture textures;
    Texture normalMap;
    u32 numVertices;
    u32 numIndices;
    Vector3 diffuseColor;
    Vector3 specColor;

    Mesh(Vertex* v, u32* tr, Texture tx, u32 numV, u32 numI) : vertices(v), triangles(tr), textures(tx), numVertices(numV), numIndices(numI) {}
    
};

struct Model {
    Mesh mesh;
    CoordinateSpace modelSpace;
    glTriangleNames identifiers;
};


struct Light {
    Vector3 worldSpaceCoord;
    Vector3 color;
    f32 irradiance;
};
#endif

