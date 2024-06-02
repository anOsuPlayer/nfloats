#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<128> f("4342.52");

    std::cout << f << "\n";
    std::cout << f.to_string(500);
}