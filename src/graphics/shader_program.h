#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram(const char *name);

    void SetVertexBuffer(void *data, GLsizei size, GLenum storageHint);

    void SetElementBuffer(int numElements, void *data,
            GLsizei size, GLenum storageHint);

    void SetModelMatrix(glm::mat4 &model);

    void SetViewMatrix(glm::mat4 &view);

    void SetProjectionMatrix(glm::mat4 &projection);

    bool LoadVertexShaderFromFile(const char *filename);

    bool LoadFragmentShaderFromFile(const char *filename);

    void Render();

    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;

    ShaderProgram(ShaderProgram &&other) : m_name(other.m_name),
    m_vertexBuffer(other.m_vertexBuffer),
    m_elementBuffer(other.m_elementBuffer),
    m_program(other.m_program),
    m_numElements(other.m_numElements),
    m_viewMatrix(other.m_viewMatrix),
    m_projectionMatrix(other.m_projectionMatrix),
    m_modelMatrix(other.m_modelMatrix)
    {
        other.m_vertexBuffer = 0;
        other.m_elementBuffer = 0;
        other.m_program = 0;
    }

    ShaderProgram &operator=(ShaderProgram &&other)
    {
        if (this != &other) {
            Cleanup();
            // gros bordel
            std::swap(m_vertexBuffer, other.m_vertexBuffer);
            std::swap(m_elementBuffer, other.m_elementBuffer);
            std::swap(m_program, other.m_program);
            m_viewMatrix = other.m_viewMatrix;
            m_projectionMatrix = other.m_projectionMatrix;
            m_modelMatrix = other.m_modelMatrix;
            m_numElements = other.m_numElements;
            m_name = other.m_name;
        }
        return *this;
    }

    ~ShaderProgram();

private:
    GLuint CreateBuffer(GLenum type, void *data, GLsizei size, GLenum storageHint);

    bool LoadShaderFromFile(const char* filename, GLenum type);
    
    void Cleanup();

    std::string m_name;

    GLuint m_vertexBuffer = 0;

    GLuint m_elementBuffer = 0;

    GLenum m_program = 0;

    unsigned int m_numElements = 0;

    glm::mat4 m_viewMatrix;

    glm::mat4 m_projectionMatrix;

    glm::mat4 m_modelMatrix;

};

#endif
