#pragma once
#include <vector>



class BinarySearch
{
public:

    /***************************************************************
     * Summary: binary search, O(logN).
     * Parameters: int subsequence.
     * Return: array index.
    ****************************************************************/
    int GetBinarySearch(const std::vector<int> &Array, int TaregtValue) const;
};