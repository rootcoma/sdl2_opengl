#ifndef STL_PARSER_H
#define STL_PARSER_H
#include <GL/glew.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include <SDL.h>
#pragma clang diagnostic pop
#include <vector>
#include <array>

struct STLFacet_t
{
    GLfloat normal[3];
    GLfloat vertex1[3];
    GLfloat vertex2[3];
    GLfloat vertex3[3];
    Uint16 data;
};

struct STLSolid_t
{
    std::string header;
    std::vector<STLFacet_t> facets;
};

bool ParseSTLFile(const char* filename, std::vector<STLSolid_t> &solids);

bool ConvertSolidToNormalVertexElements(STLSolid_t &solid,
        std::vector<std::array<GLfloat, 3> > &normals,
        std::vector<std::array<GLfloat, 3> > &vertices,
        std::vector<int> &elements);

#endif
