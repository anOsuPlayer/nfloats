#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<1024> f = nfloat<1024>("-0.3312");

    std::cout << f << "\n" << f.to_string(1000);
}