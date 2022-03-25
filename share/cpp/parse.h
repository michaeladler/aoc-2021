#ifndef _AOC_PARSE_H
#define _AOC_PARSE_H

#include <array>
#include <string>
#include <sstream>

namespace parse {

struct input_t {
    char *s;
    ssize_t len;
};

struct output_t {
    std::array<std::string, 2> answer;

    template <typename T1, typename T2>
    output_t(T1 p1, T2 p2) {
        set(0, p1);
        set(1, p2);
    }

    template <typename T>
    void set(int part, T value) {
        std::stringstream ss;
        ss << value;
        answer[part] = ss.str();
    }
};

input_t load_input(const std::string &filename);
void free_input(input_t &input);

void skip(input_t &in, int n);
void skip_whitespace(input_t &in);
void seek_next_line(input_t &in);
std::span<const char> word(input_t &in, char lower_bound, char upper_bound);
long number(input_t &in);

template <typename T>
T positive(input_t &in) {
    bool have = false;
    for (T n = 0; in.len; in.s++, in.len--) {
        uint8_t d = *in.s - '0';
        if (d <= 9) {
            n = 10 * n + d;
            have = true;
        } else if (have) {
            return n;
        }
    }
    return 0;
}

// Fixed-width positive integer.
// In strict mode, reject any prefix of a longer number.
template <typename T>
T positive(input_t &in, int len, bool strict) {
    T n = 0;
    if (in.len < len) return 0;
    for (; len--; in.s++, in.len--) {
        uint8_t d = *in.s - '0';
        if (d > 9) return 0;
        n = 10 * n + d;
    }

    if (strict && in.len && *in.s >= '0' && *in.s <= '9') {
        return 0;
    }

    return n;
}

// Extract the high bits after shifting each of the next eight bytes left
inline int octet(input_t &in, int shift);

}  // namespace parse

#endif
