#ifndef VMATH_H
#define VMATH_H
typedef float f32;
#define FLOAT_MIN 0.0001
#define ASSERT(expr) if (!(expr)) { *(volatile int*)0 = 0;}

#include <math.h>
#include <intrin.h>
#include <string.h>




struct Vector3 {
    union {
        struct {
            f32 x,y,z;
        };
        f32 arr[3];
        f32 data[3];
    };
    Vector3() {}
    Vector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z){}
    f32& operator[](int index) {
        return data[index];
    }
    Vector3& operator*= (const Vector3& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return (*this);
    }
    Vector3& operator*= (const f32 scale) {
        x *= scale;
        y *= scale;
        z *= scale;
        return (*this);
    }

    void normalize(){
        f32 mag = sqrt(z*z + y*y + x*x);
        x /= mag; y /= mag; z /= mag;
    }
};

struct Vector4 {
    union {
        struct {
            f32 x,y,z,w;
        };
        f32 data[4];
        f32 arr[4];
    };
    Vector4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}
    Vector4(f32 f) {
        arr[0] = f; arr[1] = f; arr[2] = f; arr[3] = f;
    }
    Vector4(const Vector3& in, f32 h) {
        x = in.x, y = in.y, z = in.z, w = h;
    }

    Vector4() {}
    f32& operator[](int index) {
        return data[index];
    }
    Vector4& operator*= (const Vector4& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return (*this);
    }
    Vector4& operator*= (const f32 scale) {
        x *= scale;
        y *= scale;
        z *= scale;
        w *= scale;
        return (*this);
    }
    
    
};

struct Matrix3 {
    union {
        f32 data[3][3];
        struct {
            f32 _00,  _01,  _02,
            _10,  _11,  _12,
            _20,  _21,  _22;
        };
        struct {
            Vector3 c1, c2, c3;
        };
    };
    
    Matrix3(f32 __00, f32 __01, f32 __02, 
            f32 __10, f32 __11, f32 __12, 
            f32 __20, f32 __21, f32 __22) {
        _00 = __00; _01 = __10; _02 = __20;
        _10 = __01; _11 = __11; _12 = __21;
        _20 = __02; _21 = __12; _22 = __22;
    }

    Matrix3(Vector3 x, Vector3 y, Vector3 z) : c1(x), c2(y), c3(z) {}

    Matrix3 transpose() {
        return Matrix3(_00, _01, _02, _10, _11, _12, _20, _21, _22);
        
    }

    f32& operator()(int i, int j) {
        return data[j][i];
    }
    const f32& operator()(int i, int j) const {
        return data[j][i];
    }

    Vector3& operator[](int vec) {
        return *(reinterpret_cast<Vector3*>(&data[vec][0]));
    }
    
    
};


struct alignas(64) Matrix4 {
    union {
        f32 data[4][4];
        struct {
            f32 _00,  _01,  _02,  _03,
            _10,  _11,  _12,  _13,
            _20,  _21,  _22,  _23,
            _30,  _31,  _32,  _33;
        };
        struct {
            Vector4 _v1, _v2, _v3, _v4;
            
            
            
        };
    };
    // column major (vectors are the columns)
    // When the user wants 10, I need to give them mat(1,0), which is data[0][1]
    // According to RTR 4, the last 4 values in memory are the translations
    // This implies I should do this type of storage, so the vectors are contiguous
    Matrix4(f32 __00, f32 __01, f32 __02, f32 __03,
            f32 __10, f32 __11, f32 __12, f32 __13,
            f32 __20, f32 __21, f32 __22, f32 __23,
            f32 __30, f32 __31, f32 __32, f32 __33) {
        _00 = __00; _01 = __10; _02 = __20; _03 = __30;
        _10 = __01; _11 = __11; _12 = __21; _13 = __31;
        _20 = __02; _21 = __12; _22 = __22; _23 = __32;
        _30 = __03; _31 = __13; _32 = __23; _33 = __33;
    }
    
    Matrix4(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4) {
        _v1 = v1; _v2 = v2; _v3 = v3; _v4 = v4;
    }

