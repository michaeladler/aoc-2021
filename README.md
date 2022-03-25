[![CI](https://github.com/michaeladler/aoc-2021/actions/workflows/ci.yml/badge.svg)](https://github.com/michaeladler/aoc-2021/actions/workflows/ci.yml)

# aoc-2021

My third [Advent of Code](https://adventofcode.com/).
This time using C++, the objective is to have a total runtime of less than 60 seconds.

## Benchmarks

* Intel(R) Core(TM) i7-5600U:  13.9 sec
* Intel(R) Core(TM) i7-7820HQ: 13.5 sec
* Intel(R) Core(TM) i7-11850H: 7.7 sec

## Lessons Learned

### Meson

Meson and NixOS do not work well together: [meson strips `RPATH`](https://web.archive.org/web/20211215093316/https://bleepcoder.com/meson/117487924/rpath-doesn-t-get-set-with-find-library-foo-dirs-path-to-lib)

Workaround: `LDFLAGS="-Wl,-rpath=${fmtlog}/lib"`

### Polymorphic Allocators

* [OOP2021-pmr-STL-fuer-Embedded-Applications-en.pdf](https://web.archive.org/web/20211214103145/https://www.rkaiser.de/wp-content/uploads/2021/02/OOP2021-pmr-STL-fuer-Embedded-Applications-en.pdf)
* https://www.cppstories.com/2020/08/pmr-dbg.html/

### Delete while Iterate

Deleting while iterating a hash map (or set) is not possible in Java, but in C++ it is:

```c
it = co2_numbers.erase(it);
```

## Puzzles

Hardest puzzles:

1. Day 19: Beacon Scanner
2. Day 21: Dirac Dice
3. Day 22: Reactor Reboot

### Day 2

* `std::span` is very useful

### Day 6

* took me quite a while to come up with *bins* and avoid the out-of-memory problem (for me the hardest problem in days 1-11)
* `std::accumulate`: the last argument type determines the type used for the summation! use `(uint64_t)0` to prevent integer overflow for large positive numbers.

### Day 9

* `std::tuple` is not hashable by default, see https://stackoverflow.com/a/15161034 for a solution

### Day 12

* Used the generic backtracking recipe from [Skiena](https://www3.cs.stonybrook.edu/~skiena/392/programs/). DFS would have been more efficient though.

### Day 16

* Lots of bit fiddling, but quite fun
* Should get rid of `stringstream`, possible speed up

### Day 18

* Ugly solution using string instead of a tree representation, but still reasonably fast
* *Lots* of corner cases and bugs/off-by-one errors (=> *many* tests)
* Did not enjoy this puzzle

### Day 21

* Lanternfish revisited
* To toggle between 0 and 1 in the loop: `active_idx = 1 - active_idx`.
  This is the same as `active_idx = (active_idx + 1) % 2`.
* Part 2 was tricky

### Day 22

* Many corner-cases to consider
* Quite difficult

### Day 23

* Rather easy using backtracking approach although much slower than A*
* Does not produce correct result when compiling with gcc! You *must* use clang. Weird.

### Day 24

* My favorite puzzle! Very interesting.
* Used a Python script to convert ALU to CPP code and then simplified the code until I found a solution
    => Does not work with other inputs than mine
