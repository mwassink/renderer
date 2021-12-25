#include "test.h"
#include "../vecmath.h"
#include <stdio.h>



inline Matrix4 invTransform_( Matrix4& in) {
    Vector3 _0 = in.v3(0);
    Vector3 _1 = in.v3(1);
    Vector3 _2 = in.v3(2);
    Vector3 _3 = in.v3(3);

    Vector3 t1(in(0, 0), in(0, 1), in(0, 2));

    Vector3 s = cross(_0, _1), t = cross(_2, _3);
    f32 scale = 1.0f / dot(s, _2); //the bottom row is 0 0 0 1, so this is fine
    Vector3 v = _2 * scale;
    s *= scale;
    t *= scale;
    Vector3 r0 = cross(_1, v);
    Vector3 r1 = cross(v, _0);

    return Matrix4(r0.x, r0.y, r0.z, -dot(_1, t),
        r1.x, r1.y, r1.z, -dot(_0, t),
        s.x, s.y, s.z, -dot(_3, s),
        0.0f, 0.0f, 0.0f, 1.0f);

}


void testEmpty(void){
    BEGINTEST(empty);
    ENDTEST(empty);
}

void test4x4Comp(void) {
    BEGINTEST(4x4comp);
    Matrix4 lhs(1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4 rhs(1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);

    CHECK(lhs == rhs);
    CHECK(rhs == lhs);
    ENDTEST(4x4comp);
    
    
}

void test4x4Mult(void) {
    BEGINTEST(4x4mult);
    Matrix4 lhs2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 9.0f, 23.0f, 0.0f, 3.0f, 7.0f, 6.0f, 7.0f, 3.0f, 2.0f, 1.0f);
    Matrix4 rhs2(1.0f, 2.0f, 3.0f, 4.0f, 6.0f, 5.0f, 7.0f, 8.0f, 9.0f, 12.0f, 32.0f, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f);

    Matrix4 correct(40.0f, 48.0f, 121.0f, 24.0f, 122.0f, 148.0f, 391.0f, 91.0f, 81.0f, 99.0f, 257.0f, 30.0f, 43.0f, 53.0f, 108.0f, 53.0f);

    Matrix4 out = lhs2 * rhs2;
    CHECK(out == correct);

    ENDTEST(4x4mult);
}

void testm4v4Mult(void) {
    BEGINTEST(m4v4);
    Matrix4 m4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 9.0f, 23.0f, 0.0f, 3.0f, 7.0f, 6.0f, 7.0f, 3.0f, 2.0f, 1.0f);
    Vector4 v4(1.0f, 2.0f, 3.0f, 4.0f);

    Vector4 out = m4 * v4;
    Vector4 correct(30.0f, 136.0f, 51.0f, 23.0f);
    CHECK(out == correct);
    ENDTEST(m4v4);
}

void test3x3Mult(void) {
    BEGINTEST(3x3Mult);
    Matrix3 basic(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    Matrix3 basic2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 9.0f, 23.0f, 0.0f);
    Matrix3 correct(1.0f, 2.0f, 3.0f, 1.0f, 2.0f, 3.0f, 14.0f, 30.0f, 9.0f);
    
    CHECK((basic*basic2) == correct);
    ENDTEST(3x3Mult);
}


void testWorldCameraMatrix1(void) {
    BEGINTEST(ViewMatrix);
    CoordinateSpace cameraSpace;
    cameraSpace.origin = Vector3(3, 3, 3);
    cameraSpace.r = Vector3(0, 0, 1);
    cameraSpace.s = Vector3(0, 1, 0);
    cameraSpace.t = Vector3(-1, 0, 0);

    Vector4 correctCameraSpacePoint(1, 1, 1, 1);

    Matrix4 V = WorldObjectMatrix(cameraSpace);
    Vector4 worldPoint(2, 4, 4, 1);

    Vector4 camPoint = V * worldPoint;
    CHECK(camPoint == correctCameraSpacePoint);
    Vector4 inverted = ObjectWorldMatrix(cameraSpace) * camPoint;
    CHECK(inverted == worldPoint);
    
    ENDTEST(ViewMatrix);
    
}