    Matrix4() {}
    f32& operator()(int i, int j) {
        return data[j][i];
    }
    const f32& operator()(int i, int j) const {
        return data[j][i];
    }
    
    
    Vector4& operator[](int vec) {
        return *(reinterpret_cast<Vector4*>(&data[vec][0]));
    }
    
};


// Remember that the i, j, k are the imaginary parts
// The w is like the other part
// Properties of these lend well to different types of operations
struct Quaternion {
    union {
        struct {
            f32 i, j, k, w;
        };
        f32 data[4];
    };
    
    Quaternion(f32 i, f32 j, f32 k, f32 w) {
        i = i; j = j; k = k; w = w;
    }
    
    Quaternion(const Vector3& vec, f32 w) {
        data[0] = vec.x;
        data[1] = vec.y;
        data[2] = vec.z;
        data[3] = w;
    }

    Quaternion(const Vector4& vec) {
        data[0] = vec.x;
        data[1] = vec.y;
        data[2] = vec.z;
        data[3] = vec.w;
    }
    f32 operator[](int index) {
        return data[index];
    }
    
};

/* What is the simplest way to represent this? other than its worldspace transform*/
struct CoordinateSpace {
    Vector3 origin; // in world space coordinates
    Vector3 r, s, t;
    
    void rotate(Matrix3& rotation);
    
};


inline Vector4 cross(const Vector4& a ,const Vector4& b, f32 w) {
    return Vector4(a.y*b.z-a.z-b.y,a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x, w);
}

inline Vector3 cross(const Vector3& a ,const Vector3& b) {
    return Vector3(a.y*b.z-a.z-b.y,a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
    return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
    return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Vector4 operator+(const Vector4& lhs, const Vector4& rhs) {
    return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);    
}


inline Vector4 operator-(const Vector4& lhs, const Vector4& rhs) {
    return Vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);    
}

inline Vector3 operator*(const Vector3& lhs, const Vector3& rhs) {
    return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);    
}


inline Vector3 operator/(const Vector3& lhs, const Vector3& rhs) {
    return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);    
}


inline Vector4 operator*(const Vector4& lhs, const Vector4& rhs) {
    return Vector4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);    
}


inline Vector4 operator/(const Vector4& lhs, const Vector4& rhs) {
    return Vector4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);    
}

inline Vector4 operator*(const Vector4& lhs, f32 scale) {
    return Vector4(lhs.x*scale, lhs.y*scale, lhs.z* scale, lhs.w * scale);
}


inline Vector4 operator/(const Vector4& lhs, f32 scale) {
    return Vector4(lhs.x/scale, lhs.y/scale, lhs.z/ scale, lhs.w / scale);
}

inline Vector4 operator*( f32 scale, const Vector4& rhs) {
    return Vector4(rhs.x*scale, rhs.y*scale, rhs.z* scale, rhs.w * scale);
}


inline Vector4 operator/(f32 scale, const Vector4& rhs) {
    return Vector4(rhs.x/scale, rhs.y/scale, rhs.z/ scale, rhs.w / scale);
}


inline Vector3 operator*(const Vector3& lhs, f32 scale) {
    return Vector3(lhs.x*scale, lhs.y*scale, lhs.z* scale);
}


inline Vector3 operator/(const Vector3& lhs, f32 scale) {
    return Vector3(lhs.x/scale, lhs.y/scale, lhs.z/ scale);
}

inline Vector3 operator*( f32 scale, const Vector3& rhs) {
    return Vector3(rhs.x*scale, rhs.y*scale, rhs.z* scale);
}


inline Vector3 operator/(f32 scale, const Vector3& rhs) {
    return Vector3(rhs.x/scale, rhs.y/scale, rhs.z/ scale);
}



inline f32 dot(const Vector3& lhs, const Vector3& rhs) {
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z* rhs.z; 
}


inline f32 dot(const Vector4& lhs, const Vector4& rhs) {
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z* rhs.z + lhs.w * rhs.w; 
}


inline Quaternion operator* (Quaternion& in, f32 scale) {
    in.data[0] *= scale;
    in.data[1] *= scale;
    in.data[2] *= scale;
    in.data[3] *= scale;
    return in;
    
}

inline Quaternion operator* (f32 scale, Quaternion& in) {
    in.data[0] *= scale;
    in.data[1] *= scale;
    in.data[2] *= scale;
    in.data[3] *= scale;
    return in;    
}

