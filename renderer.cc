/* 8/21/2021 */
#include "types.h"
#include "glwrangler.h"
#include "utilities.h"
#include "renderer.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#endif

// Poached from the interwebs
f32 cubeVertices[] = {
    
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};


RendererContext::RendererContext() {
    RendererUtil tmp;
    vFOV = 3.14f/6.0f;
    aspectRatio = 16.0f/9.0f;
    znear = 1.0f;
    zfar = 125.0f;
    basicLightingShader =  tmp.setShaders("../shaders/blinnPhongVertex.glsl", "../shaders/blinnPhongPixel.glsl");
    texturedLightingShader = tmp.setShaders("../shaders/basicTexturedVertex.glsl", "../shaders/basicTexturedPixel.glsl" );
    
    texturedShadowShader = tmp.setShaders("../shaders/shadowedVertex.glsl","../shaders/shadowedPixel.glsl" );
    shadowMappingShader = tmp.setShaders("../shaders/vshadowMap.glsl", "../shaders/pshadowMap.glsl");
    skyboxShader = tmp.setShaders("../shaders/vskybox.glsl", "../shaders/pskybox.glsl");
    quadShader = tmp.setShaders("../shaders/vquad.glsl", "../shaders/vpixel.glsl");
    computeTarget = Texture();
    glGenFramebuffers(1, &shadowMappingFramebuffer);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}




void RendererUtil::SetupBasicShader(Model* model, PointLight* light, GLuint shader) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, diffCol, specCol, lightCol, lightPow, err;
    Matrix4 mv = modelView(model->modelSpace, context->cameraSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, context->cameraSpace, context->vFOV, context->aspectRatio, context->znear, context->zfar );
    Matrix3 tmp = Matrix3x3(mv);
    Matrix3 normalMatrix = normalTransform(tmp);
    Matrix4 worldObject = WorldObjectMatrix(context->cameraSpace);
    Vector4 pt = Vector4(light->worldSpaceCoord, 1.0f);
    
    Vector3 l = (worldObject*pt).v3();
    
    
    glUseProgram(shader);
    uplumbMatrix4(shader, mv, "modelView");
    uplumbMatrix4(shader, mvp, "modelViewProjection");
    uplumbMatrix3(shader, normalMatrix, "normalMatrix");
    uplumbVector3(shader, l, "lightCameraSpace");
    uplumbVector3(shader, model->mesh.diffuseColor, "diffColor");
    uplumbVector3(shader, model->mesh.specColor, "specularColor");
    uplumbVector3(shader, light->color, "lightColor");
    uplumbf(shader, light->irradiance, "lightBrightness");
    
    
    
}

void RendererUtil::AddTexturingToShader (Model* model, SpotLight* light, GLuint shader) {
    
    glBindTextureUnit(0, model->mesh.textures.id);
    glBindTextureUnit(1, model->mesh.normalMap.id);
}
void RendererUtil::AddShadowsToShader(Model* model, SpotLight* light, GLuint shader) {
    
    Matrix4 glproj = glModelViewProjection(model->modelSpace, light->lightSpace, PI/4.0f, 1, 1.0f, 40.0f);
    Matrix4 l = Matrix4(.5f, 0.0f, 0.0f, 0.5f,
                        0.0f, 0.5f, 0.0f, 0.5f,
                        0.0f, 0.0f, 0.5f, 0.5f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    Matrix4 sMatrix = l  * glproj;
    uplumbMatrix4(shader, sMatrix, "shadowMatrix");
    glBindTextureUnit(2, light->depthTexture);
    
}




// (TODO) join duplicate code in this and the one below it
Model RendererUtil::addModelNormalMap(const char* fileName, const char* textureName, const char* normalMap, bool fast) {
    Model model;
    Texture tex(textureName);
    Texture norm(normalMap);
    if (fast) {
        model.mesh = BinaryMesh(fileName);
        model.mesh.textures = tex;
    } else {
        model.mesh = loadMesh(fileName, tex);
    }
    model.mesh.normalMap = norm;
    
    
    if (!model.mesh.normalVertices) {
        addMeshTangents(&model.mesh);
    }
    
    activateModel(&model);
    return model;
}

Model RendererUtil::addModel(const char* fileName, const char* textureName) {
    Model model;
    Texture req(textureName);
    model.mesh = loadMesh(fileName, req);
    activateModel(&model);
    return model;
}

// Internal
void RendererUtil::activateModel(Model* model) {
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
// The depthtex needs to be bound at this point
void RendererUtil::attachDepthTextureFramebuffer(u32 depthTex, u32 depthFBO) {
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  depthTex, 0);
    glDrawBuffer(GL_NONE);
    
    //glReadBuffer(GL_NONE);
    bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void RendererUtil::defaultTexParams(GLenum target) {
    f32 border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    
    glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
}

void RendererUtil::createShadowMapTexture(SpotLight* light, u32 res) {
    GLuint depthTex;
    
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
                 res, res, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    defaultTexParams(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // done
    
    light->depthTexture = depthTex;
    
}


void RendererUtil::addBasicTexturedVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, u32 positionNorm, u32 positionUV, glTriangleNames* names ) {
    
    glGenBuffers(1, &names->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices* sizeof(u32), indices
                 , GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &names->vao);
    glBindVertexArray(names->vao);
    glGenBuffers(1, &names->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertices, GL_STATIC_DRAW);
    
    
    glVertexAttribPointer(positionCoord, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, coord)));
    glVertexAttribPointer(positionNorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(positionUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, uv)));
    
    glEnableVertexAttribArray(positionCoord);
    glEnableVertexAttribArray(positionNorm); 
    glEnableVertexAttribArray(positionUV);
    
}


