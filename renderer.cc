/* 8/21/2021 */
#include "types.h"
struct OpenGL {
    f32 vFOV = 3.14/6.0f;
    f32 aspectRatio = 16.0f/9.0f;
    f32 znear = .5f;
    f32 zfar = 50.0f;
    CoordinateSpace cameraSpace;
    GLuint basicLightingShader;
    
} OpenGL;
// Return a normal map from the height map
// it shouldn't matter the height map's range
// since this should point out we shoudl 
void normalMap(f32* heightMap, Vector3* normalMap, int32 height, int32 map ) {
    Vector3 tmp;
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

void shadeLightBasic(Model* model, Light* light) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, diffCol, specCol, lightCol, lightPow;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace   );
    Matrix4 mvp = glModelViewProjection(model->objSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar );
    Matrix4 normalMatrix = normalTransform(mv);
    Vector3 l = light->worldSpaceCoord * mv;
    Vector3 dColor = {};
    OpenGL.basicLightingShader = setShaders("../shaders/test_vertex3.glsl", "../shaders/test_pixel3.glsl");
    glUseProgram(OpenGL.basicLightingShader);
    mvLoc = glGetUniformLocation(OpenGL.basicLightingShader, "modelView");
    mvpLoc = glGetUniformLocation(OpenGL.basicLightingShader, "modelViewProjection");
    normMatrix = glGetUniformLocation(OpenGL.basicLightingShader, "normalMatrix");
    lightPos = glGetUniformLocation(OpenGL.basicLightingShader, "lightCameraSpace");
    diffCol = glGetUniformLocation(OpenGL.basicLightingShader, "diffColor");
    specCol = glGetUniformLocation(OpenGL.basicLightingShader, "specularColor");
    lightCol = glGetUniformLocation(OpenGL.basicLightingShader, "lightColor");
    lightPow = glGetUniformLocation(OpenGL.basicLightingShader, "lightBrightness");
    if (mvpLoc * mvLoc * normMatrix * lightPos * diffCol * specCol * lightCol * lightPow < 0) {
        ASSERT(0);
    }
    
    
    
}
// So it can be drawn later, returns the GLuint for the ebo used to bind that buffer
Model addModel(const char* fileName, const char* textureName) {
    
}
GLuint addModelNormalMap(const char* fileName, const char* textureName, const Vector3* normalMap);
void removeMesh(GLuint ebo );


// Takes a mesh with basic texture, Blinn Phong shading
void DrawMeshStandard(CameraSpace* camera, Mesh* mesh);
// Allows for a normal map to be done at each pixel, as opposed to at each point on triangle
void DrawMeshNormalMap(CameraSpace* camera, );