// with (4x4)(4x1) = 4x1
/*
inline Vector4 operator*( Matrix4& lhs, Vector4&rhs) {
    f32 a, b, c, d;
    a = lhs(0,0)*rhs.data[0] + lhs(0, 1)* rhs.data[1] + lhs(0, 2)* rhs.data[2] + lhs(0, 3) * rhs.data[3];
    b= lhs(1,0)*rhs.data[0] + lhs(1, 1)* rhs.data[1] + lhs(1, 2)* rhs.data[2] + lhs(1, 3) * rhs.data[3];
    c= lhs(2,0)*rhs.data[0] + lhs(2, 1)* rhs.data[1] + lhs(2, 2)* rhs.data[2] + lhs(2, 3) * rhs.data[3];
    d= lhs(3,0)*rhs.data[0] + lhs(3, 1)* rhs.data[1] + lhs(3, 2)* rhs.data[2] + lhs(3, 3) * rhs.data[3];
    return Vector4(a, b, c, d);
    
}
*/
/* det 2x2 */
inline f32 det(f32 ul, f32 ur, f32 ll, f32 lr) {
    return ul*lr - ll*ur;
}

/* determinant of a matrix */
inline f32 det(Matrix3& in) {
    f32 l, c, r;
    
    l = in(0, 0) * det(in(1,1), in(1,2), in(2,1), in(2,2) );
    c = in(0, 1) * det(in(1, 0), in(1, 2) ,in(2,0), in(2,2));
    r = in(0, 2) * det(in(1,0), in(1,1), in(2,0), in(2,1));
    return l - c + r;
}


/* determines if the given rotation does a reflection. If it does, then this will put the vertices in the wrong order */
inline bool isReflection(Matrix4 transform) {
    Matrix3 upper3x3 = Matrix3(transform(0, 0), transform(0, 1), transform(0,2),
                                     transform(1, 0), transform(1,1), transform(1,2),
                                     transform(2,0), transform(2,1), transform(2,2));
    
    return det(upper3x3) < 0;
}

inline Quaternion conjugate(const Quaternion& in) {
    return Quaternion(-in.data[0], -in.data[1], -in.data[2], in.data[3]);
}


inline Matrix4 constructTranslation(const Vector3 &linearTranslation){
    
    return Matrix4(1, 0, 0, linearTranslation.x,
                   0, 1, 0, linearTranslation.y,
                   0, 0, 1, linearTranslation.z,
                   0, 0, 0, 1);
}

/* Rotates CCW about x axis */
inline Matrix4 rotateX(float t) {
    f32 c = cosf(t);
    f32 s = sinf(t);
    return Matrix4(1,0, 0,0,
                   0, c, -s, 0,
                   0, s, c, 0,
                   0, 0, 0, 1);
    
    
}

/* Rotate about the y axis */
inline Matrix4 rotateY(float t) {
    f32 c = cosf(t);
    f32 s = sinf(t);
    return Matrix4(c,0, s,0,
                   0, 1, 0, 0,
                   -s, 0, c, 0,
                   0, 0, 0, 1);
    
}

/* Rotate about the Z axis */
inline Matrix4 rotateZ(float t){
    f32 c = cosf(t);
    f32 s = sinf(t);
    
    return Matrix4(c, -s, 0, 0,
                   s, c, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 0);
}

inline Matrix3 rotateX3(float t) {
    f32 c = cosf(t);
    f32 s = sinf(t);
    return Matrix3(1.0f ,0.0f, 0.0f,
                   0.0f, c, -s,
                   0.0f, s, c);

    
    
}

/* Rotate about the y axis */
inline Matrix3 rotateY3(float t) {
    f32 c = cosf(t);
    f32 s = sinf(t);
    return Matrix3(c,0.0f, s,
                   0.0f, 1.0f, 0.0f, 
                   -s, 0.0f, c);
                  
    
}

/* Rotate about the Z axis */
inline Matrix3 rotateZ3(float t){
    f32 c = cosf(t);
    f32 s = sinf(t);
    
    return Matrix3(c, -s, 0.0f,
                   s, c, 0.0f, 
                   0.0f, 0.0f, 1.0f);
}




