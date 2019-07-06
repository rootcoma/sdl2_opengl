#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

void CameraView::KeyboardInput(int code, int state)
{
    if (code > MAX_KEYS || code < 1) {
        Warning("Camera view ignoring keycode: %d > 255", code);
        return;
    }
    if (state == SDL_KEYDOWN) {
        m_downKeys[code] = true;
        return;
    }

    m_downKeys[code] = false;
}

void CameraView::DoPhysics(float delaTimeTicks)
{
    //cam_position[1] += cam_velocity[1]*msecSinceLastRun;
    m_position += glm::vec3(0.0f, m_velocity.y*delaTimeTicks, 0.0f);
    //cam_velocity[1] -= GRAVITY*GRAVITY_MUL*msecSinceLastRun;
    m_velocity -= glm::vec3(0.0f, GRAVITY*GRAVITY_MUL*delaTimeTicks, 0.0f);
    if(m_velocity.y < -TERMINAL_VEL) {
        //cam_velocity[1] = -TERMINAL_VEL;
        m_velocity = glm::vec3(m_velocity.x, -TERMINAL_VEL, m_velocity.z);
    }
    if(m_position.y <= m_floor && m_velocity.y < 0.0) {
        //cam_position[1] = FLOOR_Y;
        m_position = glm::vec3(m_position.x, m_floor, m_position.z);
        //cam_velocity[1] = 0.0;
        m_velocity = glm::vec3(m_velocity.x, 0.0f, m_velocity.z);
    }
}

void CameraView::MoveCamera()
{
    // TODO Magic number removal
    const float t = SDL_GetTicks()/1000.0;
    float speedMod = 17.0 * (t-m_lastMoveTicks);

    if (m_downKeys[SDL_SCANCODE_W]) {
        m_position += glm::vec3(sin(m_rotation.y) * speedMod, 0.0f, cos(m_rotation.y) * speedMod);
    }
    if (m_downKeys[SDL_SCANCODE_S]) {
        m_position -= glm::vec3(sin(m_rotation.y) * speedMod, 0.0f, cos(m_rotation.y) * speedMod);
    }
    if (m_downKeys[SDL_SCANCODE_A]) {
        m_position += glm::vec3(cos(m_rotation.y) * speedMod, 0.0f, -sin(m_rotation.y) * speedMod);
    }
    if (m_downKeys[SDL_SCANCODE_D]) {
        m_position -= glm::vec3(cos(m_rotation.y) * speedMod, 0.0f, -sin(m_rotation.y) * speedMod);
    }
    if (m_downKeys[SDL_SCANCODE_E]) {
        m_floor += speedMod;
    }
    if (m_downKeys[SDL_SCANCODE_Q]) {
        m_floor -= speedMod;
    }
    if (m_downKeys[SDL_SCANCODE_R]) {
        m_floor = 2.0;
    }
    if (m_downKeys[SDL_SCANCODE_SPACE]) {
        if (m_position.y <= m_floor+0.00001)
            //cam_velocity[1] = JUMP_AMOUNT;
            m_velocity = glm::vec3(m_velocity.x, JUMP_AMOUNT, m_velocity.z);
    }

    DoPhysics(t-m_lastMoveTicks);

    m_lastMoveTicks = t;
}

glm::mat4 CameraView::Update()
{
    glm::vec3 right = glm::vec3(sin(m_rotation.y - 3.14f/2.0f), 0,
            cos(m_rotation.y - 3.14f/2.0f)
        );
    glm::vec3 dir = glm::vec3(sin(m_rotation.y), -tan(m_rotation.x), cos(m_rotation.y));
    m_up = glm::cross(right, dir); // um, ya..
    MoveCamera();
    return glm::lookAt(m_position, m_position+dir, m_up);
}

void CameraView::MouseMotion(int deltaX, int deltaY)
{
    if(abs(deltaX) < MOUSE_MAX_CHANGE) {
        //mouse_rot[1] -= deltaX/MOUSE_SENSITIVITY;
        m_rotation -= glm::vec3(0.0f, deltaX/MOUSE_SENSITIVITY, 0.0f);
    }
    if(abs(deltaY) < MOUSE_MAX_CHANGE) {
        float angle = m_rotation.x;
        angle += deltaY/MOUSE_SENSITIVITY;
        float maxAngle = 89.9*PI/180.0;
        if (angle > maxAngle)
            angle = maxAngle;
        if (angle < -maxAngle)
            angle = -maxAngle;
        m_rotation = glm::vec3(angle, m_rotation.y, m_rotation.z);
    }
}