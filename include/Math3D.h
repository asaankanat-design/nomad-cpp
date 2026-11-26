#pragma once
#include <cmath>
#include <cstring>

struct Vec3 {
    float x, y, z;
    Vec3(float x_=0, float y_=0, float z_=0) : x(x_), y(y_), z(z_) {}
    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(float s) const { return {x*s, y*s, z*s}; }
    float len() const { return std::sqrt(x*x + y*y + z*z); }
    Vec3 norm() const { float l = len(); return l > 1e-5f ? Vec3(x/l, y/l, z/l) : Vec3(0,0,1); }
    static float dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
    }
};

struct Mat4 {
    float m[16];
    static Mat4 identity() {
        Mat4 r{}; std::memset(r.m, 0, sizeof(r.m));
        r.m[0]=r.m[5]=r.m[10]=r.m[15]=1.0f; return r;
    }
    Mat4 operator*(const Mat4& b) const {
        Mat4 r{};
        for(int c=0;c<4;c++) for(int row=0;row<4;row++) {
            float s=0; for(int k=0;k<4;k++) s += m[k*4+row]*b.m[c*4+k];
            r.m[c*4+row]=s;
        }
        return r;
    }
    static Mat4 perspective(float fovDeg, float aspect, float zn, float zf) {
        float f = 1.0f / std::tan(fovDeg * 0.5f * 3.14159265f / 180.0f);
        Mat4 r{}; std::memset(r.m, 0, sizeof(r.m));
        r.m[0] = f / aspect; r.m[5] = f;
        r.m[10] = (zf + zn) / (zn - zf); r.m[11] = -1.0f;
        r.m[14] = (2.0f * zf * zn) / (zn - zf);
        return r;
    }
    static Mat4 lookAt(const Vec3& eye, const Vec3& tgt, const Vec3& up) {
        Vec3 f = (tgt - eye).norm();
        Vec3 s = Vec3::cross(f, up).norm();
        Vec3 u = Vec3::cross(s, f);
        Mat4 r = identity();
        r.m[0]=s.x; r.m[4]=s.y; r.m[8]=s.z;
        r.m[1]=u.x; r.m[5]=u.y; r.m[9]=u.z;
        r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;
        r.m[12]=-Vec3::dot(s, eye); r.m[13]=-Vec3::dot(u, eye); r.m[14]=Vec3::dot(f, eye);
        return r;
    }
    static Mat4 translate(const Vec3& t) {
        Mat4 r = identity(); r.m[12]=t.x; r.m[13]=t.y; r.m[14]=t.z; return r;
    }
    static Mat4 scale(const Vec3& s) {
        Mat4 r = identity(); r.m[0]=s.x; r.m[5]=s.y; r.m[10]=s.z; return r;
    }
    static Mat4 rotateY(float rad) {
        Mat4 r = identity(); float c=std::cos(rad), s=std::sin(rad);
        r.m[0]=c; r.m[8]=s; r.m[2]=-s; r.m[10]=c; return r;
    }
};
