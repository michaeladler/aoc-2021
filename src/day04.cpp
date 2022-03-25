#include "day04.h"

using parse::input_t;

struct Board {
    int id;
    bool board[5][5];
    std::unordered_map<short, std::pair<unsigned short, unsigned short>> num2pos;

    long sum_unmarked;

    Board(int id) : id(id) {
        for (size_t i = 0; i < 5; i++) {
            for (size_t j = 0; j < 5; j++) {
                board[i][j] = false;
            }
        }
        num2pos.reserve(25);
        sum_unmarked = 0;
    }

    void set_number(unsigned short row, unsigned short col, short number) {
        num2pos[number] = std::make_pair(row, col);
        sum_unmarked += number;
    }

    // returns a positive score on bingo
    long play_number(short number) {
        auto got = num2pos.find(number);
        if (got == num2pos.end()) return false;

        auto pos = got->second;
        auto row = pos.first;
        auto col = pos.second;

        if (board[row][col]) {
            return false;
        }

        board[row][col] = true;
        sum_unmarked -= number;

        // check for bingo
        {  // column
            int count = 0;
            for (int i = 0; i < 5; i++) {
                if (!board[row][i]) break;
                count++;
            }
            if (count == 5) return sum_unmarked * number;
        }
        {  // row
            int count = 0;
            for (int i = 0; i < 5; i++) {
                if (!board[i][col]) break;
                count++;
            }
            if (count == 5) return sum_unmarked * number;
        }
        return 0;
    }
};

parse::output_t day04(input_t in) {
    long part1 = 0;
    long part2 = 0;

    size_t shortest_moves = std::numeric_limits<size_t>().max();
    size_t longest_moves = std::numeric_limits<size_t>().min();

    std::array<short, 300> moves;
    size_t moves_count = 0;
    moves.fill(0);

    {
        while (in.len) {
            auto x = parse::positive<short>(in);
            moves[moves_count++] = x;
            auto c = *in.s;
            if (c == ',') {
                in.s++;
                in.len--;
            } else if (c == '\n') {
                break;
            }
        }
        in.s++, in.len--;
    }

    {
        int board_id = 0;
        while (in.len) {
            Board b(board_id++);
            for (int row = 0; row < 5; row++) {
                for (int col = 0; col < 5; col++) {
                    auto x = parse::positive<short>(in);
                    in.s++, in.len--;
                    b.set_number(row, col, x);
                }
            }

            size_t counter = 0;
            for (size_t i = 0; i < moves_count; i++) {
                auto x = moves[i];
                long score = b.play_number(x);
                if (score) {
                    if (counter < shortest_moves) {
                        shortest_moves = counter;
                        part1 = score;
                    }
                    if (counter > longest_moves) {
                        longest_moves = counter;
                        part2 = score;
                    }
                    break;
                }
                counter++;
            }

            while (in.len && *in.s < '0' && *in.s > '9') {
                in.s++;
                in.len--;
            }
        }
    }

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day04.txt");
    auto output = day04(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day04: examples") {
    auto test_cases = {
        make_tuple(
            "7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1\n"
            "\n"
            "22 13 17 11  0\n"
            " 8  2 23  4 24\n"
            "21  9 14 16  7\n"
            " 6 10  3 18  5\n"
            " 1 12 20 15 19\n"
            "\n"
            " 3 15  0  2 22\n"
            " 9 18 13 17  5\n"
            "19  8  7 25 23\n"
            "20 11 10 24  4\n"
            "14 21 16 12  6\n"
            "\n"
            "14 21 17 24  4\n"
            "10 16 15  9 19\n"
            "18  8 23 26 20\n"
            "22 11 13  6  5\n"
            " 2  0 12  3  7\n",
            "4512", "1924"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day04(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day04, part 1 & part 2") {
    input_t in = parse::load_input("input/day04.txt");
    auto output = day04(in);
    CHECK_EQ("55770", output.answer[0]);
    CHECK_EQ("2980", output.answer[1]);
}

#endif  // IS_TEST
