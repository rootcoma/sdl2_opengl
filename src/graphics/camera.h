#ifndef CAMERA_H
#define CAMERA_H
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include <SDL.h>
#pragma clang diagnostic pop
#include <glm/glm.hpp>
#include <util/log.h>
#define PI 3.14159
#define GRAVITY 9.8
#define GRAVITY_MUL 4.0
#define MAX_KEYS 255
#define MOUSE_MAX_CHANGE 30.0
#define MOUSE_SENSITIVITY 400.0
#define JUMP_AMOUNT GRAVITY*1.2
#define TERMINAL_VEL GRAVITY*100.0

class CameraView
{
    // its complicated
public:
    void KeyboardInput(int code, int state);
    void MouseMotion(int deltaX, int deltaY);
    glm::mat4 Update(); // TODO: I don't like this name

private:
    glm::vec3 m_position = glm::vec3(0.0f, 1.5f, 20.0); // cam position
    //glm::vec3 m_lookAt = glm::vec3(0.0, 0.0f, 0.0);    // look at
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);        // Up normal
    glm::vec3 m_rotation = glm::vec3(0.0f, PI, 0.0f);
    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    bool m_downKeys[MAX_KEYS+1];
    float m_lastMoveTicks = 0.0; // millis
    float m_floor = 2.0;  // y coordinate
    // bordel is hidden here
    void DoPhysics(float deltaTimeTicks);
    void MoveCamera();
};


#endif
