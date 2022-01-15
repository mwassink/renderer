#define CHECKGL(str) if (glGetError() != GL_NO_ERROR) {fatalError(str, "Error");}

Model MakeNormalPlane(RendererUtil* helper, Vector3 r, Vector3 s, Vector3 t, Vector3 o,
    const char* textureFile) {

    const char* m = "../tests/models/plane/plane.data";
    const char* texture = textureFile;
    const char* n = "../tests/models/barrel/barrelnormals.bmp";
    Model p = helper->addModelNormalMap(m, texture, n);
    CoordinateSpace oSpace;
    oSpace.origin = o;
    oSpace.r = r;
    oSpace.s = s;
    oSpace.t = t;
    p.modelSpace = oSpace;
    CHECKGL("failure when making the normal plane");
    return p;
    
}





bool pointDemoRan = false;

Array<Model> globalBarrels;
PointLight plDemoTest;
CubeArgs args;
Model lightModel;

void PointShadowDemo(Renderer* renderer, PointLight* s, Array<Model> *models ) {



    Model barrel1 = barrel();
    Model barrel2 = barrel();
    Model barrel3 = barrel();
    Model carmodel = car(0, 0, -45);
    CoordinateSpace objSpace;
    objSpace.origin = Vector3(0, 0, -50);
    objSpace.r = Vector3(-1, 0, 0);
    objSpace.s = Vector3(0, -1, 0);
    objSpace.t = Vector3(0, 0, 1);
    barrel1.modelSpace = objSpace;
    objSpace.origin = Vector3(-5, 0, -45);
    barrel2.modelSpace = objSpace;
    objSpace.origin = Vector3(5, 0, -45);
    barrel3.modelSpace = objSpace;
    objSpace.origin = Vector3(0, 0, -40);
    carmodel.modelSpace = objSpace;
    objSpace.origin = Vector3(0,0, -45);
    s->lightSpace = lookAtCoordSpace(barrel1.modelSpace.origin, objSpace.origin);
    s->irradiance = 200.0f;
    Vector3 x = Vector3(1,0,0);
    Vector3 y = Vector3(0,1,0);
    Vector3 z = Vector3(0,0,1);
    Vector3 o = Vector3(0, 0, -55);
    #if 1
    Model wall1 = MakeNormalPlane(&renderer->utilHelper, x, y, z, o, "../tests/models/plane/white.bmp"); // flat
    Model wall2 =MakeNormalPlane(&renderer->utilHelper, x, y, z, o, "../tests/models/plane/purple.bmp");
    Model wall3 =MakeNormalPlane(&renderer->utilHelper, x, y, z, o, "../tests/models/plane/yellow.bmp");
    Model wall4 = MakeNormalPlane(&renderer->utilHelper, x, y, z, o, "../tests/models/plane/green.bmp");
    Model wall5 = MakeNormalPlane(&renderer->utilHelper, x, y, z, o, "../tests/models/plane/blue.bmp");
#endif
   
    Matrix3 r1 = rotateZ3(3.14 / 2); // good
    Matrix3 r2 = rotateZ3(-1 * 3.14 / 2); // good
    Matrix3 r3 = rotateY3(3.14 / 2); // good
    Matrix3 r4 = rotateY3(-1 * 3.14/2);
    Matrix3 rwhite = rotateX3(3.14/2); // good
    Vector3 o1 = Vector3(0, -15, -45);
    Vector3 o2 = Vector3(0, 0, -70);
    Vector3 o3 = Vector3(0, 0, -20);
    Vector3 o4 = Vector3(-15, 0, -45);
    Vector3 o5 = Vector3(15, 0, -45);
    wall1.modelSpace.rotate(rwhite);
    wall2.modelSpace.rotate(r1);
    wall3.modelSpace.rotate(r2);
    wall4.modelSpace.rotate(r3);
    wall5.modelSpace.rotate(r4);
    wall1.modelSpace.origin = o1;
    wall2.modelSpace.origin = o2;
    wall3.modelSpace.origin = o3;
    wall4.modelSpace.origin = o4;
    wall5.modelSpace.origin = o5;
    carmodel.modelSpace.rotate(r3);
    barrel2.modelSpace.rotate(r1);
    models->push(barrel1);models->push(barrel2);
    models->push(barrel3); models->push(carmodel);
    models->push(wall1);
    models->push(wall2);
    models->push(wall3);
    models->push(wall4);
    models->push(wall5);
    
}


void InitialPointDemoSetup(Renderer* r) {
    plDemoTest.color = Vector3(1, 1, 1);
    PointShadowDemo(r, &plDemoTest, &globalBarrels);
    lightModel = r->CreateLightModel((SpotLight*)&plDemoTest, 0.3f);
    r->context.cameraSpace.origin = plDemoTest.lightSpace.origin;

}

void TestPointShadow(Renderer* r) {

    r->MakeDepthMap(&globalBarrels, &plDemoTest);
    r->renderModelsPointLight(&globalBarrels, &plDemoTest);
    r->DrawBoundingSphere(&lightModel);
    
}