inline Matrix3 operator* ( Matrix3 &lhs,  Matrix3& rhs) {
    f32 _00 = lhs(0,0)* rhs(0,0) + lhs(0,1)*rhs(1,0) + lhs(0, 2) * rhs(2,0);
    f32 _01 = lhs(0,0)* rhs(0,1) + lhs(0,1)*rhs(1,1) + lhs(0, 2) * rhs(2,1);
    f32 _02 = lhs(0,0)* rhs(0,2) + lhs(0,1)*rhs(1,2) + lhs(0, 2) * rhs(2,2);
    f32 _10 = lhs(1,0)* rhs(0,0) + lhs(1,1)*rhs(1,0) + lhs(1, 2) * rhs(2,0);
    f32 _11 = lhs(1,0)* rhs(0,1) + lhs(1,1)*rhs(1,1) + lhs(1, 2) * rhs(2,1); // same column as 01
    f32 _12 = lhs(1,0)* rhs(0,2) + lhs(1,1)*rhs(1,2) + lhs(1, 2) * rhs(2,2);// same column as 02
    f32 _20 = lhs(2,0)* rhs(0,0) + lhs(2,1)*rhs(1,0) + lhs(2, 2) * rhs(2,0);
    f32 _21 = lhs(2,0)* rhs(0,1) + lhs(2,1)*rhs(1,1) + lhs(2, 2) * rhs(2,1);
    f32 _22 = lhs(2,0)* rhs(0,2) + lhs(2,1)*rhs(1,2) + lhs(2, 2) * rhs(2,2);
    
    
    return Matrix3(_00, _01, _02, _10, _11, _12, _20, _21, _22);
}

inline Matrix4 operator*(Matrix4& lhs, Matrix4& rhs) {
    // The column vectors are actually contiguous

    Matrix4 emptyMatrix;
    __m128 col0 = _mm_load_ps(&lhs.data[0][0]);
    __m128 col1 = _mm_load_ps(&lhs.data[1][0]);
    __m128 col2 = _mm_load_ps(&lhs.data[2][0]);
    __m128 col3 = _mm_load_ps(&lhs.data[3][0]);

    for (int i = 0; i < 4; ++i) {
        __m128 b0 = _mm_set1_ps(rhs.data[i][0]);
        __m128 b1 = _mm_set1_ps(rhs.data[i][1]);
        __m128 b2 = _mm_set1_ps(rhs.data[i][2]);
        __m128 b3 = _mm_set1_ps(rhs.data[i][3]);
        __m128 res = _mm_add_ps(_mm_add_ps(_mm_mul_ps(col0, b0), _mm_mul_ps(col1, b1)), _mm_add_ps(_mm_mul_ps(col2, b2), _mm_mul_ps(col3, b3)));
        _mm_store_ps(&emptyMatrix.data[i][0], res);
    }

    return emptyMatrix;
    
}
 
inline Vector4 operator*(Matrix4& lhs, Vector4& rhs) {
    Vector4 targetVector;
    
    __m128 col0 = _mm_load_ps(&lhs.data[0][0]);
    __m128 col1 = _mm_load_ps(&lhs.data[1][0]);
    __m128 col2 = _mm_load_ps(&lhs.data[2][0]);
    __m128 col3 = _mm_load_ps(&lhs.data[3][0]);

    __m128 bv0 = _mm_set1_ps(rhs.data[0]);
    __m128 bv1 = _mm_set1_ps(rhs.data[1]);
    __m128 bv2 = _mm_set1_ps(rhs.data[2]);
    __m128 bv3 = _mm_set1_ps(rhs.data[3]);

    __m128 res = _mm_add_ps(_mm_add_ps(_mm_mul_ps(col0, bv0), _mm_mul_ps(col1, bv1)), _mm_add_ps(_mm_mul_ps(col2, bv2), _mm_mul_ps(col3, bv3)));
    _mm_store_ps(&targetVector.data[0], res);
    return targetVector;
}

