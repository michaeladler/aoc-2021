#include "day06.h"

using parse::input_t;

const size_t DAYS_PART1 = 80;
const size_t DAYS_PART2 = 256;

parse::output_t day06(input_t in) {
    uint64_t part1 = 0, part2 = 0;

    std::array<long long, 9> bins;
    bins.fill(0);

    while (in.len > 0) {
        size_t d = *in.s - '0';
        in.s += 2;
        in.len -= 2;
        bins[d] += 1;
    }

    for (size_t day = 1; day <= DAYS_PART2; day++) {
        auto old = bins[0];
        bins[0] = 0;
        for (size_t i = 1; i < bins.size(); i++) {
            bins[i - 1] += bins[i];  // move items one bin to the left
            bins[i] = 0;
        }
        bins[6] += old;
        bins[8] += old;

        if (day == DAYS_PART1) {
            part1 = std::accumulate(bins.cbegin(), bins.cend(), (uint64_t)0);
        }
    }

    part2 = std::accumulate(bins.cbegin(), bins.cend(), (uint64_t)0);

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day06.txt");
    auto output = day06(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day06: examples") {
    auto test_cases = {
        make_tuple(
            "3,4,3,1,2\n",
            "5934", "26984457539"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day06(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day06, part 1 & part 2") {
    input_t in = parse::load_input("input/day06.txt");
    auto output = day06(in);
    CHECK_EQ("366057", output.answer[0]);
    CHECK_EQ("1653559299811", output.answer[1]);
}

#endif  // IS_TEST
