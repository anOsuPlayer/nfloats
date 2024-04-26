# nfloats
C++ Arbitrary precision, IEEE-754 floating point numbers, declared as follows:

`nfloat<N> f = nfloat<N>("12.34");`

where `N` is the _number of bits_ on which the floating point number will be coded onto (Note that `N` is such that: `16 <= N <= 2048` and `N % 8 == 0`, for any `nfloat`).
> As of now, lengths higher than `1024` may lead to some problem. 

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

### Manipulation

The only way `nfloat`s can be manipulated is by directly tweaking their sign, exponent or mantissa. As of now, there is now way to perform arithmetic operation between them. However, one operation that can be performed is a _cast_ to another `nfloat` having a _different length_. This can be done with the `cast<>` function:

`nfloat<128> F = nfloat<64>("0.3").cast<128>();`

Keep in mind that, much like regular floating point casts, the precision of the casted number remains faithful to the size of the `nfloat` _before_ being casted. In this case, a 128-bit `nfloat` will be created, but it will hold the value `0.3` with the precision of a 64-bit floating point.

### Notes

The aim of `nfloat`s is _not_ efficiency nor any _actual application_, as some of the algorithms that were created to compute them are very inefficient (yet functiunal). My boredom may have lead to their creation, yet won't lead any further I'm afraid.

However, even if they were somehow efficient, there would be no need to store high precision decimal numbers under the rules of the IEEE-754 format, as many other techniques concerning arbitrary precision numbers do the job much better.

Regardless of that, they can show how the IEEE-754 format can be applied to floating point numbers different from the classic 32-bit floats and the 64-bit doubles found everywhere in programming.
