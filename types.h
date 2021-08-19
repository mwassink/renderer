#ifndef TYPES_H
#define TYPES_H
#include <stdlib.h>
#include <stdint.h>
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

struct TripleF {
    f32 x, y, z;
};

struct UV {
    f32 u, v;
};


template <typename Type>
struct Array {
    // can this be limited to plain old types?
    Type* data;
    int sz, cap;
    
    void push(const Type& in) {
        if (sz == cap) {
            data = realloc(data, cap*1.5*sizeof(Type));
            cap *= 1.5;
        }
        data[sz++] = obj;
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
    void insert(u32 a, u32 b, u32 c, u32 empty);
    int32 at(u32 a, u32 b, u32 c, u32 empty );
    void release();
    
};



struct Texture {
    char* fileName;
    s32 id;
};

struct Vertex {
    
    f32 coord[3];
    f32 norm[3];
    f32 u, v;
};

struct Mesh {
    Vertex* vertices;
    u32* triangles;
    Texture* textures;
    
};
#endif
