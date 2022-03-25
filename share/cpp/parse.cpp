#include <span>
#include <x86intrin.h>
#include "parse.h"

static constexpr int INPUT_PADDING = 64;

namespace parse {

void skip(input_t &in, int n) {
    if (in.len < n) abort();
    in.s += n, in.len -= n;
}

void skip_whitespace(input_t &in) {
    while (in.len && *in.s == ' ') in.s++, in.len--;
}

void seek_next_line(input_t &in) {
    while (in.len && *in.s != '\n') in.s++, in.len--;
    in.s++, in.len--;
}

// Parses a sequence of characters within the provided range [lower_bound, upper_bound] until eof or whitespace.
std::span<const char> word(input_t &in, char lower_bound, char upper_bound) {
    const char *first = in.s;
    size_t count = 0;

    while (in.len && *in.s != ' ' && *in.s >= lower_bound && *in.s <= upper_bound) {
        in.s++, in.len--;
        count++;
    }

    return std::move(std::span{first, count});
}

long number(input_t &in) {
    char *endptr;
    long result = strtol(in.s, &endptr, 10);
    in.len -= endptr - in.s;
    in.s = endptr;
    return result;
}

input_t load_input(const std::string &filename) {
    FILE *fp = fopen(filename.c_str(), "r");
    if (!fp) {
        perror(filename.c_str());
        exit(EXIT_FAILURE);
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek");
        exit(EXIT_FAILURE);
    }

    input_t in;
    in.len = ftell(fp);
    in.s = new char[in.len + INPUT_PADDING];
    std::fill(in.s + in.len, in.s + in.len + INPUT_PADDING, 0);

    rewind(fp);
    if (fread(in.s, in.len, 1, fp) != 1) {
        perror("fread");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    return in;
}

void free_input(input_t &input) {
    delete[] input.s;
}

// Extract the high bits after shifting each of the next eight bytes left
inline int octet(input_t &in, int shift) {
    auto m = *reinterpret_cast<__m64 *>(in.s);
    parse::skip(in, 8);
    return _mm_movemask_pi8(_mm_slli_si64(m, shift));  // SSE
}

}  // namespace parse
