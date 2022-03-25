// Took some inspiration from:
// * https://github.com/Praful/advent_of_code/blob/main/2021/src/day19.jl
// * https://github.com/taddeus/advent-of-code/blob/master/2021/19_beacon.py
// * https://github.com/Jellycious/aoc-2021/blob/main/src/days/day19.rs

#include "day19.h"

#define MAX_SCANNERS 32
#define MAX_POINTS 32
#define OVERLAP_BOUND 12
// // The threshold for number of overlapping probes was 12, this constitutes to binomial(n,2) = n*(n-1)/2 edges.
#define EDGE_THRESHOLD 66
using my_int = int32_t;

enum Rotation {
    IDENTITY,
    ROTZ,
    ROTX,
    SPECIAL_ONE,
    SPECIAL_TWO,
};

#define ROTATION_COUNT 24
// clang-format off
// https://www.euclideanspace.com/maths/algebra/matrix/transforms/examples/index.htm
constexpr Rotation ALL_ROTATIONS[ROTATION_COUNT] = {
    IDENTITY,
    ROTZ, ROTZ, ROTZ,
    ROTX,
    ROTZ, ROTZ, ROTZ,
    ROTX,
    ROTZ, ROTZ, ROTZ,
    SPECIAL_ONE,
    ROTZ, ROTZ, ROTZ,
    ROTX,
    ROTZ, ROTZ, ROTZ,
    SPECIAL_TWO,
    ROTZ, ROTZ, ROTZ,
};
// clang-format on

struct Point3D {
    my_int x, y, z;

    Point3D operator-(const Point3D& other) const {
        return {.x = x - other.x, .y = y - other.y, .z = z - other.z};
    }

    void operator+=(const Point3D& other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    Point3D operator+(const Point3D& other) const {
        return {.x = x + other.x, .y = y + other.y, .z = z + other.z};
    }

    inline int64_t dist(const Point3D& other) const {
        auto dx = (other.x - x);
        auto dy = other.y - y;
        auto dz = other.z - z;
        return dx * dx + dy * dy + dz * dz;
    }

    inline int64_t manhattan_dist(const Point3D& other) const {
        return std::abs(other.x - x) + std::abs(other.y - y) + std::abs(other.z - z);
    }

    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    Point3D rotx() {
        return {.x = x, .y = -z, .z = y};
    }

    Point3D rotz() {
        return {.x = -y, .y = x, .z = z};
    }

    Point3D special1() {
        /* heading = 180 degrees
         * attitude = 0
         * bank = 90 degrees
         *
         * -1  0  0
         *  0  0 -1
         *  0 -1  0
         */
        return {.x = -x, .y = -z, .z = -y};
    }

    Point3D special2() {
        return {.x = x, .y = z, .z = -y};
    }
};

template <>
struct fmt::formatter<Point3D> : formatter<string_view> {
    template <typename FormatContext>
    auto format(const Point3D& p, FormatContext& ctx) -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return format_to(
            ctx.out(),
            "x={}, y={}, z={}",
            p.x, p.y, p.z);
    }
};

MAKE_HASHABLE(Point3D, t.x, t.y, t.z)

struct Scanner {
    std::optional<Point3D> m_location;
    Point3D m_points[MAX_POINTS];
    size_t m_count = 0;
    std::vector<int64_t> m_distances;
    std::unordered_map<int64_t, std::pair<size_t, size_t>> m_distance_to_points;
    uint8_t m_rot_idx = 0;

    void add_point(my_int x, my_int y, my_int z) {
        assert(m_count < MAX_POINTS);
        m_points[m_count++] = {.x = x, .y = y, .z = z};
    }

    void compute_distances() {
        m_distances.reserve(m_count * m_count);
        m_distance_to_points.reserve(m_count * m_count);
        for (size_t i = 0; i < m_count; i++) {
            for (size_t j = i + 1; j < m_count; j++) {
                int64_t dist = m_points[i].dist(m_points[j]);
                m_distances.push_back(dist);
                m_distance_to_points[dist] = std::make_pair(i, j);
            }
        }
        std::sort(m_distances.begin(), m_distances.end());
    }

