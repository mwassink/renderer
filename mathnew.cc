// TODO: what is a good way to inline different functions? Should this go in a .h file?

typedef float f32;
#define FLOAT_MIN 0.0001
#define ASSERT(expr) if (!expr) { *((volatile int*)0) = 0;}
struct Matrix3 {
    union {
        float data[3][3];
        struct {
            f32 _00,  _01,  _02,
            _10,  _11,  _12,
            _20,  _21,  _22;
        };
    };
    
    Matrix3(f32 _00, f32 _01, f32 _02, 
            f32 _10, f32 _11, f32 _12, 
            f32 _20, f32 _21, f32 _22) {
        _00 = _00; _01 = _10; _02 = _20;
        _10 = _01; _11 = _11; _12 = _21;
        _20 = _02; _21 = _12; _22 = _22;
    }
    
};


struct Matrix4 {
    union {
        float data[4][4];
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
    Matrix4(f32 _00, f32 _01, f32 _02, f32 _03,
            f32 _10, f32 _11, f32 _12, f32 _13,
            f32 _20, f32 _21, f32 _22, f32 _23,
            f32 _30, f32 _31, f32 _32, f32 _33) {
        _00 = _00; _01 = _10; _02 = _20; _03 = _30;
        _10 = _01; _11 = _11; _12 = _21; _13 = _31;
        _20 = _02; _21 = _12; _22 = _22; _23 = _32;
        _30 = _03; _31 = _13; _32 = _23; _33 = _33;
    }
    
    Matrix4(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4) {
        _v1 = v1; _v2 = v2; _v3 = v3; _v4 = v4;
    }
    f32& operator()(int i, int j) {
        return data[j][i];
    }
    
    
    Vector4 operator[](int vec) {
        return *(reinterpret_cast<Vector4*> &data[vec][0]);
    }
    
};


struct Vector3 {
    union {
        struct {
            f32 x,y,z;
        };
        f32 arr[3];
    };
    Vector3(f32 x, f32 y, f32 z) {
        x = x; y = y; z = z;
    }
    f32 operator[](int index) {
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
};

struct Vector4 {
    union {
        struct {
            f32 x,y,z,w;
        };
        
