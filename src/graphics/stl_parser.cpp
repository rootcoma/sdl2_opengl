#include "stl_parser.h"
#include <sstream>
#include <cctype>
#include "util/file.h"
#include "util/strtrim.h"

/*
i strongly recommend using std::string_view here in place of strings/streams.
taking input as a std::string_view (passed by value) allows both std::strings and
string literals to be passed with minimal overhead (taking a std::string& can result
in unnecessary overhead in the case of passing a string literal), and making valueStr
a std::string_view instead would make it refer to a direct subrange in the memory
of the input view without having to buffer or allocate additional memory. you could
then use std::string_view::find_first_of(' ') to find the next space, along with
std::string_view::remove_prefix/suffix or std::string_view::substr to modify/move the
subview as you go. std::string_view and its related operations are quite useful for a
wide range of string-related use cases (highly applicable to parsing tasks in
particular), while remaining efficient and relatively intuitive. you may also want
consider having this function return a std::optional<float> instead taking an output
parameter and returning bool (std::optional defines more expressive semantics for
precisely this kind of scenario)
*/


#define STL_PARSER_MAX_FACETS 0x100000
// Note: STLFacet_t is not aligned and STL_FACET_SIZE
//       is used to iterate through facets in binary
//       files
#define STL_FACET_SIZE 0x32

static bool ReadVec3FromString(std::string &str, GLfloat *vec)
{
    //Success("vec3 string '%s'", str.c_str());
    // split the string into 3 floats
    std::istringstream vectorStringStream(str);
    std::string valueStr;
    for (int i=0; i<3; i++) {
        do {
            if (!std::getline(vectorStringStream, valueStr, ' ')) {
                Warning("Failed to get vertex%d for vector", i);
                return false;
            }
        } while (valueStr[0] == '\0');
        try {
            trim(valueStr);
            GLfloat val = (GLfloat)std::stof(valueStr);
            //Debug("val: %f", val);
            vec[i] = val;
        } catch (...) { // invalid_argument or out_of_range
            Warning("Failed to convert '%s' to a float in line '%s'",
                    valueStr.c_str(), str.c_str());
            return false;
        }
    }
    //Success("Read a vector 3!");
    return true;
}

// solid block100
//    facet normal -1.000000e+000 0.000000e+000 0.000000e+000
//       outer loop
//          vertex 0.000000e+000 1.000000e+002 1.000000e+002
//          vertex 0.000000e+000 1.000000e+002 0.000000e+000
//          vertex 0.000000e+000 0.000000e+000 1.000000e+002
//       endloop
//    endfacet
//    ... more facets ...
// endsolid
bool ParseSTLAscii(std::string &str, std::vector<STLSolid_t> &solids)
{
    std::istringstream strStream(str);
    std::string line;
    STLSolid_t currentSolid;
    STLFacet_t currentFacet;
    size_t skip = 0;
    bool headerRead = false;
    bool normalRead = false;
    int currentVertexIndex = 0;
    while (std::getline(strStream, line)) {
        //Debug(line);
        if (line.find("outer loop") != std::string::npos) {
        } else if (line.find("endloop") != std::string::npos) {
        } else if (line.find("endfacet") != std::string::npos) {
            currentSolid.facets.push_back(currentFacet);
            currentVertexIndex = 0;
            normalRead = false;
            memset(&currentFacet, 0, sizeof(STL_FACET_SIZE));
            currentFacet.data = 0;
        } else if (line.find("endsolid") != std::string::npos) {
            solids.push_back(currentSolid);
            headerRead = false;
            currentSolid.header = "\0";
            currentSolid.facets.clear();
        } else if ((skip = line.find("solid")) != std::string::npos) {
            if (headerRead) {
                Error("Found 'solid' block nested in line '%s'", line.c_str());
                return false;
            }
            line = line.substr(skip+5);
            trim(line); // lstrip and rstrip
            currentSolid.header = line;
            headerRead = true;
        } else if ((skip = line.find("facet normal")) != std::string::npos) {
            if (!headerRead) {
                Error("Found Facet block outside of solid definition line '%s'",
                        line.c_str());
                return false;
            }
            if (normalRead) {
                Error("Nested normal line found in STL line '%s'", line.c_str());
                return false;
            }
            line = line.substr(skip+12);
            trim(line); // lstrip and rstrip
            if (!ReadVec3FromString(line, currentFacet.normal)) {
                return false;
            }
            normalRead = true;
        } else if ((skip = line.find("vertex")) != std::string::npos) {
            if (!headerRead || !normalRead) {
                Error("Found vertex line out of sync line '%s'",
                        line.c_str());
                return false;
            }
            if (currentVertexIndex > 2) {
                Warning("Extra vertex found for facet line '%s'",
                        line.c_str());
                continue;
            }
            line = line.substr(skip+6);
            trim(line); // lstrip and rstrip
            GLfloat *targets[] = {currentFacet.vertex1, 
                    currentFacet.vertex2, currentFacet.vertex3,};
            GLfloat *target = targets[currentVertexIndex++];
            if (!ReadVec3FromString(line, target)) {
                return false;
            }
        } else {
            Debug("Skipped line in ascii STL file '%s'", line.c_str());
        }
    }
    Success("Finished parsing ascii STL file");
    return true;
}