    void rotate() {
        m_rot_idx++;
        assert(m_rot_idx < ROTATION_COUNT);
        switch (ALL_ROTATIONS[m_rot_idx]) {
            case IDENTITY:
                break;
            case ROTZ:
                for (size_t i = 0; i < m_count; i++) m_points[i] = m_points[i].rotz();
                break;
            case ROTX:
                for (size_t i = 0; i < m_count; i++) m_points[i] = m_points[i].rotx();
                break;
            case SPECIAL_ONE:
                for (size_t i = 0; i < m_count; i++) m_points[i] = m_points[i].special1();
                break;
            case SPECIAL_TWO:
                for (size_t i = 0; i < m_count; i++) m_points[i] = m_points[i].special2();
                break;
        }
    }
};

struct OverlapResult {
    size_t first, second;  // index into scanners array
    int64_t common_distance;
};

static std::optional<Point3D> scanner_location(const std::pair<Point3D, Point3D>& beacon_pair1,
                                               const std::pair<Point3D, Point3D>& beacon_pair2) {
    {
        auto loc1 = beacon_pair1.first - beacon_pair2.first;
        auto loc2 = beacon_pair1.second - beacon_pair2.second;
        if (loc1 == loc2) return loc1;
    }
    {
        auto loc1 = beacon_pair1.first - beacon_pair2.second;
        auto loc2 = beacon_pair1.second - beacon_pair2.first;
        if (loc1 == loc2) return loc1;
    }
    std::optional<Point3D> result;
    return result;
}

using parse::input_t;

