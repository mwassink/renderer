/* 8/21/2021 */
#include "types.h"
#include "glwrangler.h"
#include "utilities.h"
#include "renderer.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#endif
GL OpenGL;
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

void shadeLightBasic(Model* model, Light* light, bool init = true) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, diffCol, specCol, lightCol, lightPow;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar );
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( mv * Vector4(light->worldSpaceCoord, 1.0f)).v3();
    Vector3 dColor = model->mesh.diffuseColor;
    Vector3 sColor = model->mesh.specColor;
    Vector3 lColor = light->color;
    if (init)
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
    
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&mvp.data[0]);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (f32*)&mv.data[0]);
    glUniformMatrix3fv(normMatrix, 1, GL_FALSE, (f32*)&normalMatrix.data[0]);
    glUniform3f(lightPos, l[0], l[1], l[2]);
    glUniform3f(diffCol, dColor[0], dColor[1], dColor[2]);
    glUniform3f(specCol, sColor[0], sColor[1], sColor[2]);
    glUniform3f(lightCol, lColor[0], lColor[1], lColor[2]);
    glUniform1f(lightPow, light->irradiance);
}

void shadeLightTextured(Model* model, Light* light,  bool setup = true) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, specCol, lightCol, lightPow;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar);
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( mv * Vector4(light->worldSpaceCoord, 1.0f)).v3();
    Vector3 sColor = model->mesh.specColor;
    Vector3 lColor = light->color;
    if (setup) {
        OpenGL.texturedLightingShader = setShaders("../shaders/basicTexturedVertex.glsl", "../shaders/basicTexturedPixel.glsl" );
    }
    
    
    glUseProgram(OpenGL.texturedLightingShader);
    mvpLoc = glGetUniformLocation(OpenGL.texturedLightingShader, "modelViewProjection");
    mvLoc = glGetUniformLocation(OpenGL.texturedLightingShader, "modelView");
    normMatrix = glGetUniformLocation(OpenGL.basicLightingShader, "normalMatrix");
    lightPos = glGetUniformLocation(OpenGL.basicLightingShader, "lightCameraSpace");
    specCol = glGetUniformLocation(OpenGL.basicLightingShader, "specularColor");
    lightCol = glGetUniformLocation(OpenGL.basicLightingShader, "lightColor");
    lightPow = glGetUniformLocation(OpenGL.basicLightingShader, "lightBrightness");
    glBindTextureUnit(0, model->mesh.textures);
    glBindTextureUnit(1, model->mesh.normalMap);

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&mvp.data[0]);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (f32*)&mv.data[0]);
    glUniformMatrix3fv(normMatrix, 1, GL_FALSE, (f32*)&normalMatrix.data[0]);
    glUniform3f(lightPos, l[0], l[1], l[2]);
    glUniform3f(specCol, sColor[0], sColor[1], sColor[2]);
    glUniform3f(lightCol, lColor[0], lColor[1], lColor[2]);
    glUniform1f(lightPow, light->irradiance);
    
    
    
}

Model addModel(const char* fileName, const char* textureName, int32 width, int32 height) {
    Model model;
    Texture req(textureName, width, height);
    model.mesh = loadMesh(fileName, req);
    return model;
}

void activateModel(Model* model) {
    addBasicTexturedVerticesToShader(model->mesh.vertices, model->mesh.triangles, model->mesh.numVertices, model->mesh.numIndices, 0, 1, 2, &model->identifiers);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
}

void setDrawModel(Model* model) {
#if DEBUG
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar);
    Mesh& mesh = model->mesh;
    for (int i = 0; i < 10; ++i) {
        for (int i = 0; i < 10; ++i) {
            Vector4 v(mesh.vertices[i].coord.x, mesh.vertices[i].coord.y, mesh.vertices[i].coord.z, mesh.vertices[i].coord.w);
            v = mvp * v;
            printf("%f %f %f %f\n", v.x, v.y, v.z, v.w);
        }
    }
#endif
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    glBindBuffer(GL_ARRAY_BUFFER, model->identifiers.vbo);
    glBindVertexArray(model->identifiers.vao);
    glDrawElements(GL_TRIANGLES, model->mesh.numIndices, GL_UNSIGNED_INT, 0);
}

#if 0
GLuint addModelNormalMap(const char* fileName, const char* textureName, const Vector3* normalMap);
// Takes a mesh with basic texture, Blinn Phong shading
void DrawMeshStandard(CameraSpace* camera, Mesh* mesh);
// Allows for a normal map to be done at each pixel, as opposed to at each point on triangle
void DrawMeshNormalMap(CameraSpace* camera, );

#endif
