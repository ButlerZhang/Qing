#pragma once
#include <iostream>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class ReverseInteger {
public:

    int reverse(int x) {
        const int base = 10;

        const int MAX_LIMIT = INT32_MAX / base;
        const int MIN_LIMIT = INT32_MIN / base;

        const int MIN_POP = INT32_MAX % base;
        const int MAX_POP = INT32_MIN % base;

        int result = 0;
        while (x != 0) {
            int pop = x % base;

            if (result > MAX_LIMIT || (result == MAX_LIMIT && pop > MIN_POP)) return 0;
            if (result < MIN_LIMIT || (result == MIN_LIMIT && pop < MAX_POP)) return 0;

            result = result * base + pop;
            x /= base;
        }

        return result;
    }

    static void testCase() {
        std::cout << "Max int_32 =" << INT32_MAX << std::endl;  //2147483647
        std::cout << "Min int_32 =" << INT32_MIN << std::endl;  //-2147483648
        std::cout << std::endl;

        ReverseInteger test;

        assert(test.reverse(0) == 0);
        assert(test.reverse(123) == 321);
        assert(test.reverse(-123) == -321);
        assert(test.reverse(120) == 21);
        assert(test.reverse(-120) == -21);
        assert(test.reverse(INT32_MAX) == 0);
        assert(test.reverse(INT32_MIN) == 0);

        assert(test.reverse(1463847412) == 2147483641);
        assert(test.reverse(1534236469) == 0);
        assert(test.reverse(1473847412) == 0);
        assert(test.reverse(1464847412) == 0);
        assert(test.reverse(1463947412) == 0);
        assert(test.reverse(1463857412) == 0);
        assert(test.reverse(1463848412) == 0);
        assert(test.reverse(1463847512) == 0);
        assert(test.reverse(1463847422) == 0);
        assert(test.reverse(1463847413) == 0);

        assert(test.reverse(-1463847412) == -2147483641);
        assert(test.reverse(-1534236469) == 0);
        assert(test.reverse(-1473847412) == 0);
        assert(test.reverse(-1464847412) == 0);
        assert(test.reverse(-1463947412) == 0);
        assert(test.reverse(-1463857412) == 0);
        assert(test.reverse(-1463848412) == 0);
        assert(test.reverse(-1463847512) == 0);
        assert(test.reverse(-1463847422) == 0);
        assert(test.reverse(-1463847413) == 0);

        assert(test.reverse(2147483640) == 463847412);
        assert(test.reverse(-2147483643) == 0);
        assert(test.reverse(-2147483641) == -1463847412);
    }
};

LEET_CODE_NS_END