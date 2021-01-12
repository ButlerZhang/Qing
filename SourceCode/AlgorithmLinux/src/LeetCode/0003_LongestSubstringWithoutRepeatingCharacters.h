#pragma once
#include <string>
#include <assert.h>



namespace LC_LongestSubstringWithoutRepeatingCharacters {
    using namespace std;

    class Solution {
    public:
        int lengthOfLongestSubstring(string s) {
            int resultLength = 0;
            int tempMaxLength = 0;
            const int stringLength = static_cast<int>(s.size());

            for (int i = 0, j = 0; i < stringLength; i++) {
                if (stringLength - i <= resultLength) {
                    break;
                }

                for (; j < stringLength; j++) {
                    for (int k = i; k < j; k++) {
                        if (s[k] == s[j]) {
                            tempMaxLength = j - i;
                            if (tempMaxLength > resultLength) {
                                resultLength = tempMaxLength;
                            }

                            break;
                        }
                    }

                    if (tempMaxLength > 0) {
                        tempMaxLength = 0;
                        break;
                    }
                }

                if (j == stringLength) {
                    tempMaxLength = j - i;
                    if (tempMaxLength > resultLength) {
                        resultLength = tempMaxLength;
                    }
                }
            }

            return resultLength;
        }
    };

    void testCase() {
        Solution test;

        assert(test.lengthOfLongestSubstring("abcabcbb") == 3);
        assert(test.lengthOfLongestSubstring("bbbbb") == 1);
        assert(test.lengthOfLongestSubstring("pwwkew") == 3);
        assert(test.lengthOfLongestSubstring("") == 0);
        assert(test.lengthOfLongestSubstring("dvdf") == 3);

        assert(test.lengthOfLongestSubstring("     ") == 1);
        assert(test.lengthOfLongestSubstring("abc12345") == 8);
        assert(test.lengthOfLongestSubstring("abc abc 123 45") == 7);
        assert(test.lengthOfLongestSubstring("abcdefgabcdefghijklmn") == 14);
    }
}
