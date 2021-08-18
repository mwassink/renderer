#include <stdlib.h>
#include <windows.h>

typedef float f32;
typedef double f64;
typedef int s32l
typedef uint u32;
typedef short int s16;
typedef short unsigned int u16;
typedef unsigned char u8;

<typename Type>
struct Array {
    // can this be limited to plain old types?
    Type* data;
    int sz, cap;
    
    void push(const Type& in) {
        if (sz == cap) {
            data = realloc(data, cap*1.5*sizeof(data));
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
        data = malloc(sz);
        sz = 0;
        cap = sz;
        
    }
};

struct Texture {
    char* fileName;
    s32 id;
}

struct Vertex {
    f32 coord[3];
    f32 norm[3];
    f32 u, v;
};

struct Mesh {
    Vertex* vertices;
    u32* indices;
    Texture* textures;
    
};
