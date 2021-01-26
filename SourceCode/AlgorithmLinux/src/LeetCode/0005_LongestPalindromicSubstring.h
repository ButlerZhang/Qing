#pragma once
#include <string>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class LongestPalindromicSubstring {
public:

    string longestPalindrome(string s) {
        int stringLength = static_cast<int>(s.size());
        int resultStartIndex = 0, resultLong = 0;

        int startIndex = 0;
        while (stringLength - startIndex > resultLong) {
            int i = startIndex;
            int endIndex = stringLength - 1;
            int j = endIndex;
            int tempLong = 0;

            while (i < j) {
                if (s[i] == s[j--]) {
                    ++i;
                    tempLong += 2;
                    continue;
                }

                if (tempLong > 0) {
                    tempLong = 0;
                    i = startIndex;
                    j = (--endIndex);
                }
            }

            if (i == j) {
                ++tempLong;
            }

            if (tempLong > resultLong) {
                resultLong = tempLong;
                resultStartIndex = startIndex;
            }

            ++startIndex;
        }

        return s.substr(resultStartIndex, resultLong);
    }

    static void testCase() {
        LongestPalindromicSubstring test;

        assert(test.longestPalindrome("babad") == "bab");
        assert(test.longestPalindrome("cbbd") == "bb");
        assert(test.longestPalindrome("a") == "a");
        assert(test.longestPalindrome("ac") == "a");

        //error
        assert(test.longestPalindrome("aacabdkacaa") == "aca");

        assert(test.longestPalindrome("aca") == "aca");
        assert(test.longestPalindrome("aa") == "aa");
        assert(test.longestPalindrome("abab") == "aba");
        assert(test.longestPalindrome("aab") == "aa");
        assert(test.longestPalindrome("ghig") == "g");
        assert(test.longestPalindrome("babab") == "babab");
        assert(test.longestPalindrome("aaaaaaaaaa") == "aaaaaaaaaa");
        assert(test.longestPalindrome("abcdefghijklmnopqrstuvwxyz,.") == "a");
        assert(test.longestPalindrome("abcdefghigklmnopqrstuvwxyz,.") == "a");
    }
};

LEET_CODE_NS_END