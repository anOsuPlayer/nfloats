#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    nfloat<80> f = nfloat<80>("0.3312");

    std::cout << sizeof(nfloat<80>);
}