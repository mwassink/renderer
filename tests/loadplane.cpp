RendererUtil planeUtil;
Model plane(void) {
    Model p = planeUtil.addModel("../tests/models/plane/plane.obj", 0);
    p.mesh.diffuseColor = Vector3(1.0f, 0.0f, 0.0f);
    CoordinateSpace c;
    c.origin = Vector3(0, -40, 0);
    c.r = Vector3(1, 0, 0);
    c.s = Vector3(0,1,0);
    c.t = Vector3(0,0,1);
    p.modelSpace = c;
    return p;
}
