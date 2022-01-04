/* 8/21/2021 */
#include "types.h"
#include "glwrangler.h"
#include "utilities.h"
#include "renderer.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#endif

#define CHECKGL(str) if (glGetError() != GL_NO_ERROR) {fatalError(str, "Error");}
#define MINLIGHT .01f
#define TEXTURE_SIZE 512
#define ASPECT_RATIO 16/9
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
    texturedPointShadowShader = tmp.setShaders("../shaders/shadowedVertex.glsl", "../shaders/pointPixel.glsl");
    CHECKGL("Point shadow shader setup FAILED");
    shadowMappingShader = tmp.setShaders("../shaders/vshadowMap.glsl", "../shaders/pshadowMap.glsl");
    skyboxShader = tmp.setShaders("../shaders/vskybox.glsl", "../shaders/pskybox.glsl");
    quadShader = tmp.setShaders("../shaders/vquad.glsl", "../shaders/pquad.glsl");
    sphereShader = tmp.CreateComputeShader("../shaders/sphere.glsl");
    
    f32 vertices[] = {
        -1.0f, -1.0f,
        3.0f, -1.0f,
        -1.0f, 3.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    
    computeTarget = Texture();
    glGenFramebuffers(1, &shadowMappingFramebuffer);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    CHECKGL("Error before adding shadows");
    
    Matrix4 glproj = glModelViewProjection(model->modelSpace, light->lightSpace, PI/4.0f, 1, 1.0f, 40.0f);
    Matrix4 l = Matrix4(.5f, 0.0f, 0.0f, 0.5f,
                        0.0f, 0.5f, 0.0f, 0.5f,
                        0.0f, 0.0f, 0.5f, 0.5f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    Matrix4 sMatrix = l  * glproj;
    uplumbMatrix4(shader, sMatrix, "shadowMatrix");
    Matrix4 m = ObjectWorldMatrix(model->modelSpace);
    Matrix4 lv = WorldObjectMatrix(light->lightSpace);
    Matrix4 lightSpaceMatrix = lv * m;
    uplumbMatrix4(shader, lightSpaceMatrix, "modelLightMatrix");
    glBindTextureUnit(2, light->depthTexture);
    
}

void RendererUtil::AddShadowsToShader(Model* model, PointLight* light, GLuint shader) {
    Matrix4 proj = glModelViewProjection(model->modelSpace, light->lightSpace, PI/4.0f, 1, 1.0f, 40.0f);
    uplumbMatrix4(shader, proj, "shadowMatrix");
    Matrix4 m = ObjectWorldMatrix(model->modelSpace);
    Matrix4 lightView = WorldObjectMatrix(light->lightSpace);
    Matrix4 toLightSpace = lightView * m;
    uplumbMatrix4(shader, toLightSpace, "modelLightMatrix");
    CHECKGL("plumbing error");
    glBindTextureUnit(2, light->cubeArgs.tex);
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
    
    glGenVertexArrays(1, &names->vao);
    glBindVertexArray(names->vao);

    glGenBuffers(1, &names->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &names->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices* sizeof(u32), indices
                 , GL_STATIC_DRAW);
    
    glVertexAttribPointer(positionCoord, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, coord)));
    glVertexAttribPointer(positionNorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(positionUV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, uv)));
    
    glEnableVertexAttribArray(positionCoord);
    glEnableVertexAttribArray(positionNorm); 
    glEnableVertexAttribArray(positionUV);
    
}


void RendererUtil::addVerticesToShader(VertexLarge* vertices, u32* indices, int numVertices, int numIndices,
                                       u32 positionCoord, u32 positionNorm, u32 positionTangent, u32 positionUV, u32 positionHandedness, glTriangleNames* names) {
 
    glGenVertexArrays(1, &names->vao);
    glBindVertexArray(names->vao);
 
    glGenBuffers(1, &names->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexLarge)*numVertices, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &names->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(u32), indices, GL_STATIC_DRAW);
    
    
    
    
    
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

int RendererUtil::CreateComputeShader(const char* computeFile) {
    if (!computeFile) {
        return -1;
    }
    int size;
    const char* src = readFile(computeFile, &size);
    GLuint c = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(c, 1, &src, NULL);
    glCompileShader(c);
    if (checkFailure(c, GL_COMPILE_STATUS)) {
        free((void*)src);
        return -1;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, c);
    glLinkProgram(program);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names->ebo);
    glVertexAttribPointer(positionCoord, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex, coord)) );
    glEnableVertexAttribArray(positionCoord);
    
}

