void testRayTrace(Renderer* r) {
    Vector3 color = Vector3(1.0f, 0.0f, 0.0f);
    Sphere s;
    s.p = Vector3(0.0f, 0.0f, -20.0f);
    s.radius = 5.35f;
    r->RayTraceBoundingSphere(&s, &color);
    
}

void testRayTraceII(Renderer* r, Model* m) {
    r->DrawBoundingSphere(m);
}
