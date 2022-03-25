#include "day18.h"

using parse::input_t;

struct Pair {
    Pair* left;
    Pair* right;
    long value;

    Pair() : left(nullptr), right(nullptr), value(0) {}

    uint64_t magnitude() const {
        if (left == nullptr) return value;
        return 3 * left->magnitude() + 2 * right->magnitude();
    }

    ~Pair() {
        if (left != nullptr) delete left;
        if (right != nullptr) delete right;
    }
};

size_t parse_pair_rec(const char s[], size_t len, Pair* out) {
    if (len == 0) return 0;

    assert(*s == '[' || (*s >= '0' && *s <= '9'));
    auto start = s;
    char* endptr;
    if (*s != '[') {
        // just a number
        out->value = strtol(s, &endptr, 10);
        return endptr - s;
    }
    // pair
    out->left = new Pair;
    out->right = new Pair;
    s++, len--;  // skip [
    size_t bytes_read = parse_pair_rec(s, len, out->left);
    s += bytes_read, len -= bytes_read;
    assert(*s == ',');
    s++, len--;  // skip comma
    bytes_read = parse_pair_rec(s, len, out->right);
    s += bytes_read, len -= bytes_read;
    s++, len--;

    bytes_read = s - start;
    return bytes_read;
}

void parse_pair(const std::string& s, Pair* out) {
    parse_pair_rec(s.c_str(), s.length(), out);
}

std::ostream& operator<<(std::ostream& os, const Pair& p) {
    if (p.left == nullptr) {
        os << p.value;
    } else {
        os << "[" << *p.left << "," << *p.right << "]";
    }
    return os;
}

struct Snailfish {
    std::string repr;

    Snailfish() : repr() {}

    Snailfish(std::string repr) : repr(repr) {}

    Snailfish operator+(const Snailfish& other) {
        std::stringstream builder;
        builder << "[" << repr << "," << other.repr << "]";
        auto result = Snailfish(builder.str()).reduce();
        return result;
    }

    Snailfish reduce() const {
        DEBUG(">> reducing {}", repr);
        // explode
        int depth = 0;
        for (size_t idx = 0; idx < repr.size() - 1; idx++) {
            assert(depth >= 0);
            switch (repr[idx]) {
                case '[':
                    ++depth;
                    if (depth > 4) {
                        //  only explode if this is a pair
                        bool is_pair;
                        {
                            size_t i = idx + 1;
                            while (repr[i] >= '0' && repr[i] <= '9') i++;
                            is_pair = repr[i] == ',';
                        }
                        if (is_pair) return explode(idx).reduce();
                    }
                    break;
                case ']':
                    --depth;
                    break;
            }
        }
        // split
        for (size_t idx = 0; idx < repr.size() - 1; idx++) {
            if (repr[idx] >= '1' && repr[idx] <= '9' && repr[idx + 1] >= '0' && repr[idx + 1] <= '9') {
                return split(idx).reduce();
            }
        }
        return *this;
    }

    Snailfish explode(size_t pos) const {
        assert(pos > 0 && pos < repr.size() - 1);
        assert(repr[pos] == '[');

        char* endptr;
        long left_value = std::strtol(repr.c_str() + pos + 1, &endptr, 10);
        long right_value = std::strtol(endptr + 1, NULL, 10);
        DEBUG(">> explode at pos {}: [{},{}]", pos, left_value, right_value);

        std::stringstream builder;

        size_t left_idx = pos;
        while (left_idx > 0 && (repr[left_idx] < '0' || repr[left_idx] > '9')) left_idx--;
        // numbers can have more than one digit!
        while (left_idx > 0 && (repr[left_idx - 1] >= '0' && repr[left_idx - 1] <= '9')) left_idx--;
        if (left_idx > 0) {
            char* endptr;
            long left_number = std::strtol(repr.c_str() + left_idx, &endptr, 10);
            DEBUG("found left_number: {}, left_value: {}", left_number, left_value);
            for (size_t i = 0; i < left_idx; i++) builder << repr[i];
            long sum = (left_number + left_value);
            builder << sum;
            // skip past the number we just inserted
            while (repr[left_idx] >= '0' && repr[left_idx] <= '9') left_idx++;
            for (size_t i = left_idx; i < pos; i++) builder << repr[i];
        } else {
            for (size_t i = 0; i < pos; i++) builder << repr[i];
        }
        builder << '0';                  // replace exploded pair with 0
        while (repr[pos] != ']') pos++;  // skip exploded pair
        pos++;

        size_t right_idx = pos;
        while (right_idx < repr.size() && (repr[right_idx] < '0' || repr[right_idx] > '9')) right_idx++;
        if (right_idx < repr.size()) {
            char* endptr;
            long right_number = std::strtol(repr.c_str() + right_idx, &endptr, 10);
            DEBUG("found right_number: {}", right_number);
            for (size_t i = pos; i < right_idx; i++) builder << repr[i];
            builder << (right_number + right_value);

            const char* end = repr.c_str() + repr.size();
            for (char* s = endptr; s != end; s++) builder << *s;

        } else {
            for (size_t i = pos; i < repr.size(); i++) builder << repr[i];
        }

        return Snailfish(builder.str());
    }

