#include "day03.h"

using parse::input_t;

struct counter {
    size_t zeroes;
    size_t ones;
};

std::ostream& operator<<(std::ostream& os, const counter& c) {
    os << "{zeroes: " << c.zeroes << ", ones: " << c.ones << "}";
    return os;
}

parse::output_t day03(input_t in) {
    long part1 = 0, part2 = 0;

    std::array<counter, 16> counters;
    counters.fill(counter());

    size_t n = 0;  // number of digits in a number

    std::unordered_set<int> oxygen_numbers;
    oxygen_numbers.reserve(1024);

    {
        long current = 0;
        size_t idx = 0;
        while (in.len) {
            switch (*in.s) {
                case '0':
                    counters[idx].zeroes++;
                    idx++;
                    current <<= 1;
                    break;
                case '1':
                    current |= 1;
                    current <<= 1;
                    counters[idx].ones++;
                    idx++;
                    break;
                case '\n':
                    current >>= 1;
                    oxygen_numbers.insert(current);
                    current = 0;
                    n = idx;
                    idx = 0;
            }
            in.s++, in.len--;
        }
    }

    long gamma_rate = 0, epsilon_rate = 0;
    for (size_t i = 0; i < n; i++) {
        auto bit_pos = n - 1 - i;
        auto counter = counters[i];
        if (counter.ones > counter.zeroes) {
            gamma_rate |= 1 << bit_pos;
        } else {
            epsilon_rate |= 1 << bit_pos;
        }
    }

    part1 = gamma_rate * epsilon_rate;

    auto co2_numbers = oxygen_numbers;  // copy

    // oxygen generator rating: most common
    for (size_t i = 0; i < n; i++) {
        long ones = 0;
        long zeroes = 0;

        // update counters
        for (auto x : oxygen_numbers) {
            auto is_set = x & (1 << (n - 1 - i));
            if (is_set) {
                ones++;
            } else {
                zeroes++;
            }
        }

        for (auto it = oxygen_numbers.begin(); it != oxygen_numbers.end() && oxygen_numbers.size() > 1;) {
            // on parity, one wins
            int x = *it;
            if (x & (1 << (n - 1 - i))) {
                if (zeroes > ones) {
                    it = oxygen_numbers.erase(it);
                    continue;
                }
            } else {
                if (ones >= zeroes) {
                    it = oxygen_numbers.erase(it);
                    continue;
                }
            }
            it++;
        }

    }

    // CO2 scrubber rating: least common; partiy => zero wins
    for (size_t i = 0; i < n; i++) {
        long ones = 0;
        long zeroes = 0;

        // update counters
        for (auto x : co2_numbers) {
            auto is_set = x & (1 << (n - 1 - i));
            if (is_set) {
                ones++;
            } else {
                zeroes++;
            }
        }

        for (auto it = co2_numbers.begin(); it != co2_numbers.end() && co2_numbers.size() > 1;) {
            // on parity, zero wins
            int x = *it;
            if (x & (1 << (n - 1 - i))) {
                if (zeroes <= ones) {
                    it = co2_numbers.erase(it);
                    continue;
                }
            } else {
                if (ones < zeroes) {
                    it = co2_numbers.erase(it);
                    continue;
                }
            }
            it++;
        }

    }

    part2 = *oxygen_numbers.begin() * *co2_numbers.begin();

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day03.txt");
    auto output = day03(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day03 examples") {
    auto test_cases = {
        make_tuple("00100\n"
                   "11110\n"
                   "10110\n"
                   "10111\n"
                   "10101\n"
                   "01111\n"
                   "00111\n"
                   "11100\n"
                   "10000\n"
                   "11001\n"
                   "00010\n"
                   "01010\n",
                   "198", "230"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day03(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day03, part 1 & part 2") {
    input_t in = parse::load_input("input/day03.txt");
    auto output = day03(in);
    CHECK_EQ("693486", output.answer[0]);
    CHECK_EQ("3379326", output.answer[1]);
}

#endif  // IS_TEST
