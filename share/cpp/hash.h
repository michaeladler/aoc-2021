#pragma once

#include <functional>
#include <type_traits>

inline void hash_combine(std::size_t&) {}

/*
 *
 * Example:
 *
 * ```
 * std::size_t h=0;
 * hash_combine(h, obj1, obj2, obj3);
 * ```
 *
 * https://stackoverflow.com/a/38140932
 */
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

/*
 * Example:
 *
 * ```
 * struct SomeHashKey {
 *   std::string key1;
 *   std::string key2;
 *   bool key3;
 * };
 * MAKE_HASHABLE(SomeHashKey, t.key1, t.key2, t.key3)
 * ```
 */
#define MAKE_HASHABLE(type, ...)                      \
    namespace std {                                   \
    template <>                                       \
    struct hash<type> {                               \
        std::size_t operator()(const type& t) const { \
            std::size_t ret = 0;                      \
            hash_combine(ret, __VA_ARGS__);           \
            return ret;                               \
        }                                             \
    };                                                \
    }