    Snailfish split(size_t pos) const {
        assert(pos > 0 && pos < repr.size());
        assert(repr[pos] >= '0' && repr[pos] <= '9');
        assert(repr[pos + 1] >= '0' && repr[pos + 1] <= '9');
        char* endptr;
        long number = std::strtol(repr.c_str() + pos, &endptr, 10);
        long left = number / 2;
        long right = (number + 1) / 2;
        DEBUG(">> split at pos {}: {} -> [{},{}]", pos, number, left, right);
        std::stringstream builder;
        for (size_t i = 0; i < pos; i++) builder << repr[i];
        builder << "[" << left << "," << right << "]";

        const char* end = repr.c_str() + repr.size();
        for (char* s = endptr; s != end; s++) builder << *s;

        return Snailfish(builder.str());
    }

    uint64_t magnitude() const {
        Pair p;
        parse_pair(repr, &p);
        return p.magnitude();
    }
};

parse::output_t day18(input_t in) {
    uint64_t part1, part2 = 0;

    Snailfish fishes[100];
    size_t n = 0;

    auto parse_snailfish = [&]() -> Snailfish {
        std::stringstream builder;
        while (in.len > 0 && *in.s != '\n') {
            builder << *in.s;
            in.s++, in.len--;
        }
        in.s++, in.len--;
        Snailfish fish(builder.str());
        return fish;
    };

    Snailfish result = parse_snailfish();
    fishes[n++] = result;
    while (in.len > 0) {
        Snailfish other = parse_snailfish();
        fishes[n++] = other;
        result = result + other;
    }
    part1 = result.magnitude();

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j) continue;
            uint64_t mag = (fishes[i] + fishes[j]).magnitude();
            if (mag > part2) part2 = mag;
        }
    }

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day18.txt");
    auto output = day18(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day18: explode no left neighbor, but right neighbor") {
    const Snailfish fish(std::string("[[[[[9,8],1],2],3],4]"));
    auto result = fish.explode(4);

    CHECK_EQ(std::string("[[[[0,9],2],3],4]"), result.repr);
}

TEST_CASE("day18: explode left neighbor, but no right neighbor") {
    const Snailfish fish(std::string("[7,[6,[5,[4,[3,2]]]]]"));
    auto result = fish.explode(12);

    CHECK_EQ(std::string("[7,[6,[5,[7,0]]]]"), result.repr);
}

TEST_CASE("day18: explode III") {
    const Snailfish fish(std::string("[[6,[5,[4,[3,2]]]],1]"));
    auto result = fish.explode(10);

    CHECK_EQ(std::string("[[6,[5,[7,0]]],3]"), result.repr);
}

TEST_CASE("day18: explode both sides") {
    const Snailfish fish(std::string("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]"));
    auto result = fish.explode(10);

    CHECK_EQ(std::string("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"), result.repr);
}

TEST_CASE("day18: explode last case") {
    const Snailfish fish(std::string("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"));
    auto result = fish.explode(24);
    CHECK_EQ(std::string("[[3,[2,[8,0]]],[9,[5,[7,0]]]]"), result.repr);
}

TEST_CASE("day18: split") {
    const Snailfish fish(std::string("[[[[0,7],4],[15,[0,13]]],[1,1]]"));
    auto result = fish.split(13);
    CHECK_EQ(std::string("[[[[0,7],4],[[7,8],[0,13]]],[1,1]]"), result.repr);
}

TEST_CASE("day18: add simple") {
    Snailfish lhs(std::string("[[[[4,3],4],4],[7,[[8,4],9]]]"));
    Snailfish rhs(std::string("[1,1]"));
    auto result = lhs + rhs;
    CHECK_EQ(std::string("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"), result.repr);
}

TEST_CASE("day18: add small I") {
    Snailfish result(std::string("[1,1]"));
    auto numbers = {
        "[2,2]",
        "[3,3]",
        "[4,4]",
    };

    for (auto s : numbers) {
        Snailfish rhs(s);
        result = result + rhs;
    }
    CHECK_EQ(std::string("[[[[1,1],[2,2]],[3,3]],[4,4]]"), result.repr);
}

