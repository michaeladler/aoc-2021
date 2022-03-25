#include "day12.h"

#include <cstdint>
#include <graph.h>
#include <unordered_set>

using graph::Graph;
using parse::input_t;
using node_t = uint8_t;

namespace Day12 {

using MyGraph = graph::Graph<100, 50, uint8_t>;

struct BT {
    std::unordered_map<node_t, std::string> id2label;
    std::unordered_map<std::string, node_t> label2id;
    MyGraph *graph;
    uint32_t part1, part2;
    uint8_t start, end;
};

typedef BT *data; /* type to pass data to backtrack */

#define MAXCANDIDATES 100 /* max possible next extensions */
#define NMAX 100          /* maximum solution size */

static bool finished = false; /* found all solutions yet? */

static bool is_a_solution(int a[], int k, data ctx) {
    return a[k] == ctx->end && a[0] == ctx->start;
}

static void process_solution(int a[], int k, data ctx) {
    std::unordered_map<int, uint32_t> small_count;
    for (int i = 0; i <= k; i++) {
        std::string label = ctx->id2label[a[i]];
        if (label[0] >= 'a' && label[0] <= 'z') {
            small_count[a[i]]++;
        }
    }

    size_t duplicates = 0;
    for (auto kv : small_count) {
        if (kv.second > 1)
            duplicates++;
    }
    if (duplicates == 0)
        ctx->part1++;
    if (duplicates <= 1)
        ctx->part2++;
}

/*	What are possible elements for position k? */
static void construct_candidates(int a[], int k, data ctx, int c[],
                                 int *ncandidates) {
    assert(k > 0);
    std::unordered_map<int, uint32_t> small_count;
    small_count.reserve(k);
    for (int i = 0; i < k; i++) {
        std::string label = ctx->id2label[a[i]];
        // is lowercase?
        if (label[0] >= 'a' && label[0] <= 'z')
            small_count[a[i]]++;
    }
    size_t duplicates = 0;
    for (auto kv : small_count) {
        if (kv.second > 1)
            duplicates += kv.second;
    }
    if (duplicates > 2) {
        *ncandidates = 0;
        return;  // no candidates
    }

    int count = 0;
    for (auto x : ctx->graph->edges(a[k - 1])) {
        if (x != ctx->start)
            c[count++] = x;
    }
    *ncandidates = count;
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
static void backtrack(int a[], int k, data input) {
    int c[MAXCANDIDATES]; /* candidates for next position */
    int ncandidates;      /* next position candidate count */
    int i;                /* counter */

    if (is_a_solution(a, k, input)) {
        process_solution(a, k, input);
    } else {
        k = k + 1;
        construct_candidates(a, k, input, c, &ncandidates);
        for (i = 0; i < ncandidates; i++) {
            a[k] = c[i];
            backtrack(a, k, input);
            if (finished)
                return; /* terminate early */
        }
    }
}

}  // namespace Day12

parse::output_t day12(input_t in) {
    Day12::MyGraph g;
    Day12::BT bt;
    bt.graph = &g;
    bt.id2label.reserve(32);
    bt.label2id.reserve(32);

    node_t id_counter = 1;
    auto parse_word = [](input_t &in) -> std::string {
        const char *first = in.s;
        size_t count = 0;

        while (in.len > 0 &&
               ((*in.s >= 'a' && *in.s <= 'z') || (*in.s >= 'A' && *in.s <= 'Z'))) {
            in.s++, in.len--;
            count++;
        }
        return std::string(first, count);
    };
    while (in.len > 0) {
        auto from = parse_word(in);

        node_t from_id;
        if (!bt.label2id.contains(from)) {
            from_id = id_counter;
            bt.label2id[from] = from_id;
            bt.id2label[from_id] = from;
            id_counter++;
        } else {
            from_id = bt.label2id[from];
        }

        in.s++;
        in.len--;
        auto to = parse_word(in);

        node_t to_id;
        if (!bt.label2id.contains(to)) {
            to_id = id_counter;
            bt.label2id[to] = to_id;
            bt.id2label[to_id] = to;
            id_counter++;
        } else {
            to_id = bt.label2id[to];
        }

        g.insert_edge(from_id, to_id, false);

        in.s++;
        in.len--;
    }

    node_t start_id = bt.label2id["start"];
    node_t end_id = bt.label2id["end"];

    bt.start = start_id;
    bt.end = end_id;
    bt.part1 = 0;
    bt.part2 = 0;

    int a[MAXCANDIDATES];
    a[0] = start_id;
    backtrack(a, 0, &bt);

    return {bt.part1, bt.part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day12.txt");
    auto output = day12(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day12: examples") {
    auto test_cases = {
        make_tuple("start-A\n"
                   "start-b\n"
                   "A-c\n"
                   "A-b\n"
                   "b-d\n"
                   "A-end\n"
                   "b-end",
                   "10", "36"),
        make_tuple("dc-end\n"
                   "HN-start\n"
                   "start-kj\n"
                   "dc-start\n"
                   "dc-HN\n"
                   "LN-dc\n"
                   "HN-end\n"
                   "kj-sa\n"
                   "kj-HN\n"
                   "kj-dc",
                   "19", "103"),
        make_tuple("fs-end\n"
                   "he-DX\n"
                   "fs-he\n"
                   "start-DX\n"
                   "pj-DX\n"
                   "end-zg\n"
                   "zg-sl\n"
                   "zg-pj\n"
                   "pj-he\n"
                   "RW-he\n"
                   "fs-DX\n"
                   "pj-RW\n"
                   "zg-RW\n"
                   "start-pj\n"
                   "he-WI\n"
                   "zg-he\n"
                   "pj-fs\n"
                   "start-RW",
                   "226", "3509"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day12(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day12, part 1 & part 2") {
    input_t in = parse::load_input("input/day12.txt");
    auto output = day12(in);
    CHECK_EQ("3761", output.answer[0]);
    CHECK_EQ("99138", output.answer[1]);
}

#endif  // IS_TEST
