#include "day14.h"
#include <algorithm>
#include <cstdint>
#include <limits>
#include <tuple>
#include <unordered_map>

using parse::input_t;

struct pair_hash {
    inline std::size_t operator()(const std::pair<char, char> &v) const {
        return v.first * 31 + v.second;
    }
};

parse::output_t day14(input_t in) {
    size_t part1 = 0, part2 = 0;

    std::unordered_map<std::pair<char, char>, char, pair_hash> rules;
    rules.reserve(100);

    std::unordered_map<std::pair<char, char>, int64_t, pair_hash> polymers[2];
    polymers[0].reserve(26 * 26);
    polymers[1].reserve(26 * 26);

    std::unordered_map<char, int64_t> frequencies;
    frequencies.reserve(26);

    {
        size_t counter;
        for (counter = 0; *(in.s + counter) != '\n'; counter++)
            ;
        assert(counter > 0);
        for (size_t i = 0; i < counter - 1; i++) {
            char c1 = *(in.s + i);
            char c2 = *(in.s + i + 1);
            frequencies[c1] += 1;
            polymers[0][std::make_pair(c1, c2)]++;
        }
        frequencies[*(in.s + counter - 1)] += 1;
        in.s += counter, in.len -= counter;
    }

    while (in.len > 6) {
        while (in.len > 0 && (*in.s < 'A' || *in.s > 'Z')) {
            in.s++, in.len--;
        }
        auto lhs1 = *in.s;
        in.s++, in.len--;
        auto lhs2 = *in.s;
        in.s += 5, in.len -= 5;
        auto rhs = *in.s;
        in.s++, in.len--;

        assert(lhs1 >= 'A' && lhs1 <= 'Z');
        assert(lhs2 >= 'A' && lhs2 <= 'Z');
        assert(rhs >= 'A' && rhs <= 'Z');

        rules[std::make_pair(lhs1, lhs2)] = rhs;
    }

    uint8_t activeidx = 0;
    uint8_t inactiveidx = 1;

    constexpr uint16_t TOTAL_STEPS = 40;
    for (uint16_t step = 1; step <= TOTAL_STEPS; ++step) {
        polymers[inactiveidx].clear();

        for (auto const &kv : polymers[activeidx]) {
            const auto count = kv.second;
            auto got = rules.find(kv.first);
            if (got != rules.end()) {  // found, so split it
                char split = got->second;
                char first = kv.first.first;
                char second = kv.first.second;

                frequencies[split] += count;
                polymers[inactiveidx][std::make_pair(first, split)] += count;
                polymers[inactiveidx][std::make_pair(split, second)] += count;
            } else {  // not found, so just keep it unless
                polymers[inactiveidx][kv.first] += count;
            }
        }
        activeidx = (activeidx + 1) % 2;
        inactiveidx = (inactiveidx + 1) % 2;

        if (step == 10) {
            auto max = std::numeric_limits<int64_t>().min();
            auto min = std::numeric_limits<int64_t>().max();
            for (const auto &kv : frequencies) {
                if (kv.second > max) max = kv.second;
                if (kv.second < min) min = kv.second;
            }
            assert(max >= min);
            part1 = max - min;
        }
    }

    auto max = std::numeric_limits<int64_t>().min();
    auto min = std::numeric_limits<int64_t>().max();
    for (const auto &kv : frequencies) {
        if (kv.second > max) max = kv.second;
        if (kv.second < min) min = kv.second;
    }
    assert(max >= min);
    part2 = max - min;

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day14.txt");
    auto output = day14(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day14: examples") {
    auto test_cases = {
        make_tuple(
            "NNCB\n\n"
            "CH -> B\n"
            "HH -> N\n"
            "CB -> H\n"
            "NH -> C\n"
            "HB -> C\n"
            "HC -> B\n"
            "HN -> C\n"
            "NN -> C\n"
            "BH -> H\n"
            "NC -> B\n"
            "NB -> B\n"
            "BN -> B\n"
            "BB -> N\n"
            "BC -> B\n"
            "CC -> N\n"
            "CN -> C",
            "1588", "2188189693529"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day14(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day14, part 1 & part 2") {
    input_t in = parse::load_input("input/day14.txt");
    auto output = day14(in);
    CHECK_EQ("2988", output.answer[0]);
    CHECK_EQ("3572761917024", output.answer[1]);
}

#endif  // IS_TEST