void RendererUtil::addVerticesToShader(VertexLarge* vertices, u32* indices, int numVertices, int numIndices,
                                       u32 positionCoord, u32 positionNorm, u32 positionTangent, u32 positionUV, u32 positionHandedness, glTriangleNames* names) {
    glGenBuffers(1, &names->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(u32), indices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &names->vao);
    glBindVertexArray(names->vao);
    glGenBuffers(1, &names->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexLarge)*numVertices, vertices, GL_STATIC_DRAW);
    
    
    glVertexAttribPointer(positionCoord, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLarge),reinterpret_cast<void*>(offsetof(VertexLarge, coord)));
    glVertexAttribPointer(positionNorm, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLarge),reinterpret_cast<void*>(offsetof(VertexLarge, normal)));
    glVertexAttribPointer(positionTangent, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLarge),reinterpret_cast<void*>(offsetof(VertexLarge, tangent)));
    glVertexAttribPointer(positionUV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLarge),reinterpret_cast<void*>(offsetof(VertexLarge, uv)));
    glVertexAttribPointer(positionHandedness, 1, GL_FLOAT, GL_FALSE, sizeof(VertexLarge), reinterpret_cast<void*>(offsetof(VertexLarge, handedness)));
    
    glEnableVertexAttribArray(positionCoord);
    glEnableVertexAttribArray(positionNorm);
    glEnableVertexAttribArray(positionTangent);
    glEnableVertexAttribArray(positionUV);
    glEnableVertexAttribArray(positionHandedness);
    
    
    
}


// Returns 0 on success
int RendererUtil::checkFailure(int shader, GLenum status) {
    
    int success = 0;
    char log[512] = { 0 };
    glGetShaderiv(shader, status, &success);
    
    if(!success ){
        glGetShaderInfoLog(shader, 512, NULL, log);
        FILE* fp = fopen("error.log", "w");
        fwrite( log, 1, 512, fp);
        fclose(fp);
        return -1;
    }
    return 0;
}

int RendererUtil::checkFailureLink(int shader, GLenum status) {
    int success = 0;
    char log[512] = { 0 };
    glGetProgramiv(shader, status, &success);
    
    if (!success) {
        glGetProgramInfoLog(shader, 512, NULL, log);
        FILE* fp = fopen("error.log", "w");
        fwrite(log, 1, 512, fp);
        fclose(fp);
        return -1;
    }
    return 0;
}

