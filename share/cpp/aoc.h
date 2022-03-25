#ifndef _AOC_H
#define _AOC_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <vector>
#include <array>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <stack>
#include <sstream>
#include <span>
#include <utility>
#include <list>
#include <queue>

// SIMD
//#include <x86intrin.h>

// My libs
#include "parse.h"
#include "bits.h"
#include "numeric.h"
#include "log.h"
#include "hash.h"
#include "pair.h"

struct advent_t {
    parse::output_t (*fn)(parse::input_t);
};

int aoc_main(int argc, char **argv, const std::map<int, advent_t> &days);

#endif
