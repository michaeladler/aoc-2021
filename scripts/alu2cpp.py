#!/usr/bin/env python3

import fileinput

idx = 0
print(
    """
bool alu(const int8_t digits[]) {

    long w = 0, x = 0, y = 0, z = 0;
    """
)
for line in fileinput.input():
    parts = line.strip().split()
    if parts[0] == "inp":
        print("\n")
        print("    // Step {}".format(idx + 1))
        print("    {} = digits[{}];".format(parts[1], idx))
        idx += 1
    elif parts[0] == "add":
        print("    {} = {} + {};".format(parts[1], parts[1], parts[2]))
    elif parts[0] == "mul":
        print("    {} = {} * {};".format(parts[1], parts[1], parts[2]))
    elif parts[0] == "div":
        print("    {} = {} / {};".format(parts[1], parts[1], parts[2]))
    elif parts[0] == "mod":
        print("    {} = {} % {};".format(parts[1], parts[1], parts[2]))
    elif parts[0] == "eql":
        print("    {} = {} == {} ? 1 : 0;".format(parts[1], parts[1], parts[2]))

print(
    """
    bool valid = z == 0;
    return valid;
}
    """
)
