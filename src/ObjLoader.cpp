#include "ObjLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <array>

bool ObjLoader::loadNormalizedOBJ(const std::string& path, float targetSize, float r, float g, float b, MeshData& out) {
    std::ifstream in(path);
    if (!in.is_open()) return false;
    std::vector<Vec3> verts;
    std::vector<std::array<int, 3>> tris;
    std::string line;
    Vec3 minB(1e9f, 1e9f, 1e9f), maxB(-1e9f, -1e9f, -1e9f);

    while (std::getline(in, line)) {
        if (line.rfind("v ", 0) == 0) {
            std::istringstream ss(line.substr(2));
            Vec3 v; ss >> v.x >> v.y >> v.z;
            verts.push_back(v);
            minB.x = std::min(minB.x, v.x); minB.y = std::min(minB.y, v.y); minB.z = std::min(minB.z, v.z);
            maxB.x = std::max(maxB.x, v.x); maxB.y = std::max(maxB.y, v.y); maxB.z = std::max(maxB.z, v.z);
        } else if (line.rfind("f ", 0) == 0) {
            std::istringstream ss(line.substr(2));
            std::vector<int> idx;
            std::string tok;
            while (ss >> tok) {
                int vi = std::stoi(tok.substr(0, tok.find('/')));
                if (vi < 0) vi = (int)verts.size() + vi; else vi -= 1;
                if (vi >= 0 && vi < (int)verts.size()) idx.push_back(vi);
            }
            for (size_t k = 1; k + 1 < idx.size(); ++k) {
                tris.push_back({idx[0], idx[k], idx[k+1]});
            }
        }
    }
    if (tris.empty()) return false;
    Vec3 sz = maxB - minB;
    float sc = targetSize / std::max({0.001f, sz.x, sz.y, sz.z});
    Vec3 ctr((minB.x + maxB.x) * 0.5f, minB.y, (minB.z + maxB.z) * 0.5f);

    out.vertices.clear();
    out.vertices.reserve(tris.size() * 3);
    for (auto& t : tris) {
        Vec3 p0 = (verts[t[0]] - ctr) * sc;
        Vec3 p1 = (verts[t[1]] - ctr) * sc;
        Vec3 p2 = (verts[t[2]] - ctr) * sc;
        Vec3 n = Vec3::cross(p1 - p0, p2 - p0).norm();
        out.vertices.push_back({p0.x, p0.y, p0.z, n.x, n.y, n.z, r, g, b});
        out.vertices.push_back({p1.x, p1.y, p1.z, n.x, n.y, n.z, r, g, b});
        out.vertices.push_back({p2.x, p2.y, p2.z, n.x, n.y, n.z, r, g, b});
    }
    out.vertexCount = (int)out.vertices.size();
    return true;
}

MeshData ObjLoader::createBoxMesh(float sx, float sy, float sz, float r, float g, float b) {
    MeshData m;
    float hx = sx * 0.5f, hy = sy * 0.5f, hz = sz * 0.5f;
    auto quad = [&](Vec3 a, Vec3 b1, Vec3 c, Vec3 d, Vec3 n) {
        m.vertices.push_back({a.x,a.y,a.z,n.x,n.y,n.z,r,g,b});
        m.vertices.push_back({b1.x,b1.y,b1.z,n.x,n.y,n.z,r,g,b});
        m.vertices.push_back({c.x,c.y,c.z,n.x,n.y,n.z,r,g,b});
        m.vertices.push_back({a.x,a.y,a.z,n.x,n.y,n.z,r,g,b});
        m.vertices.push_back({c.x,c.y,c.z,n.x,n.y,n.z,r,g,b});
        m.vertices.push_back({d.x,d.y,d.z,n.x,n.y,n.z,r,g,b});
    };
    quad({-hx,hy,hz},{hx,hy,hz},{hx,hy,-hz},{-hx,hy,-hz},{0,1,0});
    quad({-hx,-hy,-hz},{hx,-hy,-hz},{hx,-hy,hz},{-hx,-hy,hz},{0,-1,0});
    quad({-hx,-hy,hz},{hx,-hy,hz},{hx,hy,hz},{-hx,hy,hz},{0,0,1});
    quad({hx,-hy,-hz},{-hx,-hy,-hz},{-hx,hy,-hz},{hx,hy,-hz},{0,0,-1});
    quad({-hx,-hy,-hz},{-hx,-hy,hz},{-hx,hy,hz},{-hx,hy,-hz},{-1,0,0});
    quad({hx,-hy,hz},{hx,-hy,-hz},{hx,hy,-hz},{hx,hy,hz},{1,0,0});
    m.vertexCount = (int)m.vertices.size();
    return m;
}
