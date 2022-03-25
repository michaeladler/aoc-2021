#include "day25.h"

using parse::input_t;

#define EAST '>'
#define SOUTH 'v'

parse::output_t day25(input_t in) {
    long part1 = 0, part2 = 0;

    int32_t row_count = 0;
    int32_t col_count;
    int new_idx = 0;
    std::unordered_map<iPair, char, std::hash<iPair>> east[2];
    std::unordered_map<iPair, char, std::hash<iPair>> south[2];

    east[new_idx].reserve(512);
    south[new_idx].reserve(512);

    while (in.len > 0) {
        col_count = 0;
        while (*in.s != '\n') {
            auto c = *in.s;
            if (c == EAST) east[new_idx][iPair(col_count, row_count)] = c;
            if (c == SOUTH) south[new_idx][iPair(col_count, row_count)] = c;
            ++col_count;
            in.s++, in.len--;
        }
        row_count++;
        in.s++, in.len--;
    }

    east[1 - new_idx].reserve(east[new_idx].size());
    south[1 - new_idx].reserve(south[new_idx].size());

    iPair destination;
    for (part1 = 1; true; part1++) {
        bool changed = false;
        auto old_idx = new_idx;
        new_idx = 1 - new_idx;

        east[new_idx].clear();
        south[new_idx].clear();

        // move east herd
        for (auto element : east[old_idx]) {
            destination.x = (element.first.x + 1) % col_count;
            destination.y = element.first.y;
            if (!east[old_idx].contains(destination) && !south[old_idx].contains(destination)) {
                east[new_idx][destination] = element.second;
                changed = true;
            } else {
                east[new_idx][element.first] = element.second;
            }
        }

        // move south herd
        for (auto element : south[old_idx]) {
            destination.x = element.first.x;
            destination.y = (element.first.y + 1) % row_count;
            if (!east[new_idx].contains(destination) && !south[old_idx].contains(destination)) {
                south[new_idx][destination] = element.second;
                changed = true;
            } else {
                south[new_idx][element.first] = element.second;
            }
        }

        if (!changed) break;
    }

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day25.txt");
    auto output = day25(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day25: examples") {
    auto test_cases = {
        make_tuple(
            "v...>>.vv>\n"
            ".vv>>.vv..\n"
            ">>.>v>...v\n"
            ">>v>>.>.v.\n"
            "v>v.vv.v..\n"
            ">.>>..v...\n"
            ".vv..>.>v.\n"
            "v.v..>>v.v\n"
            "....v..v.>\n",
            "58", "answer2"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day25(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
    }
}

TEST_CASE("day25, part 1 & part 2") {
    input_t in = parse::load_input("input/day25.txt");
    auto output = day25(in);
    CHECK_EQ("560", output.answer[0]);
}

#endif  // IS_TEST
