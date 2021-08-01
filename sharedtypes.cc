#include <stdlib.h>
#include <windows.h>

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
}
