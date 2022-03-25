#include "day17.h"

using parse::input_t;

struct Probe {
    int32_t x, y, x_velocity, y_velocity;

    void step() {
        x += x_velocity, y += y_velocity;
        if (x_velocity > 0) {
            x_velocity--;
        } else if (x_velocity < 0) {
            x_velocity++;
        }
        y_velocity--;
    }
};

struct TargetArea {
    int32_t min_x, max_x;
    int32_t min_y, max_y;

    inline bool contains(int32_t x, int32_t y) const {
        return x >= min_x && x <= max_x && y >= min_y && y <= max_y;
    }
};

parse::output_t day17(input_t in) {
    int32_t part1 = std::numeric_limits<int32_t>().min();
    uint64_t part2 = 0;

    TargetArea ta;
    {  // parse input
        in.s += 15;
        char *endptr;

        ta.min_x = static_cast<int32_t>(std::strtol(in.s, &endptr, 10));
        in.s = endptr + 2;

        ta.max_x = static_cast<int32_t>(std::strtol(in.s, &endptr, 10));
        in.s = endptr + 4;

        ta.min_y = static_cast<int32_t>(std::strtol(in.s, &endptr, 10));
        in.s = endptr + 2;

        ta.max_y = static_cast<int32_t>(std::strtol(in.s, &endptr, 10));
        in.s = endptr + 4;

        DEBUG("target area: x={}..{}, y={}..{}", ta.min_x, ta.max_x, ta.min_y, ta.max_y);
    }

    Probe p;
    for (int32_t y_velocity = ta.min_y; y_velocity < std::abs(ta.min_y); y_velocity++) {
        for (int32_t x_velocity = 1; x_velocity <= ta.max_x; x_velocity++) {
            p.x = 0;
            p.y = 0;
            p.x_velocity = x_velocity;
            p.y_velocity = y_velocity;
            int32_t max_y = std::numeric_limits<int32_t>().min();
            for (size_t step = 1; step < std::numeric_limits<size_t>().max(); step++) {
                p.step();
                if (p.x > ta.max_x || p.y < ta.min_y) break;
                if (p.y > max_y) max_y = p.y;
                if (ta.contains(p.x, p.y)) {
                    if (max_y > part1) part1 = max_y;
                    part2++;
                    break;
                }
            }
        }
    }

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day17.txt");
    auto output = day17(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day17: examples") {
    auto test_cases = {
        make_tuple(
            "target area: x=20..30, y=-10..-5\n",
            "45", "112"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        DEBUG("Input:\n{}", &first);
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day17(in);
        DEBUG("Part 1: {}, Part 2: {}", &output.answer[0], &output.answer[1]);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day17, part 1 & part 2") {
    input_t in = parse::load_input("input/day17.txt");
    auto output = day17(in);
    CHECK_EQ("2775", output.answer[0]);
    CHECK_EQ("1566", output.answer[1]);
}

#endif  // IS_TEST
