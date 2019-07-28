#include "camera.h"
#include "SDL.h"
#include <glm/gtc/matrix_transform.hpp>


void CameraView::KeyboardInput(int code, int state)
{
    if (state == SDL_KEYDOWN) {
        m_downKeys.emplace(code);
        return;
    }

    m_downKeys.erase(code);
}

void CameraView::DoPhysics(float delaTimeTicks)
{
    m_position += glm::vec3(0.0f, m_velocity.y*delaTimeTicks, 0.0f);
    m_velocity -= glm::vec3(0.0f, GRAVITY*GRAVITY_MUL*delaTimeTicks, 0.0f);
    if (m_velocity.y < -TERMINAL_VEL) {
        m_velocity = glm::vec3(m_velocity.x, -TERMINAL_VEL, m_velocity.z);
    }
    if (m_position.y <= m_floor && m_velocity.y < 0.0) {
        m_position = glm::vec3(m_position.x, m_floor, m_position.z);
        m_velocity = glm::vec3(m_velocity.x, 0.0f, m_velocity.z);
    }
}

void CameraView::MoveCamera()
{
    // TODO Magic number removal
    const float t = SDL_GetTicks()/1000.0;
    float speedMod = CAMERA_MOVE_SPEED * (t-m_lastMoveTicks);

    if (m_downKeys.count(SDL_SCANCODE_W)) {
        m_position += glm::vec3(sin(m_rotation.y) * speedMod, 0.0f, cos(m_rotation.y) * speedMod);
    }
    if (m_downKeys.count(SDL_SCANCODE_S)) {
        m_position -= glm::vec3(sin(m_rotation.y) * speedMod, 0.0f, cos(m_rotation.y) * speedMod);
    }
    if (m_downKeys.count(SDL_SCANCODE_A)) {
        m_position += glm::vec3(cos(m_rotation.y) * speedMod, 0.0f, -sin(m_rotation.y) * speedMod);
    }
    if (m_downKeys.count(SDL_SCANCODE_D)) {
        m_position -= glm::vec3(cos(m_rotation.y) * speedMod, 0.0f, -sin(m_rotation.y) * speedMod);
    }
    if (m_downKeys.count(SDL_SCANCODE_E)) {
        m_floor += speedMod;
    }
    if (m_downKeys.count(SDL_SCANCODE_Q)) {
        m_floor -= speedMod;
    }
    if (m_downKeys.count(SDL_SCANCODE_R)) {
        m_floor = DEFAULT_FLOOR_Y;
    }
    if (m_downKeys.count(SDL_SCANCODE_SPACE)) {
        if (m_position.y <= m_floor+0.00001) {
            m_velocity = glm::vec3(m_velocity.x, JUMP_AMOUNT, m_velocity.z);
        }
    }

    DoPhysics(t-m_lastMoveTicks);

    m_lastMoveTicks = t;
}

glm::vec3 CameraView::GetRotation()
{
    return m_rotation;
}

glm::mat4 CameraView::GetViewMatrix()
{
    glm::vec3 right = glm::vec3(sin(m_rotation.y - PI/2.0f), 0,
            cos(m_rotation.y - PI/2.0f));
    glm::vec3 dir = glm::vec3(sin(m_rotation.y), -tan(m_rotation.x), cos(m_rotation.y));
    m_up = glm::cross(right, dir);
    MoveCamera();
    return glm::lookAt(m_position, m_position+dir, m_up);
}

glm::vec3 CameraView::GetPosition()
{
    return m_position;
}

void CameraView::MouseMotion(int deltaX, int deltaY)
{
    if (abs(deltaX) < MOUSE_MAX_CHANGE) {
        m_rotation -= glm::vec3(0.0f, deltaX/MOUSE_SENSITIVITY, 0.0f);
    }
    if (abs(deltaY) < MOUSE_MAX_CHANGE) {
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
