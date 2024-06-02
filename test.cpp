#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<32> f(1.51f);

    std::cout << f << "\n";
    std::cout << f.to_string(500);
}