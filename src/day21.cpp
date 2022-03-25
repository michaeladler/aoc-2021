#define DEBUGLOG

#include "day21.h"

using parse::input_t;

struct Cache {
    // position (>=1) x cast (>=1) -> position
    uint8_t data[11][301];

    Cache() {
        for (int position = 1; position <= 10; position++) {
            for (int cast = 1; cast <= 300; cast++) {
                data[position][cast] = ((position + cast - 1) % 10) + 1;
            }
        }
    }
};

static Cache cache;

struct Pawn {
    uint64_t position;
    uint64_t score;

    Pawn() : position(0), score(0) {}

    void advance(uint64_t cast) {
        position = cache.data[position][cast];
        score += position;
    }

    inline bool operator==(const Pawn& other) const {
        return position == other.position && score == other.score;
    }
};

struct DeterministicDice {
    uint64_t dice = 1;
    uint64_t counter = 0;

    uint64_t roll() {
        counter++;
        uint64_t old = dice;
        dice = (dice % 100) + 1;
        return old;
    }
};

struct GameState {
    Pawn pawns[2];

    bool operator==(const GameState& other) const {
        return pawns[0] == other.pawns[0] && pawns[1] == other.pawns[1];
    }
};

struct HashGameState {
    size_t operator()(const GameState& gs) const {
        std::size_t ret = 0;
        hash_combine(ret, gs.pawns[0].position, gs.pawns[0].score, gs.pawns[1].position, gs.pawns[1].score);
        return ret;
    }
};

parse::output_t day21(input_t in) {
    uint64_t part1 = 0, part2 = 0;

    DeterministicDice dice;
    GameState initial_state;
    in.s += 28;
    auto pos_one = parse::positive<uint64_t>(in);
    initial_state.pawns[0].position = pos_one;
    in.s++;

    in.s += 28;
    auto pos_two = parse::positive<uint64_t>(in);
    initial_state.pawns[1].position = pos_two;

    GameState state = initial_state;

    size_t current = 1;
    while (state.pawns[current].score < 1000) {
        current = (current + 1) % 2;
        auto roll = dice.roll() + dice.roll() + dice.roll();
        state.pawns[current].advance(roll);
    }
    // current is the index of the winner
    size_t loser = (current + 1) % 2;
    part1 = state.pawns[loser].score * dice.counter;

    /*
     * Part 2
     */
    Cache cache;
    assert(cache.data[2][9] == 1);
    assert(cache.data[1][1] == 2);

    // for each move, how often can it occur, e.g. 4 = 1+1+2 = 1+2+1 = 2+1+1, so [4] = 3
    uint64_t moveFrequency[10];
    moveFrequency[3] = 1;
    moveFrequency[4] = 3;
    moveFrequency[5] = 6;
    moveFrequency[6] = 7;
    moveFrequency[7] = 6;
    moveFrequency[8] = 3;
    moveFrequency[9] = 1;

    std::unordered_map<GameState, uint64_t, HashGameState> states[2];  // old and new state
    int active_idx = 0;
    int active_player_idx = 0;
    uint64_t wins[2] = {0, 0};
    states[active_idx][initial_state] = 1;
    while (!states[active_idx].empty()) {
        auto new_idx = 1 - active_idx;
        states[new_idx].clear();
        for (auto it = states[active_idx].cbegin(); it != states[active_idx].cend(); it++) {
            for (size_t move = 3; move <= 9; move++) {
                GameState next_gs = it->first;  // make copy
                next_gs.pawns[active_player_idx].advance(move);
                if (next_gs.pawns[active_player_idx].score >= 21) {
                    wins[active_player_idx] += moveFrequency[move] * it->second;
                } else {
                    states[new_idx][next_gs] += moveFrequency[move] * it->second;
                }
            }
        }
        active_idx = new_idx;
        active_player_idx = 1 - active_player_idx;
    }
    part2 = std::max(wins[0], wins[1]);

    return {part1, part2};
}

#ifdef IS_MAIN
int main() {
    input_t in = parse::load_input("input/day21.txt");
    auto output = day21(in);
    fmt::print("Part 1: {}\nPart 2: {}\n", output.answer[0], output.answer[1]);
    return 0;
}
#endif  // IS_MAIN

#ifdef IS_TEST

#include <doctest/doctest.h>

using std::make_tuple;

TEST_CASE("day21: examples") {
    auto test_cases = {
        make_tuple(
            "Player 1 starting position: 4\n"
            "Player 2 starting position: 8",
            "739785", "444356092776315"),
    };

    for (auto& tc : test_cases) {
        auto first = std::string(std::get<0>(tc));
        DEBUG("Input:\n{}", first);
        input_t in = {&first[0], static_cast<ssize_t>(first.length())};

        auto output = day21(in);
        DEBUG("Part 1: {}, Part 2: {}", output.answer[0], output.answer[1]);
        CHECK_EQ(std::get<1>(tc), output.answer[0]);
        CHECK_EQ(std::get<2>(tc), output.answer[1]);
    }
}

TEST_CASE("day21, part 1 & part 2") {
    input_t in = parse::load_input("input/day21.txt");
    auto output = day21(in);
    CHECK_EQ("597600", output.answer[0]);
    CHECK_EQ("634769613696613", output.answer[1]);
}

#endif  // IS_TEST