int RendererUtil::setShaders(const char* vertexFile, const char* fragmentFile) {
    int size;
    const char* vertexShaderSrc = 0; const char* fragmentShaderSrc = 0;
    GLuint v, f, program;
    if (vertexFile) {
        vertexShaderSrc = readFile(vertexFile, &size);
        if (!vertexShaderSrc)
            return -1;
        v = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(v, 1, &vertexShaderSrc, NULL);
        glCompileShader(v);
        GLint err = glGetError();
        if (checkFailure(v, GL_COMPILE_STATUS)) {
            free((void*)vertexShaderSrc);
            return -1;
        }
        free((void*)vertexShaderSrc);
    }
    if (fragmentFile) {
        fragmentShaderSrc = readFile(fragmentFile, &size);
        if (!fragmentShaderSrc)
            return -1;
        f = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(f, 1, &fragmentShaderSrc, NULL  );
        glCompileShader(f);
        
        if (checkFailure(f, GL_COMPILE_STATUS)) {
            free((void*)fragmentShaderSrc);
            return -1;
        }
        free((void*)fragmentShaderSrc);
    }
    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    GLint err = glGetError();
    if (checkFailureLink(program, GL_LINK_STATUS)) {
        return -1;
    }
    
    
    
    return program;
    
}

// This would be necessary if it were not loaded in in the texture
Vector3* RendererUtil::loadNormals(const char* fileName, u32* widthOut, u32* heightOut) {
    u32 w, h, bpp;
    u8* data = loadBitmap(fileName, &w, &h, &bpp);
    
    if (bpp != 24) {
        free(data);
        return 0;
    }
    
    Vector3* vecs = (Vector3*)malloc(sizeof(Vector3)*w*h);
    for (int y = 0; y < h; ++y){
        for (int x = 0; x < w; ++x) {
            f32 r = data[0] / 255.0f;
            f32 g = data[1] / 255.0f;
            f32 b = data[2] / 255.0f;
            vecs[y*w + x] = Vector3(r, g, b);
            data += 3;
        }
    }
    free(data);
    
    *widthOut = w;
    *heightOut = h;
    return vecs;
    
}

// Return a normal map from the height map
// it shouldn't matter the height map's range
// since this should point out we shoudl 
void RendererUtil::normalMap(f32* heightMap, Vector3* normalMap, int32 height, int32 width ) {
    Vector3 tmp;
    for (int h = 0; h < height; ++h) {
        int32 hup = clampRangei(0, height-1, h-1);
        int32 hdown = clampRangei(0, height-1, h+1);
        for (int w = 0; w < width; ++w) {
            const f32 above = heightMap[hup*width + w ];
            const f32 below = heightMap[hdown* width + w];
            const f32 left = heightMap[h*width + clampRangei(0, width-1, w -1 )];
            const f32 right = heightMap[h*width + clampRangei(0, width-1, w + 1)];
            // if we went down from say height 2 at x = 1 to height 0 at x = 3, then the normal would point to +x, which is correct
            f32 npdx = (left - right) * .5f;
            f32 npdy = (above - below) * .5f;
            
            f32 mag = sqrt(npdx * npdx + npdy*npdy + 1.0f);
            tmp.x = clampNormal(npdx/mag);
            tmp.y = clampNormal(npdy/mag);
            tmp.z = clampNormal(1.0f/mag);
            normalMap[h* width + w] = tmp;
        }
    }
}

void RendererUtil::buildNormalMap(const char* hFile, const char* n) {
    u32 w, h;
    f32* heightmap = convertBitmapHeightmap(hFile, &w, &h ,20.0f);
    Vector3* normalMapVecs = (Vector3*)malloc(sizeof(Vector3)*w*h);
    normalMap(heightmap, normalMapVecs, h, w);
    writeNormalMapBitmap(w, h, normalMapVecs, n);
}

f32* RendererUtil::convertBitmapHeightmap(const char* bitmapFile, u32* w, u32* h, f32 maxHeight) {
    u32 width, height, bpp;
    u8* mem = loadBitmap(bitmapFile, &width, &height, &bpp);
    u8* bitmap = mem;
    if (!bitmap) {
        return 0;
    }
    f32* heightmap = (f32*)malloc(width*height*sizeof(f32));
    for (int y = 0; y < height; ++y ) {
        for (int x = 0; x < width; ++x) {
            heightmap[y*width + x] = maxHeight* (*bitmap) / 255.0f;
            bitmap += 3;
        }
    }
    
    free(mem);
    *w = width;
    *h = height;
    return heightmap;
}

void RendererUtil::addBasicVerticesShadowMapping(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, glTriangleNames* names) {
    glGenVertexArrays(1, &names->smVao );
    glBindVertexArray(names->smVao);
    
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glVertexAttribPointer(positionCoord, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, coord)) );
    glEnableVertexAttribArray(positionCoord);
    
}

