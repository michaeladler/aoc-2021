#include "day15.h"

using parse::input_t;

const int MAX_ROWS = 500;
const int MAX_COLS = 500;
const size_t MAXV = 1 + MAX_COLS * MAX_ROWS;

static int8_t grid[MAX_ROWS][MAX_COLS];
static int cols = 0;
static int rows = 0;

// 2d to 1d, one-based
inline int encode(int x, int y) {
    return y * cols + x;
}
// 1d to 2d
inline iPair decode(int index) {
    int y = index / cols;
    int x = index % cols;
    return iPair(x, y);
}

void edges(iPair p, int neighbors[], int *count) {
    auto x = p.x;
    auto y = p.y;
    int i = 0;
    if (x > 0) neighbors[i++] = encode(x - 1, y);
    if (x + 1 < cols) neighbors[i++] = encode(x + 1, y);
    if (y > 0) neighbors[i++] = encode(x, y - 1);
    if (y + 1 < rows) neighbors[i++] = encode(x, y + 1);
    *count = i;
}

struct PQNode {
    int32_t distance;
    int32_t value;

    PQNode(int32_t distance, int32_t value) : distance(distance), value(value) {}

    inline bool operator>(const PQNode &other) const {
        return distance > other.distance;
    }
};

/*
 *  Based on https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
 */
void dijkstra(int x_start, int y_start, int distance[], int parent[]) {
    constexpr int MAXINT = std::numeric_limits<int>().max() >> 1;

    std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;

    const int nvertices = 1 + encode(rows - 1, cols - 1);

    for (int i = 0; i < nvertices; i++) {
        distance[i] = MAXINT;
        parent[i] = -1;
    }

    int start = encode(x_start, y_start);
    pq.push(PQNode(0, start));
    distance[start] = 0;

    int neighbors[4];
    int neighbors_count;

    while (!pq.empty()) {
        int v = pq.top().value;
        pq.pop();
        auto v2d = decode(v);
        edges(v2d, neighbors, &neighbors_count);
        for (int idx = 0; idx < neighbors_count; idx++) {
            auto w = neighbors[idx];
            auto w2d = decode(w);
            auto weight = grid[w2d.y][w2d.x];
            auto alt = distance[v] + weight;
            if (distance[w] > alt) {
                // Update distance
                distance[w] = alt;
                parent[w] = v;
                pq.push(PQNode(distance[w], w));
            }
        }
    }
}

parse::output_t day15(input_t in) {
    long part1 = 0, part2 = 0;

    int distance[MAXV]; /* distance vertex is from start */
    int parent[MAXV];   /* distance vertex is from start */

    while (in.len > 0) {
        cols = 0;
        while (*in.s >= '0' && *in.s <= '9') {
            grid[rows][cols++] = parse::positive<int8_t>(in, 1, false);
        }
        in.s++, in.len--;
        rows++;
    }
    assert(rows > 0 && rows <= MAX_ROWS / 5);
    assert(cols > 0 && cols <= MAX_COLS / 5);

    auto last_part1 = encode(cols - 1, rows - 1);
    dijkstra(0, 0, distance, parent);
    part1 = distance[last_part1];

    const auto width = cols;

    // grow grid
    cols *= 5;
    rows *= 5;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x < width && y < width) continue;
            if (x >= width) {  // take left value;
                grid[y][x] = (grid[y][x - width] + 1);
            } else {
                assert(y >= width);
                // take upper value
                grid[y][x] = (grid[y - width][x] + 1);
            }
            if (grid[y][x] > 9) grid[y][x] = 1;
        }
    }

    auto last_part2 = encode(cols - 1, rows - 1);
    dijkstra(0, 0, distance, parent);
    part2 = distance[last_part2];

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day15.txt");
    auto output = day15(in);

    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

void reset_state() {
    for (size_t i = 0; i < MAX_ROWS; i++) {
        for (size_t j = 0; j < MAX_COLS; j++) {
            grid[i][j] = 0;
        }
    }
    cols = 0;
    rows = 0;
}

TEST_CASE("day15: examples") {
    auto test_cases = {
        make_tuple(
            "1163751742\n"
            "1381373672\n"
            "2136511328\n"
            "3694931569\n"
            "7463417111\n"
            "1319128137\n"
            "1359912421\n"
            "3125421639\n"
            "1293138521\n"
            "2311944581",
            "40", "315"),
    };

    for (auto &tc : test_cases) {
        reset_state();
        auto first = std::string(std::get<0>(tc));
        DEBUG("Input:\n{}", &first);
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day15(in);
        DEBUG("Part 1: {}, Part 2: {}", &output.answer[0], &output.answer[1]);

        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day15, part 1 & part 2") {
    reset_state();
    input_t in = parse::load_input("input/day15.txt");
    auto output = day15(in);
    CHECK_EQ("687", output.answer[0]);
    CHECK_EQ("2957", output.answer[1]);
}

#endif  // IS_TEST
