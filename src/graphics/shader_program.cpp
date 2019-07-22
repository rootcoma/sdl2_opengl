#include "shader_program.h"
#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "graphics/glutil.h"
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
    m_normalBuffer = 0;
    m_uvBuffer = 0;
    m_elementBuffer = 0;
    memset((void *)&m_material, 0, sizeof(ShaderMaterial_t));
    //m_lights.reset();
}

// GL_STATIC_DRAW:  the data will most likely not change at
//                  all or very rarely.
// GL_DYNAMIC_DRAW: the data is likely to change a lot.
// GL_STREAM_DRAW:  the data will change every time it is drawn.
GLuint ShaderProgram::CreateBuffer(GLenum type, void *data, GLsizei size, GLenum storageHint)
{
    GLuint buffer;
    glGenBuffersARB(1, &buffer);
    glBindBufferARB(type, buffer);
    glBufferDataARB(type, size, data, storageHint);
    return buffer;
}

void ShaderProgram::SetLight(const std::string &name, ShaderLight_t &light)
{
    GLint locationInShader = glGetUniformLocationARB(m_program, "lights[0].type");
    if (locationInShader < 0) {
        Error("Cannot add light '%s' to shader with no lights array",
                name.c_str());
        return;
    }
    std::pair<std::map<std::string, ShaderLight_t>::iterator, bool> op = \
            m_lights.emplace(std::make_pair(name, light));
    if (!op.second) {
        Warning("No light '%s' was inserted", name.c_str());
        return;
    }
    BindVAO();
    int i = 0;
    std::string currVarPrefix;
    std::string currVar;
    for (auto it=m_lights.begin(); it!=m_lights.end(); it++) {
        currVarPrefix = "lights[" + std::to_string(i++) + "].";
        currVar = currVarPrefix + "position"; // vec3
        SetUniformVec3(currVar, it->second.position);
        currVar = currVarPrefix + "direction"; // vec3
        SetUniformVec3(currVar, it->second.direction);
        currVar = currVarPrefix + "ambient"; // vec3
        SetUniformVec3(currVar, it->second.ambient);
        currVar = currVarPrefix + "diffuse"; // vec3
        SetUniformVec3(currVar, it->second.diffuse);
        currVar = currVarPrefix + "specular"; // vec3
        SetUniformVec3(currVar, it->second.specular);
        currVar = currVarPrefix + "innerCutOff"; // GLfloat
        SetUniformFloat(currVar, it->second.innerCutOff);
        currVar = currVarPrefix + "outerCutOff"; // GLfloat
        SetUniformFloat(currVar, it->second.outerCutOff);
        currVar = currVarPrefix + "constant"; // GLfloat
        SetUniformFloat(currVar, it->second.constant);
        currVar = currVarPrefix + "linear"; // GLfloat
        SetUniformFloat(currVar, it->second.linear);
        currVar = currVarPrefix + "quadratic" ; // GLfloat
        SetUniformFloat(currVar, it->second.quadratic);
        currVar = currVarPrefix + "type"; // GLint
        SetUniformInt(currVar, it->second.type);
    }
    SetUniformInt("numLights", (GLint)m_lights.size());
    UnbindVAO();
}

void ShaderProgram::SetVertexBuffer(void *data, GLsizei size, GLenum storageHint)
{
    glDeleteBuffers(1, &m_vertexBuffer);
    m_vertexBuffer = CreateBuffer(GL_ARRAY_BUFFER, data, size, storageHint);
    BindVAO();
    // facetVertex
    GLint locationInShader = glGetAttribLocationARB(m_program, "facetVertex");
    if (locationInShader > -1) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glVertexAttribPointerARB(locationInShader, 3, GL_FLOAT, GL_FALSE,
                sizeof(GLfloat)*3, (void *)0);
        glEnableVertexAttribArrayARB(locationInShader);
    }
    UnbindVAO();
}

void ShaderProgram::SetNormalBuffer(void *data, GLsizei size, GLenum storageHint)
{
    glDeleteBuffers(1, &m_normalBuffer);
    m_normalBuffer = CreateBuffer(GL_ARRAY_BUFFER, data, size, storageHint);
    BindVAO();
    // facetNormal
    GLint locationInShader = glGetAttribLocationARB(m_program, "facetNormal");
    if (locationInShader > -1) {
        glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
        glVertexAttribPointerARB(locationInShader, 3, GL_FLOAT, GL_FALSE,
                sizeof(GLfloat)*3, (void *)0);
        glEnableVertexAttribArrayARB(locationInShader);
    }
    UnbindVAO();
}

