#include "scene.h"
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>
#include "graphics/shader_program.h"
#include "util/log.h"
#include "util/stl_parser.h"
#include "graphics/camera.h"
#include "gui/window.h"
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"
#include "util/file.h"

// TODO: Make aspect ratio dynamic on screen redraw
#define FOV 45.0f
#define DEFAULT_ASPECT_RATIO (16.0f/9.0f)
#define PROJECTION_NEAR_CLIP 1.0f
#define PROJECTION_FAR_CLIP 1000.0f
#define MODELS_FOLDER_PREFIX "models/"

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
        //"models/unit_circle_2x2.stl",
        //"models/suzanne.obj",
        "models/nanosuit/nanosuit.obj",
};

static std::vector<ShaderProgram> shaderPrograms;
static std::vector<STLSolid_t> allSolids;
static CameraView camera;
static glm::mat4 projectionMatrix = glm::perspective(glm::radians(FOV),
        DEFAULT_ASPECT_RATIO, PROJECTION_NEAR_CLIP, PROJECTION_FAR_CLIP);
static glm::mat4 modelMatrix = glm::mat4(1.0f);
static std::pair<uint32_t, uint32_t>windowDimensions = std::make_pair(0, 0);
static std::map<std::string, GLuint> textures;


static bool CreateTexturedModelShaderProgram(const char *name,
        std::vector<glm::vec3> &vertices, std::vector<GLuint> &elements,
        std::vector<glm::vec3> &normals, std::vector<glm::vec2> &uvs,
        GLuint diffuseTex, GLuint specularTex=0)
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

    const size_t numFloats = vertices.size() * 3;
    shader->SetVertexBuffer((void *)&vertices[0], sizeof(GLfloat)*numFloats,
            GL_STATIC_DRAW);
    shader->SetNormalBuffer((void *)&normals[0], sizeof(GLfloat)*numFloats,
            GL_STATIC_DRAW);
    shader->SetUVBuffer((void *)&uvs[0], sizeof(GLfloat)*uvs.size()*2,
            GL_STATIC_DRAW);
    shader->SetElementBuffer(elements.size(), (void *)&elements[0],
            sizeof(GLuint)*elements.size(), GL_STATIC_DRAW);

    glm::mat4 viewMat = camera.GetViewMatrix();
    glm::vec3 cameraPosition = camera.GetPosition();
    shader->SetViewMatrix(viewMat, cameraPosition);
    shader->SetModelMatrix(modelMatrix);
    shader->SetProjectionMatrix(projectionMatrix);

    ShaderLight_t s;
    s.direction = glm::vec3(-0.2f, -1.0f, -0.1f);
    s.ambient = glm::vec3(0.3f, 0.24f, 0.14f);
    s.diffuse = glm::vec3(0.7f, 0.42f, 0.26f);
    s.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    s.type = 0;
    shader->SetLight("sun", s);

    ShaderLight_t s2;
    s2.direction = glm::vec3(0.2f, 1.0f, 0.1f);
    s2.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    s2.diffuse = glm::vec3(0.2f, 0.1f, 0.06f);
    s2.specular = glm::vec3(0.0f, 0.0f, 0.0f);
    s2.type = 0;
    shader->SetLight("undersun", s2);

    ShaderMaterial_t t;
    //t.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    //t.diffuse = glm::vec3(0.1f, 0.7f, 0.5f);
    //t.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    t.shininess = 32;
    t.type = 1;
    t.diffuseSampler = diffuseTex;
    t.specularSampler = diffuseTex;
    if (specularTex == 0) {
        t.specularSampler = specularTex;
    }
    shader->SetMaterial(t);

    Debug("Set up shader '%s'", name);
    return true;
}

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
    glm::mat4 viewMat = camera.GetViewMatrix();
    glm::vec3 cameraPosition = camera.GetPosition();
    shader->SetViewMatrix(viewMat, cameraPosition);
    shader->SetModelMatrix(modelMatrix);
    shader->SetProjectionMatrix(projectionMatrix);

    ShaderLight_t s;
    s.direction = glm::vec3(-0.2f, -1.0f, -0.1f);
    s.ambient = glm::vec3(0.3f, 0.24f, 0.14f);
    s.diffuse = glm::vec3(0.7f, 0.42f, 0.26f);
    s.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    s.type = 0;
    shader->SetLight("sun", s);

    ShaderLight_t s2;
    s2.direction = glm::vec3(0.2f, 1.0f, 0.1f);
    s2.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    s2.diffuse = glm::vec3(0.2f, 0.1f, 0.06f);
    s2.specular = glm::vec3(0.0f, 0.0f, 0.0f);
    s2.type = 0;
    shader->SetLight("undersun", s2);

    ShaderMaterial_t t;
    t.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    t.diffuse = glm::vec3(0.1f, 0.7f, 0.5f);
    t.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    t.shininess = 32;
    t.type = 0;
    shader->SetMaterial(t);

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

void SceneWindowResize(uint32_t width, uint32_t height)
{
    projectionMatrix = glm::perspective(glm::radians(FOV),
            (float)((double)width/(double)height), PROJECTION_NEAR_CLIP, PROJECTION_FAR_CLIP);
    
    for (auto it=shaderPrograms.begin(); it!=shaderPrograms.end(); it++) {
        it->SetProjectionMatrix(projectionMatrix);
    }

}

void SceneRender()
{
    std::pair<uint32_t, uint32_t>currWinDim = GetWindowDimensions();
    glm::mat4 viewMat = camera.GetViewMatrix();
    glm::vec3 cameraPosition = camera.GetPosition();


    //float dT = 0.001f;
    for (auto it=shaderPrograms.begin(); it!=shaderPrograms.end(); it++) {
        //it->RotateModelMatrix(dT, glm::vec3(0, 1, 0));
        //it->RotateModelMatrix(-dT*0.3f, glm::vec3(1, 0, 0));
        it->SetViewMatrix(viewMat, cameraPosition);
        it->Render();
        //dT = -dT;
    }
    if (windowDimensions != currWinDim) {
        windowDimensions = currWinDim;
    }
}

