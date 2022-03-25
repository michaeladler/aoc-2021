#ifndef _AOC_NUMERIC
#define _AOC_NUMERIC

#include <algorithm>

template <typename T>
T fastmod(T n, T mod) {
    n -= mod & -(n >= mod);
    return n;
}

template <typename T>
T gcdx(T a, T b, T &x, T &y) {
    x = 1, y = 0;
    T u = 0, v = 1, q;
    while (b) {
        q = a / b;
        x = std::exchange(u, x - q * u);
        y = std::exchange(v, y - q * v);
        a = std::exchange(b, a - q * b);
    }
    return a;
}

template <typename T>
T modinv(T b, T mod) {
    T x, y;
    gcdx(b, mod, x, y);
    x += mod & -(x < 0);
    return x;
}

template <typename T>
// implements signum (-1, 0, or 1).
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif
