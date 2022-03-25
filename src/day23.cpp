#include "day23.h"
#include <thread>

using parse::input_t;

#define COLS 13
#define ROWS 7
#define MAXCANDIDATES 32 /* max possible next extensions */
#define NMAX 1024        /* maximum solution size */

namespace Day23 {

struct Amphipod {
    char id;
    int8_t y, x;

    Amphipod() {}

    Amphipod(char id, int8_t y, int8_t x) : id(id), y(y), x(x) {}
};

struct Grid {
    char data[ROWS][COLS];

    inline size_t sideroom_x(int room) const {
        return 3 + 2 * room;
    }

    // returns true if empty
    bool side_room_top(int room, Amphipod &out) const {
        assert(room >= 0 && room <= 3);
        const int8_t x = sideroom_x(room);
        for (int8_t y = 2; y <= 5; y++) {
            if (data[y][x] >= 'A' && data[y][x] <= 'D') {
                out.id = data[y][x];
                out.y = y;
                out.x = x;
                return false;
            }
        }
        return true;
    }

    void print() {
        for (size_t y = 0; y < ROWS; y++) {
            for (size_t x = 0; x < COLS; x++) {
                fmt::print("{}", data[y][x]);
            }
            fmt::print("\n");
        }
    };
};

/* Begin Backtrack */

struct Move {
    char amphipod;
    int8_t x_from, y_from;
    int8_t x_to, y_to;

    inline long steps() const {
        return std::abs((long)y_to - (long)y_from) + std::abs((long)x_to - (long)x_from);
    }
};

struct BT {
    Grid grid;
    long min_energy = 100000;
};

static const char expected_order[] = {'A', 'B', 'C', 'D'};

/* Check if a[0], ... a[k] is a solution */
static bool is_a_solution(const BT *input) {
    for (size_t i = 0; i < 4; i++) {
        size_t x = input->grid.sideroom_x(i);
        for (size_t y = 2; input->grid.data[y][x] != '#'; y++) {
            if (input->grid.data[y][x] != expected_order[i]) return false;
        }
    }
    return true;
}

static const long costs[] = {1, 10, 100, 1000};
static long calc_cost(Move a[], int k) {
    return std::accumulate(a, a + k + 1, (long)0, [](long acc, Move move) { return acc + move.steps() * costs[move.amphipod - 'A']; });
}

static void process_solution(Move a[], int k, BT *input) {
    auto total_cost = calc_cost(a, k);
    DEBUG("found solution of length {} and cost {}", k, total_cost);
    if (total_cost < input->min_energy) {
        DEBUG("found solution of length {} and cost {}\n", k, total_cost);
        input->min_energy = total_cost;
    }
}

// row 1: #...........#
static const bool is_hallway_index[] = {false, true, true, false, true, false, true, false, true, false, true, true, false};
static const std::array<int8_t, 7> hallway_indices{1, 2, 4, 6, 8, 10, 11};
static const int8_t x_destination[4] = {3, 5, 7, 9};

/* What are possible candidates for position k?
 * The candidates are stored in c[] and its length is stored in ncandidates.
 */
static void construct_candidates(Move a[], int k, BT *input, Move c[], int *ncandidates) {
    *ncandidates = 0;
    if (k >= NMAX || (k > 0 && calc_cost(a, k - 1) >= input->min_energy)) {
        TRACE("k={}: cutting off search tree", k);
        return;
    }

    std::array<bool, COLS - 2> hallway_occupied;

    hallway_occupied.fill(false);

    for (size_t i = 0; i < hallway_indices.size(); i++) {
        char amphipod = input->grid.data[1][hallway_indices[i]];
        if (amphipod == '.') continue;
        hallway_occupied[hallway_indices[i]] = true;
        auto x = x_destination[amphipod - 'A'];

        int8_t dest_y;
        for (dest_y = 1; input->grid.data[dest_y][x] == '.'; dest_y++)
            ;
        dest_y--;
        if (dest_y == 1) continue;

        bool ok = true;
        // room contains no amphipods which do not also have that room as their own destination.
        for (int8_t y = dest_y + 1; ok; y++) {
            if (input->grid.data[y][x] == '#') break;
            ok = input->grid.data[y][x] == amphipod;
        }
        if (!ok) continue;

        bool is_blocked = false;
        int8_t delta = sgn(x - hallway_indices[i]);
        for (int8_t x_tmp = hallway_indices[i] + delta; x_tmp != x; x_tmp += delta) {
            if (input->grid.data[1][x_tmp] != '.') {
                TRACE("Amphipod {} is blocked due to 1,{}", amphipod, x_tmp);
                is_blocked = true;
            }
        }

        if (!is_blocked) {
            DEBUG("Amphipod {} can move to its destination at {},{}", amphipod, dest_y, x);
            c[*ncandidates] = {
                .amphipod = amphipod,
                .x_from = hallway_indices[i],
                .y_from = 1,
                .x_to = x,
                .y_to = dest_y,
            };
            *ncandidates = *ncandidates + 1;
        }
    }

    for (int room_id = 0; room_id < 4; room_id++) {
        Amphipod amphipod;
        bool empty = input->grid.side_room_top(room_id, amphipod);  // only the upper one can move
        if (empty) continue;

        if (amphipod.x == x_destination[amphipod.id - 'A']) {
            bool ok = true;
            for (size_t y = amphipod.y + 1; ok && input->grid.data[y][amphipod.x] != '#'; y++) {
                ok = input->grid.data[y][amphipod.x] == amphipod.id;
            }
            if (ok) {
                // already at destination
                continue;
            }
        }

        // check to the right
        for (int8_t x_candidate = amphipod.x + 1; x_candidate < COLS - 1; x_candidate++) {
            if (is_hallway_index[x_candidate]) {
                if (!hallway_occupied[x_candidate]) {
                    TRACE("Amphipod {} can move to hallway 1,{}", amphipods[0].id, x_candidate);
                    c[*ncandidates] = {
                        .amphipod = amphipod.id,
                        .x_from = amphipod.x,
                        .y_from = amphipod.y,
                        .x_to = x_candidate,
                        .y_to = 1,
                    };
                    *ncandidates = *ncandidates + 1;
                } else {
                    break;
                }
            }
        }

        // check to the left
        for (int8_t x_candidate = amphipod.x - 1; x_candidate >= 0; x_candidate--) {
            if (is_hallway_index[x_candidate]) {
                if (!hallway_occupied[x_candidate]) {
                    TRACE("Amphipod {} can move to hallway 1,{}", amphipods[0].id, x_candidate);
                    c[*ncandidates] = {
                        .amphipod = amphipod.id,
                        .x_from = amphipod.x,
                        .y_from = amphipod.y,
                        .x_to = x_candidate,
                        .y_to = 1,
                    };
                    *ncandidates = *ncandidates + 1;
                } else {
                    break;
                }
            }
        }
    }
}

static inline void make_move(Move a[], int k, BT *input) {
    const auto move = a[k];
    input->grid.data[move.y_from][move.x_from] = '.';
    input->grid.data[move.y_to][move.x_to] = move.amphipod;
}

static inline void unmake_move(Move a[], int k, BT *input) {
    const auto move = a[k];
    input->grid.data[move.y_from][move.x_from] = move.amphipod;
    input->grid.data[move.y_to][move.x_to] = '.';
}

/*
Copyright 2003 by Steven S. Skiena; all rights reserved.

Permission is granted for use in non-commerical applications
provided this copyright notice remains intact and unchanged.

This program appears in my book:

"Programming Challenges: The Programming Contest Training Manual"
by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.

See our website www.programming-challenges.com for additional information.

This book can be ordered from Amazon.com at

http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/

*/
void backtrack(Move a[], int k, BT *input) {
    Move c[MAXCANDIDATES]; /* candidates for next position */
    int ncandidates;       /* next position candidate count */
    int i;                 /* counter */

    if (is_a_solution(input)) {
        process_solution(a, k, input);
    } else {
        k++;
        construct_candidates(a, k, input, c, &ncandidates);
        for (i = 0; i < ncandidates; i++) {
            DEBUG("k={}: moving {} from {},{} to {},{}", k, c[i].amphipod, c[i].y_from, c[i].x_from, c[i].y_to, c[i].x_to);
            a[k] = c[i];
            make_move(a, k, input);
#ifdef DEBUGLOG
            input.grid.print();
#endif

            backtrack(a, k, input);
            unmake_move(a, k, input);
        }
    }
}

/* End Backtrack */

}  // namespace Day23

