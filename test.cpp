#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<32> f = nfloat<32>("0.3");

    std::cout << f.to_string(500);
}