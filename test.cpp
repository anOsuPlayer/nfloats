#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<2048> f = nfloat<2048>("+0.3312");
    f.set_sign(true);

    std::cout << f.to_string(20000);
}