        f32 arr[4];
    };
    Vector4(f32 x, f32 y, f32 z, f32 w) {
        x = x; y = y; z = z; w = w;
    }
    Vector4(f32 f) {
        arr[0] = f; arr[1] = f; arr[2] = f; arr[3] = f;
    }
    f32 operator[](int index) {
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
        w *+ scale;
        return (*this);
    }
    void normalize()
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
    f32 operator[](int index) {
        return data[index];
    }
    
};

/* What is the simplest way to represent this? other than its worldspace transform*/
struct CoordinateSpace {
    Matrix4 transform; /* this will transform this into the proper coordinate space */
};



/* det 2x2 */
inline f32 det(f32 ul, f32 ur, f32 ll, f32 lr) {
    return ul*lr - ll*ur;
}

/* determinant of a matrix */
inline f32 det(const Matrix3& in) {
    f32 l, c, r;
    
    l = in(0, 0) * det(in(1,1), in(1,2), in(2,1), in(2,2) );
    c = in(0, 1) * det(in(1, 0), in(1, 2) ,in(2,0), in(2,2));
    r = in(0, 2) * det(in(1,0), in(1,1), in(2,0), in(2,1));
    return l - c + r;
}

/* determines if the given rotation does a reflection. If it does, then this will put the vertices in the wrong order */
inline bool isReflection(Matrix4 transform) {
    const Matrix3 upper3x3 = Matrix3(transform(0, 0), transform(0, 1), transform(0,2),
                                     transform(1, 0), transform(1,1), transform(1,2),
                                     transform(2,0), transform(2,1), transform(2,2));
    
    return det(upper3x3) < 0;
}


Matrix3 operator* (const Matrix3 &lhs, const Matrix3& rhs) {
    f32 _00 = lhs(0,0)* rhs(0,0) + lhs(0,1)*rhs(1,0) + lhs(0, 2) * rhs(2,0);
    f32 _01 = lhs(0,0)* rhs(0,1) + lhs(0,1)*rhs(1,1) + lhs(0, 2) * rhs(2,1);
    f32 _02 = lhs(0,0)* rhs(0,2) + lhs(0,1)*rhs(1,2) + lhs(0, 2) * rhs(2,2);
    f32 _10 = lhs(1,0)* rhs(0,0) + lhs(1,1)*rhs(1,0) + lhs(1, 2) * rhs(2,0);
    f32 _11 = lhs(1,0)* rhs(0,1) + lhs(1,1)*rhs(1,1) + lhs(1, 2) * rhs(2,1); // same column as 01
    f32 _12 = lhs(1,0)* rhs(0,2) + lhs(1,1)*rhs(1,2) + lhs(2, 2) * rhs(2,2);// same column as 02
    f32 _20 = lhs(2,0)* rhs(0,0) + lhs(2,1)*rhs(1,0) + lhs(2, 2) * rhs(2,0);
    f32 _21 = lhs(2,0)* rhs(0,1) + lhs(2,1)*rhs(1,1) + lhs(2, 2) * rhs(2,1);
    f32 _22 = lhs(2,0)* rhs(0,2) + lhs(2,1)*rhs(1,2) + lhs(2, 2) * rhs(2,2);
    
    
    return Matrix3(_00, _01, _02, _10, _11, _12, _20, _21, _22);
}

inline Quaternion operator* (const Quaternion& in, f32 scale) {
    in.data[0] *= scale;
    in.data[1] *= scale;
    in.data[2] *= scale;
    in.data[3] *= scale;
    return in;
    
}

inline Quaternion operator* (f32 scale, const Quaternion& in) {
    in.data[0] *= scale;
    in.data[1] *= scale;
    in.data[2] *= scale;
    in.data[3] *= scale;
    return in;    
}

// with (4x4)(4x1) = 4x1
inline Vector4 operator*(const Matrix4& lhs, const Vector4&rhs) {
    f32 a, b, c, d;
    a = lhs(0,0)*rhs.data[0] + lhs(0, 1)* rhs.data[1] + lhs(0, 2)* rhs.data[2] + lhs(0, 3) * rhs.data[3];
    b= lhs(1,0)*rhs.data[0] + lhs(1, 1)* rhs.data[1] + lhs(1, 2)* rhs.data[2] + lhs(1, 3) * rhs.data[3];
    c= lhs(2,0)*rhs.data[0] + lhs(2, 1)* rhs.data[1] + lhs(2, 2)* rhs.data[2] + lhs(2, 3) * rhs.data[3];
    d= lhs(3,0)*rhs.data[0] + lhs(3, 1)* rhs.data[1] + lhs(3, 2)* rhs.data[2] + lhs(3, 3) * rhs.data[3];
    return Vector4(a, b, c, d);
    
}


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


inline f32 dot(const Vector3& lhs, const Vector3& lhs) {
    return lhs[0]*rhs[0] + lhs[1]*rhs[1] + lhs[2]* rhs[2]; 
}


inline f32 dot(const Vector4& lhs, const Vector4& lhs) {
    return lhs[0]*rhs[0] + lhs[1]*rhs[1] + lhs[2]* rhs[2] + lhs[3] * rhs[3]; 
}

Quaternion operator* (const Quaternion& lhs, const Quaternion& rhs) {
    const lhs_v = Vector3(lhs.x, lhs.y, lhs.z);
    const rhs_v = Vector3(rhs.x, rhs.y, rhs.z);
    /* emplace this here */
    const Vector3 imaginaryPart = cross(lhs_v, rhs_v) + rhs.w * lhs_v + lhs.w * rhs_v;
    f32 w = lhs.w * rhs.w + dot(lhs_v, rhs_v);
    return Quaternion(imaginaryPart, w);
    
}

inline Quaternion conjugate(const Quaternion& in) {
    return Quaternion(-in.data[0], -in.data[1], -in.data[2], in.data[3]);
}

/* this really is the adjoint of the upper 3x3 */
Matrix3 adjointUpper3x3(const Matrix4 &in) {
    return Matrix3(det(in(1, 1), in(1,2), in(2, 1), in(2, 2)), -det(in(0,1), in(0,2), in(2,1), in(2,2)), det(in(0,1), in(0,2), in(1,1), in(1,2)),
                   -det(in(1,0), in(1,2), in(2,0), in(2,2)), det(in(0,0), in(0,2), in(2,0), in(2,2)), -det(in(0,0), in(0,2), in(1,0), in(1,2)),
                   det(in(1,0), in(1,1), in(2,0), in(2,1)), -det(in(0,0), in(0,1), in(2,0), in(2,1)), det(in(0,0), in(0,1), in(1,0), in(1,1)) );
}

Matrix3 adjoint3x3(const Matrix3 &in) {
    return Matrix3(det(in(1, 1), in(1,2), in(2, 1), in(2, 2)), -det(in(0,1), in(0,2), in(2,1), in(2,2)), det(in(0,1), in(0,2), in(1,1), in(1,2)),
                   -det(in(1,0), in(1,2), in(2,0), in(2,2)), det(in(0,0), in(0,2), in(2,0), in(2,2)), -det(in(0,0), in(0,2), in(1,0), in(1,2)),
                   det(in(1,0), in(1,1), in(2,0), in(2,1)), -det(in(0,0), in(0,1), in(2,0), in(2,1)), det(in(0,0), in(0,1), in(1,0), in(1,1)) );
}


inline Matrix4 constructTranslation(const Vector3 &linearTranslation){
    
    return Matrix4(1, 0, 0, linearTranslation.x,
                   0, 1, 0, linearTranslation.y,
                   0, 0, 1, linearTranslation.z,
                   0, 0, 0, 1);
}

/* Rotate about the x axis */
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
    f32 s = cosf(t);
    
    return Matrix4(c, -s, 0, 0,
                   s, c, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 0);
}

/* This is a linear interpolation, at the heart of graphics
 * RETURNS the interpolated float between the two points
 */
f32 lerp(f32 start, f32 end, f32 t) {
    return start + (end-start)*t;
}


/* The idea with rotating about an arbitrary axis is that you can do a change of base, then rotate around say the x axis only
 * but we can also rotate about an arbitrary vector using the quaternions, therefore we will use the quaternion
 * The quaternion q = sinx uq cosx will rotate about an axis uq by x degrees
 * Does not check to see if the vector is a unit vector
 * RETURNS the quaternion that is used for this transform
 */
Quaternion rotationQuaternion(const Vector3& axis, f32 phi ) {
    f32 s = sinf(phi/2);
    f32 c = cosf(phi/2);
    
    return Quaternion(s*axis.i, s*axis.j, s*axis.k, c*axis.w);
}

/* ASSUMES that the input is a unit quaternion
 * RETURNS a homegenous vector that is the rotated point
 */

Vector4 quaternionRotatePoint(Quaternion& in, const Vector4& point) {
    Quaternion conj = conjugate(in);
    const Quaternion pure = (Quaternion) point; //optimized out
    pure = pure * conj;
    return (Vector4) in * pure; //may not be type safe
}

// Returns a change of basis matrix
Matrix4 changeOfBasis(Vector4& r, Vector4& u, Vector4& v) {
    return Matrix4(r, u, v, Vector4(0));
}


