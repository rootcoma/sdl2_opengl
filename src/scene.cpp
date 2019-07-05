#include "scene.h"
#include <vector>
#include <array>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>
#include "graphics/shader_program.h"
#include "util/log.h"
#include "graphics/stl_parser.h"

// static const GLfloat fullscreenVertices[] = {
//     -1.0f, -1.0f, -1.0f, // bottom left
//      1.0f, -1.0f, -1.0f, // bottom right
//     -1.0f,  1.0f, -1.0f, // top left
//      1.0f,  1.0f, -1.0f, // top right
// };

// static const GLuint fullscreenElements[] = {
//     0, 1, 2, 2, 1, 3,
// };

static const char *models[] = {
        //"models/block100.stl",
        //"models/bottle.stl",
        //"models/cube.stl",
        //"models/humanoid.stl",
        //"models/liver.stl",
        //"models/magnolia.stl",
        //"models/space_invader_magnet.stl",
        //"models/sphere.stl",
        //"models/tiler_3d.stl",
        "models/Suzanne.stl",
};

static std::vector<ShaderProgram> shaderPrograms;

static std::vector<STLSolid_t> allSolids;

static glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(0, 0, -130), // Camera vec3
        glm::vec3(0, 0, 0),   // Look at vec3
        glm::vec3(0, 1, 0)    // Normal (up) vec3
    );

static glm::mat4 projectionMatrix = glm::ortho(-64.0f, 64.0f, -48.0f, 48.0f, 0.0f, 300.0f);

static glm::mat4 modelMatrix = glm::mat4(1.0f);

// static bool CreateFullscreenShaderProgram()
// {
//     shaderPrograms.push_back(ShaderProgram("background"));
//     ShaderProgram *shader = &shaderPrograms[shaderPrograms.size()-1];
//     if (!shader->LoadFragmentShaderFromFile("shaders/background.frs")) {
//         return false;
//     }
//     if (!shader->LoadVertexShaderFromFile("shaders/background.vs")) {
//         return false;
//     }
//     shader->SetVertexBuffer((void *)fullscreenVertices,
//             sizeof(fullscreenVertices), GL_STATIC_DRAW);
//     shader->SetElementBuffer(6, (void *)fullscreenElements,
//             sizeof(fullscreenElements), GL_STATIC_DRAW);
//     Debug("Set up fullscreen shader object");
//     return true;
// }

// static bool CreateBoardShaderProgram()
// {
//     shaderPrograms.push_back(ShaderProgram("board"));
//     ShaderProgram *shader = &shaderPrograms[shaderPrograms.size()-1];
//     if (!shader->LoadFragmentShaderFromFile("shaders/board.frs")) {
//         return false;
//     }
//     if (!shader->LoadVertexShaderFromFile("shaders/board.vs")) {
//         return false;
//     }
//     shader->SetVertexBuffer((void *)fullscreenVertices,
//             sizeof(fullscreenVertices), GL_STATIC_DRAW);
//     shader->SetElementBuffer(6, (void *)fullscreenElements,
//             sizeof(fullscreenElements), GL_STATIC_DRAW);
//     Debug("Set up board shader program");
//     return true;
// }

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
    size_t numFloats = vertices.size() * 3;
    shader->SetVertexBuffer((void *)&vertices[0], sizeof(GLfloat)*numFloats,
            GL_STATIC_DRAW);
    shader->SetNormalBuffer((void *)&normals[0], sizeof(GLfloat)*numFloats,
            GL_STATIC_DRAW);
    shader->SetElementBuffer(elements.size(), (void *)&elements[0],
            sizeof(GLuint)*elements.size(), GL_STATIC_DRAW);
    shader->SetViewMatrix(viewMatrix);

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
    for (auto it=shaderPrograms.begin(); it!=shaderPrograms.end(); it++) {
        it->RotateModelMatrix(0.01f, glm::vec3(0, 1, 0));
        it->Render();
    }
}

bool SceneInit()
{

    // if (!CreateFullscreenShaderProgram()) {
    //     return false;
    // }
    // if (!CreateBoardShaderProgram()) {
    //     return false;
    // }

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