parse::output_t day23(input_t in) {
    Day23::BT bt_part1, bt_part2;
    {  // parse
        size_t y = 0;
        while (in.len > 0) {
            size_t x = 0;
            while (in.len > 0 && *in.s != '\n') {
                bt_part1.grid.data[y][x] = *in.s;
                bt_part2.grid.data[y >= 3 ? y + 2 : y][x] = *in.s;
                x++;
                in.s++, in.len--;
            }
            y++;
            in.s++, in.len--;
        }
    }
    {
        size_t i = 0;
        for (char c : {' ', ' ', '#', 'D', '#', 'C', '#', 'B', '#', 'A', '#', ' ', ' '}) bt_part2.grid.data[3][i++] = c;
        i = 0;
        for (char c : {' ', ' ', '#', 'D', '#', 'B', '#', 'A', '#', 'C', '#', ' ', ' '}) bt_part2.grid.data[4][i++] = c;
    }

    Day23::Move moves2[NMAX];
    std::thread second(Day23::backtrack, moves2, -1, &bt_part2);

    Day23::Move moves1[NMAX];
    backtrack(moves1, -1, &bt_part1);

    second.join();

    return {bt_part1.min_energy, bt_part2.min_energy};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day23.txt");
    auto output = day23(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day23: examples") {
    auto test_cases = {
        make_tuple(
            "#############\n"
            "#...........#\n"
            "###B#C#B#D###\n"
            "  #A#D#C#A#\n"
            "  ######### \n",
            12521, 44169),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        DEBUG("Input:\n{}", first);
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day23(in);

        CHECK_EQ(std::get<1>(tc), std::strtol(output.answer[0].c_str(), NULL, 10));
        CHECK_EQ(std::get<2>(tc), std::strtol(output.answer[1].c_str(), NULL, 10));
    }
}

TEST_CASE("day23, part 1 & part 2") {
    input_t in = parse::load_input("input/day23.txt");
    auto output = day23(in);
    CHECK_EQ(15472, std::strtol(output.answer[0].c_str(), NULL, 10));
    CHECK_EQ(46182, std::strtol(output.answer[1].c_str(), NULL, 10));
}

#endif  // IS_TEST
