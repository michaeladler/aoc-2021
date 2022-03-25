#pragma once

#include <fmt/color.h>
#include <fmt/ranges.h>

// https://stackoverflow.com/a/38237385
constexpr const char *file_name(const char *path) {
    const char *file = path;
    while (*path) {
        if (*path++ == '/') {
            file = path;
        }
    }
    return file;
}

#ifdef DEBUGLOG
#define DEBUG(format, ...)                                                                     \
    do {                                                                                       \
        fmt::print("[{}:{:03d}] ", file_name(__FILE__), __LINE__);                               \
        fmt::print(fg(fmt::terminal_color::green), "[DEBUG] ", file_name(__FILE__), __LINE__); \
        fmt::print(format, ##__VA_ARGS__);                                                     \
        fmt::print("\n");                                                                      \
    } while (0)
#else
#define DEBUG(...) \
    do {           \
    } while (0)
#endif

#ifdef TRACELOG
#define TRACE(format, ...)                                                                                \
    do {                                                                                                  \
        fmt::print("[{}:{:03d}] ", file_name(__FILE__), __LINE__); \
        fmt::print(fg(fmt::terminal_color::yellow), "[TRACE] ", file_name(__FILE__), __LINE__); \
        fmt::print(format, ##__VA_ARGS__);                                                                \
        fmt::print("\n");                                                                                 \
    } while (0)
#else
#define TRACE(...) \
    do {           \
    } while (0)
#endif
