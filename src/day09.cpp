#include "day09.h"
#include <utility>

using parse::input_t;

constexpr size_t MAX_COLS = 100;
constexpr size_t MAX_ROWS = 100;

// state
static uint8_t heatmap[MAX_ROWS][MAX_COLS];
static ssize_t rows;
static ssize_t cols;

inline bool is_lowpoint(ssize_t y, ssize_t x) {
    auto val = heatmap[y][x];
    if (x > 0 && heatmap[y][x - 1] <= val) return false;
    if (x + 1 < cols && heatmap[y][x + 1] <= val) return false;
    if (y + 1 < rows && heatmap[y + 1][x] <= val) return false;
    if (y > 0 && heatmap[y - 1][x] <= val) return false;
    return true;
}

inline long risk_score(ssize_t i, ssize_t j) { return 1 + heatmap[i][j]; }

parse::output_t day09(input_t in) {
    long part1 = 0, part2 = 0;

    while (in.len > 0) {
        cols = 0;
        while (in.len && *in.s != '\n') {
            heatmap[rows][cols++] = parse::positive<uint8_t>(in, 1, false);
        }
        rows++;
        in.s++;
        in.len--;
    }

    std::unordered_set<iPair, std::hash<iPair>> visited;
    std::vector<int> basin_sizes;
    basin_sizes.reserve(100);

    for (short y = 0; y < rows; y++) {
        for (short x = 0; x < cols; x++) {
            if (is_lowpoint(y, x)) {
                part1 += risk_score(y, x);

                // part 2: bfs
                std::queue<iPair> queue;
                queue.emplace(x, y);
                visited.emplace(x, y);
                int current_basin_size = 1;
                uint8_t lowest = heatmap[y][x];
                while (!queue.empty()) {
                    auto current = queue.front();
                    queue.pop();
                    // check neighbors
                    std::vector<iPair> neighbors;
                    neighbors.reserve(4);
                    if (current.x > 0) neighbors.emplace_back(current.x - 1, current.y);
                    if (current.x + 1 < cols) neighbors.emplace_back(current.x + 1, current.y);
                    if (current.y + 1 < rows) neighbors.emplace_back(current.x, current.y + 1);
                    if (current.y > 0) neighbors.emplace_back(current.x, current.y - 1);

                    for (auto nb : neighbors) {
                        if (heatmap[nb.y][nb.x] != 9 && heatmap[nb.y][nb.x] > lowest) {
                            if (!visited.contains(nb)) {
                                queue.push(nb);
                                visited.insert(nb);
                                current_basin_size++;
                            }
                        }
                    }
                }
                basin_sizes.push_back(current_basin_size);
            }
        }
    }
    std::sort(basin_sizes.begin(), basin_sizes.end(), [](auto x, auto y) -> bool { return x > y; });

    part2 = (long)basin_sizes[0] * (long)basin_sizes[1] * (long)basin_sizes[2];

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day09.txt");
    auto output = day09(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

void reset_state() {
    memset(heatmap, 0, MAX_ROWS * MAX_COLS);
    rows = 0;
    cols = 0;
}

TEST_CASE("day09: examples") {
    auto test_cases = {
        make_tuple("2199943210\n"
                   "3987894921\n"
                   "9856789892\n"
                   "8767896789\n"
                   "9899965678",
                   "15", "1134"),
    };

    for (auto& tc : test_cases) {
        reset_state();

        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day09(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day09, part 1 & part 2") {
    input_t in = parse::load_input("input/day09.txt");
    reset_state();
    auto output = day09(in);

    CHECK_EQ("600", output.answer[0]);
    CHECK_EQ("987840", output.answer[1]);
}

#endif  // IS_TEST
