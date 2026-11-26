#pragma once
#include "Math3D.h"
#include "ObjLoader.h"

class RendererGL {
public:
    bool init();
    void beginFrame(int w, int h, const Mat4& viewProj);
    void uploadMesh(MeshData& m);
    void drawMesh(const MeshData& m, const Mat4& model);
    void shutdown();
private:
    unsigned int prog = 0;
    int uMVP = -1, uModel = -1;
    Mat4 vp = Mat4::identity();
};