inline Quaternion operator* (Quaternion& lhs, Quaternion& rhs) {
    Vector3 lhs_v = Vector3(lhs.i, lhs.j, lhs.k);
    Vector3 rhs_v = Vector3(rhs.i, rhs.j, rhs.k);
    /* emplace this here */
    Vector3 imaginaryPart = cross(lhs_v, rhs_v) + (rhs.w * lhs_v) + (lhs.w * rhs_v);
    f32 w = lhs.w * rhs.w + dot(lhs_v, rhs_v);
    return Quaternion(imaginaryPart, w);
    
}


/* this really is the adjoint of the upper 3x3 */
inline Matrix3 adjointUpper3x3( Matrix4 &in) {
    return Matrix3(det(in(1, 1), in(1,2), in(2, 1), in(2, 2)), -det(in(0,1), in(0,2), in(2,1), in(2,2)), det(in(0,1), in(0,2), in(1,1), in(1,2)),
                   -det(in(1,0), in(1,2), in(2,0), in(2,2)), det(in(0,0), in(0,2), in(2,0), in(2,2)), -det(in(0,0), in(0,2), in(1,0), in(1,2)),
                   det(in(1,0), in(1,1), in(2,0), in(2,1)), -det(in(0,0), in(0,1), in(2,0), in(2,1)), det(in(0,0), in(0,1), in(1,0), in(1,1)) );
}

inline Matrix3 adjoint3x3( Matrix3 &in) {
    return Matrix3(det(in(1, 1), in(1,2), in(2, 1), in(2, 2)), -det(in(0,1), in(0,2), in(2,1), in(2,2)), det(in(0,1), in(0,2), in(1,1), in(1,2)),
                   -det(in(1,0), in(1,2), in(2,0), in(2,2)), det(in(0,0), in(0,2), in(2,0), in(2,2)), -det(in(0,0), in(0,2), in(1,0), in(1,2)),
                   det(in(1,0), in(1,1), in(2,0), in(2,1)), -det(in(0,0), in(0,1), in(2,0), in(2,1)), det(in(0,0), in(0,1), in(1,0), in(1,1)) );
}



/* This is a linear interpolation, at the heart of graphics
 * RETURNS the interpolated float between the two points
 */
inline f32 lerp(f32 start, f32 end, f32 t) {
    return start + (end-start)*t;
}


/* The idea with rotating about an arbitrary axis is that you can do a change of base, then rotate around say the x axis only
 * but we can also rotate about an arbitrary vector using the quaternions, therefore we will use the quaternion
 * The quaternion q = (sinx uq, cosx) will rotate about an axis uq by x degrees
 * Does not check to see if the vector is a unit vector
 * RETURNS the quaternion that is used for this transform
 */
inline Quaternion rotationQuaternion(const Vector3& axis, f32 phi ) {
    f32 s = sinf(phi/2);
    f32 c = cosf(phi/2);
    
    return Quaternion(s*axis.x, s*axis.y, s*axis.z, c);
}

/* ASSUMES that the input is a unit quaternion
 * RETURNS a homegenous vector that is the rotated point
 */
inline Vector4 quaternionRotatePoint(Quaternion& in, const Vector4& point) {
    Quaternion conj = conjugate(in);
    Quaternion pure(point);
    pure = pure * conj;
    Quaternion tmp = in * pure;
    return Vector4(tmp.i, tmp.j, tmp.k, tmp.w); 
}

// Returns a change of basis matrix
inline Matrix4 changeOfBasis(Vector4& r, Vector4& u, Vector4& v) {
    return Matrix4(r, u, v, Vector4(0));
}

// Change the model coord space with the dot of the eye and the coordinate axes
// We want to express a point in world space as the sum of elements of our new basis
// (e.g) in world space the point (4,2,3) is just the sum of 4<1,0,0> + 2<0,1,0> + 3<0,0,1>
// But we also want to subtract elements of the distance included that are already included in the origin
inline Matrix4 WorldObjectMatrix(const CoordinateSpace& modelCoordSpace){
    return Matrix4(modelCoordSpace.r.x, modelCoordSpace.r.y, modelCoordSpace.r.z, -dot(modelCoordSpace.origin, modelCoordSpace.r ),
                   modelCoordSpace.s.x, modelCoordSpace.s.y, modelCoordSpace.s.z, -dot(modelCoordSpace.origin, modelCoordSpace.s),
                   modelCoordSpace.t.x, modelCoordSpace.t.y, modelCoordSpace.t.z, -dot(modelCoordSpace.origin, modelCoordSpace.t),
            0, 0, 0,1);
}

