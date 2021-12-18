#include <stdio.h>
#include <string.h>
#include "utilities.h"
#include <stdlib.h>

void fatalError(const char* s1, const char* s2) {
    fprintf(stderr, "%s\n", s1);
    fprintf(stderr, "%s\n", s2);
    exit(1);
}


#define ASSET_BUILD 1
// Change those obj files with this
int main(int argc, char** argv) {
    
    FILE* filePointer = fopen("assetslist.txt", "r");
    char line[100] = {0}; 
    char dest[100] = {0};
    
    if (!filePointer) {
        fatalError("Fatal error!", "Cannot open assets list for writing"); 
    }
    bool normals = true;
    while (fgets(line, 100, filePointer) != NULL) {
        if (line[0] == '#') {
            continue;
        }
        
        char* ptr = line;
        while (*ptr != 0 && *ptr != '\n') {
            ptr++;
        }
        *ptr = 0;
        while (*ptr != '.' && ptr != line) {
            ptr--;
        }
        u32 offset = ptr -line;
        strncpy(dest, line, offset);
        dest[offset] = '.';
        dest[offset+1] = 'd';
        dest[offset+2] = 'a';
        dest[offset+3] = 't';
        dest[offset+4] = 'a';
        dest[offset+5] = 0;
        SerializeModel(line, dest, normals);
        
        
        
    }
    
}