void RendererUtil::addVerticesShadowMapping(VertexLarge* vertices, u32* indices, int numVertices, int numIndices,
                                            u32 positionCoord, glTriangleNames* names) {
    glGenVertexArrays(1, &names->smVao );
    glBindVertexArray(names->smVao);
    
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glVertexAttribPointer(positionCoord, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLarge),reinterpret_cast<void*>(offsetof(VertexLarge, coord)) );
    glEnableVertexAttribArray(positionCoord);
    
}


int RendererUtil::setupBitmapTexture(const char* textureString, u32* width, u32* height, u32* bitsPerPixel) {
    
    return BitmapTextureInternal(textureString, width, height, bitsPerPixel);
}


void RendererUtil::depthRenderCleanup(void) {
    glDisable(GL_POLYGON_OFFSET_FILL);
}


// Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh". Terathon Software 3D Graphics Library,2001. http://www.terathon.com/code/tangent.html
void RendererUtil::addMeshTangents(Mesh* mesh) {
    MeshTangentsInternal(mesh);
}

void Renderer::setDrawModel(Model* model) {
    glBindBuffer(GL_ARRAY_BUFFER, model->identifiers.vbo);
    glBindVertexArray(model->identifiers.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    glDrawElements(GL_TRIANGLES, model->mesh.numIndices, GL_UNSIGNED_INT, 0);
}

void Renderer::testViz(Model* model, CoordinateSpace* cs) {
    Matrix4 mvp = glModelViewProjection(model->modelSpace, *cs, context.vFOV, context.aspectRatio, context.znear, context.zfar);
    
    if (model->mesh.normalVertices) {
        for (int i = 0; i < 10; ++i) {
            Vector4 t = model->mesh.normalVertices[i].coord;
            t = mvp * t;
            printf("%f %f %f %f\n", t.x, t.y, t.z, t.w);
        }
    }
    else {
        for (int i = 0; i < 10; ++i) {
            Vector4 t = model->mesh.vertices[i].coord;
            t = mvp * t;
            printf("%f %f %f %f\n", t.x, t.y, t.z, t.w);
        }
    }
}

// Shadow maps need to be set up BEFORE this is called
void Renderer::renderModel(Model* model, SpotLight* light) {
    
    u32 shader = context.basicLightingShader;
    if (model->mesh.normalVertices) {
        shader = context.texturedLightingShader;
        if (light->shadows) {
            shader = context.texturedShadowShader;
        }
        utilHelper.SetupBasicShader(model, (PointLight*)light, shader);
        utilHelper.AddTexturingToShader(model, light, shader);
        
        if (light->shadows) {
            utilHelper.AddShadowsToShader(model, light, shader);
        }
        setDrawModel(model);
    }
    else if (model->mesh.vertices) {
        utilHelper.SetupBasicShader(model, (PointLight*)light, shader);
        setDrawModel(model);
    }
    
}

void Renderer::renderModel(Model* model, PointLight* pointLight) {
    //stub
}



// (TODO)allow for config of near, far plane
// This will give the shadows in the texture... we still need to add them to the rendering pipeline
void Renderer::ShadowPass(Model* models, SpotLight* light, u32 numModels) {
#define RES 500
    
    
    GLint err;
    RECT rect;
    GetWindowRect(context.windowHandle, &rect);
    glUseProgram(context.shadowMappingShader);
    
    if (light->depthTexture < 0) {
        utilHelper.createShadowMapTexture(light, RES);
    }
    
    GLint err1 = glGetError();
    glBindFramebuffer(GL_FRAMEBUFFER, context.shadowMappingFramebuffer);
    
    bool z = glIsEnabled(GL_DEPTH_TEST);
    
    utilHelper.attachDepthTextureFramebuffer(light->depthTexture, context.shadowMappingFramebuffer);
    glViewport(0, 0, RES, RES);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);
    
    
    for (int i = 0; i < numModels; ++i) {
        GLint err2 = glGetError();
        Matrix4 modelLightProjection = glModelViewProjection(models[i].modelSpace, light->lightSpace, PI/4.0f,
                                                             1, 1.0f, 40.0f);
        testViz(&models[i], &light->lightSpace);
        GLint mvpLoc = glGetUniformLocation(context.shadowMappingShader, "modelViewProjection");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&modelLightProjection.data[0]);
        
        glBindBuffer(GL_ARRAY_BUFFER, models[i].identifiers.vbo);
        glBindVertexArray(models[i].identifiers.smVao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i].identifiers.ebo);
        glDrawElements(GL_TRIANGLES, models[i].mesh.numIndices, GL_UNSIGNED_INT, 0);
        err2 = glGetError();
        
    }
    GLint err3 = glGetError();
    glDisable(GL_POLYGON_OFFSET_FILL);
    err3 = glGetError();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    err3 = glGetError();
    glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    err3 = glGetError();
    
