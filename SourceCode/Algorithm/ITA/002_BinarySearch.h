#pragma once
#include <vector>
#include <iostream>
#include "ITACommon.h"

ITA_NS_BEGIN

class BinarySearch {
public:

    /***************************************************************
     * Summary: binary search, O(logN).
     * Parameters: int subsequence.
     * Return: array index.
    ****************************************************************/
    int GetBinarySearch(const std::vector<int> &Array, int TargetValue) const {
        std::vector<int>::size_type Left = 0;
        std::vector<int>::size_type Right = Array.size();
        while (Left < Right) {
            std::vector<int>::size_type mid = (Left + Right) / 2;
            if (Array[mid] == TargetValue) {
                return static_cast<int>(mid);
            }

            if (Array[mid] > TargetValue) {
                Right = mid - 1;
            }
            else {
                Left = mid + 1;
            }
        }

        return -1;
    }

    static void testCase() {
        BinarySearch test;

        std::vector<int> Array = { 1,2,3,4,5,6,7,8,9,10 };
        std::cout << test.GetBinarySearch(Array, 11) << std::endl;
        std::cout << test.GetBinarySearch(Array, 1) << std::endl;
        std::cout << test.GetBinarySearch(Array, 9) << std::endl;
        std::cout << test.GetBinarySearch(Array, 10) << std::endl;
    }
};

ITA_NS_END