parse::output_t day19(input_t in) {
    long part1, part2;

    size_t scanner_count;
    Scanner scanners[MAX_SCANNERS];
    scanners[0].m_location = {0, 0, 0};
    {
        int scanner_idx = -1;
        while (in.len > 4) {
            if (*(in.s + 4) == 's') {  // start new scanner
                scanner_idx++;
                parse::seek_next_line(in);
                continue;
            }

            auto x = static_cast<my_int>(parse::number(in));
            in.s++, in.len--;  // skip comma
            auto y = static_cast<my_int>(parse::number(in));
            in.s++, in.len--;  // skip comma
            auto z = static_cast<my_int>(parse::number(in));
            scanners[scanner_idx].add_point(x, y, z);

            parse::seek_next_line(in);
            while (in.len && *in.s == '\n') { in.s++, in.len--; };
        }
        scanner_count = scanner_idx + 1;
    }
    DEBUG("Parsed {} scanners", scanner_count);

    /*
     * Step 1: For each scanner, compute the distance between any two beacons.
     */
    for (size_t alpha = 0; alpha < scanner_count; alpha++) {
        scanners[alpha].compute_distances();
    }

    /*
     * Step 2: Find overlapping scanners.
     * This function does not guarantee that two scanners overlap, but there is strong evidence
     * that two scanners might overlap. If a scanner is not in the result then it does definitely not
     * overlap with `s1`.
     */
    std::vector<OverlapResult> overlapping_scanners;
    overlapping_scanners.reserve(MAX_SCANNERS * (MAX_SCANNERS - 1));
    for (size_t alpha = 0; alpha < scanner_count; alpha++) {
        auto n = scanners[alpha].m_distances.size();
        for (size_t beta = alpha + 1; beta < scanner_count; beta++) {
            auto m = scanners[beta].m_distances.size();
            uint64_t common_count = 0;
            size_t i = 0, j = 0;
            std::vector<int64_t> common_distances;
            common_distances.reserve(MAX_POINTS * MAX_POINTS);
            while (i < n && j < m) {
                if (scanners[alpha].m_distances[i] == scanners[beta].m_distances[j]) {
                    common_count++;
                    common_distances.push_back(scanners[alpha].m_distances[i]);
                    i++;
                    j++;
                } else if (scanners[alpha].m_distances[i] > scanners[beta].m_distances[j]) {
                    j++;
                } else {
                    i++;
                }
            }
            if (common_count >= EDGE_THRESHOLD) {
                OverlapResult result = {.first = alpha, .second = beta, .common_distance = common_distances[0]};
                overlapping_scanners.push_back(result);
            }
        }
    }

    /* Step 3: For each of these overlapping scanners (s1 and s2, say), use a pair of beacons to find the correct rotation of
     * beacons in s2 so that they align to beacons in s1. As part of finding the rotation, the location of the scanner
     * (relative to the first scanner) pops out.
     */
    std::unordered_set<Point3D> unique_beacons;
    unique_beacons.reserve(MAX_SCANNERS * MAX_POINTS);
    for (size_t i = 0; i < scanners[0].m_count; i++) unique_beacons.insert(scanners[0].m_points[i]);
    size_t processed = 1;
    while (processed < scanner_count) {
        for (const auto& overlap_result : overlapping_scanners) {
            size_t i = overlap_result.first;
            size_t j = overlap_result.second;
            if (scanners[i].m_location.has_value() && scanners[j].m_location.has_value()) continue;
            if (!scanners[i].m_location.has_value() && !scanners[j].m_location.has_value()) continue;
            if (!scanners[i].m_location.has_value()) {
                auto old = i;
                i = j;
                j = old;
            }
            DEBUG("Using scanner {} at location {} to align scanner {}", i, *scanners[i].m_location, j);
            DEBUG("Common distances: {}", overlap_result.common_distances);

            // i is processed, j is not processed
            assert(scanners[i].m_location.has_value());
            assert(!scanners[j].m_location.has_value());
            assert(scanners[j].m_rot_idx == 0);

            auto s1_indices = scanners[i].m_distance_to_points[overlap_result.common_distance];
            std::pair<Point3D, Point3D> s1_beacons = std::make_pair(
                scanners[i].m_points[s1_indices.first],
                scanners[i].m_points[s1_indices.second]);
            DEBUG("s1_beacons: {}", s1_beacons);

            auto s2_indices = scanners[j].m_distance_to_points[overlap_result.common_distance];
            std ::pair<Point3D, Point3D> s2_beacons = std::make_pair(
                scanners[j].m_points[s2_indices.first],
                scanners[j].m_points[s2_indices.second]);
            DEBUG("s2_beacons: {}", s2_beacons);

            // find orientation of s2
            for (uint8_t rot_idx = 0; rot_idx < ROTATION_COUNT; rot_idx++) {
                switch (ALL_ROTATIONS[rot_idx]) {
                    case IDENTITY:
                        break;
                    case ROTZ:
                        s2_beacons.first = s2_beacons.first.rotz();
                        s2_beacons.second = s2_beacons.second.rotz();
                        break;
                    case ROTX:
                        s2_beacons.first = s2_beacons.first.rotx();
                        s2_beacons.second = s2_beacons.second.rotx();
                        break;
                    case SPECIAL_ONE:
                        s2_beacons.first = s2_beacons.first.special1();
                        s2_beacons.second = s2_beacons.second.special1();
                        break;
                    case SPECIAL_TWO:
                        s2_beacons.first = s2_beacons.first.special2();
                        s2_beacons.second = s2_beacons.second.special2();
                        break;
                }
                DEBUG("rotate2: {}", s2_beacons);
                std::optional<Point3D> maybe_location = scanner_location(s1_beacons, s2_beacons);
                if (maybe_location.has_value()) {
                    Point3D location = maybe_location.value();
                    DEBUG("Scanner {} has location {}", j, location);
                    // We need to adjust the location so that it is relative to
                    // the first scanner (ie scanners[1]), which is
                    // what scanner[id1] has been aligned to.
                    location += scanners[i].m_location.value();

                    scanners[j].m_location = location;

                    for (uint8_t rot_counter = 0; rot_counter < rot_idx; rot_counter++) {
                        scanners[j].rotate();
                    }

                    for (size_t k = 0; k < scanners[j].m_count; k++) {
                        unique_beacons.insert(scanners[j].m_points[k] + location);
                    }

                    processed++;
                    break;
                }
            }
            DEBUG("processed: {}", processed);
        }
    }

    part1 = unique_beacons.size();
    part2 = std::numeric_limits<long>().min();
    for (size_t i = 0; i < scanner_count; i++) {
        for (size_t j = 0; j < scanner_count; j++) {
            auto dist = scanners[i].m_location->manhattan_dist(*scanners[j].m_location);
            if (dist > part2) part2 = dist;
        }
    }
    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day19.txt");
    auto output = day19(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day19: examples") {
    auto test_cases = {
        make_tuple(
            "--- scanner 0 ---\n"
            "404,-588,-901\n"
            "528,-643,409\n"
            "-838,591,734\n"
            "390,-675,-793\n"
            "-537,-823,-458\n"
            "-485,-357,347\n"
            "-345,-311,381\n"
            "-661,-816,-575\n"
            "-876,649,763\n"
            "-618,-824,-621\n"
            "553,345,-567\n"
            "474,580,667\n"
            "-447,-329,318\n"
            "-584,868,-557\n"
            "544,-627,-890\n"
            "564,392,-477\n"
            "455,729,728\n"
            "-892,524,684\n"
            "-689,845,-530\n"
            "423,-701,434\n"
            "7,-33,-71\n"
            "630,319,-379\n"
            "443,580,662\n"
            "-789,900,-551\n"
            "459,-707,401\n"
            "\n"
            "--- scanner 1 ---\n"
            "686,422,578\n"
            "605,423,415\n"
            "515,917,-361\n"
            "-336,658,858\n"
            "95,138,22\n"
            "-476,619,847\n"
            "-340,-569,-846\n"
            "567,-361,727\n"
            "-460,603,-452\n"
            "669,-402,600\n"
            "729,430,532\n"
            "-500,-761,534\n"
            "-322,571,750\n"
            "-466,-666,-811\n"
            "-429,-592,574\n"
            "-355,545,-477\n"
            "703,-491,-529\n"
            "-328,-685,520\n"
            "413,935,-424\n"
            "-391,539,-444\n"
            "586,-435,557\n"
            "-364,-763,-893\n"
            "807,-499,-711\n"
            "755,-354,-619\n"
            "553,889,-390\n"
            "\n"
            "--- scanner 2 ---\n"
            "649,640,665\n"
            "682,-795,504\n"
            "-784,533,-524\n"
            "-644,584,-595\n"
            "-588,-843,648\n"
            "-30,6,44\n"
            "-674,560,763\n"
            "500,723,-460\n"
            "609,671,-379\n"
            "-555,-800,653\n"
            "-675,-892,-343\n"
            "697,-426,-610\n"
            "578,704,681\n"
            "493,664,-388\n"
            "-671,-858,530\n"
            "-667,343,800\n"
            "571,-461,-707\n"
            "-138,-166,112\n"
            "-889,563,-600\n"
            "646,-828,498\n"
            "640,759,510\n"
            "-630,509,768\n"
            "-681,-892,-333\n"
            "673,-379,-804\n"
            "-742,-814,-386\n"
            "577,-820,562\n"
            "\n"
            "--- scanner 3 ---\n"
            "-589,542,597\n"
            "605,-692,669\n"
            "-500,565,-823\n"
            "-660,373,557\n"
            "-458,-679,-417\n"
            "-488,449,543\n"
            "-626,468,-788\n"
            "338,-750,-386\n"
            "528,-832,-391\n"
            "562,-778,733\n"
            "-938,-730,414\n"
            "543,643,-506\n"
            "-524,371,-870\n"
            "407,773,750\n"
            "-104,29,83\n"
            "378,-903,-323\n"
            "-778,-728,485\n"
            "426,699,580\n"
            "-438,-605,-362\n"
            "-469,-447,-387\n"
            "509,732,623\n"
            "647,635,-688\n"
            "-868,-804,481\n"
            "614,-800,639\n"
            "595,780,-596\n"
            "\n"
            "--- scanner 4 ---\n"
            "727,592,562\n"
            "-293,-554,779\n"
            "441,611,-461\n"
            "-714,465,-776\n"
            "-743,427,-804\n"
            "-660,-479,-426\n"
            "832,-632,460\n"
            "927,-485,-438\n"
            "408,393,-506\n"
            "466,436,-512\n"
            "110,16,151\n"
            "-258,-428,682\n"
            "-393,719,612\n"
            "-211,-452,876\n"
            "808,-476,-593\n"
            "-575,615,604\n"
            "-485,667,467\n"
            "-680,325,-822\n"
            "-627,-443,-432\n"
            "872,-547,-609\n"
            "833,512,582\n"
            "807,604,487\n"
            "839,-516,451\n"
            "891,-625,532\n"
            "-652,-548,-490\n"
            "30,-46,-14",
            79, 3621),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};
        auto output = day19(in);
        CHECK_EQ(std::get<1>(tc), std::strtol(output.answer[0].c_str(), NULL, 10));
        CHECK_EQ(std::get<2>(tc), std::strtol(output.answer[1].c_str(), NULL, 10));
    }
}

TEST_CASE("day19, part 1 & part 2") {
    input_t in = parse::load_input("input/day19.txt");
    auto output = day19(in);
    CHECK_EQ(365, std::strtol(output.answer[0].c_str(), NULL, 10));
    CHECK_EQ(11060, std::strtol(output.answer[1].c_str(), NULL, 10));
}

#endif  // IS_TEST
