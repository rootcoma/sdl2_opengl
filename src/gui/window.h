#ifndef WINDOW_H
#define WINDOW_H
#include <utility>
#include <cstdint>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include <SDL.h>
#pragma clang diagnostic pop

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

SDL_GLContext GetGLContext();

SDL_Window* GetWindow();

int32_t GetWindowWidth();

int32_t GetWindowHeight();

std::pair<int32_t, int32_t> GetWindowDimensions(); 

void ClearDepthBuffer();

void SwapBuffer();

bool CreateWindow();

void DestroyWindow();

void WindowResize(int32_t newWidth, int32_t newHeight);

void SetWireframe(bool wireframe);

#endif
