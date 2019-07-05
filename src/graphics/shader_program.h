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

    void SetNormalBuffer(void *data, GLsizei size, GLenum storageHint);

    void SetElementBuffer(int numElements, void *data,
            GLsizei size, GLenum storageHint);

    void SetModelMatrix(glm::mat4 &model);

    void RotateModelMatrix(float angleRadians, glm::vec3 up);

    void SetViewMatrix(glm::mat4 &view);

    void SetProjectionMatrix(glm::mat4 &projection);

    bool LoadVertexShaderFromFile(const char *filename);

    bool LoadFragmentShaderFromFile(const char *filename);

    void Render();

    // Copies cannot be made of this object
    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram& operator=(const ShaderProgram &) = delete;

    // Moves are allowed
    ShaderProgram(ShaderProgram &&other);
    ShaderProgram& operator=(ShaderProgram &&other);

    ~ShaderProgram();

private:
    void UpdateVertexArray();

    GLuint CreateBuffer(GLenum type, void *data, GLsizei size, GLenum storageHint);

    bool LoadShaderFromFile(const char* filename, GLenum type);
    
    void Cleanup();

    std::string m_name;

    GLuint m_vertexArray = 0;

    GLuint m_vertexBuffer = 0;

    GLuint m_normalBuffer = 0;

    GLuint m_elementBuffer = 0;

    GLenum m_program = 0;

    unsigned int m_numElements = 0;

    glm::mat4 m_viewMatrix;

    glm::mat4 m_projectionMatrix;

    glm::mat4 m_modelMatrix;

};

#endif
