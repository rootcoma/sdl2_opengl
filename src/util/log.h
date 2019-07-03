#ifndef LOG_H
#define LOG_H
#include <string>
#include <cstdio>
#include <cstdarg>

static inline void Debug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string format = fmt;
    format.insert(0, "[*] ");
    format.append("\n");
    vfprintf(stderr, format.c_str(), args);
    va_end(args);
}

static inline void Debug(const std::string &str)
{
    Debug(str.c_str());
}

static inline void Info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string format = fmt;
    format.insert(0, "[>] ");
    format.append("\n");
    vfprintf(stderr, format.c_str(), args);
    va_end(args);
}

static inline void Info(const std::string &str)
{
    Info(str.c_str());
}

static inline void Warning(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string format = fmt;
    format.insert(0, "[!] ");
    format.append("\n");
    vfprintf(stderr, format.c_str(), args);
    va_end(args);
}

static inline void Warning(const std::string &str)
{
    Warning(str.c_str());
}

static inline void Error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string format = fmt;
    format.insert(0, "[-] ");
    format.append("\n");
    vfprintf(stderr, format.c_str(), args);
    va_end(args);
}

static inline void Error(const std::string &str)
{
    Error(str.c_str());
}

static inline void Success(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string format = fmt;
    format.insert(0, "[+] ");
    format.append("\n");
    vfprintf(stderr, format.c_str(), args);
    va_end(args);
}

static inline void Success(const std::string &str)
{
    Success(str.c_str());
}

#endif
