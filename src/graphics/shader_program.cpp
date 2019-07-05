#include "shader_program.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "util/glutil.h"
#include "util/file.h"

ShaderProgram::ShaderProgram(const char *name)
{
    m_name = name;
    m_numElements = 0;
    m_modelMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::mat4(1.0f);
    m_projectionMatrix = glm::mat4(1.0f);
    m_program = 0;
    m_vertexBuffer = 0;
    m_elementBuffer = 0;
}
// GL_STATIC_DRAW: the data will most likely not change at all or very rarely.
// GL_DYNAMIC_DRAW: the data is likely to change a lot.
// GL_STREAM_DRAW: the data will change every time it is drawn.
GLuint ShaderProgram::CreateBuffer(GLenum type, void *data, GLsizei size, GLenum storageHint)
{
    GLuint buffer;
    glGenBuffersARB(1, &buffer);
    glBindBufferARB(type, buffer);
    glBufferDataARB(type, size, data, storageHint);
    return buffer;
}

void ShaderProgram::SetVertexBuffer(void *data, GLsizei size, GLenum storageHint)
{
    glDeleteBuffers(1, &m_vertexBuffer);
    m_vertexBuffer = CreateBuffer(GL_ARRAY_BUFFER, data, size, storageHint);
    UpdateVertexArray();
}

void ShaderProgram::SetElementBuffer(int num, void *data, GLsizei size, GLenum storageHint)
{
    glDeleteBuffers(1, &m_elementBuffer);
    m_numElements = num;
    m_elementBuffer = CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, data, size, storageHint);
    UpdateVertexArray();
}

void ShaderProgram::SetNormalBuffer(void *data, GLsizei size, GLenum storageHint)
{
    glDeleteBuffers(1, &m_normalBuffer);
    m_normalBuffer = CreateBuffer(GL_ARRAY_BUFFER, data, size, storageHint);
    UpdateVertexArray();
}

void ShaderProgram::SetViewMatrix(glm::mat4 &view)
{
    m_viewMatrix = view;
    UpdateVertexArray();
}

void ShaderProgram::SetModelMatrix(glm::mat4 &model)
{
    m_modelMatrix = model;
    UpdateVertexArray();
}

void ShaderProgram::SetProjectionMatrix(glm::mat4 &projection)
{
    m_projectionMatrix = projection;
    UpdateVertexArray();
}

bool ShaderProgram::LoadShaderFromFile(const char* filename, GLenum type)
{
    if (m_program == 0) {
        m_program = glCreateProgramObjectARB();
    }
    std::string target;
    Sint64 len = ReadFile(filename, target);
    if (len<=0) {
        Error("Failed to open file '%s'", filename);
        return false;
    }
    GLenum shaderObject = glCreateShaderObjectARB(type);
    bool success = CompileShader(shaderObject, target, len);
    if (!success) {
        Error("Failed to compile shader '%s'", filename);
        glDeleteShader(shaderObject);
        return false;
    }
    glAttachObjectARB(m_program, shaderObject);
    glLinkProgramARB(m_program);
    glDeleteShader(shaderObject);
    return true;
}

bool ShaderProgram::LoadVertexShaderFromFile(const char* filename)
{
    return LoadShaderFromFile(filename, GL_VERTEX_SHADER);
}

bool ShaderProgram::LoadFragmentShaderFromFile(const char* filename)
{
    return LoadShaderFromFile(filename, GL_FRAGMENT_SHADER);
}

void ShaderProgram::UpdateVertexArray() {
    if (!m_program) {
        Warning("Tried to update vertex array without program");
        return;
    }
    glUseProgram(m_program);
    if (m_vertexArray == 0) {
        glGenVertexArrays(1, &m_vertexArray);
    }
    glBindVertexArray(m_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointerARB(
            glGetAttribLocationARB(m_program, "verts"),
            3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, (void *)0);
    glEnableVertexAttribArrayARB(
            glGetAttribLocationARB(m_program, "verts"));
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glVertexAttribPointerARB(
            glGetAttribLocationARB(m_program, "normal"),
            3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, (void *)0);
    glEnableVertexAttribArrayARB(
            glGetAttribLocationARB(m_program, "normal"));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    GLint matUniform = glGetUniformLocationARB(m_program, "model");
    if (matUniform > -1) {
        glUniformMatrix4fv(matUniform, 1, GL_FALSE,
                glm::value_ptr(m_modelMatrix));
    }
    matUniform = glGetUniformLocationARB(m_program, "view");
    if (matUniform > -1) {
        glUniformMatrix4fv(matUniform, 1, GL_FALSE,
                glm::value_ptr(m_viewMatrix));
    }
    matUniform = glGetUniformLocationARB(m_program, "projection");
    if (matUniform > -1) {
        glUniformMatrix4fv(matUniform, 1, GL_FALSE,
                glm::value_ptr(m_projectionMatrix));
    }
    glBindVertexArray(0);
}

void ShaderProgram::Render()
{
    if (m_program == 0) {
        Warning("Attempted to render uninitialized shader '%s'",
                m_name.c_str());
        return;
    }
    if (m_numElements == 0) {
        Warning("Attempted to render shader with 0 elements '%s'",
                m_name.c_str());
    }
    
    glUseProgram(m_program);
    glBindVertexArray(m_vertexArray);
    glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);

}

ShaderProgram::~ShaderProgram()
{
    Cleanup();
    //Debug("Cleaned up shader '%s'", m_name.c_str());
}

void ShaderProgram::Cleanup()
{
    m_viewMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::mat4(1.0f);
    m_projectionMatrix = glm::mat4(1.0f);
    m_numElements = 0;
    glDeleteProgram(m_program);
    m_program = 0;
    glDeleteBuffers(1, &m_vertexBuffer);
    m_vertexBuffer = 0;
    glDeleteBuffers(1, &m_elementBuffer);
    m_elementBuffer = 0;
    glDeleteVertexArrays(1, &m_vertexArray);
    m_vertexArray = 0;
}
