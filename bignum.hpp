#ifndef BIGNUM
    #define BIGNUM

    #include <iostream>
    #include <string>
    #include <bitset>

    typedef signed char digit_t;

    using std::bitset;

    class big_num {
        private:
            digit_t* digits = nullptr;
            size_t nlen = 0;
            bool sign;

            void resize(int d) {
                size_t prev = this->nlen;
                this->nlen += d;
                this->digits = ((this->digits == nullptr) ? (digit_t*) malloc(sizeof(digit_t)*(this->nlen))
                    : (digit_t*) realloc(this->digits, sizeof(digit_t)*(this->nlen)));
                if (prev < this->nlen && prev != 0) {
                    for (int i = 0; i < this->nlen-prev; i++) {
                        this->digits[this->nlen-1-i] = 0;
                    }
                }
            }

        public:
            big_num() {
                this->sign = false;
                this->resize(1);
                this->digits[0] = 0;
            }

            big_num(__int128_t n) {
                if (n == 0) {
                    this->sign = false;
                    this->resize(1);
                    this->digits[0] = 0;
                    return;
                }

                this->sign = (n < 0);
                n = (this->sign) ? -n : n;
                while (n != 0) {
                    this->resize(1);
                    digits[nlen-1] = n % 10;
                    n /= 10;
                }
            }
            
            big_num(const std::string& s) {
                std::string str = s;
                switch (str[0]) {
                    case '-' : {
                        this->sign = true;
                        str = str.substr(1, str.length());
                        break;
                    }
                    case '+' : {
                        this->sign = false;
                        str = str.substr(1, str.length());
                        break;
                    }
                    default : {
                        this->sign = false;
                    }
                }

                int i;
                for (i = 0; i < str.size()-1 && str[i] == '0'; i++);
                str = str.substr(i);

                this->resize(str.length());

                for (int i = this->nlen-1; i >= 0; i--) {
                    this->digits[i] = (digit_t) (str[this->nlen-1-i] - 48);
                }
            }

            big_num(const big_num& n) {
                *this = n;
            }

            ~big_num() {
                free(this->digits);
            }

            bool get_sign() const {
                return this->sign;
            }

            const digit_t* get_digits() const {
                return this->digits;
            }

            size_t get_nlen() const {
                return this->nlen;
            }

            friend std::ostream& operator << (std::ostream& os, const big_num& n) {
                os << ((n.sign) ? "-" : "+");
                for (int i = n.nlen-1; i >= 0; i--) {
                    os << (unsigned int) n.digits[i];
                }
                return os;
            }

            digit_t operator [] (size_t index) const {
                return this->digits[index];
            }

            big_num& operator = (const big_num& n) {
                if (this->digits != nullptr) {
                    free(this->digits);
                }

                this->digits = (digit_t*) malloc(sizeof(digit_t)*n.nlen);
                this->nlen = n.nlen;
                this->sign = n.sign;

                for (size_t i = 0; i < this->nlen; i++) {
                    this->digits[i] = n.digits[i];
                }

                return *this;
            }

            friend big_num operator + (const big_num& n1, const big_num& n2) {
                big_num greater = big_num((n2.nlen > n1.nlen) ? n2 : (n2.nlen == n1.nlen && n2.absGreater(n1)) ? n2 : n1);
                big_num small = big_num((n2.nlen > n1.nlen) ? n1 : (n2.nlen == n1.nlen && n2.absGreater(n1)) ? n1 : n2);

                bool change = false;
                if (greater.sign && !small.sign) {
                    change = true;
                    greater.sign = !greater.sign;
                    small.sign = !small.sign;
                }

                for (int i = 0; i < small.nlen; i++) {
                    greater.digits[i] += small.digits[i] * (small.sign ? -1 : 1);
                }

                for (int e = 0; e < greater.nlen-1; e++) {
                    if (greater.digits[e] > 9) {
                        greater.digits[e] -= 10;
                        greater.digits[e+1]++;
                    }
                    if (greater.digits[e] < 0) {
                        greater.digits[e] += 10;
                        greater.digits[e+1]--;
                    }
                }

                if (greater.digits[greater.nlen-1] > 9) {
                    greater.resize(1);
                    greater.digits[greater.nlen-2] -= 10;
                    greater.digits[greater.nlen-1]++;
                }
                if (greater.digits[greater.nlen-1] < 0) {
                    greater.digits[greater.nlen-1] *= -1;
                    greater.sign = !greater.sign;
                }

                while (greater.digits[greater.nlen-1] == 0 && greater.nlen != 1) {
                    greater.resize(-1);
                }

                if (change) {
                    greater.sign = !greater.sign;
                    small.sign = !small.sign;
                }

                return greater;
            }

            void operator += (const big_num& n) {
                *this = *this + n;
            }

            big_num& operator ++ () {
                *this += 1;
                return *this;
            }
            big_num operator ++ (int) {
                big_num r = big_num(*this);
                ++*this;
                return r;
            }

            friend big_num operator - (const big_num& n1, const big_num& n2) {
                if (!n1.sign) {
                    if (!n2.sign) {
                        if (n1 > n2) {
                            return n1 + (-n2);
                        }
                        else {
                            return -(n2 + (-n1));
                        }
                    }
                    else {
                        return n1 + (-n2);
                    }
                }
                else {
                    if (!n2.sign) {
                        return -(n2 - n1);
                    }
                    else {
                        if (n1 > n2) {
                            return -n2 + n1;
                        }
                        else {
                            return -(-n1 + n2); 
                        } 
                    }
                }
            }

            void operator -= (const big_num& n) {
                *this = *this - n;
            }

            big_num& operator -- () {
                *this -= 1;
                return *this;
            }
            big_num operator -- (int) {
                big_num r = big_num(*this);
                --*this;
                return r;
            }

            friend big_num operator * (const big_num& n1, const big_num& n2) {
                if (n1 == 0 || n2 == 0) { return 0; }

                bool sign = n1.sign ^ n2.sign;
                
                if (n1 == 1) {
                    return big_num(n2);
                }
                if (n2 == 1) {
                    return big_num(n1);
                }

                const big_num& less = !n1 < !n2 ? !n1 : !n2;
                const big_num& greater = !n1 < !n2 ? !n2 : !n1;
                big_num result = 0;

                for (int i = 0; i < less.nlen; i++) {
                    big_num factor = 0;
                    for (int e = 0; e < less.digits[i]; e++) {
                        factor += greater;
                    }
                    factor <<= i;
                    result += factor;
                }

                result.sign = sign;
                return result;
            }
            
            void operator *= (const big_num& n) {
                *this = *this * n;
            }

            friend big_num operator / (const big_num& n1, const big_num& n2) {
                if (n1 == 0) {
                    return 0;
                } 
                if (n2 == 0) {
                    std::cout << "Illegal division by 0\n";
                    return 0;
                }

                bool sign = n1.sign ^ n2.sign;

                if (!n2 > !n1) { return 0; }
                if (!n2 == !n1) { return 1; }

                big_num result = 0, accumulator;

                for (accumulator = 0; (accumulator + !n2) <= n1; accumulator += !n2) {
                    result++;
                }

                result.sign = sign;
                return result;
            }
            
            void operator /= (const big_num& n) {
                *this = *this / n;
            }

            friend big_num operator ^ (const big_num& n1, const big_num& n2) {
                if (n2 < 0) {
                    std::cout << "Only positive exponents are allowed\n";
                    return 0;
                }

                if (n2 == 0 || n1 == 1) { return 1; }
                if (n1 == 0) { return 0; }

                big_num result = 1;

                for (big_num n = 0; n < n2; n++) {
                    result *= n1;
                }

                return result;
            }

            void operator ^= (const big_num& n2) {
                *this = *this ^ n2;
            }

            friend big_num operator - (const big_num& n) {
                big_num n2 = big_num(n);
                n2.sign = !n2.sign;
                return n2;
            }
            friend big_num operator ! (const big_num& n) {
                return n.sign ? -n : big_num(n);
            }

            friend big_num operator % (const big_num& n1, const big_num n2) {
                if (!n2 > !n2) {
                    return n1;
                }

                if (n2 == 2) {
                    return n1.digits[0] % 2 == 0;
                }

                big_num result = 0, accumulator;

                for (accumulator = 0; (accumulator + !n2) <= n1; accumulator += !n2) {
                    result++;
                }

                return !(n1 - accumulator);
            }

            friend big_num operator << (const big_num& n1, unsigned int diff) {
                if (diff == 0) {
                    return n1;
                }

                big_num n = n1;
                n.resize(diff);

                for (int i = n.nlen-1; i >= diff; i--) {
                    n.digits[i] = n.digits[i-diff];
                }
                for (int i = 0; i < diff; i++) {
                    n.digits[i] = 0;
                }

                return n;
            }

            void operator <<= (unsigned int diff) {
                *this = *this << diff;
            }

            friend big_num operator >> (const big_num& n1, unsigned int diff) {
                big_num n = n1;
                for (int i = diff; i < n.nlen; i++) {
                    n.digits[i-diff] = n.digits[i];
                }
                
                n.resize(-((signed int) diff));
                return n;
            }

            void operator >>= (unsigned int diff) {
                *this = *this >> diff;
            }

            bool operator == (const big_num& n) const {
                if (this->nlen != n.nlen || (this->sign != n.sign &&
                    !(this->nlen == 1 && n.nlen == 1 && n.digits[0] == 0 && this->digits[0] == 0))) {
                    
                    return false;
                }
                for (size_t i = 0; i < this->nlen; i++) {
                    if (this->digits[i] != n[i]) {
                        return false;
                    }
                }

                return true;
            }

            bool operator != (const big_num& n) const {
                return !big_num::operator == (n);
            }

            bool operator >= (const big_num& n) const {
                return big_num::operator == (n) || big_num::operator > (n); 
            }

            bool operator <= (const big_num& n) const {
                return big_num::operator == (n) || big_num::operator < (n);
            }
            
            bool operator > (const big_num& n) const {
                if (!this->sign) {
                    if (!n.sign) {
                        if (this->nlen > n.nlen) {
                            return true;
                        }
                        else if (this->nlen == n.nlen) {
                            for (int e = this->nlen-1; e >= 0; e--) {
                                if (this->digits[e] == n.digits[e]) {
                                    continue;
                                }
                                return this->digits[e] > n.digits[e];
                            }
                        }
                        return false;
                    }
                    return true;
                }
                else {
                    if (!n.sign) {
                        return false;
                    }
                    if (this->nlen > n.nlen) {
                        return false;
                    }
                    else if (this->nlen == n.nlen) {
                        for (int e = 0; e < this->nlen; e++) {
                            for (int e = this->nlen-1; e >= 0; e--) {
                                if (this->digits[e] == n.digits[e]) {
                                    continue;
                                }
                                return this->digits[e] < n.digits[e];
                            }
                        }
                    }
                    return true;
                }
            }

            bool operator < (const big_num& n) const {
                return !big_num::operator > (n) && !big_num::operator == (n);
            }

            bool absGreater(const big_num& n) const {
                if (this->nlen > n.nlen) {
                    return true;
                }
                else if (this->nlen == n.nlen) {
                    for (int e = this->nlen-1; e >= 0; e--) {
                        if (this->digits[e] == n.digits[e]) {
                            continue;
                        }
                        return this->digits[e] > n.digits[e];
                    }
                }
                return false;
            }
    };

    unsigned int closest_2pow(const big_num& num) {
        big_num p = 1;
        unsigned int pow;
        for (pow = 1; p <= num; pow++) {
            p *= 2;
        }

        return pow-1;
    }

    template <size_t size> big_num from_bitset(const bitset<size>& set) {
        big_num n, pow = 1;
        for (int i = 0; i < size; i++) {
            n += (set[i] * pow);
            pow *= 2;
        }

        return n;
    }

    template <size_t size> bitset<size> to_bitset(const big_num& num) {
        bitset<size> set;
        big_num copy(num);
        for (int i = 0; i < size; i++) {
            if ((copy % 2) == 1) {
                set.set(i);
            }
            copy /= 2;
            if (copy == 0) {
                break;
            }
        }

        return set;
    }

#endif