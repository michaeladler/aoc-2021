#include "day10.h"

using parse::input_t;

inline char close_bracket(char bracket) {
    switch (bracket) {
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        case '<':
            return '>';
        default:
            abort();
    };
}

inline int64_t compute_score(char bracket) {
    switch (bracket) {
        case ')':
            return 3;
        case ']':
            return 57;
        case '}':
            return 1197;
        case '>':
            return 25137;
        default:
            abort();
    }
}

inline int64_t compute_completion_score(char bracket) {
    switch (bracket) {
        case ')':
            return 1;
        case ']':
            return 2;
        case '}':
            return 3;
        case '>':
            return 4;
        default:
            abort();
    }
}

parse::output_t day10(input_t in) {
    int64_t part1 = 0, part2 = 0;

    std::vector<int64_t> all_completion_scores;
    all_completion_scores.reserve(100);

    while (in.len > 0) {
        ssize_t pos_eol = 0;
        while (pos_eol < in.len && in.s[pos_eol] != '\n') pos_eol++;
        std::span<const char> word = std::span(in.s, pos_eol);
        pos_eol++;
        in.s += pos_eol;
        in.len -= pos_eol;

        std::stack<char> open_chunks;

        int64_t completion_score = 0;

        for (auto c : word) {
            switch (c) {
                case '(':
                case '[':
                case '{':
                case '<':
                    open_chunks.push(c);
                    break;
                case ')':
                case ']':
                case '}':
                case '>':
                    const char last_open = open_chunks.top();
                    open_chunks.pop();
                    const char expected = close_bracket(last_open);
                    if (expected != c) {
                        part1 += compute_score(c);
                        goto found_error;
                    }
                    break;
            }
        }

        // at this point, the word is incomplete

        while (!open_chunks.empty()) {
            auto missing_bracket = close_bracket(open_chunks.top());
            open_chunks.pop();
            completion_score *= 5;
            completion_score += compute_completion_score(missing_bracket);
        }
        all_completion_scores.push_back(completion_score);

    found_error:;
    }

    std::sort(all_completion_scores.begin(), all_completion_scores.end());
    assert(all_completion_scores.size() % 2 == 1);
    part2 = all_completion_scores[all_completion_scores.size() / 2];

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day10.txt");
    auto output = day10(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day10: examples") {
    auto test_cases = {
        make_tuple(
            "[({(<(())[]>[[{[]{<()<>>\n"
            "[(()[<>])]({[<{<<[]>>(\n"
            "{([(<{}[<>[]}>{[]{[(<()>\n"
            "(((({<>}<{<{<>}{[]{[]{}\n"
            "[[<[([]))<([[{}[[()]]]\n"
            "[{[{({}]{}}([{[{{{}}([]\n"
            "{<[[]]>}<{[{[{[]{()[[[]\n"
            "[<(<(<(<{}))><([]([]()\n"
            "<{([([[(<>()){}]>(<<{{\n"
            "<{([{{}}[<[[[<>{}]]]>[]]",
            "26397", "288957"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day10(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day10, part 1 & part 2") {
    input_t in = parse::load_input("input/day10.txt");
    auto output = day10(in);
    CHECK_EQ("296535", output.answer[0]);
    CHECK_EQ("4245130838", output.answer[1]);
}

#endif  // IS_TEST
