#include "day16.h"

using parse::input_t;

namespace day16_internal {

struct Packet; /* forward decl */

struct Payload {
    uint64_t value;
    std::vector<Packet> children;
};

struct Packet {
    /* header */
    uint8_t version;
    uint8_t type_id;
    Payload payload;

    inline bool is_literal() const {
        return type_id == 4;
    }

    inline bool is_operator() const {
        return !is_literal();
    }
};

const char* hex_to_binary(char hex) {
    switch (hex) {
        case '0':
            return "0000";
        case '1':
            return "0001";
        case '2':
            return "0010";
        case '3':
            return "0011";
        case '4':
            return "0100";
        case '5':
            return "0101";
        case '6':
            return "0110";
        case '7':
            return "0111";
        case '8':
            return "1000";
        case '9':
            return "1001";
        case 'A':
            return "1010";
        case 'B':
            return "1011";
        case 'C':
            return "1100";
        case 'D':
            return "1101";
        case 'E':
            return "1110";
        case 'F':
            return "1111";
    }
    std::string msg("Unable to convert hex char: ");
    msg += hex;
    throw std::invalid_argument(msg);
}

struct ParseResult {
    Packet packet;
    size_t bytes_read;
};

ParseResult parse_packet_bin(std::span<char> binary) {
    ParseResult result;

    auto it = binary.begin();
    auto get_offset = [&]() -> size_t {
        return it - binary.begin();
    };

    // the first three bits encode the packet *version*
    result.packet.version = 0;
    for (int i = 0; i <= 2; ++i) {
        if (*it == '1') result.packet.version |= 1 << (2 - i);
        ++it;
    }

    // the next three bits encode the packet *type ID*
    result.packet.type_id = 0;
    for (int i = 0; i <= 2; ++i) {
        if (*it == '1') result.packet.type_id |= 1 << (2 - i);
        ++it;
    }

    if (result.packet.is_literal()) {
        std::stringstream ss;
        bool is_last = false;
        while (!is_last) {
            is_last = *it == '0';
            ++it;
            for (int i = 0; i <= 3; ++i) {
                ss << *it;
                ++it;
            }
        };
        auto s = ss.str();
        result.packet.payload.value = std::stol(s, 0, 2);
    } else {
        uint8_t length_type_id = *it - '0';
        ++it;
        assert(length_type_id == 0 || length_type_id == 1);
        if (length_type_id == 0) {
            // the next *15* bits are a number that represents the *total length in bits* of the sub-packets contained by this packet
            size_t subpackets_len = 0;
            for (int i = 0; i <= 14; ++i) {
                if (*it == '1') subpackets_len |= 1 << (14 - i);
                ++it;
            }
            auto end = get_offset() + subpackets_len;
            while (get_offset() < end) {
                std::span<char> sub_binary(it, binary.end());
                auto child_result = parse_packet_bin(sub_binary);
                it += child_result.bytes_read;
                result.packet.payload.children.push_back(child_result.packet);
            }

        } else {
            // the next *11* bits are a number that represents the *number of sub-packets immediately contained* by this packet
            size_t subpackets_count = 0;
            for (int i = 0; i <= 10; ++i) {
                if (*it == '1') subpackets_count |= 1 << (10 - i);
                ++it;
            }
            for (size_t i = 0; i < subpackets_count; i++) {
                std::span<char> sub_binary(it, binary.end());
                auto child_result = parse_packet_bin(sub_binary);
                it += child_result.bytes_read;
                result.packet.payload.children.push_back(child_result.packet);
            }
        }
    }

    result.bytes_read = get_offset();
    return result;
}

Packet parse_packet_hex(std::span<const char> hexstring) {
    DEBUG("parsing hex: {}", std::string(hexstring.begin(), hexstring.end()));
    constexpr size_t MAX_LEN = 6000;

    char buf[MAX_LEN];
    size_t n = 0;

    for (auto c : hexstring) {
        auto bin = hex_to_binary(c);
        assert(n < MAX_LEN - 1);
        for (int i = 0; i < 4; ++i) buf[n++] = *(bin + i);
    }

    std::span<char> binary(buf, n);
    return parse_packet_bin(binary).packet;
}

uint64_t version_sum(const Packet& packet) {
    uint64_t sum = packet.version;
    if (packet.is_literal()) {
        return sum;
    }
    for (const auto& child : packet.payload.children) {
        sum += day16_internal::version_sum(child);
    }
    return sum;
}

uint64_t eval(const Packet& packet) {
    using namespace day16_internal;
    if (packet.is_literal()) {
        return packet.payload.value;
    }
    uint64_t result;
    switch (packet.type_id) {
        case 0:
            result = 0;
            for (const auto& child : packet.payload.children) result += eval(child);
            break;
        case 1:
            result = 1;
            for (const auto& child : packet.payload.children) result *= eval(child);
            break;
        case 2:
            result = std::numeric_limits<uint64_t>().max();
            for (const auto& child : packet.payload.children) result = std::min(result, eval(child));
            break;
        case 3:
            result = std::numeric_limits<uint64_t>().min();
            for (const auto& child : packet.payload.children) result = std::max(result, eval(child));
            break;
        case 5:
            result = eval(packet.payload.children[0]) > eval(packet.payload.children[1]) ? 1 : 0;
            break;
        case 6:
            result = eval(packet.payload.children[0]) < eval(packet.payload.children[1]) ? 1 : 0;
            break;
        case 7:
            result = eval(packet.payload.children[0]) == eval(packet.payload.children[1]) ? 1 : 0;
            break;
        default:
            throw std::invalid_argument("unsupported type_id");
    }
    return result;
}

}  // namespace day16_internal