TEST_CASE("day18: add small II") {
    Snailfish result(std::string("[1,1]"));
    auto numbers = {
        "[2,2]",
        "[3,3]",
        "[4,4]",
        "[5,5]",
    };

    for (auto s : numbers) {
        Snailfish rhs(s);
        result = result + rhs;
    }
    CHECK_EQ(std::string("[[[[3,0],[5,3]],[4,4]],[5,5]]"), result.repr);
}

TEST_CASE("day18: add small III") {
    Snailfish result(std::string("[1,1]"));
    auto numbers = {
        "[2,2]",
        "[3,3]",
        "[4,4]",
        "[5,5]",
        "[6,6]",
    };

    for (auto s : numbers) {
        Snailfish rhs(s);
        result = result + rhs;
    }
    CHECK_EQ(std::string("[[[[5,0],[7,4]],[5,5]],[6,6]]"), result.repr);
}

TEST_CASE("day18: add bug") {
    Snailfish lhs(std::string("[[[[6,6],[6,6]],[[6,0],[6,7]]],[[[7,7],[8,9]],[8,[8,1]]]]"));
    Snailfish rhs(std::string("[2,9]"));
    auto result = lhs + rhs;
    CHECK_EQ(std::string("[[[[6,6],[7,7]],[[0,7],[7,7]]],[[[5,5],[5,6]],9]]"), result.repr);
}

TEST_CASE("day18: add slightly larger") {
    Snailfish result(std::string("[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]"));
    auto numbers = {
        "[7,[[[3,7],[4,3]],[[6,3],[8,8]]]]",
        "[[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]]",
        "[[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]]",
        "[7,[5,[[3,8],[1,4]]]]",
        "[[2,[2,2]],[8,[8,1]]]",
        "[2,9]",
        "[1,[[[9,3],9],[[9,0],[0,7]]]]",
        "[[[5,[7,4]],7],1]",
        "[[[[4,2],2],6],[8,7]]",
    };

    for (auto s : numbers) {
        Snailfish rhs(s);
        result = result + rhs;
    }
    CHECK_EQ(std::string("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]"), result.repr);
}

TEST_CASE("day18: magnitude") {
    auto test_cases = {
        make_tuple("[[1,2],[[3,4],5]]", "143"),
        make_tuple("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]", "1384"),
        make_tuple("[[[[1,1],[2,2]],[3,3]],[4,4]]", "445"),
        make_tuple("[[[[3,0],[5,3]],[4,4]],[5,5]]", "791"),
        make_tuple("[[[[5,0],[7,4]],[5,5]],[6,6]]", "1137"),
        make_tuple("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]", "3488")};

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        DEBUG("Input:\n{}", &first);
        auto actual = Snailfish(first).magnitude();
        std::string expected_str = std::get<1>(tc);
        long expected = std::strtol(expected_str.c_str(), NULL, 10);
        CHECK_EQ(expected, actual);
    }
}

TEST_CASE("day18: explode bug") {
    Snailfish fish(std::string("[[[[0,7],4],[7,[[8,4],9]]],[1,1]]"));
    auto result = fish.explode(16);
    CHECK_EQ(std::string("[[[[0,7],4],[15,[0,13]]],[1,1]]"), result.repr);
}

TEST_CASE("day18: examples") {
    auto test_cases = {
        make_tuple(
            "[[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]\n"
            "[[[5,[2,8]],4],[5,[[9,9],0]]]\n"
            "[6,[[[6,2],[5,6]],[[7,6],[4,7]]]]\n"
            "[[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]\n"
            "[[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]\n"
            "[[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]\n"
            "[[[[5,4],[7,7]],8],[[8,3],8]]\n"
            "[[9,3],[[9,9],[6,[4,9]]]]\n"
            "[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]\n"
            "[[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]]\n",
            "4140", "3993"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        DEBUG("Input:\n{}", &first);
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day18(in);
        DEBUG("Part 1: {}, Part 2: {}", &output.answer[0], &output.answer[1]);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day18 parse Pair") {
    std::string s("[1,2]");
    Pair pair;
    parse_pair(s, &pair);
    CHECK_EQ(1, pair.left->value);
    CHECK_EQ(2, pair.right->value);
}

TEST_CASE("day18, part 1 & part 2") {
    input_t in = parse::load_input("input/day18.txt");
    auto output = day18(in);
    CHECK_EQ("4111", output.answer[0]);
    CHECK_EQ("4917", output.answer[1]);
}

#endif  // IS_TEST