void ShaderProgram::SetUVBuffer(void *data, GLsizei size, GLenum storageHint)
{
    glDeleteBuffers(1, &m_uvBuffer);
    m_uvBuffer = CreateBuffer(GL_ARRAY_BUFFER, data, size, storageHint);
    BindVAO();
    // facetUV
    GLint locationInShader = glGetAttribLocationARB(m_program, "facetUV");
    if (locationInShader > -1) {
        glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
        glVertexAttribPointerARB(locationInShader, 2, GL_FLOAT, GL_FALSE,
                sizeof(GLfloat)*2, (void *)0);
        glEnableVertexAttribArrayARB(locationInShader);
    }
    UnbindVAO();
}

void ShaderProgram::SetShininess(GLfloat shininess)
{
    m_material.shininess = shininess;
}

void ShaderProgram::SetElementBuffer(int num, void *data, GLsizei size,
        GLenum storageHint)
{
    glDeleteBuffers(1, &m_elementBuffer);
    m_numElements = num;
    m_elementBuffer = CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, data, size,
            storageHint);
    BindVAO();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    UnbindVAO();
}

void ShaderProgram::SetViewMatrix(const glm::mat4 &view, const glm::vec3 &camPos)
{
    m_viewMatrix = view;
    m_cameraPosition = camPos;
    BindVAO();
    SetUniformMat4("view", m_viewMatrix);
    SetUniformVec3("cameraPosition", m_cameraPosition);
    UnbindVAO();
}

void ShaderProgram::SetUniformFloat(const std::string &varName, GLfloat f)
{
    GLint locationInShader = glGetUniformLocationARB(m_program,
            varName.c_str());
    if (locationInShader < 0) {
        Warning("Could not find GLfloat uniform '%s' in shader '%s'", varName.c_str(),
                m_name.c_str());
        return;
    }
    glUniform1f(locationInShader, f);
}

void ShaderProgram::SetUniformUInt(const std::string &varName, GLuint u)
{
    GLint locationInShader = glGetUniformLocationARB(m_program,
            varName.c_str());
    if (locationInShader < 0) {
        Warning("Could not find GLuint uniform '%s' in shader '%s'", varName.c_str(),
                m_name.c_str());
        return;
    }
    glUniform1ui(locationInShader, u);
}

void ShaderProgram::SetUniformVec3(const std::string &varName, glm::vec3 &v)
{
    GLint locationInShader = glGetUniformLocationARB(m_program,
            varName.c_str());
    if (locationInShader < 0) {
        Warning("Could not find vec3 uniform '%s' in shader '%s'", varName.c_str(),
                m_name.c_str());
        return;
    }
    glUniform3fv(locationInShader, 1, glm::value_ptr(v));
}

void ShaderProgram::SetUniformInt(const std::string &varName, GLint i)
{
    GLint locationInShader = glGetUniformLocationARB(m_program,
            varName.c_str());
    if (locationInShader < 0) {
        Warning("Could not find GLint uniform '%s' in shader '%s'", varName.c_str(),
                m_name.c_str());
        return;
    }
    glUniform1i(locationInShader, i);
}


void ShaderProgram::SetUniformMat4(const std::string &varName, glm::mat4 &mat)
{
    GLint locationInShader = glGetUniformLocationARB(m_program,
            varName.c_str());
    if (locationInShader < 0) {
        Warning("Could not find mat4 uniform '%s' in shader '%s'",
                varName.c_str(), m_name.c_str());
        return;
    }
    glUniformMatrix4fv(locationInShader, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::SetMaterial(ShaderMaterial_t &material)
{
    m_material = material;
    BindVAO();
    // GLint diffuseSampler; // TEXTURE0
    SetUniformUInt("material.diffuseSampler", material.diffuseSampler);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_material.diffuseSampler);
    // GLint specularSampler; // TEXTURE1
    SetUniformUInt("material.specularSampler", material.specularSampler);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_material.specularSampler);
    // glm::vec3 ambient;
    SetUniformVec3("material.ambient", material.ambient);
    // glm::vec3 diffuse;
    SetUniformVec3("material.diffuse", material.diffuse);
    // glm::vec3 specular;
    SetUniformVec3("material.specular", material.specular);
    // GLfloat shininess;
    SetUniformFloat("material.shininess", material.shininess);
    // GLint type;
    SetUniformInt("material.type", material.type);
    UnbindVAO();
}