parse::output_t day16(input_t in) {
    using namespace day16_internal;
    uint64_t part1 = 0, part2 = 0;

    if (*(in.s + in.len - 1) == '\n') in.len--;
    std::span<const char> input(in.s, in.len);
    Packet packet = parse_packet_hex(input);
    part1 = version_sum(packet);
    part2 = eval(packet);

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day16.txt");
    auto output = day16(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;
using namespace day16_internal;

TEST_CASE("day16: literal packet") {
    std::string s("D2FE28");
    std::span<char> input_hex(&s[0], s.length());
    auto p = parse_packet_hex(input_hex);
    CHECK_EQ(6, p.version);
    CHECK_EQ(4, p.type_id);
    CHECK_EQ(true, p.is_literal());
    CHECK_EQ(false, p.is_operator());
    CHECK_EQ(2021, p.payload.value);
}

TEST_CASE("day16: operator packet with length type ID 0 that contains two sub-packets") {
    std::string s("38006F45291200");
    std::span<char> input_hex(&s[0], s.length());
    auto p = parse_packet_hex(input_hex);
    CHECK_EQ(1, p.version);
    CHECK_EQ(6, p.type_id);
    CHECK_EQ(false, p.is_literal());
    CHECK_EQ(true, p.is_operator());
    CHECK_EQ(2, p.payload.children.size());
    CHECK_EQ(10, p.payload.children[0].payload.value);
    CHECK_EQ(20, p.payload.children[1].payload.value);
}

TEST_CASE("day16: operator packet with length type ID 1 that contains three sub-packets") {
    std::string s("EE00D40C823060");
    std::span<char> input_hex(&s[0], s.length());
    auto p = parse_packet_hex(input_hex);
    CHECK_EQ(7, p.version);
    CHECK_EQ(3, p.type_id);
    CHECK_EQ(false, p.is_literal());
    CHECK_EQ(true, p.is_operator());
    CHECK_EQ(3, p.payload.children.size());
    CHECK_EQ(1, p.payload.children[0].payload.value);
    CHECK_EQ(2, p.payload.children[1].payload.value);
    CHECK_EQ(3, p.payload.children[2].payload.value);
}

TEST_CASE("day16: examples part 1") {
    auto test_cases = {
        make_tuple(
            "8A004A801A8002F478",
            "16"),
        make_tuple(
            "620080001611562C8802118E34",
            "12"),
        make_tuple(
            "C0015000016115A2E0802F182340",
            "23"),
        make_tuple(
            "A0016C880162017C3686B18A3D4780",
            "31"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day16(in);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
    }
}

TEST_CASE("day16: examples part 2") {
    auto test_cases = {
        make_tuple(
            "C200B40A82",
            "3"),
        make_tuple(
            "04005AC33890",
            "54"),
        make_tuple(
            "880086C3E88112",
            "7"),
        make_tuple(
            "CE00C43D881120",
            "9"),
        make_tuple(
            "D8005AC2A8F0",
            "1"),
        make_tuple(
            "F600BC2D8F",
            "0"),
        make_tuple(
            "9C005AC2F8F0",
            "0"),
        make_tuple(
            "9C0141080250320F1802104A08",
            "1"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day16(in);
        CHECK_EQ(std::get<1>(tc), output.answer[1]);
    }
}

TEST_CASE("day16, part 1 & part 2") {
    input_t in = parse::load_input("input/day16.txt");
    auto output = day16(in);
    CHECK_EQ("984", output.answer[0]);
    long part2 = std::stol(output.answer[1]);
    CHECK_EQ(1015320896946, part2);
}

#endif  // IS_TEST
