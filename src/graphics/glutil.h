#ifndef GLUTIL_H
#define GLUTIL_H
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include <SDL.h>
#pragma clang diagnostic pop
#include <GL/glew.h>
#include <limits>
#include "util/log.h"

static inline bool CompileShader(GLenum shaderObject, const std::string &src,
        const Sint64 &len)
{
    // Sint64 = 64bit
    // GLint >=32bit
    GLint filesizeMax = std::numeric_limits<GLint>::max();
    if (len >= filesizeMax) {
        Error("Shader file is too big (%lld >= %d)", len, filesizeMax);
        return false;
    }
    glShaderSource(shaderObject, 1, (const GLchar **)&src,
            (GLint *)&len);
    glCompileShader(shaderObject);
    GLint isCompiled = 0;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &maxLength);
        std::string logData;
        logData.resize(maxLength);
        glGetShaderInfoLog(shaderObject, maxLength, &maxLength,
                (GLchar *)&logData[0]);
        glDeleteShader(shaderObject);
        Error("Shader could not compile");
        Error("%s", logData.c_str());
        return false;
    }

    return true;
}

#endif
