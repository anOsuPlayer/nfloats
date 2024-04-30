#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<32> f = nfloat<32>("0.999999999999999999999");

    std::cout << f << "\n" << f.to_string(50000);
}