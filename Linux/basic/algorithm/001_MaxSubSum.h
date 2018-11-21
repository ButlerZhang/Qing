#pragma once
#include <vector>



class MaxSubSum
{
public:

    /***************************************************************
     * Summary: Triple cycle, O(N^3).
     * Parameters: int array.
     * Return: max sub sum.
    ****************************************************************/
    int MaxSubSumTripleCycle(const std::vector<int> &Array) const;

    /***************************************************************
     * Summary: Two cycle, O(N^2).
     * Parameters: int array.
     * Return: max sub sum.
    ****************************************************************/
    int MaxSubSumTwoCycle(const std::vector<int> &Array) const;

    /***************************************************************
     * Summary: Recursive, O(NlgN).
     * Parameters: int array.
     * Return: max sub sum.
    ****************************************************************/
    int MaxSubSumRecursive(const std::vector<int> &Array) const;

    /***************************************************************
     * Summary: Single cycle, O(N).
     * Parameters: int array.
     * Return: max sub sum.
    ****************************************************************/
    int MaxSubSumSingleCycle(const std::vector<int> &Array) const;

private:

    /***************************************************************
     * Summary: Recursive, O(NlgN).
     * Parameters: int array.
     * Return: max sub sum.
    ****************************************************************/
    int MaxSubSumRec(const std::vector<int> &Array, int Left, int Right) const;
};