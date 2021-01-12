#pragma once
#include <string>
#include <algorithm>
#include <assert.h>



namespace LC_LongestSubstringWithoutRepeatingCharacters {
    using namespace std;

    class Solution {
    public:
        int lengthOfLongestSubstring(string s) {
            int result = 0;
            const int stringLength = static_cast<int>(s.size());

            int start = 0, end = 0;
            while (stringLength - start > result) {
                for (int index = start; index < end; index++) {
                    if (s[index] == s[end]) {
                        result = std::max(result, end - start);
                        start = index + 1;
                        break;
                    }
                }

                if (++end >= stringLength) {
                    result = std::max(result, end - start);
                    break;
                }
            }

            return result;
        }
    };

    void testCase() {
        Solution test;

        assert(test.lengthOfLongestSubstring("abcabcbb") == 3);
        assert(test.lengthOfLongestSubstring("bbbbb") == 1);
        assert(test.lengthOfLongestSubstring("pwwkew") == 3);
        assert(test.lengthOfLongestSubstring("") == 0);
        assert(test.lengthOfLongestSubstring("dvdf") == 3);

        assert(test.lengthOfLongestSubstring("1") == 1);
        assert(test.lengthOfLongestSubstring("     ") == 1);
        assert(test.lengthOfLongestSubstring("abc12345") == 8);
        assert(test.lengthOfLongestSubstring("abc abc 123 45") == 7);
        assert(test.lengthOfLongestSubstring("abcdefgabcdefghijklmn") == 14);
    }
}
