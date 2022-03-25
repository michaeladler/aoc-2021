#include "day20.h"

using parse::input_t;

constexpr char LIGHT_PIXEL = '#';
constexpr char DARK_PIXEL = '.';

class Image {
   private:
    std::unordered_map<iPair, char, std::hash<iPair>> pixels;

   public:
    int32_t min_x = std::numeric_limits<int32_t>().max();
    int32_t min_y = std::numeric_limits<int32_t>().max();
    int32_t max_x = std::numeric_limits<int32_t>().min();
    int32_t max_y = std::numeric_limits<int32_t>().min();
    char outside_region = DARK_PIXEL;  // LIGHT_PIXEL is also possible

    Image() : pixels() {
        pixels.reserve(1024);
    }

    void insert(int32_t x, int32_t y, const char c) {
        if (x < min_x)
            min_x = x;
        else if (x > max_x)
            max_x = x;
        if (y < min_y)
            min_y = y;
        else if (y > max_y)
            max_y = y;
        pixels[iPair(x, y)] = c;
    }

    char get(int32_t x, int32_t y) const {
        auto got = pixels.find(iPair(x, y));
        if (got != pixels.end()) {
            return got->second;
        }
        return outside_region;
    }

    void render() const {
        for (int32_t y = min_y - 3; y <= max_y + 3; y++) {
            for (int32_t x = min_x - 3; x <= max_x + 3; x++) {
                fmt::print("{}", get(x, y));
            }
            fmt::print("\n");
        }
    }
};

struct Algorithm {
    char data[512];

    Algorithm(input_t &in) {
        for (size_t i = 0; i < 512; i++) {
            data[i] = *in.s;
            in.s++, in.len--;
        }
    }

    Image apply(const Image &input) const {
        Image output;
        for (int32_t y = input.min_y - 1; y <= input.max_y + 1; y++) {
            for (int32_t x = input.min_x - 1; x <= input.max_x + 1; x++) {
                size_t offset = 8;
                int32_t index = 0;
                for (int32_t dy = -1; dy <= 1; dy++) {
                    for (int32_t dx = -1; dx <= 1; dx++) {
                        // turning dark pixels (`.`) into `0` and light pixels (`#`) into `1`
                        auto x_neighbor = x + dx;
                        auto y_neighbor = y + dy;
                        auto pixel = input.get(x_neighbor, y_neighbor);
                        if (pixel == LIGHT_PIXEL) {
                            index |= 1 << offset;
                        }
                        offset--;
                    }
                }
                output.insert(x, y, data[index]);
            }
        }
        output.outside_region = input.outside_region == DARK_PIXEL ? data[0] : data[511];
        DEBUG("outside_region: {} -> {}", input.outside_region, output.outside_region);
        return output;
    }
};

parse::output_t day20(input_t in) {
    int32_t part1 = 0, part2 = 0;

    auto count_light_pixels = [](Image &img) -> int32_t {
        int32_t counter = 0;
        for (int32_t y = img.min_y; y <= img.max_y; y++) {
            for (int32_t x = img.min_x; x <= img.max_x; x++) {
                if (img.get(x, y) == LIGHT_PIXEL) counter++;
            }
        }
        return counter;
    };

    const Algorithm algorithm(in);

    Image img;
    {
        while (*in.s != LIGHT_PIXEL && *in.s != DARK_PIXEL) {
            in.s++, in.len--;
        }
        size_t y = 0;
        while (in.len > 0) {
            size_t x = 0;
            while (*in.s != '\n') {
                if (*in.s == LIGHT_PIXEL) {
                    img.insert(x, y, LIGHT_PIXEL);
                }
                in.s++, in.len--;
                x++;
            }
            in.s++, in.len--;
            y++;
        }
    }

    for (size_t step = 1; step <= 2; step++) {
        DEBUG("*** step {} ***", step);
        img = algorithm.apply(img);
    }
    part1 = count_light_pixels(img);

    for (size_t step = 3; step <= 50; step++) {
        DEBUG("*** step {} ***", step);
        img = algorithm.apply(img);
    }
    part2 = count_light_pixels(img);

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day20.txt");
    auto output = day20(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day20: examples") {
    auto test_cases = {
        make_tuple(
            "..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..##"
            "#..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###"
            ".######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#."
            ".#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#....."
            ".#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#.."
            "...####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#....."
            "..##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#\n"
            "\n"
            "#..#.\n"
            "#....\n"
            "##..#\n"
            "..#..\n"
            "..###\n",
            "35", "3351"),
    };

    for (auto &tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        DEBUG("Input:\n{}", first);
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day20(in);
        DEBUG("Part 1: {}, Part 2: {}", output.answer[0], output.answer[1]);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day20, part 1 & part 2") {
    input_t in = parse::load_input("input/day20.txt");
    auto output = day20(in);
    CHECK_EQ("5359", output.answer[0]);
    CHECK_EQ("12333", output.answer[1]);
}

#endif  // IS_TEST
