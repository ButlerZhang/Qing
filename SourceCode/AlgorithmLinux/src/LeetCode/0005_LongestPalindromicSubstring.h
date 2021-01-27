#pragma once
#include <string>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class LongestPalindromicSubstring {
public:

    string longestPalindrome(string s) {
        //return bruteForceSearch(s);
        return dynamicProgramming(s);
    }

    string dynamicProgramming(string s) {
        int resultStartIndex = 0, resultLong = 0;
        int stringLength = static_cast<int>(s.size());
        std::vector<std::vector<bool>> dp(stringLength, std::vector<bool>(stringLength));

        for (int len = 0; len < stringLength; len++) {
            for (int i = 0; (i + len) < stringLength; i++) {
                int j = i + len;
                if (len == 0) {
                    dp[i][i] = true;
                }
                else if (len == 1) {
                    dp[i][j] = s[i] == s[j];
                }
                else {
                    dp[i][j] = (s[i] == s[j]) && dp[i + 1][j - 1];
                }

                if (dp[i][j] && len + 1 > resultLong) {
                    resultLong = len + 1;
                    resultStartIndex = i;
                }
            }
        }

        return s.substr(resultStartIndex, resultLong);
    }

    string bruteForceSearch(string s) {
        int stringLength = static_cast<int>(s.size());
        int resultStartIndex = 0, resultLong = 0;

        for (int i = 0; i < stringLength - resultLong; i++) {
            for (int j = i; j < stringLength; j++) {
                int tempLong = j - i + 1;
                if (tempLong > resultLong && isPalindrome(s, i, j)) {
                    resultStartIndex = i;
                    resultLong = tempLong;
                }
            }
        }

        return s.substr(resultStartIndex, resultLong);
    }

    bool isPalindrome(const std::string &s, int start, int stop) {
        for (int i = start, j = stop; i <= j; i++, j--) {
            if (s[i] != s[j]) {
                return false;
            }
        }

        return true;
    }

    static void testCase() {
        LongestPalindromicSubstring test;

        //error
        assert(test.longestPalindrome("aacabdkacaa") == "aca");

        assert(test.longestPalindrome("babad") == "bab");
        assert(test.longestPalindrome("cbbd") == "bb");
        assert(test.longestPalindrome("a") == "a");
        assert(test.longestPalindrome("ac") == "a");

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