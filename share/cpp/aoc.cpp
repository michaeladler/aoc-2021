#include <chrono>

#include "aoc.h"

int aoc_main(int argc, char **argv, const std::map<int, advent_t> &days) {
    double total_time = 0;

    std::unordered_set<int> indices;

    if (argc > 1) {
        char *ptr;
        long x = strtol(argv[1], &ptr, 10);
        indices.insert(static_cast<int>(x));
    } else {
        indices.reserve(25);
        for (size_t day = 1; day <= 25; day++) {
            indices.insert(day);
        }
    }

    fmt::print("          Time         Part 1           Part 2\n"
               "========================================================\n");
    for (const auto element : days) {
        if (indices.find(element.first) == indices.end()) continue;
        auto &A = element.second;
        if (!A.fn) continue;

        char filename[32];
        sprintf(filename, "input/day%02d.txt", element.first);

        auto input = parse::load_input(filename);
        auto t0 = std::chrono::steady_clock::now();
        auto output = A.fn(input);
        auto elapsed = std::chrono::steady_clock::now() - t0;
        parse::free_input(input);

        double t = 1e-6 * std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
        total_time += t;

        fmt::print("Day {:02d}: {:9.3f} ms     {:<16} {:<16}\n",
                   element.first, t, output.answer[0], output.answer[1]);
    }
    fmt::print("========================================================\n"
               "Total:  {:9.3f} ms\n",
               total_time);

    return 0;
}
