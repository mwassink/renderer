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
float cubeVertices[] = {
           
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
    glGenFramebuffers(1, &shadowMappingFramebuffer);
        
}




void RendererUtil::SetupBasicShader(Model* model, PointLight* light, GLuint shader) {
    GLint mvLoc, mvpLoc, normMatrix, lightPos, diffCol, specCol, lightCol, lightPow, err;
    Matrix4 mv = modelView(model->modelSpace, context->cameraSpace);
    Matrix4 mvp = glModelViewProjection(model->modelSpace, context->cameraSpace, context->vFOV, context->aspectRatio, context->znear, context->zfar );
    Matrix3 normalMatrix = normalTransform(Matrix3x3(mv));
    Vector3 l = ( WorldObjectMatrix(context->cameraSpace)* Vector4(light->worldSpaceCoord, 1.0f)).v3();

    
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
    Matrix4 sMatrix = Matrix4(.5f, 0.0f, 0.0f, 0.5f,
                              0.0f, 0.5f, 0.0f, 0.5f,
                              0.0f, 0.0f, 0.5f, 0.5f,
                              0.0f, 0.0f, 0.0f, 1.0f) * glproj;
    uplumbMatrix4(shader, sMatrix, "shadowMatrix");
    glBindTextureUnit(2, light->depthTexture);
    
}

    
Model RendererUtil::addModelNormalMap(const char* fileName, const char* textureName, const char* normalMap ) {
    Model model;
    Texture tex(textureName);
    Texture norm(normalMap);
    model.mesh = loadMesh(fileName, tex);
    model.mesh.normalMap = norm;

    
    addMeshTangents(&model.mesh);
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

void RendererUtil::defaultShadowTexParams(GLenum target) {
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
    defaultShadowTexParams(GL_TEXTURE_2D);
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
    VertexLarge* normalVerts = (VertexLarge*)malloc(sizeof(VertexLarge)*mesh->numVertices);
    Vertex* verts = mesh->vertices;
    u32* indexList = mesh->triangles;
    Vector3* bitangents = (Vector3*)malloc(sizeof(Vector3)*mesh->numVertices);
    for (int i = 0; i < mesh->numVertices; ++i) {
        normalVerts[i].coord = verts[i].coord;
        normalVerts[i].normal  = verts[i].normal;
        normalVerts[i].tangent = Vector3(0.0f, 0.0f, 0.0f);
        normalVerts[i].uv = verts[i].uv;
        normalVerts[i].handedness = 1.0f;
        bitangents[i] = Vector3(0.0f, 0.0f, 0.0f);
    }
    for (int i = 0; i < mesh->numIndices/3; ++i) {
        u32 i0 = mesh->triangles[i*3], i1 = mesh->triangles[i*3+1], i2 = mesh->triangles[i*3+2];
        Vector3 p0 = verts[i0].coord.v3();
        Vector3 p1 = verts[i1].coord.v3();
        Vector3 p2 = verts[i2].coord.v3();

        UV uv0 = verts[i0].uv;
        UV uv1 = verts[i1].uv;
        UV uv2 = verts[i2].uv;

        Vector3 q1 = p1 - p0;
        Vector3 q2 = p2 - p0;
        f32 s1 = uv1.u - uv0.u, s2 = uv2.u - uv0.u;
        f32 t1 = uv1.v - uv0.v, t2 = uv2.v - uv0.v;

        f32 adjScale = 1/(s1*t2 - s2*t1);
        Matrix3 tsmat  = Matrix3(t2, -t1, 0, -s2, s1, 0, 0, 0,0);
        //tsmat:
        // t2 -t1 0
        // -s2 s1 0
        // 0   0  0
        Matrix3 qmat = Matrix3(q1, q2, Vector3(0,0,0));
        qmat = qmat.transpose();
        // <- q1 ->
        // <- q2 ->
        // 0  0  0 
        Matrix3 res = adjScale * tsmat * qmat;

        res = res.transpose();
        
        Vector3 t = res[0];
        Vector3 b = res[1];

        normalVerts[i0].tangent = normalVerts[i0].tangent + t;
        bitangents[i0] = bitangents[i0] + b;
        normalVerts[i1].tangent = normalVerts[i1].tangent + t;
        bitangents[i1] = bitangents[i1] + b;
        normalVerts[i2].tangent = normalVerts[i2].tangent + t;
        bitangents[i2] = bitangents[i2] + b;
    }
    for (int i = 0; i < mesh->numVertices; ++i) {
        Vector3 t = normalVerts[i].tangent;
        Vector3& n = normalVerts[i].normal;
        normalVerts[i].tangent = t - dot(normalVerts[i].normal, t)*normalVerts[i].normal;
        normalVerts[i].tangent.normalize();
        if (dot(cross(t, bitangents[i]), n) < 0.0f) {
            normalVerts[i].handedness = -1.0f;
        }
    }

    mesh->normalVertices = normalVerts;
    free(verts);
    free(bitangents);
    mesh->vertices = 0;
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
    // (TODO) can I reuse these buffers?

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
    // ORDER of cube map storage +X, -X, +Y, -Y, +Z, -Z
    // M faces
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
    if (light->cubeArgs.tex = -1) {
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

    //Matrix4 modelViewProjection = shadowMapProj(PI/4, 1.0f, n, f  ) * invCameraMatrix * ObjectWorldMatrix(model->modelSpace);
    Matrix4 modelViewProjection = glProjectionMatrix(PI / 4, 1.0f, n, f) * invCameraMatrix * ObjectWorldMatrix(model->modelSpace);
    GLint err = glGetError();
    GLint mvpLoc = glGetUniformLocation(context.shadowMappingShader, "modelViewProjection");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)modelViewProjection.data[0]);

    glBindBuffer(GL_ARRAY_BUFFER, model->identifiers.vbo);
    glBindVertexArray(model->identifiers.smVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->identifiers.ebo);
    glDrawElements(GL_TRIANGLES, model->mesh.numIndices, GL_UNSIGNED_INT, 0);
}


// (TODO) add this later
// For offline rendering
void Renderer::envMapRender(Model* model, Matrix4& invCameraMatrix, int res, f32 n, f32 f) {
    Matrix4 modelViewProjection = glProjectionMatrix(PI/4, 1.0f, n, f  ) * invCameraMatrix * ObjectWorldMatrix(model->modelSpace);
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
        utilHelper.defaultShadowTexParams(GL_TEXTURE_CUBE_MAP);
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
    uplumbMatrix4(context.skyboxShader, p * v, "viewProjection" );

    
    glDepthFunc(GL_LEQUAL);
    glBindBuffer(GL_ARRAY_BUFFER, box.vbo);
    glBindVertexArray(box.vao);
    glBindTextureUnit(0, box.texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    CHECKGL("Failure rendering skybox")
}

