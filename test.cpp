#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<32> f = nfloat<32>("0.3421");

    std::cout << f << " " << f.to_string(50);
}