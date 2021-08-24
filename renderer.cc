/* 8/21/2021 */
#include "types.h"
#include "glwrangler.h"
#include "utilities.h"

struct OpenGL {
    f32 vFOV = 3.14/6.0f;
    f32 aspectRatio = 16.0f/9.0f;
    f32 znear = .5f;
    f32 zfar = 50.0f;
    CoordinateSpace cameraSpace;
    u32 basicLightingShader;
    
} OpenGL;
// Return a normal map from the height map
// it shouldn't matter the height map's range
// since this should point out we shoudl 
void normalMap(f32* heightMap, Vector3* normalMap, int32 height, int32 width ) {
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
            f32 pdy = (above - below) * .5f;

            f32 mag = sqrt(pdx * pdx + pdy*pdy + 1.0f);
            tmp.x = clampNormal(pdx/mag);
            tmp.y = clampNormal(pdy/mag);
            tmp.z = clampNormal(1.0f/mag);
            normalMap[h* width + w] = tmp;
        }
    }
}

void shadeLightBasic(Model* model, Light* light, bool compileShader = true) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, diffCol, specCol, lightCol, lightPow;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar );
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( mv * Vector4(light->worldSpaceCoord, 1.0f)).v3();
    Vector3 dColor = model->mesh.diffuseColor;
    Vector3 sColor = model->mesh.specColor;
    Vector3 lColor = light->color;
    if (compileShader)
        OpenGL.basicLightingShader = setShaders("../shaders/blinnPhongVertex.glsl", "../shaders/blinnPhongPixel.glsl");
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
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&mvp.data[0]);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (f32*)&mv.data[0]);
    glUniformMatrix3fv(normMatrix, 1, GL_FALSE, (f32*)&normalMatrix.data[0]);
    glUniform3f(lightPos, l[0], l[1], l[2]);
    glUniform3f(diffCol, dColor[0], dColor[1], dColor[2]);
    glUniform3f(specCol, sColor[0], sColor[1], sColor[2]);
    glUniform3f(lightCol, lColor[0], lColor[1], lColor[2]);
    glUniform1f(lightPow, light->irradiance);
}


Model addModel(const char* fileName, const char* textureName) {
    Model model;
    model.mesh = loadMesh(fileName, textureName);
    return model;
}

void activateModel(Model* model) {
    addBasicTexturedVerticesToShader(model->mesh.vertices, model->mesh.triangles, model->mesh.numVertices, model->mesh.numIndices, 0, 1, 2, &model->identifiers);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
}

#if 0
GLuint addModelNormalMap(const char* fileName, const char* textureName, const Vector3* normalMap);
// Takes a mesh with basic texture, Blinn Phong shading
void DrawMeshStandard(CameraSpace* camera, Mesh* mesh);
// Allows for a normal map to be done at each pixel, as opposed to at each point on triangle
void DrawMeshNormalMap(CameraSpace* camera, );

#endif