#undef RES
}


// Returns Mcamera with a cube map
// Model space is analogous to cube space
// In the case of the shadow then mcube should be at the light
Matrix4 Renderer::invCubeFaceCamera(Matrix4& mCube, Matrix4& mFace) {
    Matrix4 mCamera = mCube * mFace;
    return invTransform(mCamera);
    
}

Array<Matrix4> Renderer::cubeMapMatrices(CoordinateSpace& renderSpace) {
    Array<Matrix4> invCameraMatrices(6);
    Matrix4 mCube = ObjectWorldMatrix(renderSpace);
    invCameraMatrices[0] = Matrix4(0,0,1,0,-1,0,-1,0,0);
    invCameraMatrices[1] = Matrix4(0,0,-1,0,-1,0,1,0,0);
    invCameraMatrices[2] = Matrix4(1,0,0,0,0,1,0,1,0);
    invCameraMatrices[3] = Matrix4(1,0,0,0,0 ,-1,0,-1,0);
    invCameraMatrices[4] = Matrix4(1,0,0,0,-1,0, 0,0,1);
    invCameraMatrices[5] = Matrix4(-1,0,0,0,-1,0,0,0,-1);
    
    invCameraMatrices[0] = invCubeFaceCamera(mCube, invCameraMatrices[0] );
    invCameraMatrices[1] = invCubeFaceCamera(mCube, invCameraMatrices[1] );
    invCameraMatrices[2] = invCubeFaceCamera(mCube, invCameraMatrices[2] );
    invCameraMatrices[3] = invCubeFaceCamera(mCube, invCameraMatrices[3] );
    invCameraMatrices[4] = invCubeFaceCamera(mCube, invCameraMatrices[4] );
    invCameraMatrices[5] = invCubeFaceCamera(mCube, invCameraMatrices[5] );
    return invCameraMatrices;
}

void Renderer::renderPointShadow(Array<Model>* models, PointLight* light) {
    if (light->cubeArgs.tex == -1) {
        light->cubeArgs.internalFormat = GL_DEPTH_COMPONENT32;
        light->cubeArgs.format = GL_DEPTH_COMPONENT;
        light->cubeArgs.shader = context.shadowMappingShader;
        light->cubeArgs.attachment = GL_DEPTH_ATTACHMENT;
        light->cubeArgs.res = 500;
    }
    // (TODO) could just have the far plane be after attenuates fully
    CubeMapRender(models, light->lightSpace, 1.0f, 20.0f, &light->cubeArgs  );
}

// this is not just the regular projection matrix
Matrix4 Renderer::shadowMapProj(f32 vFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane ) {
    f32 c = 1.0f/ tanf(vFOV/2);
    return Matrix4(c/(aspectRatio*2), 0, 1/2, 0,
                   0, c/2, 1/2, 0,
                   0, 0, -(farPlane)/(farPlane - nearPlane), -nearPlane*farPlane/(farPlane - nearPlane),
                   0, 0, -1, 0); 
}


// don't try to reuse the plumbing for some of the other ones
void Renderer::depthRender(Model* model, Matrix4& invCameraMatrix, int res, f32 n, f32 f) {
    
    Matrix4 proj = glProjectionMatrix(PI / 4, 1.0f, n, f);
    Matrix4 m = ObjectWorldMatrix(model->modelSpace);
    Matrix4 vm = (invCameraMatrix * m);
    Matrix4 modelViewProjection =  proj * vm;
    GLint err = glGetError();
    GLint mvpLoc = glGetUniformLocation(context.shadowMappingShader, "modelViewProjection");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)modelViewProjection.data[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, model->identifiers.vbo);
    glBindVertexArray(model->identifiers.smVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    glDrawElements(GL_TRIANGLES, model->mesh.numIndices, GL_UNSIGNED_INT, 0);
}