void ShaderProgram::SetModelMatrix(const glm::mat4 &model)
{
    m_modelMatrix = model;
    BindVAO();
    SetUniformMat4("model", m_modelMatrix);
    UnbindVAO();
}

void ShaderProgram::RotateModelMatrix(float angleRadians, const glm::vec3 &up)
{
    m_modelMatrix = glm::rotate(m_modelMatrix, angleRadians, up);
    BindVAO();
    SetUniformMat4("model", m_modelMatrix);
    UnbindVAO();
}

void ShaderProgram::SetProjectionMatrix(const glm::mat4 &projection)
{
    m_projectionMatrix = projection;
    BindVAO();
    SetUniformMat4("projection", m_projectionMatrix);
    UnbindVAO();
}

bool ShaderProgram::LoadShaderFromFile(const char* filename, GLenum type)
{
    if (m_program == 0) {
        m_program = glCreateProgramObjectARB();
    }
    std::string target;
    int64_t len = ReadFile(filename, target);
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

void ShaderProgram::BindVAO()
{
    if (!m_program) {
        Warning("Tried to update vertex array object without program");
        return;
    }
    glUseProgram(m_program);
    if (m_vertexArray == 0) {
        glGenVertexArrays(1, &m_vertexArray);
    }
    glBindVertexArray(m_vertexArray);
}

void ShaderProgram::UnbindVAO()
{
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
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
    Debug("Cleaned up shader '%s'", m_name.c_str());
}

void ShaderProgram::Cleanup()
{
    // It is safe to call any glDelete function on 0
    m_viewMatrix = glm::mat4(1.0f);
    m_modelMatrix = glm::mat4(1.0f);
    m_projectionMatrix = glm::mat4(1.0f);
    m_numElements = 0;
    glDeleteProgram(m_program);
    m_program = 0;
    glDeleteBuffers(1, &m_uvBuffer);
    m_uvBuffer = 0;
    glDeleteBuffers(1, &m_vertexBuffer);
    m_vertexBuffer = 0;
    glDeleteBuffers(1, &m_elementBuffer);
    m_elementBuffer = 0;
    glDeleteVertexArrays(1, &m_vertexArray);
    m_vertexArray = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram &&other)
{
    if (this != &other) {
        Cleanup();
        // When a move is made of the program OpenGL
        // references need to be set to 0 before a
        // destructor is called on the `other`
        std::swap(m_vertexArray, other.m_vertexArray);
        std::swap(m_vertexBuffer, other.m_vertexBuffer);
        std::swap(m_uvBuffer, other.m_uvBuffer);
        std::swap(m_elementBuffer, other.m_elementBuffer);
        std::swap(m_program, other.m_program);
        std::swap(m_lights, other.m_lights);
        memcpy((void *)&m_material, (void *)&other.m_material,
                sizeof(ShaderMaterial_t));
        m_viewMatrix = other.m_viewMatrix;
        m_cameraPosition = other.m_cameraPosition;
        m_projectionMatrix = other.m_projectionMatrix;
        m_modelMatrix = other.m_modelMatrix;
        m_numElements = other.m_numElements;
        m_name = other.m_name;
    }
    return *this;
}

ShaderProgram::ShaderProgram(ShaderProgram &&other) : m_name(other.m_name),
        m_vertexArray(other.m_vertexArray),
        m_vertexBuffer(other.m_vertexBuffer),
        m_normalBuffer(other.m_normalBuffer),
        m_uvBuffer(other.m_uvBuffer),
        m_elementBuffer(other.m_elementBuffer),
        m_program(other.m_program),
        m_numElements(other.m_numElements),
        m_viewMatrix(other.m_viewMatrix),
        m_cameraPosition(other.m_cameraPosition),
        m_projectionMatrix(other.m_projectionMatrix),
        m_modelMatrix(other.m_modelMatrix),
        m_material(other.m_material),
        m_lights(other.m_lights)
{
    // Set new ShaderProgram to have
    // references to the OpenGL buffers and program
    // and unset GLuint references on `other` so that
    // when Cleanup() is called in destructor our
    // program and buffers won't be destroyed
    other.m_vertexBuffer = 0;
    other.m_elementBuffer = 0;
    other.m_uvBuffer = 0;
    other.m_program = 0;
    other.m_vertexArray = 0;
}
