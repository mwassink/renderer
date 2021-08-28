struct GL {
    f32 vFOV, aspectRatio, znear, zfar;
    CoordinateSpace cameraSpace;
    u32 basicLightingShader;
    u32 texturedLightingShader;

    void initGL() {
        vFOV = 3.14f/6.0f;
        aspectRatio = 16.0f/9.0f;
        znear = 5.0f;
        zfar = 125.0f;
        basicLightingShader =  setShaders("../shaders/blinnPhongVertex.glsl", "../shaders/blinnPhongPixel.glsl");
        texturedLightingShader = setShaders("../shaders/basicTexturedVertex.glsl", "../shaders/basicTexturedPixel.glsl" );
        
        
    }
    
};
extern GL OpenGL;
Model addModel(const char* fileName, const char* textureName, int32 width, int32 height);
void activateModel(Model* model);
void shadeLightBasic(Model* model, Light* light);
void setDrawModel(Model* model);
void normalMap(f32* heightMap, Vector3* normalMap, int32 height, int32 width );
