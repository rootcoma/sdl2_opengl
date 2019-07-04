#ifndef STL_PARSER_H
#define STL_PARSER_H
#include <GL/glew.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include <SDL.h>
#pragma clang diagnostic pop
#include <vector>
#include <array>
#include <glm/glm.hpp>

struct STLFacet_t
{
    glm::vec3 normal;
    glm::vec3 vertices[3];
    Uint16 data;
};

struct STLSolid_t
{
    std::string header;
    std::vector<STLFacet_t> facets;
};

bool ParseSTLFile(const char* filename, std::vector<STLSolid_t> &solids);

bool ConvertSolidToNormalVertexElements(STLSolid_t &solid,
        std::vector<glm::vec3> &normals,
        std::vector<glm::vec3> &vertices,
        std::vector<GLuint> &elements);

#endif