void testWorldCameraMatrix2(void) {
    BEGINTEST(ViewMatrix2);
    CoordinateSpace cameraSpace;
    cameraSpace.origin = Vector3(-3, -3, -3);
    cameraSpace.r = Vector3(1, 0, 0);
    cameraSpace.s = Vector3(0, 0, 1);
    cameraSpace.t = Vector3(0, -1, 0);

    Vector4 correctCameraSpacePoint(-1.0f, -1.0f, -1.0f, 1.0f);
    Vector4 worldPoint(-4.0f, -2.0f, -4.0f, 1.0f);
    Matrix4 V = WorldObjectMatrix(cameraSpace);
    
    Vector4 camPoint = V * worldPoint;

    CHECK(camPoint == correctCameraSpacePoint);
    Vector4 inverted = ObjectWorldMatrix(cameraSpace) * camPoint;
    CHECK(inverted == worldPoint);
    ENDTEST(ViewMatrix2);
}
// Say we have a camera that is completely turned around
// We pass in positive values for the proj, even though negative Ze values are visible
// because this is a rational function we cram a bunch of values in near the end
// example n = .5... range .5 to 1 will get like half of the values
void testGLProjMatrix(void) {
    BEGINTEST(glProj);
    CoordinateSpace cameraSpace;
    cameraSpace.origin = Vector3(0, 0, 0);
    cameraSpace.r = Vector3(1, 0, 0);
    cameraSpace.s = Vector3(0, -1, 0);
    cameraSpace.t = Vector3(0, 0, -1);

    CoordinateSpace objSpace;
    objSpace.origin = Vector3(0, 0, 2);
    objSpace.r = Vector3(1, 0, 0);
    objSpace.s = Vector3(0, 1, 0);
    objSpace.t = Vector3(0, 0, 1);
    f32 zfar = 8.0f;
    f32 znear = 2.0f;
    Matrix4 mvp = glModelViewProjection(objSpace, cameraSpace, 3.14f/6.0f, 16.0f/9.0f,znear, zfar );
    Matrix4 m = ObjectWorldMatrix(objSpace);
    Matrix4 v = WorldObjectMatrix(cameraSpace);
    Matrix4 mv = v * m;
    
    Vector4 objSpacePoint(0.0f, 0.0f, 1.0f, 1.0f);
    Vector4 cameraSpaceCoord = mv * objSpacePoint;
    Vector4 clipTest = glProjectionMatrix(3.14f / 6.0f, 16.0f / 9.0f, .5f, 8.0f) * cameraSpaceCoord;
    Vector4 clipSpacePoint = mvp * objSpacePoint;
    
    CHECK(clipSpacePoint.z < clipSpacePoint.w);
    




    ENDTEST(glProj);
    
}
void testadj(void) {
    BEGINTEST(adj);
    Matrix3 mat(1,2,3,0,1,4,5,6,0);
    mat = adjoint3x3(mat);
    Matrix3 correct(-24, 18, 5, 20, -15, -4, -5, 4,1);
    CHECK(mat == correct);

    ENDTEST(adj);
}


void testInvTransform(void) {
    BEGINTEST(inv);
    Matrix4 test(-1, 2, 7, 4, -1, 4, 3, 4, -1, 9, 3, 4, 0, 0, 0, 1);
    Matrix4 out = invTransform(test);
    
    for (int i = 0; i < 4; ++i) {
        f32 a = out(i, 0), b = out(i, 1), c = out(i,2), d = out(i,3);
        printf("%f %f %f %f\n", a, b, c, d);
    }
    



    ENDTEST(inv);
}



void testPlaneConstruction(void) {
    BEGINTEST(plane);
    Vector3 origin = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 ul = Vector3(-10, 10, 20);
    Vector3 ll = Vector3(-10, -10, 20);
    Plane p = Plane(origin, ul, ll);
    f32 d1 = dot(origin, p);
    f32 d2 = dot(ul, p);
    f32 d3 = dot(ll, p );
    CHECK(fabsf(d1) < .01);
    CHECK(fabsf(d2) < .01);
    CHECK(fabsf(d3) < .01);
    ENDTEST(plane);
}




int main(int argCount, char** argv) {
    testEmpty();
    test4x4Mult();
    test4x4Comp();
    testm4v4Mult();
    test3x3Mult();
    testWorldCameraMatrix1();
    testWorldCameraMatrix2();
    testGLProjMatrix();
    testadj();
    testInvTransform();
    testPlaneConstruction();
    print_report(0, 0);
}
