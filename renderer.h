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
Model addModel(const char* fileName, const char* textureName);
Model addModelNormalMap(const char* f, const char* t, const char* n);
void activateModel(Model* model);
void shadeLightBasic(Model* model, Light* light, GLuint tex);
void setDrawModel(Model* model);
void renderModel(Model* m, Light* l);
void createShadowMapTexture(Light* light, u32 res);
void attachDepthTextureFramebuffer(u32 depthTex, u32 depthFBO);
void addShadowMapping(Model* models, Light* light, u32 numModels);
