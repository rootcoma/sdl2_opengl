#ifndef STL_FILEPARSER_H
#define STL_FILEPARSER_H
#include <string>
#include "util/log.h"

static inline Sint64 ReadFile(const char* filename, std::string &target)
{
    Sint64 len = -1;
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

#endif