// (TODO) add this later
void Renderer::envMapRender(Model* model, Matrix4& invCameraMatrix, int res, f32 n, f32 f) {
    Matrix4 p = glProjectionMatrix(PI/4, 1.0f, n, f  ), m = ObjectWorldMatrix(model->modelSpace);
    Matrix4 vm = invCameraMatrix * m;
    Matrix4 modelViewProjection =  p * vm;
    glViewport(0, 0, res, res);
    glClear(GL_COLOR_BUFFER_BIT);
    GLint err = glGetError();
    
}


void Renderer::CubeMapRender(Array<Model>* models, CoordinateSpace& renderCS, f32 n, f32 f, CubeArgs* renderArgs) {
    
    
    GLuint id;
    RECT rect;
    Array<Matrix4> invCameraMatrices = cubeMapMatrices(renderCS);
    GetWindowRect(context.windowHandle, &rect);
    
    if (renderArgs->tex == -1) {
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        utilHelper.defaultTexParams(GL_TEXTURE_CUBE_MAP);
        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, renderArgs->internalFormat, renderArgs->res, renderArgs->res, 0, renderArgs->format, GL_FLOAT, 0);   
            GLint err = glGetError();
            err = glGetError();
        }
        renderArgs->tex = id;
    }
    
    glUseProgram(renderArgs->shader);
    glBindFramebuffer(GL_FRAMEBUFFER, context.shadowMappingFramebuffer);        
    for (int i = 0; i < 6; ++i) {
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, renderArgs->tex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, renderArgs->attachment,GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               renderArgs->tex, 0);
        glDrawBuffer(GL_NONE);
        glViewport(0, 0, renderArgs->res, renderArgs->res);
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 4.0f);
        GLint err = glGetError();
        for (int k = 0; k < models->sz; ++k) {
            if (renderArgs->shader == context.shadowMappingShader) {
                depthRender(&(*models)[k], invCameraMatrices[i], renderArgs->res, 1.0f, 20.0f);
            }
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, renderArgs->attachment,GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0);
    }
    
    if (renderArgs->shader == context.shadowMappingShader) {
        utilHelper.depthRenderCleanup();
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    invCameraMatrices.release();
    
}


Renderer::Renderer() {
#define DEPTHTEXRES 500
    context = RendererContext();
    utilHelper.context = &context;
    
}

#define CHECKGL(str) if (glGetError() != GL_NO_ERROR) {fatalError(str, "Error");}

int RendererUtil::InitializeCubeMaps(const char* fileNames[6]) {
    
    GLuint tex;
    void* textureData[6];
    u32 widths[6];
    u32 heights[6];
    u32 bpps[6];
    u32 mips = 0;
    u32 widthCounter;
    f32 borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    for (int i = 0; i < 6; ++i) {
        textureData[i] = (void*)loadBitmap(fileNames[i], &widths[i], &heights[i], &bpps[i]);
        
        ASSERT(bpps[i] == 24 && widths[i] == heights[i]); // for now
        if (i) {
            ASSERT(widths[i] == widths[i-1]);
            ASSERT(heights[i] == heights[i-1]);
        } else {
            writeOutBMP("testOutput.bmp", widths[i], heights[i], (u8*)textureData[i]);
        }
    }
    u32 width = widths[0];
    u32 height = heights[0];
    widthCounter = widths[0];
    while (widthCounter >>= 1) mips++;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &tex);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    for (int f = 0; f < 6; f++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, textureData[f]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    for (int i = 0; i < 6; i++) {
        free(textureData[i]);
    }
    if (mips != 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateTextureMipmap(tex);
    }
    
    return tex;
}


Skybox Renderer::MakeSkybox(const char* fileNames[6]) {
    
    
    
    Skybox skybox;
    u32 vao, vbo;
    skybox.texture = utilHelper.InitializeCubeMaps(fileNames);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) *3, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    skybox.vao = vao;
    skybox.vbo = vbo;
    return skybox;
    
}