bool LoadSTLModel(const char* filename)
{
    if (!ParseSTLModel(filename, allSolids) || allSolids.size() < 1) {
        return false;
    }
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> elements;

    ConvertSolidToNormalVertexElements(allSolids[allSolids.size()-1], normals,
            vertices, elements);
    Debug("normals.size(): %lu, vertices.size(): %lu, elements.size(): %lu",
            normals.size(), vertices.size(), elements.size());
    if (!CreateModelShaderProgram("mesh_shader", vertices, elements, normals)) {
        return false;
    }
    return true;
}

static bool LoadTexture(const char* filename)
{
    int w = 0;
    int h = 0;
    int comp = 0;
    unsigned char *image = stbi_load(filename, &w, &h, &comp,
            STBI_default);
    if (!image) {
        Error("Failed to load texture image '%s'", filename);
        return false;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    switch (comp) { // vec3 RGB vec4 RGBA
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                GL_UNSIGNED_BYTE, image);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, image);
        break;
    default:
        Error("Invalid number of components in obj model texture '%s': %d",
                filename, comp);
        return false;
    }
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    textures.insert(std::make_pair(filename, texID));

    Debug("Number of textures: %d", textures.size());
    return true;
}

// Check if `mesh_t` contains smoothing group id.
// static bool hasSmoothingGroup(const tinyobj::shape_t& shape)
// {
//   for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++) {
//     if (shape.mesh.smoothing_group_ids[i] > 0) {
//       return true;
//     }
//   }
//   return false;
// }

static bool LoadObjModel(const char* filename)
{
    Info("Parsing OBJ file '%s'", filename);
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    std::string baseDir = GetBaseDir(filename);
    bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
            filename, baseDir.c_str());

    if (!warn.empty()) {
        Warning("%s", warn.c_str());
    }

    if (!err.empty()) {
        Error("%s", err.c_str());
    }

    if (!res) {
        Error("Failed to load obj model '%s'", filename);
        return false;
    }

    Success("Finished parsing OBJ file");
    Debug("# of shapes: %d", (int)shapes.size());
    Debug("# of materials: %d", (int)materials.size());
    Debug("normals.size(): %lu, vertices.size(): %lu, texcoords.size(): %lu",
            attrib.normals.size()/3, attrib.vertices.size()/3,
            attrib.texcoords.size()/2);

    // Load any textures parsed
    std::string texFilename;
    for (int i=0; i<materials.size(); i++) {
        if (materials[i].diffuse_texname.size() == 0) {
            continue;
        }
        texFilename = baseDir+materials[i].diffuse_texname;
        if (textures.find(texFilename) != textures.end()) {
            Debug("Skipping alread loaded texture '%s'", texFilename.c_str());
            continue;
        }
        Debug("materials[%lu].diffuse_texname: %s",
                i, materials[i].diffuse_texname.c_str());
        if (!LoadTexture(texFilename.c_str())) {
            return false;
        }
    }

    for (auto shape : shapes) {
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<GLuint> elements;
        GLuint i = 0;
        //ShaderMaterial_t material;
        int matId =  shape.mesh.material_ids[0]; // assume same material
                                                 // for whole object
        tinyobj::material_t mat = materials[matId];

        for (size_t f=0; f<shape.mesh.indices.size(); f++) {
            // // Index struct to support different indices for vtx/normal/texcoord.
            // // -1 means not used.
            // typedef struct {
            //   int vertex_index;
            //   int normal_index;
            //   int texcoord_index;
            // } index_t;
            tinyobj::index_t idx = shape.mesh.indices[f];
            if (idx.vertex_index == -1) {
                Error("No vertex information for index");
                return false;
            }
            if (idx.normal_index == -1) {
                Error("No normal information for index");
                return false;
            }
            if (idx.texcoord_index == -1) {
                Error("No UV information for index with texture material");
                return false;
            }

            elements.push_back(i++);
            vertices.emplace_back(glm::vec3(attrib.vertices[3*idx.vertex_index+0],
                    attrib.vertices[3*idx.vertex_index+1],
                    attrib.vertices[3*idx.vertex_index+2]));
            normals.emplace_back(glm::vec3(attrib.normals[idx.normal_index],
                    attrib.normals[3*idx.normal_index+1],
                    attrib.normals[3*idx.normal_index+2]));
            uvs.emplace_back(glm::vec2(attrib.texcoords[2*idx.texcoord_index],
                    attrib.texcoords[2*idx.texcoord_index+1]));
        }

        // Create shader program
        std::string texName = baseDir + mat.diffuse_texname;
        if (!CreateTexturedModelShaderProgram("obj_mesh_shader", vertices,
                elements, normals, uvs, textures[texName], textures[texName])) {
            return false;
        }
    }

    return true;
}

bool SceneInit()
{
    for (int i=0; i<sizeof(models)/sizeof(const char *); i++) {
        std::string extension = models[i];
        extension = extension.substr(extension.find_last_of("."));
        for(auto& c : extension) {
           c = tolower(c);
        }
        if (extension.compare(".stl") == 0) {
            if (!LoadSTLModel(models[i])) {
                return false;
            }
        } else if (extension.compare(".obj") == 0) {
            if (!LoadObjModel(models[i])) {
                return false;
            }
        } else {
            // default
            Warning("No parser available for file type '%s' model '%s'",
                    extension.c_str(), models[i]);
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
