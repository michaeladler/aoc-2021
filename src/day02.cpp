#include "day02.h"

using parse::input_t;

parse::output_t day02(input_t in) {
    long part1 = 0, part2 = 0;

    long horizontal = 0, depth_part1 = 0;
    long depth_part2 = 0, aim = 0;

    while (in.len) {
        auto direction = parse::word(in, 'a', 'z');
        parse::skip_whitespace(in);
        int units = parse::positive<int>(in);
        parse::seek_next_line(in);

        // directions are uniquely determined by their first character
        switch (direction.front()) {
            case 'f':  // forward
                horizontal += units;
                depth_part2 += aim * units;
                break;
            case 'd':  // down
                depth_part1 += units;
                aim += units;
                break;
            case 'u':  // up
                depth_part1 -= units;
                aim -= units;
                break;
        }
    }

    part1 = depth_part1 * horizontal;
    part2 = depth_part2 * horizontal;

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day02.txt");
    auto output = day02(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day02 examples") {
    auto test_cases = {
        make_tuple("forward 5\n"
                   "down 5\n"
                   "forward 8\n"
                   "up 3\n"
                   "down 8\n"
                   "forward 2\n",
                   "150", "900"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day02(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day02, part 1 & part 2") {
    input_t in = parse::load_input("input/day02.txt");
    auto output = day02(in);
    CHECK_EQ("2036120", output.answer[0]);
    CHECK_EQ("2015547716", output.answer[1]);
}

#endif  // IS_TEST
