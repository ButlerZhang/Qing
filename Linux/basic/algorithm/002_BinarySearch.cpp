#include "002_BinarySearch.h"



int BinarySearch::GetBinarySearch(const std::vector<int> &Array, int TargetValue) const
{
    std::vector<int>::size_type Left = 0;
    std::vector<int>::size_type Right = Array.size();
    while (Left < Right)
    {
        std::vector<int>::size_type mid = (Left + Right) / 2;
        if (Array[mid] == TargetValue)
        {
            return static_cast<int>(mid);
        }

        if (Array[mid] > TargetValue)
        {
            Right = mid - 1;
        }
        else
        {
            Left = mid + 1;
        }
    }

    return -1;
}