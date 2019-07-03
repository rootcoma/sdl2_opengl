#include "window.h"
#include <GL/glew.h>
#include "util/log.h"

SDL_Window *window = NULL;

SDL_GLContext glContext = NULL;

SDL_GLContext GetGLContext()
{
    return glContext;
}

SDL_Window* GetWindow()
{
    return window;
}

static void Fail(const char* msg)
{
    Error(msg);
    Error(SDL_GetError());
}

static bool InitSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Fail("Failed to initialize SDL2");
        return false;
    }
    return true;
}

static bool CreateSDLWindow()
{
    window = SDL_CreateWindow("sdl2", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        Fail("Failed to create SDL2 window");
        return false;
    }
    glContext = SDL_GL_CreateContext(window);

    return true;
}

static bool SetupGLEW()
{
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        Error("Failed to initialize GLEW");
        Error((const char *)glewGetErrorString(err));
        return false;
    }
#endif
    return true;
}

static bool SetGLAttributes()
{
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)) {
        Fail("Failed to set GL_CONTEXT_MAJOR_VERSION");
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2)) {
        Fail("Failed to set GL_CONTEXT_MINOR_VERSION");
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE)) {
        Fail("Failed to set GL_CONTEXT_PROFILE_MASK");
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
        Fail("Failed to set GL_DOUBLEBUFFER");
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)) {
        Fail("Failed to set GL_DEPTH_SIZE");
        return false;
    }
    if (SDL_GL_SetSwapInterval(0)) {
        Fail("Failed to disable vsync");
        return false;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    return true;
}

void SwapBuffer()
{
    SDL_GL_SwapWindow(window);
}

void ClearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

static void ClearColorBuffer()
{
    glClearColor(1.0, 1.0, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void DestroyWindow()
{
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void WindowResize(Sint32 newWidth, Sint32 newHeight)
{
    //Debug("window resized to %ld, %ld", newWidth, newHeight);
    glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

bool CreateWindow()
{
    if (!InitSDL()) {
        return false;
    }
    if (!CreateSDLWindow()) {
        return false;
    }
    if (!SetGLAttributes()) {
        return false;
    }
    if (!SetupGLEW()) {
        return false;
    }

    ClearColorBuffer();
    SwapBuffer();

    return true;
}
