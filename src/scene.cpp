#include "scene.h"
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "graphics/shader_program.h"
#include "util/log.h"
#include "graphics/stl_parser.h"

static const GLfloat fullscreenVertices[] = {
    -1.0f, -1.0f, -1.0f, // bottom left
     1.0f, -1.0f, -1.0f, // bottom right
    -1.0f,  1.0f, -1.0f, // top left
     1.0f,  1.0f, -1.0f, // top right
};

static const GLushort fullscreenElements[] = {
    0, 1, 2, 2, 1, 3,
};

static const char *models[] = {
        "models/cube.stl",
        "models/space_invader_magnet.stl",
        "models/block100.stl",
        "models/bottle.stl",
        "models/humanoid.stl",
};

static ShaderProgram shaderFullscreen = ShaderProgram("background");

static ShaderProgram shaderBoard = ShaderProgram("board");

static std::vector<STLSolid_t> allSolids;

static bool CreateFullscreenShaderProgram()
{
    if (!shaderFullscreen.LoadFragmentShaderFromFile("shaders/background.frs")) {
        return false;
    }
    if (!shaderFullscreen.LoadVertexShaderFromFile("shaders/background.vs")) {
        return false;
    }
    shaderFullscreen.SetVertexBuffer((void *)fullscreenVertices,
            sizeof(fullscreenVertices), GL_STATIC_DRAW);
    shaderFullscreen.SetElementBuffer(6, (void *)fullscreenElements,
            sizeof(fullscreenElements), GL_STATIC_DRAW);
    Debug("Set up fullscreen shader object");
    return true;
}

static bool CreateBoardShaderProgram()
{
    if (!shaderBoard.LoadFragmentShaderFromFile("shaders/board.frs")) {
        return false;
    }
    if (!shaderBoard.LoadVertexShaderFromFile("shaders/board.vs")) {
        return false;
    }
    shaderBoard.SetVertexBuffer((void *)fullscreenVertices,
            sizeof(fullscreenVertices), GL_STATIC_DRAW);
    shaderBoard.SetElementBuffer(6, (void *)fullscreenElements,
            sizeof(fullscreenElements), GL_STATIC_DRAW);
    Debug("Set up board shader program");
    return true;
}

static bool CreateModelShaderProgram(std::vector<glm::vec3> &vertices,
        std::vector<int> &elements, std::vector<glm::vec3> &normals)
{
    return true;
}

static bool ParseSTLModel(const char* filename, std::vector<STLSolid_t> &solids)
{
    bool ret = ParseSTLFile(filename, solids);
    if (!ret) {
        Error("Failed to parse STL file '%s'", filename);
        return false;
    }
    Debug("Parsed model '%s' successfully, total models: %d", filename, solids.size());
    if (solids.size() <= 0) {
        return ret;
    }
    Debug("Number of facets in solids[%d]: %lu", solids.size()-1,
            solids[solids.size()-1].facets.size());
    return ret;  
}

void SceneRender()
{
    shaderFullscreen.Render();
    shaderBoard.Render();
}

bool SceneInit()
{
    if (!CreateFullscreenShaderProgram()) {
        return false;
    }
    if (!CreateBoardShaderProgram()) {
        return false;
    }

    for (int i=0; i<sizeof(models)/sizeof(const char *); i++) {
        if (!ParseSTLModel(models[i], allSolids)) {
            return false;
        }
    }

    for (int i=0; i<allSolids.size(); i++) {
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> vertices;
        std::vector<int> elements;
        ConvertSolidToNormalVertexElements(allSolids[i], normals, vertices,
                elements);
        Debug("normals.size(): %lu, vertices.size(): %lu, elements.size(): %lu",
                normals.size(), vertices.size(), elements.size());
        CreateModelShaderProgram(vertices, elements, normals);
    }

    return true;
}
