#include "day08.h"

using parse::input_t;

using digit = short;

/* Representation as a bitset:
 *                 0
 *               +---+
 *             1 |   | 2
 *               +-3-+
 *             4 |   | 5
 *               +---+
 *                 6
 *
 *
 *  0:      1:      2:      3:      4:
 *  aaaa    ....    aaaa    aaaa    ....
 * b    c  .    c  .    c  .    c  b    c
 * b    c  .    c  .    c  .    c  b    c
 *  ....    ....    dddd    dddd    dddd
 * e    f  .    f  e    .  .    f  .    f
 * e    f  .    f  e    .  .    f  .    f
 *  gggg    ....    gggg    gggg    ....
 *
 *   5:      6:      7:      8:      9:
 *  aaaa    aaaa    aaaa    aaaa    aaaa
 * b    .  b    .  .    c  b    c  b    c
 * b    .  b    .  .    c  b    c  b    c
 *  dddd    dddd    ....    dddd    dddd
 * .    f  e    f  .    f  e    f  .    f
 * .    f  e    f  .    f  e    f  .    f
 *  gggg    gggg    ....    gggg    gggg
 *
 */
constexpr digit ZERO = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 4) | (1 << 5) | (1 << 6);
constexpr digit ONE = (1 << 2) | (1 << 5);
constexpr digit TWO = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 6);
constexpr digit THREE = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6);
constexpr digit FOUR = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 5);
constexpr digit FIVE = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 5) | (1 << 6);
constexpr digit SIX = (1 << 0) | (1 << 1) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);
constexpr digit SEVEN = (1 << 0) | (1 << 2) | (1 << 5);
constexpr digit EIGHT = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);
constexpr digit NINE = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6);

inline char l2pos(char c) {
    assert(c >= 'a');
    return c - 'a';
}

// Returns -1 on error.
int8_t word2number(std::array<short, 7> &assignment, std::span<const char> word) {
    digit d = 0;
    for (auto c : word) {
        size_t pos = assignment[l2pos(c)];
        d |= 1 << pos;
    }
    switch (d) {
        case ZERO:
            return 0;
        case ONE:
            return 1;
        case TWO:
            return 2;
        case THREE:
            return 3;
        case FOUR:
            return 4;
        case FIVE:
            return 5;
        case SIX:
            return 6;
        case SEVEN:
            return 7;
        case EIGHT:
            return 8;
        case NINE:
            return 9;
        default:
            return -1;
    }
}

bool is_valid(std::array<short, 7> &assignment, std::vector<std::span<const char>> &words) {
    for (auto &some_word : words) {
        auto number = word2number(assignment, some_word);
        if (number < 0) {
            return false;
        }
    }
    return true;
}