// The dot of the world space coordinate system is always directly onto x, y, z since there is no rotation, so we should be good
// We want to take our point in obj space and express each's contributions to x,y, z respectivel 
// Then we can add the origin, and the dots like the one earlier are just all 1 for each bases
inline Matrix4 ObjectWorldMatrix(const CoordinateSpace& modelCoordSpace) {
    return Matrix4(modelCoordSpace.r.x, modelCoordSpace.s.x, modelCoordSpace.t.x, modelCoordSpace.origin.x,
        modelCoordSpace.r.y, modelCoordSpace.s.y, modelCoordSpace.t.y, modelCoordSpace.origin.y,
        modelCoordSpace.r.z, modelCoordSpace.s.z, modelCoordSpace.t.z, modelCoordSpace.origin.z,
            0, 0, 0, 1);
}

// gl asks for the near and far plane to be expressed positively, but it still represents direction along negative z axis
// therefore the 3rd column of the usual projection matrix has its sign flipped
inline Matrix4 glProjectionMatrix(f32 vFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane) {

    ASSERT(farPlane > -.001f);
    ASSERT(nearPlane > -.001f);
    f32 c = 1.0f/ tanf(vFOV/2);
    return Matrix4(c/aspectRatio, 0, 0, 0,
                   0, c, 0, 0,
                   0, 0, -(farPlane + nearPlane)/(farPlane - nearPlane), -2*(farPlane*nearPlane)/(farPlane - nearPlane),
                   0, 0, -1, 0);
}

// PVM is the right order? Make sure that after division by w these are all in the unit cube
inline Matrix4 glModelViewProjection(const CoordinateSpace& objSpace, const CoordinateSpace& cameraSpace, f32 vFOV, f32 aspectRatio, f32 nearPlane, f32 farPlane) {
    Matrix4 m = ObjectWorldMatrix(objSpace);
    Matrix4 v = WorldObjectMatrix(cameraSpace);
    Matrix4 p = glProjectionMatrix(vFOV, aspectRatio, nearPlane, farPlane);
    return (p*(v*m));
}

inline Matrix4 modelView(const CoordinateSpace& objSpace, const CoordinateSpace& objSpace) {
    Matrix4 m = ObjectWorldMatrix(objSpace);
    Matrix4 v = WorldObjectMatrix(cameraSpace);
    return v*m;
}

inline bool fcmp(f32* p1, f32* p2, int cnt) {
    for (int i = 0; i < cnt; ++i) {
        if (fabs(p1[i] - p2[i]) > .01) {
            return false;
        }
    }
    return true;
}
inline bool operator==(const Matrix4& lhs, const Matrix4& rhs) {
    f32* p1 = (f32*)&lhs.data;
    f32* p2 = (f32*)&rhs.data;
    return (fcmp(p1, p2, 16));
}


inline bool operator==(const Matrix3& lhs, const Matrix3& rhs) {
    f32* p1 = (f32*)&lhs.data;
    f32* p2 = (f32*)&rhs.data;
    return (fcmp(p1, p2, 9));
}


inline bool operator==(const Quaternion& lhs, const Quaternion& rhs) {
    f32* p1 = (f32*)&lhs.data;
    f32* p2 = (f32*)&rhs.data;
    return (fcmp(p1, p2, 4));
}


inline bool operator==(const Vector4& lhs, const Vector4& rhs) {
    f32* p1 = (f32*)&lhs.data;
    f32* p2 = (f32*)&rhs.data;
    return (fcmp(p1, p2, 4));
}


inline bool operator==(const Vector3& lhs, const Vector3& rhs) {
    f32* p1 = (f32*)&lhs.data;
    f32* p2 = (f32*)&rhs.data;
    return (fcmp(p1, p2, 3));
}

void CoordinateSpace::rotate( Matrix3& rotation) {
        Matrix3 m(r, s, t);
        m = m.transpose();
        m = rotation * m;
        m = m.transpose();
        r = m[0]; s = m[1]; t = m[2];
    }

#endif
