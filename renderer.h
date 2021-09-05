void uplumbMatrix4(u32 shader, Matrix4& m, const char* name);
void uplumbMatrix3(u32 s, Matrix3& m, const char* name);
void uplumbVector4(u32 s, Vector4& v, const char* name);
void uplumbVector3(u32 s, Vector3& v, const char* name);

Model addModel(const char* fileName, const char* textureName);
Model addModelNormalMap(const char* f, const char* t, const char* n);
void activateModel(Model* model);
void shaderLightBasic(Model* model, PointLight* light, GLuint tex);
void setDrawModel(Model* model);
void renderModel(Model* m, SpotLight* l);
void renderModel(Model* m, PointLight* l);
void createShadowMapTexture(SpotLight* light, u32 res);
void attachDepthTextureFramebuffer(u32 depthTex, u32 depthFBO);
void addShadowMapping(Model* models, SpotLight* light, u32 numModels);
void addMeshTangents(Mesh* mesh);
Vector3* loadNormals(const char* f, u32* w, u32* h);
void normalMap(f32* heightMap, Vector3* normalMap, int32 height, int32 width );
void buildNormalMap(const char* hFile, const char* n);
f32* convertBitmapHeightmap(const char* b, u32* w, u32* h, f32 m);
int checkFailure(int shader);
int checkFailureLink(int shader, GLenum status);
int setShaders(const char*, const char*);
void addBasicTexturedVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, u32 positionNorm, u32 positionUV, glTriangleNames* names );
void addVerticesToShader(VertexLarge* verts, u32* indices, int numVerts, int numIndices, u32 posCoord, u32 posNorm, u32 posTangent, u32 posUV, u32 posHandedness, glTriangleNames* ids);
void addBasicVerticesShadowMapping(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, glTriangleNames* names);
void addVerticesShadowMapping(VertexLarge* vertices, u32* indices, int numVertices, int numIndices,
                              u32 positionCoord, glTriangleNames* names);
int setupBitmapTexture(const char* textureString, u32* width, u32* height, u32* bitsPerPixel);
void CubeMapRender(Array<Model>* models, CoordinateSpace& cs, f32 near, f32 far, CubeArgs* renderArgs);
void defaultShadowTexParams(GLenum target);
Matrix4 invCubeFaceCamera(Matrix4& mCube, Matrix4& mFace);
void renderPointShadow(Array<Model>* m, PointLight* l);
struct GL {
    f32 vFOV, aspectRatio, znear, zfar;
    CoordinateSpace cameraSpace;
    u32 basicLightingShader;
    u32 texturedLightingShader;
    u32 shadowMappingShader;
    u32 texturedShadowShader;
    u32 shadowMappingFramebuffer;
    HWND windowHandle;
    void initGL() {
        vFOV = 3.14f/6.0f;
        aspectRatio = 16.0f/9.0f;
        znear = 1.0f;
        zfar = 125.0f;
        basicLightingShader =  setShaders("../shaders/blinnPhongVertex.glsl", "../shaders/blinnPhongPixel.glsl");
        texturedLightingShader = setShaders("../shaders/basicTexturedVertex.glsl", "../shaders/basicTexturedPixel.glsl" );
        
        texturedShadowShader = setShaders("../shaders/shadowedVertex.glsl","../shaders/shadowedPixel.glsl" );
        shadowMappingShader = setShaders("../shaders/vshadowMap.glsl", "../shaders/pshadowMap.glsl");
        glGenFramebuffers(1, &shadowMappingFramebuffer);
        
    }
    
};
extern GL OpenGL;

