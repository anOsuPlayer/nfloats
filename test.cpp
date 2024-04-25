#include <iostream>
#include <quadmath.h>

#include "nfloat.hpp"

int main() {

    big_num n1 = 20;
    big_num n2 = 10;

    std::cout << std::boolalpha << (n1 <= n2);
}