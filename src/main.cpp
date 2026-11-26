#include <SDL2/SDL.h>
#include "RendererGL.h"
#include "Engine.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_Window* win = SDL_CreateWindow("Nomad Command C++ OpenGL ES",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GLContext ctx = SDL_GL_CreateContext(win);

    RendererGL renderer;
    renderer.init();
    Engine engine;
    engine.init(renderer);

    bool running = true, dragging = false;
    int lastX = 0, lastY = 0;
    Uint64 prev = SDL_GetPerformanceCounter();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN) { dragging = true; lastX = e.button.x; lastY = e.button.y; }
            else if (e.type == SDL_MOUSEBUTTONUP) { dragging = false; }
            else if (e.type == SDL_MOUSEMOTION && dragging) {
                engine.panCamera((float)(e.motion.x - lastX), (float)(e.motion.y - lastY));
                lastX = e.motion.x; lastY = e.motion.y;
            } else if (e.type == SDL_MOUSEWHEEL) {
                engine.zoomCamera(e.wheel.y > 0 ? 0.9f : 1.1f);
            } else if (e.type == SDL_FINGERMOTION) {
                int w, h; SDL_GetWindowSize(win, &w, &h);
                engine.panCamera(e.tfinger.dx * w, e.tfinger.dy * h);
            }
        }
        Uint64 now = SDL_GetPerformanceCounter();
        float dt = (float)(now - prev) / (float)SDL_GetPerformanceFrequency();
        prev = now;
        if (dt > 0.05f) dt = 0.05f;

        int w, h; SDL_GetWindowSize(win, &w, &h);
        engine.update(dt);
        engine.render(renderer, w, h);
        SDL_GL_SwapWindow(win);
    }
    renderer.shutdown();
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
