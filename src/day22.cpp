#include "day22.h"

using parse::input_t;

enum State { OFF,
             ON };

struct Cuboid {
    int32_t x_min, x_max, y_min, y_max, z_min, z_max;

    bool operator==(const Cuboid &) const = default;

    inline bool is_valid() const { return x_min <= x_max && y_min <= y_max && z_min <= z_max; }

    inline Cuboid intersect(const Cuboid &other) const {
        return {
            .x_min = std::max(x_min, other.x_min),
            .x_max = std::min(x_max, other.x_max),
            .y_min = std::max(y_min, other.y_min),
            .y_max = std::min(y_max, other.y_max),
            .z_min = std::max(z_min, other.z_min),
            .z_max = std::min(z_max, other.z_max),
        };
    }

    inline bool is_disjoint(const Cuboid &other) const {
        return !intersect(other).is_valid();
    }

    inline uint64_t volume() const {
        return ((uint64_t)((x_max - x_min) + 1)) * ((uint64_t)((y_max - y_min) + 1)) * ((uint64_t)((z_max - z_min) + 1));
    }
};

template <>
struct fmt::formatter<Cuboid> : formatter<string_view> {
    template <typename FormatContext>
    auto format(const Cuboid &cuboid, FormatContext &ctx) -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return format_to(
            ctx.out(),
            "x={}..{},y={}..{},z={}..{}",
            cuboid.x_min, cuboid.x_max, cuboid.y_min, cuboid.y_max, cuboid.z_min, cuboid.z_max);
    }
};

template <typename Container>
uint64_t calc_volume(const Container &cuboids) {
    return std::accumulate(std::begin(cuboids), std::end(cuboids), (uint64_t)0,
                           [](uint64_t acc, Cuboid rhs) -> uint64_t {
                               return acc + rhs.volume();
                           });
}

/* Returns a vector of disjoint cuboids.
 * NOTE: `other` is the last element in the vector.
 */
