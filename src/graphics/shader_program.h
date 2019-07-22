#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

struct ShaderMaterial_t {
    GLuint diffuseSampler;
    GLuint specularSampler;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    GLfloat shininess;
    GLint type; // 0 = ambient, float diffuse
                // 1 = Sampler Texture0(diffuse) Texture1(specular)
};

struct ShaderLight_t {
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // https://learnopengl.com/Lighting/Light-casters
    GLfloat innerCutOff;
    GLfloat outerCutOff; // Angle
    // http://www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation
    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;
    GLint type; // 0 Directional (sun)
                // 1 Point (360 lamp)
                // 2 Spotlight
};

class ShaderProgram
{
    // Wraps an opengl shader. With parts taken from
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#RAII_and_hidden_destructor_calls
public:
    ShaderProgram(const char *name);

    void SetVertexBuffer(void *data, GLsizei size, GLenum storageHint);

    void SetNormalBuffer(void *data, GLsizei size, GLenum storageHint);

    void SetUVBuffer(void *data, GLsizei size, GLenum storageHint);

    void SetLight(const std::string &name, ShaderLight_t &light);

    void SetElementBuffer(int numElements, void *data,
            GLsizei size, GLenum storageHint);

    void SetShininess(GLfloat shininess);

    void SetMaterial(ShaderMaterial_t &material);

    void SetModelMatrix(const glm::mat4 &model);

    void RotateModelMatrix(float angleRadians, const glm::vec3 &up);

    void SetViewMatrix(const glm::mat4 &view, const glm::vec3 &camPos);

    void SetProjectionMatrix(const glm::mat4 &projection);

    bool LoadVertexShaderFromFile(const char *filename);

    bool LoadFragmentShaderFromFile(const char *filename);

    void Render();

    // Sans default constructor
    ShaderProgram() = delete;
    // Copies are not allowed
    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram& operator=(const ShaderProgram &) = delete;

    // Moves are allowed
    ShaderProgram(ShaderProgram &&other);
    ShaderProgram& operator=(ShaderProgram &&other);

    ~ShaderProgram();

private:
    void SetUniformInt(const std::string &varName, GLint i);

    void SetUniformUInt(const std::string &varName, GLuint u);

    void SetUniformVec3(const std::string &varName, glm::vec3 &v);

    void SetUniformMat4(const std::string &varName, glm::mat4 &mat);

    void SetUniformFloat(const std::string &varName, GLfloat f);

    void BindVAO();

    void UnbindVAO();

    GLuint CreateBuffer(GLenum type, void *data, GLsizei size, GLenum storageHint);

    bool LoadShaderFromFile(const char *filename, GLenum type);
    
    void Cleanup();

    std::string m_name;

    GLuint m_vertexArray = 0;

    GLuint m_vertexBuffer = 0;

    GLuint m_normalBuffer = 0;

    GLuint m_uvBuffer = 0;

    GLuint m_elementBuffer = 0;

    GLenum m_program = 0;

    unsigned int m_numElements = 0;

    glm::mat4 m_viewMatrix;

    glm::vec3 m_cameraPosition;

    glm::mat4 m_projectionMatrix;

    glm::mat4 m_modelMatrix;

    ShaderMaterial_t m_material;

    std::map<std::string, ShaderLight_t> m_lights;

};

#endif
