#include "day07.h"

using parse::input_t;

using num = int64_t;

parse::output_t day07(input_t in) {
    long part1 = 0, part2 = 0;

    std::array<num, 1024> __numbers;
    size_t numbers_count = 0;

    num min = std::numeric_limits<num>().max();
    num max = std::numeric_limits<num>().min();

    while (in.len > 0) {
        num x = parse::positive<int>(in);
        if (x < min) min = x;
        if (x > max) max = x;
        __numbers[numbers_count++] = x;
        in.s++;
        in.len--;
    }

    std::span<num> numbers(__numbers.begin(), numbers_count);
    std::sort(numbers.begin(), numbers.end());
    num median = numbers[(numbers.size() + 1) / 2];

    for (auto x : numbers) {
        part1 += x > median ? x - median : median - x;
    }

    long avg = std::accumulate(numbers.begin(), numbers.end(), (uint64_t)0) / numbers.size();

    num min_fuel_part2 = std::numeric_limits<num>().max();
    for (long delta = -1; delta <= 1; delta++) {
        long pos = avg + delta;
        if (pos < 0) continue;
        num fuel_part2 = 0;
        for (auto x : numbers) {
            num delta = x > pos ? x - pos : pos - x;
            // sum of numbers 1 .. n = n*(n+1)/2
            fuel_part2 += (delta * (delta + 1)) / 2;
        }
        if (fuel_part2 < min_fuel_part2) min_fuel_part2 = fuel_part2;
    }

    part2 = min_fuel_part2;

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day07.txt");
    auto output = day07(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day07: examples") {
    auto test_cases = {
        make_tuple(
            "16,1,2,0,4,2,7,1,2,14\n",
            "37", "168"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day07(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day07, part 1 & part 2") {
    input_t in = parse::load_input("input/day07.txt");
    auto output = day07(in);
    CHECK_EQ("344535", output.answer[0]);
    CHECK_EQ("95581659", output.answer[1]);
}

#endif  // IS_TEST
