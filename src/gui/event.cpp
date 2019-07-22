#include "event.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include "SDL.h"
#pragma clang diagnostic pop
#include "window.h"
#include "scene.h"

void RunEventLoop()
{
    SDL_Event event;
    float oldx = 0.0f, oldy = 0.0f;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return;
            } else if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    WindowResize(event.window.data1,
                        event.window.data2); // TODO: Consolidate resize funcs
                    SceneWindowResize(event.window.data1,
                        event.window.data2);
                }
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    return;
                default:
                    KeyboardInput(event.key.keysym.scancode, SDL_KEYDOWN);
                    break;
                }
            } else if (event.type == SDL_KEYUP) {
                KeyboardInput(event.key.keysym.scancode, SDL_KEYUP);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // if (event.button.button & SDL_BUTTON_LMASK) {
                SDL_ShowCursor(SDL_FALSE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
                oldx = event.button.x;
                oldy = event.button.y;
                // }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                // if (event.button.button & SDL_BUTTON_LMASK) {
                SDL_ShowCursor(SDL_TRUE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
                SDL_WarpMouseInWindow(GetWindow(), oldx, oldy);
                // }
            } else if (event.type == SDL_MOUSEMOTION) {
                if(event.motion.state) { // & SDL_BUTTON_LMASK
                    MouseMotion(event.motion.xrel, event.motion.yrel);
                }
            }
        }
        ClearDepthBuffer();
        SceneRender();
        SwapBuffer();
        SDL_Delay(10);
    }
}
