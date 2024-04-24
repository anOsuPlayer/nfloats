# nfloats
C++ Arbitrary precision, IEEE-754 floating point numbers, declared as follows:

`nfloat<N> f = nfloat<N>("12.34");`

where `N` is the _number of bits_ on which the floating point number will be coded onto (Note that `N` is such that: `16 <= N <= 2048` and `N % 8 == 0`, for any `nfloat`).

### Layout

`nfloat`s are stored as `std::bitset`s of the same size. The first bit always represents _the sign_ of the floating point number, while the _exponent_ and the _mantissa_ follow these rules:
* The _exponent_ of an `nfloat` is stored in a region of the `bitset` which occupies `round(6.1 * ln(s/16 + 0.7) + 1.8)` bits, where `s` is the size of the `nfloat`. This funciton was experimentally obtained by interpolating a series of points which had the value `s / 16` as `x` coordinate and the _number of bits_ of the _exponent_ of `s`-sized IEEE-754 floating point implementation as `y` coordinate (computed on `s` values of 16, 32, 64, 128 and 256, standard exponent sizes were obtained from [this page](https://en.wikipedia.org/wiki/IEEE_754)).
* The _mantissa_ is stored in the remaining bits.

### Initializers

`nfloat`s can be initialized from any already existing floating point supported by C++:

`nfloat<32> f1 = nfloat<32>(1.0f);`, from `float`s;

`nfloat<64> f2 = nfloat<32>(1.0);`, from `double`s;

`nfloat<128> f3 = nfloat<128>(1.0Q);` and from `quadmath.h`'s `__float128`s.

Note that for any `nfloat` derived from an already existing floating point number its size will have to be _equal_ to the one of said type. For example, when creating a `nfloat` from a `double`, given that their size is `64 bits`, then the declaration will have to declare `nfloat<64>` as type.

`nfloat`s can also be generated from a `std::string`:

`nfloat<80> fstr = nfloat<80>("1.0001");`

