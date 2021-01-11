#pragma once
#include <vector>
#include <assert.h>
#include <unordered_map>



namespace LC_TwoSum {

    using namespace std;

    class Solution {
    public:
        vector<int> twoSum(vector<int>& nums, int target) {
            std::vector<int> result;
            if (nums.size() < 2) {
                return result;
            }

            std::unordered_map<int, int> minusMap;
            for (std::vector<int>::size_type index = 0; index < nums.size(); index++) {
                int difference = target - nums[index];

                std::unordered_map<int, int>::const_iterator it = minusMap.find(difference);
                if (it == minusMap.end()) {
                    minusMap[nums[index]] = static_cast<int>(index);
                    continue;
                }

                result.push_back(minusMap[difference]);
                result.push_back(static_cast<int>(index));
                break;
            }

            return result;
        }
    };

    void testCase() {
        Solution test;

        std::vector<int> case1 = { 2, 7, 11, 15 };
        const std::vector<int> &result1 = test.twoSum(case1, 9);
        assert(result1[0] == 0 && result1[1] == 1);

        std::vector<int> case2 = { 3, 2, 4 };
        const std::vector<int> &result2 = test.twoSum(case2, 6);
        assert(result2[0] == 1 && result2[1] == 2);

        std::vector<int> case3 = { 3, 3 };
        const std::vector<int> &result3 = test.twoSum(case3, 6);
        assert(result3[0] == 0 && result3[1] == 1);

        std::vector<int> case4;
        const std::vector<int> &result4 = test.twoSum(case4, 6);
        assert(result4.size() == 0);

        std::vector<int> case5 = { 3, 3, 2, 1, 6 };
        const std::vector<int> &result5 = test.twoSum(case5, 2);
        assert(result5.size() == 0);
    }
}
