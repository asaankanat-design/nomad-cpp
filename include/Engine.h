#pragma once
#include <vector>
#include <string>
#include "Math3D.h"
#include "RendererGL.h"

struct Unit {
    int id, fid;
    std::string type;
    float x, y, z, yaw;
    float tx, tz;
    bool hasTarget = false, selected = false, alive = true;
    float hp = 400.0f, maxHp = 400.0f, speed = 13.0f, range = 28.0f, dmg = 55.0f, cd = 0.0f;
};

struct Building {
    int id, fid;
    std::string type;
    float x, z, hp, maxHp;
    bool alive = true;
};

class Engine {
public:
    void init(RendererGL& r);
    void update(float dt);
    void render(RendererGL& r, int w, int h);
    void onTap(float normX, float normY, int w, int h);
    void panCamera(float dx, float dy);
    void zoomCamera(float factor);
    void spawnUnit(int fid, const std::string& type, float x, float z);
private:
    Vec3 camTarget{-50.0f, 0.0f, 50.0f};
    float camDist = 90.0f, camYaw = 0.65f, camPitch = 0.72f;
    float aiTimer = 0.0f;
    int nextId = 1;
    std::vector<Unit> units;
    std::vector<Building> buildings;
    MeshData groundMesh, riverMesh, hqMesh, tankMesh, turretMesh, trooperMesh, beamMesh;
};
