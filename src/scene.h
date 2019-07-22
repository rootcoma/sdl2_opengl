#ifndef SCENE_H
#define SCENE_H
#include <cstdint>

void SceneRender();

bool SceneInit();

void KeyboardInput(int code, int state);

void MouseMotion(int deltaX, int deltaY);

void SceneWindowResize(uint32_t width, uint32_t height);

#endif
