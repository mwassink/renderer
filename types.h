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
    u32 smVao;
};

// pod array
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
        if (fileName)
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

struct Sphere {
    Vector3 p;
    f32 radius;
};


struct Mesh {
    Mesh() {
        boundingSphere.radius = -1;
    };
    Vertex* vertices;
    VertexLarge* normalVertices = 0;
    u32* triangles;
    Texture textures;
    Texture normalMap;
    u32 numVertices;
    u32 numIndices;
    Vector3 diffuseColor;
    Vector3 specColor;
    Sphere boundingSphere;

    Mesh(Vertex* v, u32* tr, Texture tx, u32 numV, u32 numI) : vertices(v), triangles(tr), textures(tx), numVertices(numV), numIndices(numI) {
        boundingSphere.radius = -1.0f;
    }
    
};

struct Model {
    Mesh mesh;
    CoordinateSpace modelSpace;
    glTriangleNames identifiers;
};

struct CubeArgs {
    u32 tex;
    int internalFormat;
    u32 attachment;
    u32 format;
    u32 shader;
    int res;
};


struct SpotLight {
    SpotLight() {
        cubeArgs.tex = -1;
    }

    SpotLight(Vector3 o, Vector3 r, Vector3 s, Vector3 t, f32 irrad, Vector3 color) {
        worldSpaceCoord = o;
        color = color;
        irradiance = irrad;
        lightSpace.r = r;
        lightSpace.s = s;
        lightSpace.t = t;
        lightSpace.origin = o;
    }
    union {
        struct {
            Vector3 worldSpaceCoord;
            Vector3 r, s, t;
        };
        CoordinateSpace lightSpace; // for shadow mapping
    };
    int depthTexture = -1;
    CubeArgs cubeArgs;
    Vector3 color;
    f32 irradiance;
    bool shadows = true;
};

struct PointLight {
    PointLight() {
        cubeArgs.tex = -1;
    }
    union {
        struct {
            Vector3 worldSpaceCoord;
            Vector3 r, s, t;
        };
        CoordinateSpace lightSpace; // for shadow mapping
    };
    int depthTexture = -1;
    CubeArgs cubeArgs;
    Vector3 color;
    f32 irradiance;
    bool shadows = true;
};

struct Skybox {
    int texture, vbo, vao;
};




#endif