void RendererUtil::addVerticesShadowMapping(VertexLarge* vertices, u32* indices, int numVertices, int numIndices,
                                            u32 positionCoord, glTriangleNames* names) {
    glGenVertexArrays(1, &names->smVao );
    glBindVertexArray(names->smVao);
    
    glBindBuffer(GL_ARRAY_BUFFER, names->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names->ebo);
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
    glBindVertexArray(model->identifiers.vao);
    glDrawElements(GL_TRIANGLES, model->mesh.numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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

u32 Renderer::ShaderFlags(Model* model, bool shadows ) {
    u32 flags = 0;
    flags = model->mesh.normalVertices ? NORMALMAPPING : BASIC;
    flags = shadows ? (flags | SHADOWS) : (flags);

    return flags;
}

// Shadow maps need to be set up BEFORE this is called
void Renderer::renderModel(Model* model, SpotLight* light) {

    // (TODO) use bounding sphere instead of origin
    Vector3 v = light->lightSpace.origin - model->modelSpace.origin;
    if (dot(v, v) > (100 * light->irradiance)) {
        return;
    }

    u32 flags = ShaderFlags(model, light->shadows != NULL);
    u32 shader;
    switch (flags) {
    case BASIC: {
        shader = context.basicLightingShader;
        utilHelper.SetupBasicShader(model, (PointLight*)light, shader);

    } break;
    case NORMALMAPPING: {
        shader = context.texturedLightingShader;
        utilHelper.SetupBasicShader(model, (PointLight*)light, shader);
        utilHelper.AddTexturingToShader(model, light, shader);
    } break;
    case NORMALMAPPING | SHADOWS : {
        shader = context.texturedShadowShader;
        utilHelper.SetupBasicShader(model, (PointLight*)light, shader);
        utilHelper.AddTexturingToShader(model, light, shader);
        utilHelper.AddShadowsToShader(model, light, shader);
    } break;
    }
    
    setDrawModel(model);    
    
}

void Renderer::renderModel(Model* model, PointLight* light) {

    CHECKGL("error before drawing cube map");
    GLint err;
    Vector3 l = light->lightSpace.origin - model->modelSpace.origin;
    if (dot(l, l) > (100 * light->irradiance)) {
        return;
    }
    u32 flags = ShaderFlags(model, light->shadows != NULL);
    u32 shader;
    switch (flags) {
    case BASIC: {
        shader = context.basicLightingShader;
        utilHelper.SetupBasicShader(model, light, shader);

    } break;
    case NORMALMAPPING: {
        shader = context.texturedLightingShader;
        utilHelper.SetupBasicShader(model, light, shader);
        utilHelper.AddTexturingToShader(model, (SpotLight* ) light, shader);
    } break;
    case NORMALMAPPING | SHADOWS : {
        shader = context.texturedPointShadowShader;
        utilHelper.SetupBasicShader(model, light, shader);
        utilHelper.AddTexturingToShader(model, (SpotLight*) light, shader);
        utilHelper.AddShadowsToShader(model, light, shader);
        err = glGetError();

    } break;
    }
    CHECKGL("error when drawing cube map");
    setDrawModel(model);
}



// (TODO)allow for config of near, far plane
// This will give the shadows in the texture... we still need to add them to the rendering pipeline
void Renderer::ShadowPass(Model* models, SpotLight* light, u32 numModels) {
#define RES 500
    
    CHECKGL("Error w/ shadow pass");
    
    GLint err;
    RECT rect;
    GetWindowRect(context.windowHandle, &rect);
    glUseProgram(context.shadowMappingShader);
    
    if (light->depthTexture < 0) {
        utilHelper.createShadowMapTexture(light, RES);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, context.shadowMappingFramebuffer);
    
    
    utilHelper.attachDepthTextureFramebuffer(light->depthTexture, context.shadowMappingFramebuffer);
    glViewport(0, 0, RES, RES);
    glClearDepth(1.0f);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);
 
    
    
    for (int i = 0; i < numModels; ++i) {

        if (SphereFrustumCull(&models[i], &light->lightSpace, 40.0f, 1.0f, 1.0f)) {
            continue;
        }

        
        Matrix4 modelLightProjection = glModelViewProjection(models[i].modelSpace, light->lightSpace, PI/4.0f,
                                                             1, 1.0f, 40.0f);
        
        GLint mvpLoc = glGetUniformLocation(context.shadowMappingShader, "modelViewProjection");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&modelLightProjection.data[0]);
        
        
        glBindVertexArray(models[i].identifiers.smVao);
        
        glDrawElements(GL_TRIANGLES, models[i].mesh.numIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
    glClear(GL_DEPTH_BUFFER_BIT);

    
#undef RES
}


// Returns the inverse cube map matrices for rendering
Array<CoordinateSpace> Renderer::cubeMapCS(CoordinateSpace& renderSpace) {
    Array<CoordinateSpace> renderBases(6);

    Vector3 posX = renderSpace.origin + renderSpace.r;
    Vector3 negX = renderSpace.origin - renderSpace.r;
    Vector3 posY = renderSpace.origin + renderSpace.s;
    Vector3 negY = renderSpace.origin - renderSpace.s;
    Vector3 posZ = renderSpace.origin + renderSpace.t;
    Vector3 negZ = renderSpace.origin - renderSpace.t;


    renderBases[0] = lookAtCoordSpace(posX, renderSpace.origin);
    renderBases[1] = lookAtCoordSpace(negX, renderSpace.origin);
    renderBases[2] = lookAtCoordSpace(posY, renderSpace.origin);
    renderBases[3] = lookAtCoordSpace(negY, renderSpace.origin);
    renderBases[4] = lookAtCoordSpace(posZ, renderSpace.origin);
    renderBases[5] = lookAtCoordSpace(negZ, renderSpace.origin);

    
    return renderBases;
}

// This will make it so that the light now has a texture attached
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
    for (int i = 0; i < models->sz; ++i) {
        renderModel(&(*models)[i], light);
    }
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
    
    glBindVertexArray(model->identifiers.smVao);
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
    
    GetWindowRect(context.windowHandle, &rect);
    Array<CoordinateSpace> coordinateSpaces =  cubeMapCS(renderCS);
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
        for (int k = 0; k < models->sz; ++k) {
            // TODO: can quickly cull most of the models. Just skip them with bounding boxes if possible
            if (renderArgs->shader == context.shadowMappingShader) {
                Matrix4 invCamera  = WorldObjectMatrix(coordinateSpaces[i]);
                // Directions need to be consistent when we sample these.
                // Directions are consistent because they use the same directions (the lights for shadows)
                // object's for env mapping
                depthRender(&(*models)[k], invCamera, renderArgs->res, 1.0f, 20.0f);
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

    coordinateSpaces.release();
    
}


Renderer::Renderer() {
#define DEPTHTEXRES 500
    context = RendererContext();
    utilHelper.context = &context;
    
}



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
    glBindVertexArray(box.vao);
    glBindTextureUnit(0, box.texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    CHECKGL("Failure rendering skybox")
    glBindVertexArray(0);
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
        f32 dMax =  -10000000.0f;
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
    CHECKGL("ERROR w/ FULL SCREEN QUAD")
    FullScreenQuad();
}

Texture RendererUtil::RenderTarget(void) {
    GLuint texID;
    Texture texture;
    RECT rect;
    GetWindowRect(context->windowHandle, &rect);
    //int w = rect.right - rect.left, h = rect.bottom - rect.top;
    int w = TEXTURE_SIZE * ASPECT_RATIO, h = TEXTURE_SIZE;
    glCreateTextures(GL_TEXTURE_2D, 1, &texID);
    texture.id = (int)texID;
    glBindTexture(GL_TEXTURE_2D, texture.id);
    defaultTexParams(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT,
                 NULL);
    
    texture.width = w;
    texture.height = h;
    return texture;
}

// Assumes the compute shader is bound before this
void Renderer::RunComputeShader(int minX, int minY, int minZ) {
    
    int maxGroupX, maxGroupY, maxGroupZ;
    int maxSizeX, maxSizeY, maxSizeZ;
    int groupInvocations;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxGroupX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxGroupY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxGroupZ);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxSizeX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxSizeY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxSizeZ);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &groupInvocations);
    if (maxSizeX < minX || maxSizeY < minY || maxSizeZ < minZ) {
        return;
    }
#if 0
    // (TODO) change this to be LOCAL work groups
    if (minX * minY > groupInvocations) {
        return;
    }
#endif
    
    glDispatchCompute(minX, minY, minZ);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    
}


