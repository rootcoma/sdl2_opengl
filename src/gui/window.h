#ifndef WINDOW_H
#define WINDOW_H
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include <SDL.h>
#pragma clang diagnostic pop

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480

SDL_GLContext GetGLContext();

SDL_Window* GetWindow();

void ClearDepthBuffer();

void SwapBuffer();

bool CreateWindow();

void DestroyWindow();

void WindowResize(Sint32 newWidth, Sint32 newHeight);

#endif
