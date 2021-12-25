
RendererUtil *testUtil = 0;

#define BINARY 1
void buildNormalsTest(void) {
    
    
    u32 w, h;
    f32* heightmap = testUtil->convertBitmapHeightmap("../tests/models/barrel/barrelheights.bmp", &w, &h, 20.0f);
    Vector3* normalmap = (Vector3*)malloc(sizeof(Vector3)*w*h);
    testUtil->normalMap(heightmap, normalmap, h, w);
    writeNormalMapBitmap(w, h, normalmap, "../tests/models/barrel/barrelnormals.bmp" );
}


// This test will test the normal mapping capabilities of the renderer
// Subsequent calls can be used to just render this
Model testAddModel(void) {
#if 0
    buildNormalsTest();
#else
    
#if BINARY
    Model normalModel = testUtil->addModelNormalMap("../tests/models/barrel/barrel.data",
                                                    "../tests/models/barrel/barrel.bmp",
                                                    "../tests/models/barrel/barrelnormals.bmp");
#else
    Model normalModel = testUtil->addModelNormalMap("../tests/models/barrel/barrel.obj",
                                                    "../tests/models/barrel/barrel.bmp",
                                                    "../tests/models/barrel/barrelnormals.bmp");
    
#endif
    PointLight testlight;
    CoordinateSpace objSpace;
    objSpace.origin = Vector3(0, 0, -60);
    objSpace.r = Vector3(-1, 0, 0);
    objSpace.s = Vector3(0, -1, 0);
    objSpace.t = Vector3(0, 0, 1);
    normalModel.modelSpace = objSpace;
    normalModel.mesh.diffuseColor = Vector3(0.6f, 0.6f, 0.6f);
    normalModel.mesh.specColor = Vector3(1.0f, 1.0f, 1.0f);
    
    
    testlight.worldSpaceCoord = Vector3(0.0f, 0.0f, -70.0f);
    testlight.color = Vector3(1.0f, 1.0f, 1.0f);
    testlight.irradiance = 50.0f;
    
    return normalModel;
    
#endif
}

Model car(f32 x, f32 y, f32 z) {
#if BINARY
    const char* m = "../tests/models/car/car_obj.data";
#else
    const char* m = "../tests/models/car/car_obj.obj";
#endif
    const char* t = "../tests/models/car/textures/colors.bmp";
    const char* n = "../tests/models/car/textures/normals.bmp";
    
    Model car = testUtil->addModelNormalMap(m, t, n);
    //Model car = addModel(m, t);
    
    CoordinateSpace objSpace;
    objSpace.origin = Vector3(x, y, z);
    objSpace.r = Vector3(1, 0, 0);
    objSpace.s = Vector3(0, 1, 0);
    objSpace.t = Vector3(0, 0, 1);
    car.modelSpace = objSpace;
    car.mesh.diffuseColor = Vector3(0.6f, 0.6f, 0.6f);
    car.mesh.specColor = Vector3(1.0f, 1.0f, 1.0f);
    
    return car;
}


Model barrel(void) {
#if BINARY
    const char* m = "../tests/models/barrel/barrel.data";
#else 
    const char* m = "../tests/models/barrel/barrel.obj";
#endif
    const char* t = "../tests/models/barrel/barrel.bmp";
    const char* n = "../tests/models/barrel/bnormals4.bmp";
    const char* h = "../tests/models/barrel/heightmap4.bmp";
    
    testUtil->buildNormalMap(h , n );
    
    Model barrel = testUtil->addModelNormalMap(m, t, n);
    
    CoordinateSpace objSpace;
    objSpace.origin = Vector3(0, -1.5f, -60);
    objSpace.r = Vector3(-1, 0, 0);
    objSpace.s = Vector3(0, -1, 0);
    objSpace.t = Vector3(0, 0, 1);
    barrel.modelSpace = objSpace;
    barrel.mesh.diffuseColor = Vector3(0.6f, 0.6f, 0.6f);
    barrel.mesh.specColor = Vector3(1.0f, 1.0f, 1.0f);
    
    PointLight testlight;
    
    testlight.worldSpaceCoord = Vector3(0.0f, 5.0f, -60.0f);
    testlight.r  = Vector3(1, 0, 0);
    testlight.s = Vector3(0,0,-1);
    testlight.t = Vector3(0,1,0);
    testlight.color = Vector3(1.0f, 1.0f, 1.0f);
    testlight.irradiance = 50.0f;
    
    return barrel;
    
}

PointLight pl(void) {
    
    PointLight testlight;
    testlight.worldSpaceCoord = Vector3(0.0f, 5.0f, -60.0f);
    testlight.r = Vector3(1, 0, 0);
    testlight.s = Vector3(0, 0, -1);
    testlight.t = Vector3(0, 1, 0);
    testlight.color = Vector3(1.0f, 1.0f, 1.0f);
    testlight.irradiance = 50.0f;
    return testlight;
}

Model barrel(CoordinateSpace* c) {
    
#if BINARY
    const char* m = "../tests/models/barrel/barrel.data";
#else
    const char* m = "../tests/models/barrel/barrel.obj";
#endif
    const char* t = "../tests/models/barrel/barrel.bmp";
    const char* n = "../tests/models/barrel/bnormals4.bmp";
    const char* h = "../tests/models/barrel/heightmap4.bmp";
    
    testUtil->buildNormalMap(h , n );
    PointLight testlight;
    Model barrel = testUtil->addModelNormalMap(m, t, n);
    barrel.modelSpace = *c;
    barrel.mesh.diffuseColor = Vector3(0.6f, 0.6f, 0.6f);
    barrel.mesh.specColor = Vector3(1.0f, 1.0f, 1.0f);
    
    
    testlight.worldSpaceCoord = Vector3(0.0f, 5.0f, -60.0f);
    testlight.r  = Vector3(1, 0, 0);
    testlight.s = Vector3(0,0,-1);
    testlight.t = Vector3(0,1,0);
    testlight.color = Vector3(1.0f, 1.0f, 1.0f);
    testlight.irradiance = 100.0f;
    
    return barrel;
}



void populateModels(Array<Model>* models) {
    CoordinateSpace objSpace;
    objSpace.origin = Vector3(0, -10.5f, -60);
    objSpace.r = Vector3(-1, 0, 0);
    objSpace.s = Vector3(0, -1, 0);
    objSpace.t = Vector3(0, 0, 1);
    models->push(barrel(&objSpace));
    objSpace.origin = Vector3(0, -1.5f, -70);
    models->push(barrel(&objSpace));
    objSpace.origin = Vector3(0, -1.5f, -60);
    models->push(barrel(&objSpace));
    objSpace.origin = Vector3(40, -1.5f, -60);
    models->push(barrel(&objSpace));
    objSpace.origin = Vector3(-40, -1.5f, -60);
    models->push(barrel(&objSpace));
}
