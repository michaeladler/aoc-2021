#include "day01.h"

using parse::input_t;

parse::output_t day01(input_t in) {
    long part1 = 0, part2 = 0;

    std::array<int, 2048> numbers;
    numbers.fill(0);
    size_t numbers_idx = 0;

    long old = parse::positive<long>(in);
    parse::skip_whitespace(in);
    numbers[numbers_idx] = old;
    numbers_idx++;

    // first element of the window
    long window = old;

    for (size_t i = 0; i < 2; i++) {
        long current = parse::positive<long>(in);
        parse::skip_whitespace(in);
        numbers[numbers_idx] = current;
        numbers_idx++;
        if (current > old) part1++;
        old = current;

        window += current;
    }
    // first window is complete

    while (in.len) {
        long current = parse::positive<long>(in);
        parse::skip_whitespace(in);
        numbers[numbers_idx] = current;
        numbers_idx++;
        if (current > old) part1++;
        old = current;

        auto first = numbers[numbers_idx - 4];
        long next_window = window - first + current;
        if (next_window > window) {
            part2++;
        }
        window = next_window;
    }

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day01.txt");
    auto output = day01(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day01: examples") {
    auto test_cases = {
        make_tuple("199\n"
                   "200\n"
                   "208\n"
                   "210\n"
                   "200\n"
                   "207\n"
                   "240\n"
                   "269\n"
                   "260\n"
                   "263\n",
                   "7", "5"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day01(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day01, part 1 & part 2") {
    input_t in = parse::load_input("input/day01.txt");
    auto output = day01(in);
    CHECK_EQ("1688", output.answer[0]);
    CHECK_EQ("1728", output.answer[1]);
}

#endif  // IS_TEST
