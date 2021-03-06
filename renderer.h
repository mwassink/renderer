
#define BASIC 1
#define NORMALMAPPING 2
#define SHADOWS 4

struct SimpleScene;

struct RendererContext {
    f32 vFOV, aspectRatio, znear, zfar;
    CoordinateSpace cameraSpace;
    u32 basicLightingShader;
    u32 texturedLightingShader;
    u32 shadowMappingShader;
    u32 texturedShadowShader;
    u32 shadowMappingFramebuffer;
    u32 ssaoFramebuffer;
    u32 skyboxShader;
    u32 sphereShader;
    u32 quadShader;
    u32 ballTracerShader;
    u32 structureBufferShader;
    Texture computeTarget;
    HWND windowHandle;
    GLuint quadVAO;
    GLuint quadVBO;
    u32 texturedPointShadowShader;
    Texture ssaoStructureBuffer;
    
    RendererContext();
};


struct RendererUtil {
    RendererContext* context;
    
    void SetupBasicShader(Model* model, PointLight* light, GLuint shader);
    void AddTexturingToShader (Model* model, SpotLight* light, GLuint shader);
    void AddShadowsToShader(Model* model, SpotLight* light, GLuint shader);
    void AddShadowsToShader(Model* model, PointLight* light, GLuint shader);
    Model addModelNormalMap(const char* fileName, const char* textureName, const char* normalMap, bool f= true);
    Model addModel(const char* fileName, const char* textureName);
    void activateModel(Model* model);
    void attachDepthTextureFramebuffer(u32 depthTex, u32 depthFBO);
    void defaultTexParams(GLenum target);
    void createShadowMapTexture(SpotLight* light, u32 res);
    void addBasicTexturedVerticesToShader(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, u32 positionNorm, u32 positionUV, glTriangleNames* names );
    void addVerticesToShader(VertexLarge* vertices, u32* indices, int numVertices, int numIndices,
                             u32 positionCoord, u32 positionNorm, u32 positionTangent, u32 positionUV, u32 positionHandedness, glTriangleNames* names);
    int checkFailure(int shader, GLenum status);
    int checkFailureLink(int shader, GLenum status);
    int setShaders(const char* vertexFile, const char* fragmentFile);
    int CreateComputeShader(const char *f);
    Vector3* loadNormals(const char* fileName, u32* widthOut, u32* heightOut);
    void normalMap(f32* heightMap, Vector3* normalMap, int32 height, int32 width );
    void buildNormalMap(const char* hFile, const char* n);
    f32* convertBitmapHeightmap(const char* bitmapFile, u32* w, u32* h, f32 maxHeight);
    void addBasicVerticesShadowMapping(Vertex* vertices, u32* indices, int numVertices, int numIndices, u32 positionCoord, glTriangleNames* names);
    void addVerticesShadowMapping(VertexLarge* vertices, u32* indices, int numVertices, int numIndices,
                                  u32 positionCoord, glTriangleNames* names);
    int setupBitmapTexture(const char* textureString, u32* width, u32* height, u32* bitsPerPixel);
    void depthRenderCleanup(void);
    void addMeshTangents(Mesh* mesh);
    int InitializeCubeMaps(char* fileNames[6]);
    void ShadowTexParams(GLenum target);
    Texture RenderTarget();
};


struct Renderer {
    RendererContext context;
    RendererUtil utilHelper;
    Array<s32> handles;
    
    
    void setDrawModel(Model* model);
    void testViz(Model* model, CoordinateSpace* cs);
    void renderModel(Model* model, SpotLight* light);
    void renderModel(Model* model, PointLight* pointLight);
    
    void ShadowPass(Model* models, SpotLight* light, u32 numModels);
    Matrix4 invCubeFaceCamera(Matrix4& mCube, Matrix4& mFace);
    Array<Matrix4> cubeMapMatrices(CoordinateSpace& renderSpace);
    void renderModelsPointLight(Array<Model>* models, PointLight* light);
    Matrix4 shadowMapProj(f32 vFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane );
    void depthRender(Model* model, Matrix4& invCameraMatrix, int res, f32 n, f32 f);
    void envMapRender(Model* model, Matrix4& invCameraMatrix, int res, f32 n, f32 f);
    void CubeMapRender(Array<Model>* models, CoordinateSpace& renderCS, f32 n, f32 f, CubeArgs* renderArgs);
    Renderer();
    Skybox MakeSkybox(char* fileNames[6]);
    void RenderSkybox(Skybox& box);
    Sphere OKBoundingSphere(Vector3* verts, int nVerts);
    void AdjustBoundingSphere(Sphere *sp, Vector3* vertices, int nVerts);
    void FullScreenQuad(void);
    void DrawTexture(Texture* t);
    void RunComputeShader(int x, int y, int z);
    void RayTraceBoundingSphere(Sphere* s, Vector3* color);
    void DrawBoundingSphere(Model* model);
    Sphere GetBoundingSphere(Vector3* verts, int numVerts);
    Model CreateLightModel(SpotLight* s, f32 r);
    bool SphereFrustumCull(Model* m, CoordinateSpace* vm, f32 f, f32 n, f32 as);
    f32 farPlaneSpotLight(SpotLight* s);
    u32 ShaderFlags(Model* model, bool s);
    Vector2 GetABPointShadow(f32 f , f32 n);
    //(TODO) avoid unsafe type casts and code duplication with a little bit of inheritance
    void SSAOPass(Array<Model>* models, GLuint currentImage);
    void MakeDepthMap(Array<Model>* mls, PointLight* light);
    void RenderScene(SimpleScene* s);
    void renderModelsSpotLight(Array<Model>* models, SpotLight* light);
};



// Flat array of models and lights
struct SimpleScene {
    Array<Model> models;
    Array<SpotLight> spotLights;
    Array<PointLight> pointLights;
    Array<Skybox> skyboxes;
    RendererUtil* helperUtil; // be careful about these
    bool valid;

    SimpleScene() = default;
    SimpleScene(const char* file, Renderer* util, bool ser);
    void StripArrows(char* line);
    void ProcessLinePointLight(char* line);
    void ProcessLineSpotLight(char* line);
    void ProcessLineModel(char* line, RendererUtil* util, bool ser);
    void ProcessSkybox(char* line, Renderer* r);
    int Split(char* line, int start, char delim);

    
};