void Renderer::RayTraceBoundingSphere(Sphere* s, Vector3* color) {
    
    if (context.computeTarget.id < 0) {
        context.computeTarget = utilHelper.RenderTarget();
    }
    glBindImageTexture(0, context.computeTarget.id,0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUseProgram(context.sphereShader);
    Vector3& center = s->p;
    uplumbVector3(context.sphereShader, center, "center");
    uplumbVector3(context.sphereShader, *color, "colorSphere");
    uplumbf(context.sphereShader, s->radius, "radius");
    RECT rect;
    GetWindowRect(context.windowHandle, &rect);
    //int w = rect.right - rect.left, h = rect.bottom - rect.top;
    int w = TEXTURE_SIZE * ASPECT_RATIO , h = TEXTURE_SIZE;
    RunComputeShader(w, h, 1);
    CHECKGL("ERROR w/ running compute shader")
    DrawTexture(&context.computeTarget);
    
    
    
    
}


// Assumes whatever prior work that needs to be done is already done
void Renderer::FullScreenQuad(void) {

    
    glBindVertexArray(context.quadVAO);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDepthFunc(GL_LESS);
    
    
}

Sphere Renderer::GetBoundingSphere(Vector3* verts, int numVerts) {
    Sphere s = OKBoundingSphere(verts, numVerts);
    AdjustBoundingSphere(&s, verts, numVerts);
    return s;
}

// Need to call free on these
Vector3* GetVertices(Model* model) {
    
    Vector3* vertices = (Vector3*) malloc(model->mesh.numVertices * sizeof (Vector3));
    if (model->mesh.normalVertices) {
        for (int i = 0; i < model->mesh.numVertices; i++) {
            vertices[i] = model->mesh.normalVertices[i].coord.v3();
        }
    } else {
        for (int i = 0; i < model->mesh.numVertices; i++) {
            vertices[i] = model->mesh.vertices[i].coord.v3();
        }
    }
    return vertices;
}

void Renderer::DrawBoundingSphere(Model* model) {
    if (model->mesh.boundingSphere.radius < 0) {
        Vector3* verts = GetVertices(model);
        model->mesh.boundingSphere = GetBoundingSphere(verts, model->mesh.numVertices);
        free(verts);
    }
    
    Sphere s = model->mesh.boundingSphere;
    Matrix4 toWorldSpace = ObjectWorldMatrix(model->modelSpace);
    Matrix4 toCameraSpace= WorldObjectMatrix(context.cameraSpace);
    Vector4 spw = Vector4(s.p, 1.0f);
    spw = toWorldSpace * spw;
    spw = toCameraSpace * spw;
    s.p = spw.v3();
    bool cullable = SphereFrustumCull(model, &context.cameraSpace, context.zfar, context.znear,
        context.aspectRatio);
    Vector3 color = Vector3(0.0f, 1.0f, 0.0f );
    if (cullable) {
        color = Vector3(1.0f, 0.0f, 0.0f );
    }
    RayTraceBoundingSphere(&s, &color);
    
}

Model Renderer::CreateLightModel(SpotLight* s, f32 radius = 0.3f) {
    Model lightModel;
                
    Sphere lightCapsule;
    lightCapsule.p = Vector3(0, 0,0);
    lightCapsule.radius = radius;
    lightModel.mesh.boundingSphere = lightCapsule;
    lightModel.modelSpace = s->lightSpace;
    return lightModel;
}

// Returns whether the model can be culled based on its bounding sphere
bool Renderer::SphereFrustumCull(Model* model, CoordinateSpace* viewMatrix, f32 f, f32 n,
    f32 aspectRatio ) {
    // (TODO) do this when the model is created
    if (model->mesh.boundingSphere.radius < 0) {
        Vector3* verts = GetVertices(model);
        model->mesh.boundingSphere = GetBoundingSphere(verts, model->mesh.numVertices);
        free(verts);
    }
    Sphere s = model->mesh.boundingSphere;
    Matrix4 toWorldSpace = ObjectWorldMatrix(model->modelSpace);
    Matrix4 toCameraSpace= WorldObjectMatrix(*viewMatrix);
    Vector4 spw = Vector4(s.p, 1.0f);
    spw = toWorldSpace * spw;
    spw = toCameraSpace * spw;
    s.p = spw.v3();

    Plane farPlane = Plane(0.0f, 0.0f, 1.0f, f);
    Plane nearPlane = Plane(0.0f, 0.0f, -1.0f, n  );
    f32 yMax = tanf(context.vFOV/2) * f;
    f32 xMax = yMax*aspectRatio;
    Vector3 eye = Vector3(0, 0, 0);
    Vector3 ll = Vector3(-xMax, -yMax, -context.zfar);
    Vector3 ul = Vector3(-xMax, yMax, -context.zfar);
    Vector3 lr = Vector3(xMax, -yMax, -context.zfar);
    Vector3 ur = Vector3(xMax, yMax, -context.zfar);
    Plane left = Plane(eye, ll, ul);
    Plane right = Plane(eye, lr, ur);
    Vector3 leftNormal = left.GetNormal();
    Vector3 rightNormal = right.GetNormal();
    Vector3 x = Vector3(1,0,0);

    if (dot(leftNormal, x ) < 0) {
        // wrong way. This is supposed to point to the right
        left = Plane(leftNormal*-1, left.d*-1);
    }
    if (dot(rightNormal, x) > 0) {
        right = Plane(rightNormal*-1, right.d*-1);
    }
    f32 dots[4];
    dots[0]= dot(s.p, farPlane);
    dots[1] = dot(s.p, nearPlane);
    dots[2] = dot(s.p, left);
    dots[3] = dot(s.p, right);
    for (int i = 0; i < 4; i++) {
        if (dots[i] < -s.radius ) {
            return true;
        }
    }
    return false;
}

f32 Renderer::farPlaneSpotLight(SpotLight* s) {
    f32 dSquared = s->irradiance / MINLIGHT;
    return sqrt(dSquared);
}

