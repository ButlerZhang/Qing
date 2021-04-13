#pragma once
#include <vector>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class PalindromeNumber {
public:

    bool isPalindrome(int x) {
        if (x < 0) {
            return false;
        }

        int base = 10;
        if (x < base) {
            return true;
        }

        std::vector<int> numberVector;
        while (x > 0) {
            numberVector.push_back(x % base);
            x = x / base;
        }

        std::vector<int>::size_type start = 0;
        std::vector<int>::size_type stop = numberVector.size() - 1;
        while (start < stop) {
            if (numberVector[start++] != numberVector[stop--]) {
                return false;
            }
        }

        return true;
    }

    static void testCase() {
        PalindromeNumber test;

        assert(test.isPalindrome(121) == true);
        assert(test.isPalindrome(-121) == false);
        assert(test.isPalindrome(10) == false);
        assert(test.isPalindrome(-101) == false);
        assert(test.isPalindrome(123) == false);
        assert(test.isPalindrome(1) == true);
        assert(test.isPalindrome(0) == true);
        assert(test.isPalindrome(111111111) == true);
        assert(test.isPalindrome(1234567899) == false);
    }
};

LEET_CODE_NS_END