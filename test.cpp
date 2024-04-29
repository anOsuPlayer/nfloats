#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<128> f = nfloat<128>("-0.3312");


    std::cout << f;
}