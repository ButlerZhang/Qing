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
        bool isEven = (numberCount % 2) == 0;
        int midIndex = numberCount / 2;

        if (nums1.empty()) {
            return !isEven ? nums2[midIndex] : (nums2[midIndex] + nums2[midIndex - 1]) / 2.0;
        }

        if (nums2.empty()) {
            return !isEven ? nums1[midIndex] : (nums1[midIndex] + nums1[midIndex - 1]) / 2.0;
        }

        int start1 = 0, end1 = static_cast<int>(nums1.size());
        int start2 = 0, end2 = static_cast<int>(nums2.size());
        int stepCount = isEven ? (midIndex - 1) : midIndex;

        while (stepCount != 0) {
            if (nums1[start1] <= nums2[start2]) {
                if (++start1 >= end1) {
                    start1 = -1;
                    start2 += stepCount;
                    break;
                }
            }
            else {
                if (++start2 >= end2) {
                    start2 = -1;
                    start1 += stepCount;
                    break;
                }
            }

            --stepCount;
        }

        if (start1 < 0) {
            return isEven ? (nums2[start2 - 1] + nums2[start2]) / 2.0 : nums2[start2];
        }

        if (start2 < 0) {
            return isEven ? (nums1[start1 - 1] + nums1[start1]) / 2.0 : nums1[start1];
        }

        if (isEven) {
            return (nums1[start1] + nums2[start2]) / 2.0;
        }
        else {
            return nums1[start1] <= nums2[start2] ? nums1[start1] : nums2[start2];
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
            assert(result >= 1.99999 && result < 2.00001);
        }

        //Input: nums1 = [1,2], nums2 = [3,4]
        //Output: 2.50000
        //Explanation : merged array = [1, 2, 3, 4] and median is(2 + 3) / 2 = 2.5.
        {
            std::vector<int> nums1 = { 1, 2 };
            std::vector<int> nums2 = { 3, 4 };
            double result = test.findMedianSortedArrays(nums1, nums2);
            assert(result >= 2.44449 && result < 2.50001);
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
    }
};

LEET_CODE_NS_END