#include "day24.h"
#include <thread>

using parse::input_t;

static inline long calc(int8_t digits[]) {
    digits[3] = digits[2] + 3;
    digits[10] = digits[9] + 2;

    // Step 4
    long z = digits[0] * 26 + digits[1] + 3;  // >= 30

    // Step 5
    z = z * 26 + digits[4] + 13;  // >= 26*30 + 13 = 793

    // Step 6
    z = z * 26 + digits[5] + 9;  // z >= 26*793 + 10 = 20628

    // Step 9
    digits[8] = (z % 26) - 8;
    if (digits[8] <= 0 || digits[8] > 9) return 0;
    z = z / 26;

    // Step 12
    digits[11] = (z % 26) - 5;
    if (digits[11] <= 0 || digits[11] > 9) return 0;
    z = z / 26;

    // Step 13
    digits[12] = (z % 26) - 9;
    if (digits[12] <= 0 || digits[12] > 9) return 0;
    z = z / 26;  // x = 0, 0 <= z <= 25*26

    // Step 14
    digits[13] = (z % 26) - 1;
    if (digits[13] <= 0 || digits[13] > 9) return 0;
    z = z / 26;  // x = 0, 0 <= z <= 25
    if (z == 0) {
        long value = 0;
        for (int i = 0; i < 14; i++) {
            value *= 10;
            value += digits[i];
        }
        return value;
    }
    return 0;
}

long alu_max() {
    int8_t digits[14];

    // Key observation: all x must be zero, otherwise z / 26^4 >= 1
    digits[6] = 9;
    digits[7] = 1;

    for (digits[0] = 9; digits[0] > 0; digits[0]--) {
        for (digits[1] = 9; digits[1] > 0; digits[1]--) {
            for (digits[2] = 6; digits[2] > 0; digits[2]--) {
                for (digits[4] = 9; digits[4] > 0; digits[4]--) {
                    for (digits[5] = 9; digits[5] > 0; digits[5]--) {
                        for (digits[9] = 7; digits[9] > 0; digits[9]--) {
                            auto value = calc(digits);
                            if (value != 0) {
                                return value;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

long alu_min() {
    int8_t digits[14];

    // Key observation: all x must be zero, otherwise z / 26^4 >= 1
    digits[6] = 9;
    digits[7] = 1;

    for (digits[0] = 1; digits[0] <= 9; digits[0]++) {
        for (digits[1] = 1; digits[1] <= 9; digits[1]++) {
            for (digits[2] = 1; digits[2] <= 6; digits[2]++) {
                for (digits[4] = 1; digits[4] < 9; digits[4]++) {
                    for (digits[5] = 1; digits[5] <= 9; digits[5]++) {
                        for (digits[9] = 1; digits[9] <= 7; digits[9]++) {
                            auto value = calc(digits);
                            if (value != 0) {
                                return value;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

parse::output_t day24(input_t) {
    long part1 = alu_max();
    long part2 = alu_min();
    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day24.txt");
    auto output = day24(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

TEST_CASE("day24, part 1 & part 2") {
    input_t in;
    auto output = day24(in);
    CHECK_EQ(99691891979938, std::strtol(output.answer[0].c_str(), NULL, 10));
    CHECK_EQ(27141191213911, std::strtol(output.answer[1].c_str(), NULL, 10));
}

#endif  // IS_TEST
