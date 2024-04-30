#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    big_num n = 125123412313123;

    std::cout << to_bitset<500>(n);
}