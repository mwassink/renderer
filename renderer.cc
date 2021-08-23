/* 8/21/2021 */

// Return a normal map from the height map
// it shouldn't matter the height map's range
// since this should point out we shoudl 
void normalMap(f32* heightMap, TripleF* normalMap, int32 height, int32 map ) {
    TripleF tmp;
    for (int h = 0; h < height; ++h) {
        int32 hup = clampRangei(0, height, h-1);
        int32 hdown = clampRangei(0, height, h+1);
        for (int w = 0; w < width; ++w) {
            const f32 above = heightMap[hup*width + w ];
            const f32 below = heightMap[hdown* width + w];
            const f32 left = heightMap[h*width + clampRangei(0, width, w -1 )];
            const f32 right = heightMap[h*width + clampRangei(0, width, w + 1)];
            f32 pdx = (right - left) * .5f;
            f32 pdy = (up - down) * .5f;

            f32 mag = sqrt(pdx * pdx + pdy*pdy + 1.0f);
            tmp.x = clampNormal(pdx/mag);
            tmp.y = clampNormal(pdy/mag);
            tmp.z = clampNormal(1.0f/mag);
            normalMap[h* width + w] = tmp;
        }
    }
}
// So it can be drawn later, returns the GLuint for the ebo used to bind that buffer
GLuint addMesh(const char* fileName, const char* textureName);
GLuint addMeshNormalMap(const char* fileName, const char* textureName, const char* normalMap);
void removeMesh(GLuint ebo );


// Takes a mesh with basic texture, Blinn Phong shading
void DrawMeshStandard(CameraSpace* camera, Mesh* mesh);
// Allows for a normal map to be done at each pixel, as opposed to at each point on triangle
void DrawMeshNormalMap(CameraSpace* camera, );

