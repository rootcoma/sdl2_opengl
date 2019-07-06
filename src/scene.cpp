#include "scene.h"
#include <vector>
#include <array>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>
#include "graphics/shader_program.h"
#include "util/log.h"
#include "graphics/stl_parser.h"
#include "graphics/camera.h"

#define FOV 45.0f
#define ASPECT_RATIO (16.0f/9.0f)

static const char *models[] = {
        //"models/block100.stl",
        //"models/bottle.stl",
        "models/cube.stl",
        //"models/humanoid.stl",
        //"models/liver.stl",
        //"models/magnolia.stl",
        //"models/space_invader_magnet.stl",
        "models/sphere.stl",
        //"models/tiler_3d.stl",
        //"models/Suzanne.stl",
        //"models/unit_circle_2x2.stl",
};

static std::vector<ShaderProgram> shaderPrograms;
static std::vector<STLSolid_t> allSolids;
static CameraView camera;
static glm::mat4 projectionMatrix = glm::perspective(glm::radians(FOV), ASPECT_RATIO, 0.1f, 1000.0f);
static glm::mat4 modelMatrix = glm::mat4(1.0f);

static bool CreateModelShaderProgram(const char *name,
        std::vector<glm::vec3> &vertices, std::vector<GLuint> &elements,
        std::vector<glm::vec3> &normals)
{
    shaderPrograms.push_back(ShaderProgram(name));
    ShaderProgram *shader = &shaderPrograms[shaderPrograms.size()-1];
    if (!shader->LoadFragmentShaderFromFile("shaders/model.frs")) {
        return false;
    }
    if (!shader->LoadVertexShaderFromFile("shaders/model.vs")) {
        return false;
    }
    if (vertices.size() != normals.size()) {
        Warning("vertices.size() != normals.size(): (%d, %d)",
                vertices.size(), normals.size());
        return false;
    }

    size_t numFloats = vertices.size() * 6; // vec3 (vertex) + vec3 (nromal)
    std::vector<glm::vec3> combinedVertexInformation;
    for (int i=0; i<vertices.size(); i++) {
        combinedVertexInformation.push_back(vertices[i]);
        combinedVertexInformation.push_back(normals[i]);
    }
    shader->SetVertexBuffer((void *)&combinedVertexInformation[0], sizeof(GLfloat)*numFloats,
            GL_STATIC_DRAW);
    shader->SetElementBuffer(elements.size(), (void *)&elements[0],
            sizeof(GLuint)*elements.size(), GL_STATIC_DRAW);
    glm::mat4 view = camera.Update();
    shader->SetViewMatrix(view);

    shader->SetModelMatrix(modelMatrix);

    shader->SetProjectionMatrix(projectionMatrix);
    Debug("Set up shader '%s'", name);
    return true;
}

static bool ParseSTLModel(const char* filename, std::vector<STLSolid_t> &solids)
{
    bool ret = ParseSTLFile(filename, solids);
    if (!ret) {
        Error("Failed to parse STL file '%s'", filename);
        return false;
    }
    //Debug("Parsed model '%s' successfully, total models: %d", filename, solids.size());
    if (solids.size() <= 0) {
        return ret;
    }
    //Debug("Number of facets in solids[%d]: %lu", solids.size()-1,
    //        solids[solids.size()-1].facets.size());
    return ret;  
}

void SceneRender()
{
    glm::mat4 cam = camera.Update();
    float fun = 0.001f;
    for (auto it=shaderPrograms.begin(); it!=shaderPrograms.end(); it++) {
        it->RotateModelMatrix(fun, glm::vec3(0, 1, 0));
        it->RotateModelMatrix(-fun*0.3f, glm::vec3(1, 0, 0));
        it->SetViewMatrix(cam);
        it->Render();
        fun = -fun;
    }
}

bool SceneInit()
{
    for (int i=0; i<sizeof(models)/sizeof(const char *); i++) {
        if (!ParseSTLModel(models[i], allSolids)) {
            return false;
        }
    }
    for (int i=0; i<allSolids.size(); i++) {
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> vertices;
        std::vector<GLuint> elements;


        ConvertSolidToNormalVertexElements(allSolids[i], normals, vertices,
                elements);
        Debug("normals.size(): %lu, vertices.size(): %lu, elements.size(): %lu",
                normals.size(), vertices.size(), elements.size());
        if (!CreateModelShaderProgram("test", vertices, elements, normals)) {
            return false;
        }
    }

    return true;
}

void KeyboardInput(int code, int state)
{
    camera.KeyboardInput(code, state);
}

void MouseMotion(int deltaX, int deltaY)
{
    camera.MouseMotion(deltaX, deltaY);
}
