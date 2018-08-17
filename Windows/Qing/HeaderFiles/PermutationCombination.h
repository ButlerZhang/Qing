#pragma once
#include "QingBase.h"
#include <vector>
#include <bitset>

QING_NAMESPACE_BEGIN



template<typename T, unsigned int ElementCount>
class QING_DLL PermutationCombination
{
public:

    PermutationCombination(void) {}
    ~PermutationCombination(void) {}

    bool GetCombination(const std::vector<T> &SourceVector, int CombinationElementCount, std::vector<std::vector<T> > &ResultVector);
    bool GetPermutation(const std::vector<T> &SourceVector, int PermutationElementCount, std::vector<std::vector<T> > &ResultVector);

private:

    bool AdjustPosition(std::bitset<ElementCount> &FlagArray, unsigned int SourceVectorElementCount);
};



template<typename T, unsigned int ElementCount>
bool PermutationCombination<T, ElementCount>::GetCombination(const std::vector<T> &SourceVector, int CombinationElementCount, std::vector<std::vector<T> >& ResultVector)
{
    if (SourceVector.size() <= 0 || CombinationElementCount <= 0 || CombinationElementCount > static_cast<int>(SourceVector.size()))
    {
        return false;
    }

    std::bitset<ElementCount> FlagArray;
    for (int Index = 0; Index < CombinationElementCount; Index++)
    {
        FlagArray.set(Index);
    }

    do
    {
        ResultVector.push_back(std::vector<T>(CombinationElementCount, T()));
        std::vector<T> &TempArray = ResultVector[ResultVector.size() - 1];

        std::vector<T>::size_type TempIndex = 0;
        for (std::size_t BitSetIndex = 0; BitSetIndex < FlagArray.size(); BitSetIndex++)
        {
            if (FlagArray[BitSetIndex])
            {
                TempArray[TempIndex] = SourceVector[BitSetIndex];
                if (++TempIndex == CombinationElementCount)
                {
                    break;
                }
            }
        }
    } while (AdjustPosition(FlagArray, SourceVector.size()));

    return true;
}

template<typename T, unsigned int ElementCount>
bool PermutationCombination<T, ElementCount>::GetPermutation(const std::vector<T> &SourceVector, int PermutationElementCount, std::vector<std::vector<T> >& ResultVector)
{
    if (SourceVector.size() <= 0 || PermutationElementCount <= 0 || PermutationElementCount > static_cast<int>(SourceVector.size()))
    {
        return false;
    }

    std::bitset<ElementCount> FlagArray;
    for (int Index = 0; Index < PermutationElementCount; Index++)
    {
        FlagArray.set(Index);
    }

    std::vector<T> TempArray(PermutationElementCount, T());

    do
    {
        std::vector<T>::size_type TempIndex = 0;
        for (std::size_t BitSetIndex = 0; BitSetIndex < FlagArray.size(); BitSetIndex++)
        {
            if (FlagArray[BitSetIndex])
            {
                TempArray[TempIndex] = SourceVector[BitSetIndex];
                if (++TempIndex == PermutationElementCount)
                {
                    do
                    {
                        ResultVector.push_back(TempArray);
                    } while (next_permutation(TempArray.begin(), TempArray.end()));

                    break;
                }
            }
        }
    } while (AdjustPosition(FlagArray, SourceVector.size()));

    return true;
}

//*********************** 0-1 exchange algorithm ************************
//  example : C(5,3)
//      1   1   1   0   0
//      1   1   0   1   0
//      1   0   1   1   0
//      0   1   1   1   0
//      1   1   0   0   1
//      1   0   1   0   1
//      0   1   1   0   1
//      1   0   0   1   1
//      0   1   0   1   1
//      0   0   1   1   1
//*********************** 0-1 exchange algorithm ************************
template<typename T, unsigned int ElementCount>
bool PermutationCombination<T, ElementCount>::AdjustPosition(std::bitset<ElementCount> &FlagArray, unsigned int SourceVectorElementCount)
{
    SourceVectorElementCount -= 1;
    for (unsigned int i = 0; i < SourceVectorElementCount; i++)
    {
        if (FlagArray[i] && !FlagArray[i + 1])
        {
            FlagArray.reset(i);
            FlagArray.set(i + 1);

            if (!FlagArray[0] && i != 0)
            {
                int TrueCount = 0;
                for (unsigned int j = 0; j < i; j++)
                {
                    if (FlagArray[j])
                    {
                        ++TrueCount;
                    }

                    FlagArray.reset(j);
                }

                for (int k = 0; k < TrueCount; k++)
                {
                    FlagArray.set(k);
                }
            }

            return true;
        }
    }

    return false;
}

QING_NAMESPACE_END