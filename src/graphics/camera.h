#ifndef CAMERA_H
#define CAMERA_H
#include <set>
#include <glm/glm.hpp>
#include <util/log.h>

#define PI 3.141592653f
#define GRAVITY 9.8f
#define GRAVITY_MUL 4.0f
#define MOUSE_MAX_CHANGE 30.0f
#define MOUSE_SENSITIVITY 400.0f
#define JUMP_AMOUNT GRAVITY*1.2f
#define TERMINAL_VEL GRAVITY*100.0f
#define DEFAULT_FLOOR_Y 2.0f
#define CAMERA_MOVE_SPEED 17.0f

class CameraView
{
public:
    void KeyboardInput(int code, int state);
    void MouseMotion(int deltaX, int deltaY);
    glm::mat4 GetViewMatrix();
    glm::vec3 GetPosition();
    glm::vec3 GetRotation();
private:
    glm::vec3 m_position = glm::vec3(0.0f, DEFAULT_FLOOR_Y, 20.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f, PI, 0.0f);
    glm::vec3 m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    std::set<int> m_downKeys;
    float m_lastMoveTicks = 0.0f;
    float m_floor = DEFAULT_FLOOR_Y; // y coordinate floor

    void DoPhysics(float deltaTimeTicks);
    void MoveCamera();
};


#endif
