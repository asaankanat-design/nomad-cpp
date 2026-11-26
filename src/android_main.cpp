#ifdef __ANDROID__
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <ctime>
#include "RendererGL.h"
#include "Engine.h"

struct AppState {
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    int width = 1280, height = 720;
    bool ready = false;
    bool dragging = false;
    float lastX = 0, lastY = 0;
    RendererGL renderer;
    Engine engine;
};

static void initEGL(android_app* app, AppState* st) {
    st->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(st->display, nullptr, nullptr);
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    EGLConfig cfg; EGLint num;
    eglChooseConfig(st->display, attribs, &cfg, 1, &num);
    st->surface = eglCreateWindowSurface(st->display, cfg, app->window, nullptr);
    const EGLint ctxAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    st->context = eglCreateContext(st->display, cfg, EGL_NO_CONTEXT, ctxAttribs);
    eglMakeCurrent(st->display, st->surface, st->surface, st->context);
    eglQuerySurface(st->display, st->surface, EGL_WIDTH, &st->width);
    eglQuerySurface(st->display, st->surface, EGL_HEIGHT, &st->height);
    st->renderer.init();
    st->engine.init(st->renderer);
    st->ready = true;
}

static void termEGL(AppState* st) {
    if (st->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(st->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (st->context != EGL_NO_CONTEXT) eglDestroyContext(st->display, st->context);
        if (st->surface != EGL_NO_SURFACE) eglDestroySurface(st->display, st->surface);
        eglTerminate(st->display);
    }
    st->display = EGL_NO_DISPLAY; st->surface = EGL_NO_SURFACE; st->context = EGL_NO_CONTEXT;
    st->ready = false;
}

static void handleCmd(android_app* app, int32_t cmd) {
    auto* st = (AppState*)app->userData;
    if (cmd == APP_CMD_INIT_WINDOW && app->window) initEGL(app, st);
    else if (cmd == APP_CMD_TERM_WINDOW) termEGL(st);
}

static int32_t handleInput(android_app* app, AInputEvent* ev) {
    auto* st = (AppState*)app->userData;
    if (!st->ready || AInputEvent_getType(ev) != AINPUT_EVENT_TYPE_MOTION) return 0;
    int act = AMotionEvent_getAction(ev) & AMOTION_EVENT_ACTION_MASK;
    float x = AMotionEvent_getX(ev, 0), y = AMotionEvent_getY(ev, 0);
    if (act == AMOTION_EVENT_ACTION_DOWN) { st->dragging = true; st->lastX = x; st->lastY = y; }
    else if (act == AMOTION_EVENT_ACTION_MOVE && st->dragging) {
        st->engine.panCamera(x - st->lastX, y - st->lastY);
        st->lastX = x; st->lastY = y;
    } else if (act == AMOTION_EVENT_ACTION_UP) { st->dragging = false; }
    return 1;
}

void android_main(android_app* app) {
    AppState st;
    app->userData = &st;
    app->onAppCmd = handleCmd;
    app->onInputEvent = handleInput;

    struct timespec prev; clock_gettime(CLOCK_MONOTONIC, &prev);
    while (true) {
        int events; android_poll_source* src;
        while (ALooper_pollAll(st.ready ? 0 : -1, nullptr, &events, (void**)&src) >= 0) {
            if (src) src->process(app, src);
            if (app->destroyRequested) { termEGL(&st); return; }
        }
        if (st.ready) {
            struct timespec now; clock_gettime(CLOCK_MONOTONIC, &now);
            float dt = (now.tv_sec - prev.tv_sec) + (now.tv_nsec - prev.tv_nsec) * 1e-9f;
            prev = now;
            if (dt > 0.05f) dt = 0.05f;
            st.engine.update(dt);
            st.engine.render(st.renderer, st.width, st.height);
            eglSwapBuffers(st.display, st.surface);
        }
    }
}
#endif
