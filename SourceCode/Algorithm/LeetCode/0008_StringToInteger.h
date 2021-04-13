#pragma once
#include <iostream>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class StringToInteger{
public:

    int myAtoi(string s) {
        int result = 0;

        std::string::size_type start = 0;
        while (start < s.size() && s[start] == ' ') {
            start++;
        }

        if (start >= s.size()) {
            return result;
        }

        bool isPositive = true;
        if (s[start] == '+') {
            start++;
        }
        else if (s[start] == '-') {
            isPositive = false;
            start++;
        }

        if (start >= s.size()) {
            return result;
        }

        int single = 0;
        const int base = 10;

        if (isPositive) {
            const int MIN_POP = INT32_MAX % base;
            const int MAX_LIMIT = INT32_MAX / base;
            for (std::string::size_type index = start; index < s.size(); index++) {
                if (s[index] < '0' || s[index] > '9') {
                    break;
                }

                single = s[index] - '0';
                if (result > MAX_LIMIT || (result == MAX_LIMIT && single > MIN_POP)) {
                    result = INT32_MAX;
                    break;
                }

                result = result * base + single;
            }
        }
        else {
            const int MAX_POP = INT32_MIN % base;
            const int MIN_LIMIT = INT32_MIN / base;
            for (std::string::size_type index = start; index < s.size(); index++) {
                if (s[index] < '0' || s[index] > '9') {
                    break;
                }

                single = s[index] - '0';
                if (result < MIN_LIMIT || (result == MIN_LIMIT && (-single) < MAX_POP)) {
                    result = INT32_MIN;
                    break;
                }

                result = result * base - single;
            }
        }

        return result;
    }

    static void testCase() {
        StringToInteger test;

        assert(test.myAtoi("42") == 42);
        assert(test.myAtoi("   -42") == -42);
        assert(test.myAtoi("4193 with words") == 4193);
        assert(test.myAtoi("words and 987") == 0);
        assert(test.myAtoi("-91283472332") == INT32_MIN);

        assert(test.myAtoi("     ") == 0);
        assert(test.myAtoi("+") == 0);
        assert(test.myAtoi("-") == 0);
        assert(test.myAtoi("+42") == 42);
        assert(test.myAtoi(".42") == 0);
        assert(test.myAtoi("1.42") == 1);
        assert(test.myAtoi("0022") == 22);
        assert(test.myAtoi("192039591283472332") == INT32_MAX);
        assert(test.myAtoi("hello") == 0);
        assert(test.myAtoi("-2147483649") == INT32_MIN); //error case
        assert(test.myAtoi("-2147483648") == INT32_MIN);
        assert(test.myAtoi("2147483647") == INT32_MAX);
        assert(test.myAtoi("2147483648") == INT32_MAX);
    }
};

LEET_CODE_NS_END