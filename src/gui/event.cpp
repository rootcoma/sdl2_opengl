#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include "SDL.h"
#pragma clang diagnostic pop
#include "window.h"
#include "scene.h"

void RunEventLoop()
{
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return;
            } else if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    WindowResize(event.window.data1,
                        event.window.data2);
                }
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    return;
                }
            }
        }
        ClearDepthBuffer();
        SceneRender();
        SwapBuffer();
        SDL_Delay(10);
    }
}
