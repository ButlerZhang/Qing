#include "001_MaxSubSum.h"
#include <iostream>
#include <algorithm>



int MaxSubSum::MaxSubSumTripleCycle(const std::vector<int> &Array) const
{
    int MaxSum = 0;

    for (std::vector<int>::size_type i = 0; i < Array.size(); i++)
    {
        for (std::vector<int>::size_type j = i + 1; j < Array.size(); j++)
        {
            int CurrentSum = 0;

            for (std::vector<int>::size_type k = i; k < j; k++)
            {
                CurrentSum += Array[k];
            }

            if (CurrentSum > MaxSum)
            {
                MaxSum = CurrentSum;
            }
        }
    }

    return MaxSum;
}

int MaxSubSum::MaxSubSumTwoCycle(const std::vector<int> &Array) const
{
    int MaxSum = 0;

    for (std::vector<int>::size_type i = 0; i < Array.size(); i++)
    {
        int CurrentSum = 0;
        for (std::vector<int>::size_type j = i; j < Array.size(); j++)
        {
            CurrentSum += Array[j];
            if (CurrentSum > MaxSum)
            {
                MaxSum = CurrentSum;
            }
        }
    }

    return MaxSum;
}

int MaxSubSum::MaxSubSumRecursive(const std::vector<int> &Array) const
{
    return MaxSubSumRec(Array, 0, Array.size() - 1);
}

int MaxSubSum::MaxSubSumSingleCycle(const std::vector<int> &Array) const
{
    int MaxSum = 0, CurrentSum = 0;

    for (std::vector<int>::size_type i = 0; i < Array.size(); i++)
    {
        CurrentSum += Array[i];

        if (CurrentSum > MaxSum)
        {
            MaxSum = CurrentSum;
        }
        else if (CurrentSum < 0)
        {
            CurrentSum = 0;
        }
    }

    return MaxSum;
}

int MaxSubSum::MaxSubSumRec(const std::vector<int> &Array, int Left, int Right) const
{
    if (Left == Right)   //base case
    {
        return (Array[Left] > 0) ? Array[Left] : 0;
    }

    int Center = (Left + Right) / 2;
    int MaxLeftSum = MaxSubSumRec(Array, Left, Center);
    int MaxRightSum = MaxSubSumRec(Array, Center + 1, Right);

    int MaxLeftBorderSum = 0, LeftBorderSum = 0;
    for (int i = Center; i >= Left; i--)
    {
        LeftBorderSum += Array[i];
        if (LeftBorderSum > MaxLeftBorderSum)
        {
            MaxLeftBorderSum = LeftBorderSum;
        }
    }

    int MaxRightBorderSum = 0, RightBorderSum = 0;
    for (int j = Center + 1; j <= Right; j++)
    {
        RightBorderSum += Array[j];
        if (RightBorderSum > MaxRightBorderSum)
        {
            MaxRightBorderSum = RightBorderSum;
        }
    }

    int TempMax = std::max(MaxLeftSum, MaxRightSum);
    return std::max(TempMax, MaxLeftBorderSum + MaxRightBorderSum);
}

void MaxSubSum::Test() const
{
    std::vector<int> Array = { -2, 11, -4, 13, -5, -2 };

    std::cout << MaxSubSumTripleCycle(Array) << std::endl;
    std::cout << MaxSubSumTwoCycle(Array) << std::endl;
    std::cout << MaxSubSumRecursive(Array) << std::endl;
    std::cout << MaxSubSumSingleCycle(Array) << std::endl;
}
