#pragma once
#include <string>
#include <vector>
#include "Math3D.h"

struct Vertex3D {
    float px, py, pz;
    float nx, ny, nz;
    float r, g, b;
};

struct MeshData {
    std::vector<Vertex3D> vertices;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    int vertexCount = 0;
};

class ObjLoader {
public:
    static bool loadNormalizedOBJ(const std::string& path, float targetSize, float r, float g, float b, MeshData& out);
    static MeshData createBoxMesh(float sx, float sy, float sz, float r, float g, float b);
};