bool solve(std::vector<std::span<const char>> &words, std::array<short, 7> &assignment) {
    /* Numbers uniquely determined by its length:
     *   1) 1 has two segments
     *   2) 7 has three segments
     *   3) 4 has four segments
     *   4) 8 has seven segments
     *
     * Algorithm:
     *
     * 1) Find word for 1). Assign letters to slots 2,5.
     * 2) Find word for 2). Knowning 1), this uniquely determines 0.
     * 3) Find word for 3). Knowing 1), this determines 1,3.
     * 4) The two remaining letters must be assigned to 4,6.
     * 5) This leaves us with 64 combinations. Try each one and see if we can get a contradiction using ZERO, ..., EIGHT.
     */
    std::span<const char> two_letter, three_letter, four_letter;
    for (auto &w : words) {
        const size_t n = w.size();
        switch (n) {
            case 2:
                two_letter = w;
                break;
            case 3:
                three_letter = w;
                break;
            case 4:
                four_letter = w;
                break;
        }
    }

    // a is pos 0, b is pos 0 etc.; the values are the assigned segments.
    std::array<std::unordered_set<short>, 7> candidates;
    // Step 1
    for (int i = 0; i < 2; i++) {
        candidates[l2pos(two_letter[i])].insert(2);
        candidates[l2pos(two_letter[i])].insert(5);
    }

    // Step 2
    for (int i = 0; i < 3; i++) {
        if (three_letter[i] != two_letter[0] && three_letter[i] != two_letter[1]) {
            candidates[l2pos(three_letter[i])].insert(0);
        }
    }

    // Step 3
    for (int i = 0; i < 4; i++) {
        if (four_letter[i] != two_letter[0] && four_letter[i] != two_letter[1]) {
            candidates[l2pos(four_letter[i])].insert(1);
            candidates[l2pos(four_letter[i])].insert(3);
        }
    }

    for (size_t i = 0; i < candidates.size(); i++) {
        if (candidates[i].empty()) {
            candidates[i].insert(4);
            candidates[i].insert(6);
        }
    }


    bool found = false;
    for (auto a_value : candidates[0]) {
        assignment[0] = a_value;
        for (auto b_value : candidates[1]) {
            if (a_value == b_value) continue;
            assignment[1] = b_value;
            for (auto c_value : candidates[2]) {
                if (a_value == c_value || b_value == c_value) continue;
                assignment[2] = c_value;
                for (auto d_value : candidates[3]) {
                    if (a_value == d_value || b_value == d_value || c_value == d_value) continue;
                    assignment[3] = d_value;
                    for (auto e_value : candidates[4]) {
                        if (a_value == e_value || b_value == e_value || c_value == e_value || d_value == e_value) continue;
                        assignment[4] = e_value;
                        for (auto f_value : candidates[5]) {
                            if (a_value == f_value || b_value == f_value || c_value == f_value || d_value == f_value || e_value == f_value) continue;
                            assignment[5] = f_value;
                            for (auto g_value : candidates[6]) {
                                if (a_value == g_value || b_value == g_value || c_value == g_value || d_value == g_value || e_value == g_value || f_value == g_value) continue;
                                assignment[6] = g_value;
                                if (is_valid(assignment, words)) {
                                    found = true;
                                    goto done;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

done:
    return found;
}

parse::output_t day08(input_t in) {
    uint64_t part1 = 0, part2 = 0;

    std::vector<std::span<const char>> words;
    words.reserve(10);

    std::vector<short> rhs_digits;
    rhs_digits.reserve(4);

    std::array<short, 7> assignment;

    bool rhs = false;
    while (in.len > 0) {
        auto word = parse::word(in, 'a', 'z');
        if (!rhs) {
            words.push_back(word);
        } else {
            auto n = word.size();
            if (n == 2 || n == 3 || n == 4 || n == 7) part1++;
            int8_t number = word2number(assignment, word);
            rhs_digits.push_back(number);
        }
        parse::skip_whitespace(in);
        if (*in.s == '|') {
            solve(words, assignment);
            rhs = true;
            in.s++;
            in.len--;
            parse::skip_whitespace(in);
        } else if (*in.s == '\n') {
            rhs = false;

            uint64_t entry = rhs_digits[3] + 10 * rhs_digits[2] + 100 * rhs_digits[1] + 1000 * rhs_digits[0];
            part2 += entry;

            words.clear();
            rhs_digits.clear();
            in.s++;
            in.len--;
        }
    }

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day08.txt");
    auto output = day08(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day08: examples") {
    auto test_cases = {
        make_tuple(
            "be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe\n"
            "edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc\n"
            "fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg\n"
            "fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb\n"
            "aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea\n"
            "fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb\n"
            "dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe\n"
            "bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef\n"
            "egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb\n"
            "gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce\n",
            "26", "61229"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day08(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day08, part 1 & part 2") {
    input_t in = parse::load_input("input/day08.txt");
    auto output = day08(in);
    CHECK_EQ("421", output.answer[0]);
    CHECK_EQ("986163", output.answer[1]);
}

TEST_CASE("solve") {
    std::vector<std::span<const char>> words;

    const char *a = "acedgfb";
    const char *b = "cdfbe";
    const char *c = "gcdfa";
    const char *d = "fbcad";
    const char *e = "dab";
    const char *f = "cefabd";
    const char *g = "cdfgeb";
    const char *h = "eafb";
    const char *i = "cagedb";
    const char *j = "ab";

    words.emplace_back(a, strlen(a));
    words.emplace_back(b, strlen(b));
    words.emplace_back(c, strlen(c));
    words.emplace_back(d, strlen(d));
    words.emplace_back(e, strlen(e));
    words.emplace_back(f, strlen(f));
    words.emplace_back(g, strlen(g));
    words.emplace_back(h, strlen(h));
    words.emplace_back(i, strlen(i));
    words.emplace_back(j, strlen(j));

    /* Expected:
     *   dddd
     *  e    a
     *  e    a
     *   ffff
     *  g    b
     *  g    b
     *   cccc
     */
    // expected = {2, 5, 6, 0, 1, 3, 4}
    std::array<short, 7> expected;
    expected[l2pos('d')] = 0;
    expected[l2pos('e')] = 1;
    expected[l2pos('a')] = 2;
    expected[l2pos('f')] = 3;
    expected[l2pos('g')] = 4;
    expected[l2pos('b')] = 5;
    expected[l2pos('c')] = 6;
    CHECK_EQ(true, is_valid(expected, words));

    std::array<short, 7> assignment;
    assignment.fill(-1);
    bool found = solve(words, assignment);
    CHECK_EQ(true, found);
    CHECK_EQ(expected, assignment);
}

#endif  // IS_TEST