void merge_cuboids(const Cuboid &old, const Cuboid &new_cuboid, std::vector<Cuboid> &result) {
    /* Example:
     *
     * ┌─────────────┐                        ┌─────++──────+
     * │    old      │                        │     ||      |
     * │             │                        │     |+------+
     * │      ┌──────┼──────────┐             │     |┌─────────────────┐
     * │      │      │          │             │     |│                 │
     * │      │      │          │             │     |│                 │
     * │      │      │   new    │             │     |│                 │
     * │      │      │          │             │     |│                 │
     * │      │      │          │             │     |│                 │
     * │      └──────┼──────────┘             │     |└─────────────────┘
     * │             │                        │     ||------+
     * │             │                        │     ||      |
     * └─────────────┘                        └─────++──────+
     */

    if (old.x_min < new_cuboid.x_min) {
        Cuboid left = old;
        left.x_max = std::min(old.x_max, new_cuboid.x_min - 1);
        if (left.is_valid()) {
            TRACE("left cube: {}", left);
            result.push_back(left);
        }
    }

    /*  Corner cases:
     *
     * ┌────────────────────────┐  ┌───────────────┐
     * │                        │  │               │
     * │                        │  │               │
     * │   new                  │  │       old     │
     * │                        │  │               │
     * └────────────────────────┘  └───────────────┘
     *
     */

    if (old.x_max > new_cuboid.x_max) {
        Cuboid right = old;
        right.x_min = std::max(old.x_min, new_cuboid.x_max + 1);
        if (right.is_valid()) {
            TRACE("right cube: {}", right);
            result.push_back(right);
        }
    }

    if (old.y_min < new_cuboid.y_min) {
        Cuboid top = old;
        top.x_min = std::max(old.x_min, new_cuboid.x_min);
        top.x_max = std::min(old.x_max, new_cuboid.x_max);

        top.y_max = std::min(old.y_max, new_cuboid.y_min - 1);
        if (top.is_valid()) {
            TRACE("top cube: {}", top);
            result.push_back(top);
        }
    }

    /*
     * new
     * ┌─┐
     * │ │
     * └─┼─┐
     *   │ │old
     *   └─┘
     */

    if (old.y_max > new_cuboid.y_max) {
        Cuboid bottom = old;
        bottom.x_min = std::max(old.x_min, new_cuboid.x_min);
        bottom.x_max = std::min(old.x_max, new_cuboid.x_max);

        bottom.y_min = std::max(old.y_min, new_cuboid.y_max + 1);
        if (bottom.is_valid()) {
            TRACE("bottom cube: {}", bottom);
            result.push_back(bottom);
        }
    }

    if (old.z_min < new_cuboid.z_min) {
        Cuboid below = old;
        below.x_min = std::max(old.x_min, new_cuboid.x_min);
        below.x_max = std::min(old.x_max, new_cuboid.x_max);
        below.y_min = std::max(old.y_min, new_cuboid.y_min);
        below.y_max = std::min(old.y_max, new_cuboid.y_max);

        below.z_max = std::min(old.z_max, new_cuboid.z_min - 1);
        if (below.is_valid()) {
            TRACE("below cube: {}", below);
            result.push_back(below);
        }
    }

    /*
     * new
     * ┌─┐
     * │ │
     * └─┼─┐
     *   │ │old
     *   └─┘
     *
     *
     * old = {.x_min = 11, .x_max = 13, .y_min = 11, .y_max = 13, .z_min = 11, .z_max = 13};
     * new = {.x_min = 9, .x_max = 11, .y_min = 9, .y_max = 11, .z_min = 9, .z_max = 11};
     * Need to find x=11, y=11, z = 12..13
     */

    if (old.z_max > new_cuboid.z_max) {
        Cuboid upper = old;
        upper.x_min = std::max(old.x_min, new_cuboid.x_min);
        upper.x_max = std::min(old.x_max, new_cuboid.x_max);
        upper.y_min = std::max(old.y_min, new_cuboid.y_min);
        upper.y_max = std::min(old.y_max, new_cuboid.y_max);

        upper.z_min = std::max(old.z_min, new_cuboid.z_max + 1);
        if (upper.is_valid()) {
            TRACE("upper cube: {}", upper);
            result.push_back(upper);
        }
    }

    result.push_back(new_cuboid);
}

void remove_cuboid(const Cuboid &on, const Cuboid &off, std::vector<Cuboid> &result) {
    merge_cuboids(on, off, result);
    result.pop_back();
}

std::vector<Cuboid> make_disjoint(const std::vector<Cuboid> &cuboids) {
    std::vector<Cuboid> result;
    size_t i, j;
    bool found_intersection = false;
    for (i = 0; i < cuboids.size(); i++) {
        for (j = i + 1; j < cuboids.size(); j++) {
            if (!cuboids[i].is_disjoint(cuboids[j])) {
                found_intersection = true;
                goto done;
            }
        }
    }
done:
    if (found_intersection) {
        std::vector<Cuboid> merge_result;
        merge_result.reserve(cuboids.size() + 7);
        merge_cuboids(cuboids[i], cuboids[j], merge_result);
        for (size_t k = 0; k < cuboids.size(); k++) {
            if (k == i || k == j) continue;
            merge_result.push_back(cuboids[k]);
        }
        return make_disjoint(merge_result);
    } else {
        return cuboids;
    }
}

#define AREA 50

