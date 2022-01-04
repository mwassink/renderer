#define CHECKGL(str) if (glGetError() != GL_NO_ERROR) {fatalError(str, "Error");}

Model MakeNormalPlane(RendererUtil* helper, Vector3 r, Vector3 s, Vector3 t, Vector3 o) {

    const char* m = "../tests/models/plane/plane.data";
    const char* texture = "../tests/models/plane/white.bmp";
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

void PointShadowDemo(Renderer* renderer, PointLight* s, Array<Model> *models ) {



    Model barrel1 = barrel();
    Model barrel2 = barrel();
    Model barrel3 = barrel();
    Model barrel4 = barrel();
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
    barrel4.modelSpace = objSpace;
    objSpace.origin = Vector3(0,0, -45);
    s->lightSpace = lookAtCoordSpace(barrel1.modelSpace.origin, objSpace.origin);
    Vector3 x = Vector3(1,0,0);
    Vector3 y = Vector3(0,1,0);
    Vector3 z = Vector3(0,0,1);
    Vector3 o = Vector3(0, 0, -55);
    Model wall1 = MakeNormalPlane(&renderer->utilHelper, x, y, z, o); // flat
    Model wall2 = wall1; // up
    Model wall3 = wall1; // up
    Model wall4 = wall1;
    Model wall5 = wall1;
    Matrix3 r1 = rotateX3(3.14 / 2);
    Matrix3 r2 = rotateX3(-1 * 3.14 / 2);
    Matrix3 r3 = rotateY3(3.14 / 2);
    Matrix3 r4 = rotateY3(-1 * 3.14/2);
    Vector3 o1 = Vector3(0, 0, -45);
    Vector3 o2 = Vector3(0, 0, -70);
    Vector3 o3 = Vector3(0, 0, -20);
    Vector3 o4 = Vector3(-15, 0, -45);
    Vector3 o5 = Vector3(15, 0, -45);
    wall2.modelSpace.rotate(r1);
    wall3.modelSpace.rotate(r2);
    wall4.modelSpace.rotate(r3);
    wall5.modelSpace.rotate(r4);
    wall1.modelSpace.origin = o1;
    wall2.modelSpace.origin = o2;
    wall3.modelSpace.origin = o3;
    wall4.modelSpace.origin = o4;
    wall5.modelSpace.origin = o5;
    models->push(barrel1);models->push(barrel2);
    models->push(barrel3);models->push(barrel4);
    models->push(wall1); models->push(wall2);
    models->push(wall3); models->push(wall4);
    models->push(wall5);    
}


void InitialPointDemoSetup(Renderer* r) {
    PointShadowDemo(r, &plDemoTest, &globalBarrels);


}

void TestPointShadow(Renderer* r) {

    r->renderPointShadow(&globalBarrels, &plDemoTest);
    
}
