#pragma once
#include <vector>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class MedianOfTwoSortedArrays {
public:

    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        if (nums1.empty() && nums2.empty()) {
            return 0.0;
        }

        int numberCount = static_cast<int>(nums1.size() + nums2.size());
        int stepCount = numberCount / 2 + 1;
        int result1 = 0, result2 = 0;
        int start1 = 0, start2 = 0;

        while (start1 + start2 < stepCount) {
            result2 = result1;

            if (start1 == nums1.size()) {
                result1 = nums2[start2++];
                continue;
            }

            if (start2 == nums2.size()) {
                result1 = nums1[start1++];
                continue;
            }

            result1 = nums1[start1] <= nums2[start2] ? nums1[start1++] : nums2[start2++];
        }

        if (numberCount % 2 == 0) {
            return (result1 + result2) / 2.0;
        }
        else {
            return result1 > result2 ? result1 : result2;
        }
    }

    static void testCase() {
        MedianOfTwoSortedArrays test;

        //Input: nums1 = [1, 3], nums2 = [2]
        //Output : 2.00000
        //Explanation : merged array = [1, 2, 3] and median is 2.
        {
            std::vector<int> nums1 = { 1, 3 };
            std::vector<int> nums2 = { 2 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 2.0);
        }

        //Input: nums1 = [1,2], nums2 = [3,4]
        //Output: 2.50000
        //Explanation : merged array = [1, 2, 3, 4] and median is(2 + 3) / 2 = 2.5.
        {
            std::vector<int> nums1 = { 1, 2 };
            std::vector<int> nums2 = { 3, 4 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 2.5);
        }

        //Input: nums1 = [0,0], nums2 = [0,0]
        //Output: 0.00000
        {
            std::vector<int> nums1 = { 0, 0 };
            std::vector<int> nums2 = { 0, 0 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 0.0);
        }

        //Input: nums1 = [], nums2 = [1]
        //Output: 1.00000
        {
            std::vector<int> nums1;
            std::vector<int> nums2 = { 1 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 1.0);
        }

        //Input: nums1 = [2], nums2 = []
        //Output: 2.00000
        {
            std::vector<int> nums1 = { 2 };
            std::vector<int> nums2;
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 2.0);
        }

        //error
        {
            std::vector<int> nums1 = { 1, 2 };
            std::vector<int> nums2 = {-1, 3 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 1.5);
        }

        //error
        {
            std::vector<int> nums1 = { -1, 3 };
            std::vector<int> nums2 = { 1, 2 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 1.5);
        }

        {
            std::vector<int> nums1 = { 1, 2 };
            std::vector<int> nums2 = { -1, 3, 5 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 2.0);
        }

        {
            std::vector<int> nums1 = { 1,2,7,10,92 };
            std::vector<int> nums2 = { 2,7,20,70,91,99 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 10.0);
        }

        {
            std::vector<int> nums1 = { 1,2,7,10,92,101 };
            std::vector<int> nums2 = { 2,7,20,70,91,99 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 15.0);
        }

        {
            std::vector<int> nums1 = { 1,2 };
            std::vector<int> nums2 = { 7,20,70,91,99,100,102,105 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 80.5);
        }

        {
            std::vector<int> nums1 = { 7,20,70,91,99,100,102,105 };
            std::vector<int> nums2 = { 1,2 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 80.5);
        }

        {
            std::vector<int> nums1 = { 7,20,70,91,99,100,102,105 };
            std::vector<int> nums2 = { 106 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 99.0);
        }

        {
            std::vector<int> nums1 = { 106 };
            std::vector<int> nums2 = { 7,20,70,91,99,100,102,105 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 99.0);
        }

        {
            std::vector<int> nums1;
            std::vector<int> nums2;
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result == 0.0);
        }
    }
};

LEET_CODE_NS_END