parse::output_t day22(input_t in) {
    uint64_t part1 = 0, part2 = 0;

    auto parse_min_max = [&]() -> std::pair<int32_t, int32_t> {
        do {
            in.s++, in.len--;
        } while (in.len > 0 && *in.s != '=');
        in.s++, in.len--;

        int32_t min_val = parse::number(in);
        in.s += 2, in.len -= 2;
        int32_t max_val = parse::number(in);
        return std::make_pair(min_val, max_val);
    };

    auto parse_cuboid = [&]() -> std::pair<State, Cuboid> {
        State state;
        in.s++, in.len--;
        state = *in.s == 'f' ? OFF : ON;

        auto x = parse_min_max();
        int32_t x_min = x.first;
        int32_t x_max = x.second;

        auto y = parse_min_max();
        int32_t y_min = y.first;
        int32_t y_max = y.second;

        auto z = parse_min_max();
        int32_t z_min = z.first;
        int32_t z_max = z.second;

        in.s++, in.len--;

        return std::make_pair(state, Cuboid{.x_min = x_min, .x_max = x_max, .y_min = y_min, .y_max = y_max, .z_min = z_min, .z_max = z_max});
    };

    std::vector<Cuboid> cubes[2];
    cubes[0].reserve(1 << 12);
    cubes[1].reserve(1 << 12);
    int active_idx = 0;

    {
        auto result = parse_cuboid();
        assert(result.first == ON);
        cubes[active_idx].push_back(result.second);
    }

    while (in.len > 0 && *in.s) {
        auto parse_result = parse_cuboid();
        DEBUG("Parsed cuboid: {} {}", parse_result.first == ON ? "on" : "off", parse_result.second);
        if (parse_result.first == ON) {
            cubes[active_idx].push_back(parse_result.second);
            continue;
        }

        auto old_idx = active_idx;
        active_idx = 1 - active_idx;
        cubes[active_idx].clear();
        for (const auto &old_cube : cubes[old_idx]) {
            if (!old_cube.is_disjoint(parse_result.second)) {
                remove_cuboid(old_cube, parse_result.second, cubes[active_idx]);
            } else {
                cubes[active_idx].push_back(old_cube);
            }
        }
        cubes[old_idx] = make_disjoint(cubes[active_idx]);
        active_idx = old_idx;
    }

    auto disjoint_result = make_disjoint(cubes[active_idx]);

    Cuboid area_of_interest{.x_min = -50, .x_max = 50, .y_min = -50, .y_max = 50, .z_min = -50, .z_max = 50};
    for (const auto &c : disjoint_result) {
        auto c_intersected = area_of_interest.intersect(c);
        if (c_intersected.is_valid()) {
            part1 += c_intersected.volume();
        }
    }

    part2 = calc_volume(disjoint_result);

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day22.txt");
    auto output = day22(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day22: turning off cubes") {
    Cuboid on = {.x_min = 11, .x_max = 13, .y_min = 11, .y_max = 13, .z_min = 11, .z_max = 13};
    Cuboid off = {.x_min = 9, .x_max = 11, .y_min = 9, .y_max = 11, .z_min = 9, .z_max = 11};

    auto volume1 = on.volume() - on.intersect(off).volume();
    CHECK_EQ(3 * 3 * 3 - 1, volume1);

    std::vector<Cuboid> result;
    remove_cuboid(on, off, result);
    auto volume = calc_volume(result);
    CHECK_EQ(volume1, volume);
}

TEST_CASE("day22: small example") {
    auto test_cases = {
        make_tuple(
            "on x=10..12,y=10..12,z=10..12\n"
            "on x=11..13,y=11..13,z=11..13\0",
            46, 46),
        make_tuple(
            "on x=10..12,y=10..12,z=10..12\n"
            "on x=11..13,y=11..13,z=11..13\n"
            "off x=9..11,y=9..11,z=9..11\0",
            38, 38),
        make_tuple(
            "on x=10..12,y=10..12,z=10..12\n"
            "on x=11..13,y=11..13,z=11..13\n"
            "off x=9..11,y=9..11,z=9..11\n"
            "on x=10..10,y=10..10,z=10..10\0",
            39, 39),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day22(in);

        CHECK_EQ(std::get<1>(tc), std::strtol(output.answer[0].c_str(), NULL, 10));
        CHECK_EQ(std::get<2>(tc), std::strtol(output.answer[1].c_str(), NULL, 10));
    }
}

TEST_CASE("day22: examples part 1") {
    auto test_cases = {
        make_tuple(
            "on x=-20..26,y=-36..17,z=-47..7\n"
            "on x=-20..33,y=-21..23,z=-26..28\n"
            "on x=-22..28,y=-29..23,z=-38..16\n"
            "on x=-46..7,y=-6..46,z=-50..-1\n"
            "on x=-49..1,y=-3..46,z=-24..28\n"
            "on x=2..47,y=-22..22,z=-23..27\n"
            "on x=-27..23,y=-28..26,z=-21..29\n"
            "on x=-39..5,y=-6..47,z=-3..44\n"
            "on x=-30..21,y=-8..43,z=-13..34\n"
            "on x=-22..26,y=-27..20,z=-29..19\n"
            "off x=-48..-32,y=26..41,z=-47..-37\n"
            "on x=-12..35,y=6..50,z=-50..-2\n"
            "off x=-48..-32,y=-32..-16,z=-15..-5\n"
            "on x=-18..26,y=-33..15,z=-7..46\n"
            "off x=-40..-22,y=-38..-28,z=23..41\n"
            "on x=-16..35,y=-41..10,z=-47..6\n"
            "off x=-32..-23,y=11..30,z=-14..3\n"
            "on x=-49..-5,y=-3..45,z=-29..18\n"
            "off x=18..30,y=-20..-8,z=-3..13\n"
            "on x=-41..9,y=-7..43,z=-33..15\n"
            "on x=-54112..-39298,y=-85059..-49293,z=-27449..7877\n"
            "on x=967..23432,y=45373..81175,z=27513..53682\0",
            "590784"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day22(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
    }
}

TEST_CASE("day22: examples part 2" * doctest::skip()) {
    auto test_cases = {
        make_tuple(
            "on x=-5..47,y=-31..22,z=-19..33\n"
            "on x=-44..5,y=-27..21,z=-14..35\n"
            "on x=-49..-1,y=-11..42,z=-10..38\n"
            "on x=-20..34,y=-40..6,z=-44..1\n"
            "off x=26..39,y=40..50,z=-2..11\n"
            "on x=-41..5,y=-41..6,z=-36..8\n"
            "off x=-43..-33,y=-45..-28,z=7..25\n"
            "on x=-33..15,y=-32..19,z=-34..11\n"
            "off x=35..47,y=-46..-34,z=-11..5\n"
            "on x=-14..36,y=-6..44,z=-16..29\n"
            "on x=-57795..-6158,y=29564..72030,z=20435..90618\n"
            "on x=36731..105352,y=-21140..28532,z=16094..90401\n"
            "on x=30999..107136,y=-53464..15513,z=8553..71215\n"
            "on x=13528..83982,y=-99403..-27377,z=-24141..23996\n"
            "on x=-72682..-12347,y=18159..111354,z=7391..80950\n"
            "on x=-1060..80757,y=-65301..-20884,z=-103788..-16709\n"
            "on x=-83015..-9461,y=-72160..-8347,z=-81239..-26856\n"
            "on x=-52752..22273,y=-49450..9096,z=54442..119054\n"
            "on x=-29982..40483,y=-108474..-28371,z=-24328..38471\n"
            "on x=-4958..62750,y=40422..118853,z=-7672..65583\n"
            "on x=55694..108686,y=-43367..46958,z=-26781..48729\n"
            "on x=-98497..-18186,y=-63569..3412,z=1232..88485\n"
            "on x=-726..56291,y=-62629..13224,z=18033..85226\n"
            "on x=-110886..-34664,y=-81338..-8658,z=8914..63723\n"
            "on x=-55829..24974,y=-16897..54165,z=-121762..-28058\n"
            "on x=-65152..-11147,y=22489..91432,z=-58782..1780\n"
            "on x=-120100..-32970,y=-46592..27473,z=-11695..61039\n"
            "on x=-18631..37533,y=-124565..-50804,z=-35667..28308\n"
            "on x=-57817..18248,y=49321..117703,z=5745..55881\n"
            "on x=14781..98692,y=-1341..70827,z=15753..70151\n"
            "on x=-34419..55919,y=-19626..40991,z=39015..114138\n"
            "on x=-60785..11593,y=-56135..2999,z=-95368..-26915\n"
            "on x=-32178..58085,y=17647..101866,z=-91405..-8878\n"
            "on x=-53655..12091,y=50097..105568,z=-75335..-4862\n"
            "on x=-111166..-40997,y=-71714..2688,z=5609..50954\n"
            "on x=-16602..70118,y=-98693..-44401,z=5197..76897\n"
            "on x=16383..101554,y=4615..83635,z=-44907..18747\n"
            "off x=-95822..-15171,y=-19987..48940,z=10804..104439\n"
            "on x=-89813..-14614,y=16069..88491,z=-3297..45228\n"
            "on x=41075..99376,y=-20427..49978,z=-52012..13762\n"
            "on x=-21330..50085,y=-17944..62733,z=-112280..-30197\n"
            "on x=-16478..35915,y=36008..118594,z=-7885..47086\n"
            "off x=-98156..-27851,y=-49952..43171,z=-99005..-8456\n"
            "off x=2032..69770,y=-71013..4824,z=7471..94418\n"
            "on x=43670..120875,y=-42068..12382,z=-24787..38892\n"
            "off x=37514..111226,y=-45862..25743,z=-16714..54663\n"
            "off x=25699..97951,y=-30668..59918,z=-15349..69697\n"
            "off x=-44271..17935,y=-9516..60759,z=49131..112598\n"
            "on x=-61695..-5813,y=40978..94975,z=8655..80240\n"
            "off x=-101086..-9439,y=-7088..67543,z=33935..83858\n"
            "off x=18020..114017,y=-48931..32606,z=21474..89843\n"
            "off x=-77139..10506,y=-89994..-18797,z=-80..59318\n"
            "off x=8476..79288,y=-75520..11602,z=-96624..-24783\n"
            "on x=-47488..-1262,y=24338..100707,z=16292..72967\n"
            "off x=-84341..13987,y=2429..92914,z=-90671..-1318\n"
            "off x=-37810..49457,y=-71013..-7894,z=-105357..-13188\n"
            "off x=-27365..46395,y=31009..98017,z=15428..76570\n"
            "off x=-70369..-16548,y=22648..78696,z=-1892..86821\n"
            "on x=-53470..21291,y=-120233..-33476,z=-44150..38147\n"
            "off x=-93533..-4276,y=-16170..68771,z=-104985..-24507\0",
            "474140", "2758514936282235"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day22(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day22, part 1 & part 2") {
    input_t in = parse::load_input("input/day22.txt");
    auto output = day22(in);
    CHECK_EQ("576028", output.answer[0]);
    CHECK_EQ("1387966280636636", output.answer[1]);
}

TEST_CASE("day22, merge with overlap") {
    const Cuboid first = {.x_min = 10, .x_max = 12, .y_min = 10, .y_max = 12, .z_min = 10, .z_max = 12};
    const Cuboid second = {.x_min = 11, .x_max = 13, .y_min = 11, .y_max = 13, .z_min = 11, .z_max = 13};

    CHECK_EQ(27, first.volume());
    CHECK_EQ(27, second.volume());

    std::vector<Cuboid> result;
    merge_cuboids(first, second, result);
    CHECK_EQ(second, result[result.size() - 1]);
    auto volume = calc_volume(result);

    CHECK_EQ(27 + 19, volume);
}

TEST_CASE("day22, merge no overlap") {
    const Cuboid first = {.x_min = 10, .x_max = 15, .y_min = 10, .y_max = 15, .z_min = 10, .z_max = 15};
    const Cuboid second = {.x_min = 7, .x_max = 9, .y_min = 7, .y_max = 9, .z_min = 10, .z_max = 12};

    std::vector<Cuboid> result;
    merge_cuboids(first, second, result);
    CHECK_EQ(second, result[result.size() - 1]);
    std::sort(std::begin(result), std::end(result), [](const Cuboid &lhs, const Cuboid &rhs) {
        return lhs.volume() < rhs.volume();
    });
    CHECK_EQ(2, result.size());
    CHECK_EQ(second, result[0]);
    CHECK_EQ(first, result[1]);
}

#endif  // IS_TEST
