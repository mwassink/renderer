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

void shadeLightBasic(Model* model, Light* light, GLuint shader) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, diffCol, specCol, lightCol, lightPow, err;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar );
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( mv * Vector4(light->worldSpaceCoord, 1.0f)).v3();
    Vector3 dColor = model->mesh.diffuseColor;
    Vector3 sColor = model->mesh.specColor;
    Vector3 lColor = light->color;
    
    glUseProgram(shader);
    mvLoc = glGetUniformLocation(shader, "modelView");
    mvpLoc = glGetUniformLocation(shader, "modelViewProjection");
    normMatrix = glGetUniformLocation(shader, "normalMatrix");
    lightPos = glGetUniformLocation(shader, "lightCameraSpace");
    diffCol = glGetUniformLocation(shader, "diffColor");
    specCol = glGetUniformLocation(shader, "specularColor");
    lightCol = glGetUniformLocation(shader, "lightColor");
    lightPow = glGetUniformLocation(shader, "lightBrightness");
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

void shadeLightTextured(Model* model, Light* light, GLuint shader) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, specCol, lightCol, lightPow, err;
    Matrix4 mv = modelView(OpenGL.cameraSpace, model->modelSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, OpenGL.cameraSpace, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar);
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( mv * Vector4(light->worldSpaceCoord, 1.0f)).v3();
    Vector3 sColor = model->mesh.specColor;
    Vector3 lColor = light->color;
    
    
    // (TODO) which of these do we need? and which ones do we still need to add?
    glUseProgram(shader);
    mvpLoc = glGetUniformLocation(shader, "modelViewProjection");
    mvLoc = glGetUniformLocation(shader, "modelView");
    normMatrix = glGetUniformLocation(shader, "normMatrix");
    lightPos = glGetUniformLocation(shader, "lightCameraSpace");
    specCol = glGetUniformLocation(shader, "specularColor");
    lightCol = glGetUniformLocation(shader, "lightColor");
    lightPow = glGetUniformLocation(shader, "lightBrightness");
    err = glGetError();
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

void shaderShadowed(Model* model, Light* light) {
    
}

void shaderShadowedTextured(Model* model, Light* light) {

    GLint sMatrixLoc = glGetUniformLocation(OpenGL.texturedShadowShader, "shadowMatrix");
    Matrix4 glproj = glModelViewProjection(model->modelSpace, light->lightSpace, PI/4.0f, 1, 1.0f, 40.0f);
    Matrix4 sMatrix = Matrix4(.5f, 0.0f, 0.0f, 0.5f,
                              0.0f, 0.5f, 0.0f, 0.5f,
                              0.0f, 0.0f, 0.5f, 0.5f,
                              0.0f, 0.0f, 0.0f, 1.0f) * glproj;
                           
    //Matrix4 sMatrix = glproj;
    glUniformMatrix4fv(sMatrixLoc, 1, GL_FALSE, (f32*)&sMatrix.data[0]);

    glBindTextureUnit(2, light->depthTexture);
    GLint err = glGetError();
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
        addBasicVerticesShadowMapping(model->mesh.vertices, model->mesh.triangles, model->mesh.numVertices, model->mesh.numIndices, 0, &model->identifiers);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    }
    else {
        addVerticesToShader(model->mesh.normalVertices, model->mesh.triangles, model->mesh.numVertices, model->mesh.numIndices, 0, 1, 2, 3, 4, &model->identifiers);
        addVerticesShadowMapping(model->mesh.normalVertices, model->mesh.triangles, model->mesh.numVertices, model->mesh.numIndices, 0, &model->identifiers);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    }
    
}



void setDrawModel(Model* model) {
    glBindBuffer(GL_ARRAY_BUFFER, model->identifiers.vbo);
    glBindVertexArray(model->identifiers.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    glDrawElements(GL_TRIANGLES, model->mesh.numIndices, GL_UNSIGNED_INT, 0);
}

void testViz(Model* model, CoordinateSpace* cs) {
    Matrix4 mvp = glModelViewProjection(model->modelSpace, *cs, OpenGL.vFOV, OpenGL.aspectRatio, OpenGL.znear, OpenGL.zfar);
    for (int i = 0; i < 10; ++i) {
        Vector4 t = model->mesh.normalVertices[i].coord;
        t = mvp * t;
        printf("%f %f %f %f\n", t.x, t.y, t.z, t.w);
    }
}

// Shadow maps need to be set up BEFORE this is called
void renderModel(Model* model, Light* light) {
    GLint err;
    u32 shader = OpenGL.basicLightingShader;
    if (model->mesh.normalVertices) {
        shader = OpenGL.texturedLightingShader;
        if (light->shadows) {
            shader = OpenGL.texturedShadowShader;
        }
        err = glGetError();
        shadeLightTextured(model, light, shader);
        if (light->shadows) {
            shaderShadowedTextured(model, light);
        }
        setDrawModel(model);

    }
    else if (model->mesh.vertices) {
        shadeLightBasic(model, light, shader);
        setDrawModel(model);
    }
    
}

// The depthtex needs to be bound at this point
void attachDepthTextureFramebuffer(u32 depthTex, u32 depthFBO) {
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  depthTex, 0);
    glDrawBuffer(GL_NONE);
    GLint err = glGetError();
    //glReadBuffer(GL_NONE);
    bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void createShadowMapTexture(Light* light, u32 res) {
    GLuint depthTex;
    f32 border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
                 res, res, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0); // done

    light->depthTexture = depthTex;
    
}

// (TODO)allow for config of near, far plane
// This will give the shadows in the texture... we still need to add them to the rendering pipeline
void addShadowMapping(Model* models, Light* light, u32 numModels) {
#define RES 2500
#define RUN 1
#if RUN
    GLint err;
    RECT rect;
    GetWindowRect(OpenGL.windowHandle, &rect);

    if (light->depthTexture < 0) {
        createShadowMapTexture(light, RES);
    }
#endif
    glUseProgram(OpenGL.shadowMappingShader);
    // (TODO) can I reuse these buffers?
#if RUN 
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL.shadowMappingFramebuffer);
    
    bool z = glIsEnabled(GL_DEPTH_TEST);
    
    attachDepthTextureFramebuffer(light->depthTexture, OpenGL.shadowMappingFramebuffer);
    glViewport(0, 0, RES, RES);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);
#endif
    
    for (int i = 0; i < numModels; ++i) {
        Matrix4 modelLightProjection = glModelViewProjection(models[i].modelSpace, light->lightSpace, PI/4.0f,
                                                             1, 1.0f, 40.0f);
        testViz(&models[i], &light->lightSpace);
        GLint mvpLoc = glGetUniformLocation(OpenGL.shadowMappingShader, "modelViewProjection");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&modelLightProjection.data[0]);

        glBindBuffer(GL_ARRAY_BUFFER, models[i].identifiers.vbo);
        glBindVertexArray(models[i].identifiers.smVao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i].identifiers.ebo);
        glDrawElements(GL_TRIANGLES, models[i].mesh.numIndices, GL_UNSIGNED_INT, 0);
    }
#if RUN
    glDisable(GL_POLYGON_OFFSET_FILL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
#undef RES
}
