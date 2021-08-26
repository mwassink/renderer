struct GL {
    f32 vFOV = 3.14/6.0f;
    f32 aspectRatio = 16.0f/9.0f;
    f32 znear = 5.0f;
    f32 zfar = 125.0f;
    CoordinateSpace cameraSpace;
    u32 basicLightingShader;
    u32 texturedLightingShader;
    
};
extern GL OpenGL;
Model addModel(const char* fileName, const char* textureName, int32 width, int32 height);
void activateModel(Model* model);
void shadeLightBasic(Model* model, Light* light, bool compileShader);
void setDrawModel(Model* model);
