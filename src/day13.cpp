#include "day13.h"

using parse::input_t;

struct Fold {
    int32_t pos;
    bool horizontal;  // e.g. y=7, fold paper up

    friend std::ostream &operator<<(std::ostream &os, const Fold &f) {
        os << "fold along " << (f.horizontal ? "y" : "x") << "=" << f.pos;
        return os;
    }
};

using Board = std::unordered_set<iPair, std::hash<iPair>>;

void print_small_board(std::stringstream &os, const Board &board) {
    int32_t y_max = std::numeric_limits<int32_t>().min();
    int32_t x_max = std::numeric_limits<int32_t>().min();
    for (auto p : board) {
        if (p.x > x_max) x_max = p.x;
        if (p.y > y_max) y_max = p.y;
    }

    os << std::endl;
    for (int32_t y = 0; y <= y_max; y++) {
        for (int32_t x = 0; x <= x_max; x++) {
            if (board.contains(iPair(x, y))) {
                os << "#";
            } else {
                os << ".";
            }
        }
        os << std::endl;
    }
}

parse::output_t day13(input_t in) {
    size_t part1 = 0;
    std::string part2;

    Board points;

    while (in.len > 0) {
        auto x = parse::positive<int32_t>(in);
        in.s++, in.len--;
        auto y = parse::positive<int32_t>(in);
        points.emplace(x, y);
        in.s++, in.len--;
        if (*in.s == '\n') break;
    }

    std::vector<Fold> folds;
    folds.reserve(8);
    while (in.len > 8) {
        while (in.len > 0 && *in.s != 'y' && *in.s != 'x') {
            in.s += 1, in.len -= 1;
        }
        bool horizontal = *in.s == 'y';
        in.s += 1, in.len -= 1;
        int32_t pos = parse::positive<int32_t>(in);
        folds.push_back(Fold{pos, horizontal});
    }

    std::vector<iPair> new_points;
    new_points.reserve(1024);

    auto apply_fold = [&](const Fold &fold) {
        new_points.clear();
        if (fold.horizontal) { /* fold the paper up */
            for (auto it = points.begin(); it != points.end();) {
                auto y = it->y;
                if (y > fold.pos) {
                    int32_t delta = y - fold.pos;
                    new_points.emplace_back(it->x, y - 2 * delta);
                    it = points.erase(it);
                } else {
                    it++;
                }
            }
        } else { /* fold left */
            for (auto it = points.begin(); it != points.end();) {
                auto x = it->x;
                if (x > fold.pos) {
                    int32_t delta = x - fold.pos;
                    new_points.emplace_back(x - 2 * delta, it->y);
                    it = points.erase(it);
                } else {
                    it++;
                }
            }
        }
        for (auto p : new_points) {
            points.insert(std::move(p));
        }
    };

    apply_fold(folds[0]);

    part1 = points.size();

    for (size_t i = 1; i < folds.size(); i++) {
        apply_fold(folds[i]);
    }

    std::stringstream ss;
    print_small_board(ss, points);

    part2 = ss.str();

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day13.txt");
    auto output = day13(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day13: examples") {
    auto test_cases = {
        make_tuple(
            "6,10\n"
            "0,14\n"
            "9,10\n"
            "0,3\n"
            "10,4\n"
            "4,11\n"
            "6,0\n"
            "6,12\n"
            "4,1\n"
            "0,13\n"
            "10,12\n"
            "3,4\n"
            "3,0\n"
            "8,4\n"
            "1,10\n"
            "2,14\n"
            "8,10\n"
            "9,0\n"
            "\n"
            "fold along y=7\n"
            "fold along x=5 \n",
            "17"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day13(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
    }
}

TEST_CASE("day13, part 1 & part 2") {
    input_t in = parse::load_input("input/day13.txt");
    auto output = day13(in);
    CHECK_EQ("669", output.answer[0]);
    CHECK_EQ(
        "\n"
        "#..#.####.####.####..##..#..#..##....##\n"
        "#..#.#....#.......#.#..#.#..#.#..#....#\n"
        "#..#.###..###....#..#....#..#.#.......#\n"
        "#..#.#....#.....#...#....#..#.#.......#\n"
        "#..#.#....#....#....#..#.#..#.#..#.#..#\n"
        ".##..####.#....####..##...##...##...##.\n",
        output.answer[1]);
}

#endif  // IS_TEST
