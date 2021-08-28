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

void shadeLightBasic(Model* model, Light* light) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, diffCol, specCol, lightCol, lightPow, err;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar );
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( mv * Vector4(light->worldSpaceCoord, 1.0f)).v3();
    Vector3 dColor = model->mesh.diffuseColor;
    Vector3 sColor = model->mesh.specColor;
    Vector3 lColor = light->color;
    
    glUseProgram(OpenGL.basicLightingShader);
    mvLoc = glGetUniformLocation(OpenGL.basicLightingShader, "modelView");
    mvpLoc = glGetUniformLocation(OpenGL.basicLightingShader, "modelViewProjection");
    normMatrix = glGetUniformLocation(OpenGL.basicLightingShader, "normalMatrix");
    lightPos = glGetUniformLocation(OpenGL.basicLightingShader, "lightCameraSpace");
    diffCol = glGetUniformLocation(OpenGL.basicLightingShader, "diffColor");
    specCol = glGetUniformLocation(OpenGL.basicLightingShader, "specularColor");
    lightCol = glGetUniformLocation(OpenGL.basicLightingShader, "lightColor");
    lightPow = glGetUniformLocation(OpenGL.basicLightingShader, "lightBrightness");
    glBindTexture(0, model->mesh.textures.id);
    err = glGetError();
    
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&mvp.data[0]);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (f32*)&mv.data[0]);
    glUniformMatrix3fv(normMatrix, 1, GL_FALSE, (f32*)&normalMatrix.data[0]);
    glUniform3f(lightPos, l[0], l[1], l[2]);
    glUniform3f(diffCol, dColor[0], dColor[1], dColor[2]);
    glUniform3f(specCol, sColor[0], sColor[1], sColor[2]);
    glUniform3f(lightCol, lColor[0], lColor[1], lColor[2]);
    glUniform1f(lightPow, light->irradiance);
}

void shadeLightTextured(Model* model, Light* light) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, specCol, lightCol, lightPow, err;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar);
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( mv * Vector4(light->worldSpaceCoord, 1.0f)).v3();
    Vector3 sColor = model->mesh.specColor;
    Vector3 lColor = light->color;
    
    
    // (TODO) which of these do we need? and which ones do we still need to add?
    glUseProgram(OpenGL.texturedLightingShader);
    mvpLoc = glGetUniformLocation(OpenGL.texturedLightingShader, "modelViewProjection");
    mvLoc = glGetUniformLocation(OpenGL.texturedLightingShader, "modelView");
    normMatrix = glGetUniformLocation(OpenGL.texturedLightingShader, "normMatrix");
    lightPos = glGetUniformLocation(OpenGL.texturedLightingShader, "lightCameraSpace");
    specCol = glGetUniformLocation(OpenGL.texturedLightingShader, "specularColor");
    lightCol = glGetUniformLocation(OpenGL.texturedLightingShader, "lightColor");
    lightPow = glGetUniformLocation(OpenGL.texturedLightingShader, "lightBrightness");
    glBindTextureUnit(0, model->mesh.textures.id);
    glBindTextureUnit(1, model->mesh.normalMap.id);
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&mvp.data[0]);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (f32*)&mv.data[0]);
    glUniformMatrix3fv(normMatrix, 1, GL_FALSE, (f32*)&normalMatrix.data[0]);
    glUniform3f(lightPos, l[0], l[1], l[2]);
    glUniform3f(specCol, sColor[0], sColor[1], sColor[2]);
    glUniform3f(lightCol, lColor[0], lColor[1], lColor[2]);
    glUniform1f(lightPow, light->irradiance);
    
    
    
}

Model addModelNormalMap(const char* fileName, const char* textureName, const char* normalMap ) {
    Model model;
    Texture tex(textureName);
    Texture norm(normalMap);
    model.mesh = loadMesh(fileName, tex);
    model.mesh.normalMap = norm;

    
    addMeshTangents(&model.mesh);
    activateModel(&model);
    return model;
}

Model addModel(const char* fileName, const char* textureName) {
    Model model;
    Texture req(textureName);
    model.mesh = loadMesh(fileName, req);
    activateModel(&model);
    return model;
}

// Internal
void activateModel(Model* model) {
    if (!model->mesh.normalVertices) {
        addBasicTexturedVerticesToShader(model->mesh.vertices, model->mesh.triangles, model->mesh.numVertices, model->mesh.numIndices, 0, 1, 2, &model->identifiers);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    }
    else {
        addVerticesToShader(model->mesh.normalVertices, model->mesh.triangles, model->mesh.numVertices, model->mesh.numIndices, 0, 1, 2, 3, 4, &model->identifiers);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    }
}



void setDrawModel(Model* model) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    glBindBuffer(GL_ARRAY_BUFFER, model->identifiers.vbo);
    glBindVertexArray(model->identifiers.vao);
    glDrawElements(GL_TRIANGLES, model->mesh.numIndices, GL_UNSIGNED_INT, 0);
}

void testViz(Model* model) {
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar);
    for (int i = 0; i < 10; ++i) {
        Vector4 t = model->mesh.normalVertices[i].coord;
        t = mvp * t;
        printf("%f %f %f %f\n", t.x, t.y, t.z, t.w);
    }
}


void renderModel(Model* model, Light* light) {
    
    if (model->mesh.normalVertices) {
        
        shadeLightTextured(model, light);
        setDrawModel(model);
    }
    else if (model->mesh.vertices) {
        shadeLightBasic(model, light);
        setDrawModel(model);
    }
    
}