struct internalSTLSolid_t
{
    Uint8 header[80];
    Uint32 numFacets;
};

// UINT8[80] - Header (must not begin with "solid")
// UINT32 - Number of triangles
// for each triangle
//     REAL32[3] - Normal vec3
//     REAL32[3] - Vert 1 vec3
//     REAL32[3] - Vert 2 vec3
//     REAL32[3] - Vert 3 vec3
//     UINT16 - Attribute byte count (typically zero)
//              some variantes of STL store color
//              information in the attribute byte
//              count
bool ParseSTLBinary(std::string &str, std::vector<STLSolid_t> &solids)
{
    Uint32 totalSize = str.size();
    Uint32 offset = 0;
    do {
        if (totalSize - offset < sizeof(internalSTLSolid_t)) {
            Error("Malformed STL Header TotalSize < HeaderSize");
            return false;
        }
        internalSTLSolid_t *stlHeader = (internalSTLSolid_t *)&str[offset];
        STLSolid_t solid;
        solid.header.resize(80);
        memcpy(&solid.header[0], stlHeader->header, 80);
        offset += sizeof(internalSTLSolid_t);
        if (totalSize - offset < STL_FACET_SIZE*stlHeader->numFacets) {
            Error("Malformed STL Facets Totalsize < FacetSize (%lu < %lu)",
                    totalSize - offset, STL_FACET_SIZE*stlHeader->numFacets);
            return false;
        }
        //Debug("Solid has %ld Facets.", stlHeader->numFacets);
        STLFacet_t *facets = (STLFacet_t *)&str[offset];
        for (int i=0; i<stlHeader->numFacets; i++) {
            solid.facets.push_back(facets[0]);
            //Debug("Added facet size: %d", solid.facets.size());
            offset += STL_FACET_SIZE;
            facets = (STLFacet_t *)&str[offset];
        }
        solids.push_back(solid);
    } while (totalSize - offset > sizeof(STLSolid_t));
    Success("Finished parsing binary STL file");
    return true;
}

bool ParseSTLFile(const char *filename, std::vector<STLSolid_t> &solids)
{
    std::string target;
    Sint64 len = ReadFile(filename, target);
    if (len <= 0) {
        Error("Failed to open STL file '%s'", filename);
        return false;
    }
    if (len < 5) {
        Error("Malformed STL file '%s' is too small", filename);
        return false;
    }
    
    if (trim_copy(target.substr(0, 32)).compare(0, 5, "solid") == 0) {
        Info("Parsing Ascii STL file '%s'", filename);
        return ParseSTLAscii(target, solids);
    }
    Info("Parsing Binary STL file", filename);
    return ParseSTLBinary(target, solids);
}

static bool CompareNormalVertexToExisting(GLfloat *norm, GLfloat *vert,
        std::vector<std::array<GLfloat, 3> > &normals,
        std::vector<std::array<GLfloat, 3> > &vertices)
{
    for (int i=0; i<normals.size(); i++) {
        if (memcmp((void *)norm, (void *)&normals[i][0],
                sizeof(GLfloat)*3)) {
            continue;
        }
        for (int j=0; j<3; j++) {
            if (!memcmp((void *)vert, (void *)&vertices[i][0],
                    sizeof(GLfloat)*3)) {
                return true;
            }
        }
    }
    return false;
}

bool ConvertSolidToNormalVertexElements(STLSolid_t &solid,
        std::vector<std::array<GLfloat, 3> > &normals,
        std::vector<std::array<GLfloat, 3> > &vertices,
        std::vector<int> &elements)
{
    if (normals.size() != vertices.size()) {
        Error("Cannot compare facet with uneven vectors");
        return false;
    }
    int numElements = 0;
    for (int i=0; i<solid.facets.size(); i++) {
        GLfloat *targets[] = {solid.facets[i].vertex1, 
                solid.facets[i].vertex2, solid.facets[i].vertex3,};
        for (int j=0; j<3; j++) {
            bool match = CompareNormalVertexToExisting(solid.facets[i].normal,
                    targets[j], normals, vertices);
            if (!match) {
                elements.push_back(numElements++);
                std::array<GLfloat, 3> tmpNorm;
                memcpy(&tmpNorm[0], solid.facets[i].normal, 3*sizeof(GLfloat));
                normals.push_back(tmpNorm);
                std::array<GLfloat, 3> tmpVert;
                memcpy(&tmpVert[0], targets[j], 3*sizeof(GLfloat));
                vertices.push_back(tmpVert);
                continue;
            }
            elements.push_back((i*3)+j);
        }
    }
    return true;
}
