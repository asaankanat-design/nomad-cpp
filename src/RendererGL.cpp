#include "RendererGL.h"
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#endif
#include <cstdio>

static const char* VS_SRC = R"(
#version 300 es
precision mediump float;
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNorm;
layout(location=2) in vec3 aCol;
uniform mat4 uMVP;
uniform mat4 uModel;
out vec3 vCol;
out vec3 vNorm;
void main() {
    vCol = aCol;
    vNorm = mat3(uModel) * aNorm;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

static const char* FS_SRC = R"(
#version 300 es
precision mediump float;
in vec3 vCol;
in vec3 vNorm;
out vec4 FragColor;
void main() {
    vec3 n = normalize(vNorm);
    vec3 sunDir = normalize(vec3(0.55, 0.80, 0.40));
    float diff = max(dot(n, sunDir), 0.0);
    vec3 lit = vCol * (0.35 + diff * 0.85);
    FragColor = vec4(lit, 1.0);
}
)";

static unsigned int compileShader(unsigned int type, const char* src) {
    unsigned int s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

bool RendererGL::init() {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, VS_SRC);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, FS_SRC);
    prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);
    uMVP = glGetUniformLocation(prog, "uMVP");
    uModel = glGetUniformLocation(prog, "uModel");
    glEnable(GL_DEPTH_TEST);
    return prog != 0;
}

void RendererGL::beginFrame(int w, int h, const Mat4& viewProj) {
    vp = viewProj;
    glViewport(0, 0, w, h);
    glClearColor(0.11f, 0.17f, 0.24f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
}

void RendererGL::uploadMesh(MeshData& m) {
    if (m.vertices.empty()) return;
    glGenVertexArrays(1, &m.vao);
    glGenBuffers(1, &m.vbo);
    glBindVertexArray(m.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex3D), m.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void RendererGL::drawMesh(const MeshData& m, const Mat4& model) {
    if (!m.vao) return;
    Mat4 mvp = vp * model;
    glUniformMatrix4fv(uMVP, 1, GL_FALSE, mvp.m);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, model.m);
    glBindVertexArray(m.vao);
    glDrawArrays(GL_TRIANGLES, 0, m.vertexCount);
}

void RendererGL::shutdown() {
    if (prog) glDeleteProgram(prog);
}
