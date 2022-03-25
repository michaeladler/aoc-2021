#include <cstdint>
#include "hash.h"

struct iPair {
    int32_t x, y;

    iPair() : x(0), y(0) {}

    iPair(int32_t x, int32_t y) : x(x), y(y) {}

    inline bool operator==(const iPair& other) const {
        return x == other.x && y == other.y;
    }
};

MAKE_HASHABLE(iPair, t.x, t.y)