void Renderer::RenderSkybox(Skybox& box) {
    
    glUseProgram(context.skyboxShader);
    CoordinateSpace cameraOrigin = context.cameraSpace;
    cameraOrigin.origin = Vector3(0, 0, 0);
    Matrix4 v = WorldObjectMatrix(cameraOrigin);
    Matrix4 p = glProjectionMatrix(context.vFOV, context.aspectRatio, context.znear, context.zfar);
    Matrix4 pv = p * v;
    uplumbMatrix4(context.skyboxShader, pv, "viewProjection" );
    
    
    glDepthFunc(GL_LEQUAL);
    glBindBuffer(GL_ARRAY_BUFFER, box.vbo);
    glBindVertexArray(box.vao);
    glBindTextureUnit(0, box.texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    CHECKGL("Failure rendering skybox")
}

// Project onto many different directions and find the most extreme difference
// This gives a good estimate of the furthest points
Sphere Renderer::OKBoundingSphere(Vector3* vertices, int numVerts) {
    
    const int nDirs = 27;
    Vector3 directions[nDirs];
    f32 maxDots[nDirs];
    f32 minDots[nDirs];
    int minIndexes[nDirs];
    int maxIndexes[nDirs];
    int ctr = 0;
    for (int x = -1; x < 2; ++x) {
        for (int y = -1; y < 2; ++y) {
            for (int z = -1; z < 2; ++z) {
                directions[ctr++] = Vector3(x, y, z);
            }
        }
    }
    
    
    for (int i = 0; i < nDirs; ++i) {
        Vector3& direction = directions[i];
        f32 dMin = 10000000.0f;
        f32 dMax =  10000000.0f;
        int minIndex, maxIndex = -1;
        for (int j = 0; j < numVerts; ++j) {
            f32 dp = dot(direction, vertices[j]);
            minIndex = dp < dMin ? j : minIndex;
            maxIndex = dp > dMax ? j : maxIndex;
            dMin = dp < dMin ? dp : dMin;
            dMax = dp > dMax ? dp : dMax;
        }
        maxDots[i] = dMax;
        minDots[i] = dMin;
        minIndexes[i] = minIndex;
        maxIndexes[i] = maxIndex;
    }
    
    f32 diameter = -1.0f;
    int bestDir = -1;
    Vector3 bestMin, bestMax;
    for (int i = 0; i < nDirs; ++i) {
        Vector3 minVec  = vertices[minIndexes[i]];
        Vector3 maxVec = vertices[maxIndexes[i]];
        Vector3 dv = maxVec - minVec;
        if (dot(dv, dv) > diameter) {
            diameter =  dot(dv, dv);
            bestDir = i;
            bestMax = maxVec;
            bestMin = minVec;
        }
        
    }
    
    Sphere sp;
    sp.p = (bestMax + bestMin) / 2.0f;
    sp.radius = (bestMax - bestMin).mag() / 2.0f;
    return sp;
    
    
}

void Renderer::AdjustBoundingSphere(Sphere* sp, Vector3* vertices, int numVerts ) {
    
    Vector3 center = sp->p;
    for (int i = 0; i < numVerts; i++) {
        f32 ds = center.dist(vertices[i]);
        if (ds > sp->radius) {
            sp->radius = ds;
        }
    }
}

void Renderer::DrawTexture(Texture* texture) {
    glUseProgram(context.quadShader);
    glBindTextureUnit(0, texture->id);
    FullScreenQuad();
}

Texture RendererUtil::RenderTarget(void) {
    GLuint texID;
    Texture texture;
    RECT rect;
    GetWindowRect(context->windowHandle, &rect);
    int w = rect.right - rect.left, h = rect.bottom - rect.top;
    glCreateTextures(GL_TEXTURE_2D, 1, &texID);
    texture.id = (int)texID;
    defaultTexParams(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT,
                 NULL);
    texture.width = w;
    texture.height = h;
    return texture;
}

// Assumes the compute shader is bound before this
void Renderer::RunComputeShader(int computeShader, int minX, int minY, int minZ) {
    
    int maxGroupX, maxGroupY, maxGroupZ;
    int maxSizeX, maxSizeY, maxSizeZ;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxGroupX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxGroupY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxGroupZ);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxSizeX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxSizeY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxSizeZ);
    
    if (maxSizeX < minX || maxSizeY < minY || maxSizeZ < minZ) {
        return;
    }
    
    glDispatchCompute(minX, minY, minZ);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    
}


void Renderer::RayTraceBoundingSphere(void) {
    if (context.computeTarget.id < 0) {
        context.computeTarget = utilHelper.RenderTarget();
    }
    glBindImageTexture(0, context.computeTarget.id,0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    
    
}


// Assumes whatever prior work that needs to be done is already done
void Renderer::FullScreenQuad(void) {
    f32 vertices[] = {
        -1.0f, -1.0f,
        3.0f, -1.0f,
        -1.0f, 3.0f
    };
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
}
