#ifndef _AOC_BITS
#define _AOC_BITS

#include <cstddef>

// Iterate over bits using range-for syntax:
//   for (int i : bits(mask))
template <typename T>
struct bits {
    T mask;
    bits(T mask) : mask(mask) {}
    bits& operator++() {
        mask &= mask - 1;
        return *this;
    }
    bool operator!=(const bits& o) const { return mask != o.mask; }
    inline int operator*() const { return _mm_tzcnt_64(mask); }
    bits begin() const { return mask; }
    bits end() const { return 0; }
};

template <typename T>
T reverse_bits(T num) {
    // source: https://www.geeksforgeeks.org/write-an-efficient-c-program-to-reverse-bits-of-a-number/
    size_t count = sizeof(num) * 8 - 1;
    T reverse_num = num;

    num >>= 1;
    while (num) {
        reverse_num <<= 1;
        reverse_num |= num & 1;
        num >>= 1;
        count--;
    }
    reverse_num <<= count;
    return reverse_num;
}

#endif
