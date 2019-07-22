#ifndef STL_FILEPARSER_H
#define STL_FILEPARSER_H
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#include "SDL.h"
#pragma clang diagnostic pop
#include <cstdint>
#include <string>
#include "util/log.h"

static inline int64_t ReadFile(const char* filename, std::string &target)
{
    int64_t len = -1;
    SDL_RWops *f = SDL_RWFromFile(filename, "rb");
    if (f == NULL) {
        Error("Could not open file '%s'", filename);
        return -1;
    }
    len = f->seek(f, 0, RW_SEEK_END);
    f->seek(f, 0, RW_SEEK_SET);
    target.resize(len);
    if (f->read(f, &target[0], len, 1) != 1) {
        Error("Failed reading file '%s'", filename);
        f->close(f);
        return -1;
    }

    f->close(f);
    return len;
}

static inline std::string GetBaseDir(std::string filename)
{
    const auto pos = filename.find_last_of('/');
    return filename.substr(0, pos+1);
}

#endif
