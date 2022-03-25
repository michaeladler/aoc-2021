#include "day05.h"

using parse::input_t;

constexpr size_t WIDTH = 1000;
constexpr size_t HEIGHT = 1000;

static short board[WIDTH][HEIGHT];
static short board2[WIDTH][HEIGHT];

parse::output_t day05(input_t in) {
    long part1 = 0, part2 = 0;

    while (in.len >= 10) {
        auto x1 = parse::positive<short>(in);
        in.s++;
        in.len--;
        auto y1 = parse::positive<short>(in);
        in.s += 4;
        in.len -= 4;
        auto x2 = parse::positive<short>(in);
        in.s++;
        in.len--;
        auto y2 = parse::positive<short>(in);

        if (x1 == x2) {
            for (int i = std::min(y1, y2); i <= std::max(y1, y2); i++) {
                board[x1][i]++;
                board2[x1][i]++;
            }
        } else if (y1 == y2) {
            for (int i = std::min(x1, x2); i <= std::max(x1, x2); i++) {
                board[i][y1]++;
                board2[i][y1]++;
            }
        } else {
            if (x2 - x1 == y2 - y1 || x1 - x2 == y2 - y1) {
                short dx = x1 < x2 ? 1 : -1;
                short dy = y1 < y2 ? 1 : -1;
                while (x1 != x2 && y1 != y2) {
                    board2[x1][y1]++;
                    x1 += dx;
                    y1 += dy;
                }
                board2[x1][y1]++;
            }
        }

        while (in.len > 0 && *in.s < '0' && *in.s > '9') {
            in.s++;
            in.len--;
        }
    }

    // TODO: this is very inefficient; keep track where lines begin and end in each row/column
    for (size_t i = 0; i < WIDTH; i++) {
        for (size_t j = 0; j < HEIGHT; j++) {
            if (board[i][j] > 1) part1++;
            if (board2[i][j] > 1) part2++;
        }
    }

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day05.txt");
    auto output = day05(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day05: examples") {
    memset(board, 0, sizeof(board));
    memset(board2, 0, sizeof(board));

    auto test_cases = {
        make_tuple(
            "0,9 -> 5,9\n"
            "8,0 -> 0,8\n"
            "9,4 -> 3,4\n"
            "2,2 -> 2,1\n"
            "7,0 -> 7,4\n"
            "6,4 -> 2,0\n"
            "0,9 -> 2,9\n"
            "3,4 -> 1,4\n"
            "0,0 -> 8,8\n"
            "5,5 -> 8,2\n",
            "5", "12"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day05(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day05, part 1 & part 2") {
    memset(board, 0, sizeof(board));
    memset(board2, 0, sizeof(board));

    input_t in = parse::load_input("input/day05.txt");
    auto output = day05(in);
    CHECK_EQ("5442", output.answer[0]);
    CHECK_EQ("19571", output.answer[1]);
}

#endif  // IS_TEST
