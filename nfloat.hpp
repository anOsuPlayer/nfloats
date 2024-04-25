#ifndef FLOATN
    #define FLOATN

    #include <quadmath.h>
    #include <iostream>
    #include <string>
    #include <bitset>
    #include <cmath>

    #include "bignum.hpp"

    typedef int fsize_t;

    using std::bitset;

    constexpr fsize_t _exp_size(fsize_t size) {
        return (fsize_t)(round(6.1 * log(((double) size) / 16 + 0.7) + 1.8));
    }
    constexpr fsize_t _mant_size(fsize_t size) {
        return (size - 1 - _exp_size(size));
    }

    constexpr fsize_t _bias(fsize_t size) {
        return (fsize_t) pow(2, _exp_size(size)-1)-1;
    }

    template <fsize_t size, fsize_t exp_size = _exp_size(size), fsize_t mant_size = _mant_size(size)> class nfloat {
        static_assert(size % 8 == 0 && size >= 16 && size <= 2048);

        private:
            bitset<size>* _body;

        public:
            nfloat() {
                this->_body = new bitset<size>();
            }
            nfloat(const bitset<size>& _body) {
                this->_body = new bitset<size>(_body);
            }
        
            nfloat(float f) {
                static_assert(size == 32);
                this->_body = new bitset<32>(*reinterpret_cast<long*>(&f));
            }
            nfloat(double d) {
                static_assert(size == 64);
                this->_body = new bitset<64>(*reinterpret_cast<long long*>(&d));
            }
            nfloat(__float128 m) {
                static_assert(size == 128);
                this->_body = new bitset<128>();
                *this->_body |= static_cast<long long>(*reinterpret_cast<__uint128_t*>(&m) >> 64);
                *this->_body <<= 64;
                *this->_body |= static_cast<long long>(*reinterpret_cast<__uint128_t*>(&m));
            }

            nfloat(const std::string& str) {
                this->_body = new bitset<size>();

                if (str == "0" || str == "+0") {
                    return;
                }
                if (str == "-0") {
                    this->set_sign(true);
                    return;
                }
                if (str == "infinity") {
                    this->set_exponent(bitset<exp_size>().flip());
                    return;
                }
                if (str == "-infinity") {
                    this->set_sign(true);
                    this->set_exponent(bitset<exp_size>().flip());
                    return;
                }

                if (str[0] == '-') {
                    this->set_sign(true);
                }

                std::string integral_str;
                std::string decimal_str;
                
                if (str.find('.') != -1) {
                    integral_str = str.substr(0, str.find_first_of('.'));
                    decimal_str = str.substr(str.find_first_of('.')+1);
                }
                else {
                    integral_str = str;
                    decimal_str = "0";
                }

                big_num integral(integral_str);
                big_num decimal(decimal_str);

                long pow = closest_2pow(integral)-1;
                long long exp;
                
                if (pow < 0) {
                    pow = 0;
                    if (decimal != 0) {
                        exp = _bias(size);
                        big_num threshold = (big_num(10) ^ decimal_str.length());
                        while (decimal <= threshold && exp > 0) {
                            decimal *= 2;
                            exp--;
                        }
                        decimal -= threshold;
                    }
                }
                else {
                    exp = pow + _bias(size);

                    *(this->_body) |= to_bitset<size>(integral);
                    *(this->_body) <<= (mant_size - pow);
                    this->_body->reset(mant_size);
                }   

                if (decimal != 0) {
                    big_num threshold = (big_num(10) ^ decimal_str.length());
                    for (int i = 0; i < mant_size - pow; i++) {
                        decimal *= 2;
                        if (decimal >= threshold) {
                            decimal -= threshold;
                            this->_body->set(mant_size-pow-1-i);
                            if (decimal == 0) {
                                break;
                            }
                            continue;
                        }
                    }
                }

                for (int i = 0; i < exp_size; i++) {
                    if (exp % 2 == 1) {
                        this->_body->set(mant_size+i);
                    }
                    exp /= 2;
                }

                std::cout << this->mantissa() << "\n";

                this->_body->flip(0);
                if (this->body()[0] == 0) {
                    this->_body->flip(1);
                    for (int e = 0; e < size-1 && this->body()[e+1] == 0; e++) {
                        this->_body->flip(e);
                        this->_body->flip(e+1);
                    }
                }

                std::cout << this->mantissa() << "\n";

                this->_body->set(0);
            }

            template <fsize_t new_size> nfloat(const nfloat<new_size>& nf) {
                *this = nf;
            }
            template <fsize_t new_size> nfloat& operator = (nfloat<new_size>& nf) {
                if (this->_body != nullptr) {
                    delete this->_body;
                }
                this->_body = new bitset<size>();
                *this->_body |= *nf._body;
                return nf;
            }

            ~nfloat() {
                delete this->_body;
            }

            friend std::ostream& operator << (std::ostream& os, const nfloat<size>& f) {
                os << f.sign() << " " << f.exponent() << " " << f.mantissa();
                return os;
            }

            std::string to_string(unsigned int accuracy) {
                big_num exp = from_bitset<exp_size>(this->exponent());
                exp -= _bias(size);

                big_num integral = 0, decimal = 1;
                
                if (exp < 0) {
                    for (big_num n = exp; n < -1; n++) {
                        decimal *= 10;
                    }
                }

                for (int i = -1; i < mant_size; i++) {
                    if (exp < 0) {
                        decimal *= 10;
                    }
                    if (i == -1 || this->body()[mant_size-1-i] == 1) {
                        if (exp >= 0) {
                            integral += (big_num(2) ^ (exp));
                        }
                        else {
                            decimal += (big_num(5) ^ (!exp));
                        }
                    }
                    exp--;
                }

                std::string out = "";

                out += this->sign() ? '-' : '+';

                for (int i = integral.get_nlen()-1; i >= 0; i--) {
                    out += (integral[i] + 48);
                }

                if (accuracy > 0) {
                    out += '.';
                    for (int i = 0; i < accuracy && i < decimal.get_nlen()-1; i++) {
                        out += (decimal[decimal.get_nlen()-2-i] + 48);
                    }
                }

                return out;
            }

            static nfloat<size> inf() {
                return nfloat("infinity");
            }
            static nfloat<size> neg_inf() {
                return nfloat("-infinity");
            }

            const bitset<size>& body() const {
                return *this->_body;
            }

            bool sign() const {
                return this->_body[size-1] == 1;
            }
            void set_sign(bool sign) {
                if (sign) {
                    this->_body->set(size-1);
                    return;
                }
                this->_body->reset(size-1);
                return;
            }

            bitset<exp_size> exponent() const {
                bitset<exp_size> exp;
                const bitset<size>& this_set = *this->_body;
                for (int i = 0; i < exp_size; i++) {
                    if (this_set[mant_size+i] == 1) {
                        exp.set(i);
                    }
                }
                return exp;
            }
            void set_exponent(const bitset<exp_size>& exp) {
                for (int i = 0; i < exp_size; i++) {
                    if (exp[i] == 1) {
                        this->_body->set(mant_size+i);
                    }
                }
            }

            bitset<mant_size> mantissa() const {
                bitset<mant_size> mant;
                const bitset<size>& this_set = *this->_body;
                for (int i = 0; i < mant_size; i++) {
                    if (this_set[i] == 1) {
                        mant.set(i);
                    }
                }
                return mant;
            }
            void set_mantissa(const bitset<mant_size>& mant) {
                for (int i = 0; i < mant_size; i++) {
                    if (mant[i] == 1) {
                        this->_body->set(i);
                    }
                }
            }

            template <fsize_t new_size> nfloat<new_size, _exp_size(new_size), _mant_size(new_size)> cast() const {
                static_assert(new_size % 8 == 0 && new_size >= 16 && new_size <= 2048);
                if (new_size == size) { return nfloat<size>(*this->_body); }

                bitset<new_size> set;
                if (this->sign()) {
                    set.set(new_size-1);
                }

                long long num_exp = this->exponent().to_ullong();
                num_exp -= _bias(size); num_exp += _bias(new_size);
                if (num_exp >= ((long long) pow((_bias(new_size)+1), 2))) {
                    return sign() ? nfloat<new_size>::neg_inf() : nfloat<new_size>::inf();
                }
                for (int i = 0; i < _exp_size(new_size); i++) {
                    if (num_exp % 2 == 1) {
                        set.set(_mant_size(new_size)+i);
                    }
                    num_exp >>= 1;
                }

                bitset<mant_size> this_mant = mantissa();
                for (int i = 0; i < _mant_size(new_size) && i < mant_size; i++) {
                    if (this_mant[mant_size-1-i] == 1) {
                        set.set(_mant_size(new_size)-1-i);
                    }
                }

                return nfloat<new_size>(set);
            }

            template <fsize_t new_size> operator nfloat<new_size>*() const {
                return this->cast<new_size>();
            }

            operator float() const {
                nfloat<32> nf = this->cast<32>();
                long v = nf.body().to_ulong();
                float f = *reinterpret_cast<float*>(&v);
                return f;
            }

            operator double() const {
                nfloat<64> nf = this->cast<64>();
                long long v = nf.body().to_ullong();
                double f = *reinterpret_cast<double*>(&v);
                return f;
            }
    };

#endif