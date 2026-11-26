#include "Engine.h"
#include <cmath>
#include <algorithm>

void Engine::init(RendererGL& r) {
    groundMesh = ObjLoader::createBoxMesh(400.0f, 0.5f, 400.0f, 0.28f, 0.36f, 0.22f);
    riverMesh  = ObjLoader::createBoxMesh(24.0f, 0.6f, 380.0f, 0.08f, 0.46f, 0.62f);
    hqMesh     = ObjLoader::createBoxMesh(14.0f, 5.5f, 14.0f, 0.72f, 0.16f, 0.16f);
    turretMesh = ObjLoader::createBoxMesh(1.2f, 1.0f, 3.2f, 0.85f, 0.65f, 0.15f);
    beamMesh   = ObjLoader::createBoxMesh(0.35f, 0.35f, 1.0f, 0.22f, 0.85f, 1.0f);

    if (!ObjLoader::loadNormalizedOBJ("models/tesla.obj", 4.8f, 0.68f, 0.16f, 0.16f, tankMesh))
        tankMesh = ObjLoader::createBoxMesh(3.2f, 1.4f, 4.8f, 0.68f, 0.16f, 0.16f);
    if (!ObjLoader::loadNormalizedOBJ("models/trooper.obj", 2.6f, 0.75f, 0.20f, 0.20f, trooperMesh))
        trooperMesh = ObjLoader::createBoxMesh(0.9f, 1.8f, 0.7f, 0.75f, 0.20f, 0.20f);

    for (auto* m : {&groundMesh, &riverMesh, &hqMesh, &tankMesh, &turretMesh, &trooperMesh, &beamMesh})
        r.uploadMesh(*m);

    buildings.push_back({nextId++, 0, "hub", -65.0f, 65.0f, 2500.0f, 2500.0f, true});
    buildings.push_back({nextId++, 1, "hub",  65.0f,-65.0f, 2500.0f, 2500.0f, true});

    spawnUnit(0, "tesla", -45.0f, 52.0f);
    spawnUnit(0, "tesla", -52.0f, 44.0f);
    spawnUnit(0, "trooper", -40.0f, 58.0f);
    spawnUnit(1, "tesla", 48.0f, -52.0f);
}

void Engine::spawnUnit(int fid, const std::string& type, float x, float z) {
    Unit u; u.id = nextId++; u.fid = fid; u.type = type; u.x = x; u.y = 0.7f; u.z = z;
    if (type == "trooper") { u.hp = u.maxHp = 140.0f; u.dmg = 20.0f; u.range = 22.0f; }
    units.push_back(u);
}

void Engine::update(float dt) {
    for (auto& u : units) {
        if (!u.alive) continue;
        if (u.cd > 0.0f) u.cd = std::max(0.0f, u.cd - dt);

        Unit* best = nullptr; float bestD = u.range;
        for (auto& o : units) {
            if (!o.alive || o.fid == u.fid) continue;
            float d = std::hypot(o.x - u.x, o.z - u.z);
            if (d <= bestD) { bestD = d; best = &o; }
        }
        if (best && u.cd <= 0.0f) {
            u.cd = 1.2f; u.yaw = std::atan2(best->x - u.x, best->z - u.z);
            best->hp -= u.dmg;
            if (best->hp <= 0.0f) best->alive = false;
        }
        if (u.hasTarget) {
            float dx = u.tx - u.x, dz = u.tz - u.z, d = std::hypot(dx, dz);
            if (d < 1.5f) u.hasTarget = false;
            else {
                u.yaw = std::atan2(dx, dz);
                float step = std::min(d, u.speed * dt);
                u.x += (dx / d) * step; u.z += (dz / d) * step;
            }
        }
    }
    aiTimer += dt;
    if (aiTimer >= 12.0f) {
        aiTimer = 0.0f;
        spawnUnit(1, "tesla", 55.0f, -55.0f);
        units.back().tx = -45.0f; units.back().tz = 45.0f; units.back().hasTarget = true;
    }
}

void Engine::render(RendererGL& r, int w, int h) {
    float hh = std::sin(camPitch) * camDist, rr = std::cos(camPitch) * camDist;
    Vec3 eye(camTarget.x + std::sin(camYaw)*rr, std::max(16.0f, hh), camTarget.z + std::cos(camYaw)*rr);
    Mat4 vp = Mat4::perspective(45.0f, (float)w / std::max(1, h), 1.0f, 1200.0f) * Mat4::lookAt(eye, camTarget, Vec3(0,1,0));
    r.beginFrame(w, h, vp);

    r.drawMesh(groundMesh, Mat4::translate({0, -0.25f, 0}));
    r.drawMesh(riverMesh,  Mat4::translate({0, -0.05f, 0}) * Mat4::rotateY(0.785f));
    for (auto& b : buildings) if (b.alive) r.drawMesh(hqMesh, Mat4::translate({b.x, 2.75f, b.z}));
    for (auto& u : units) {
        if (!u.alive) continue;
        const MeshData& md = (u.type == "trooper") ? trooperMesh : tankMesh;
        r.drawMesh(md, Mat4::translate({u.x, u.y, u.z}) * Mat4::rotateY(u.yaw));
    }
}

void Engine::panCamera(float dx, float dy) {
    float s = std::sin(camYaw), c = std::cos(camYaw), spd = camDist * 0.0024f;
    camTarget.x = std::clamp(camTarget.x + (-dx*c - dy*s)*spd, -180.0f, 180.0f);
    camTarget.z = std::clamp(camTarget.z + ( dx*s - dy*c)*spd, -180.0f, 180.0f);
}

void Engine::zoomCamera(float factor) { camDist = std::clamp(camDist * factor, 24.0f, 220.0f); }

void Engine::onTap(float normX, float normY, int w, int h) {
    // Ground plane raycast for unit selection & movement
    (void)normX; (void)normY; (void)w; (void)h;
    for (auto& u : units) if (u.alive && u.fid == 0) { u.selected = true; u.tx = camTarget.x; u.tz = camTarget.z; u.hasTarget = true; }
}
