#include "day11.h"

using parse::input_t;

constexpr size_t WIDTH = 10;
constexpr size_t HEIGHT = 10;

using num = int8_t;

parse::output_t day11(input_t in) {
    uint64_t part1 = 0, part2 = std::numeric_limits<uint64_t>().max();

    num board[HEIGHT][WIDTH];

    size_t y = 0;
    while (in.len > 0) {
        size_t x = 0;
        while (in.len > 0 && *in.s != '\n') {
            board[y][x++] = parse::positive<num>(in, 1, false);
        }
        in.s++;
        in.len--;
        x = 0;
        y++;
    }

    auto get_neighbors = [&](uint8_t y, uint8_t x) -> std::vector<std::pair<uint8_t, uint8_t>> {
        std::vector<std::pair<uint8_t, uint8_t>> neighbors;
        neighbors.reserve(8);
        if (x > 0) neighbors.emplace_back(y, x - 1);
        if (x > 0 && y > 0) neighbors.emplace_back(y - 1, x - 1);
        if (y > 0) neighbors.emplace_back(y - 1, x);
        if (y > 0 && x < WIDTH - 1) neighbors.emplace_back(y - 1, x + 1);
        if (x < WIDTH - 1) neighbors.emplace_back(y, x + 1);
        if (y < HEIGHT - 1 && x < WIDTH - 1) neighbors.emplace_back(y + 1, x + 1);
        if (y < HEIGHT - 1) neighbors.emplace_back(y + 1, x);
        if (y < HEIGHT - 1 && x > 0) neighbors.emplace_back(y + 1, x - 1);
        return neighbors;
    };

    std::queue<std::pair<uint8_t, uint8_t>> flashes;
    for (size_t step = 1; step < std::numeric_limits<size_t>().max(); step++) {
        for (uint8_t y = 0; y < HEIGHT; y++) {
            for (size_t x = 0; x < WIDTH; x++) {
                // First, the energy level of each octopus increases by `1`
                board[y][x]++;
                // Then, any octopus with an energy level greater than `9` *flashes*
                if (board[y][x] == 10) {
                    flashes.push(std::make_pair(y, x));
                }
            }
        }
        while (!flashes.empty()) {
            auto current = flashes.front();
            auto neighbors = get_neighbors(current.first, current.second);
            for (auto p : neighbors) {
                board[p.first][p.second]++;
                if (board[p.first][p.second] == 10) {
                    flashes.push(p);
                }
            }
            flashes.pop();
        }

        // Finally, any octopus that flashed during this step has its energy level set to `0`
        size_t counter = 0;
        for (uint8_t y = 0; y < HEIGHT; y++) {
            for (size_t x = 0; x < WIDTH; x++) {
                if (board[y][x] > 9) {
                    board[y][x] = 0;
                    if (step <= 100) part1++;
                }
                if (board[y][x] == 0) counter++;
            }
        }
        if (counter == WIDTH * HEIGHT) {
            part2 = std::min(step, part2);
            if (step >= 100) goto done;
        }

    }
done:


    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day11.txt");
    auto output = day11(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day11: examples") {
    auto test_cases = {
        make_tuple(
            "5483143223\n"
            "2745854711\n"
            "5264556173\n"
            "6141336146\n"
            "6357385478\n"
            "4167524645\n"
            "2176841721\n"
            "6882881134\n"
            "4846848554\n"
            "5283751526",
            "1656", "195"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day11(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day11, part 1 & part 2") {
    input_t in = parse::load_input("input/day11.txt");
    auto output = day11(in);
    CHECK_EQ("1617", output.answer[0]);
    CHECK_EQ("258", output.answer[1]);
}

#endif  // IS_TEST
