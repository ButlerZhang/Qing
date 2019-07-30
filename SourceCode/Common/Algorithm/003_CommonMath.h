#pragma once



class CommonMath
{
public:

    /***************************************************************
     * Summary: greatest common divisor, Euclidean algorithm, O(logN).
     * Parameters: long.
     * Return: greatest common divisor.
    ****************************************************************/
    long GreatestCommonDivisor(long m, long n) const;

    /***************************************************************
     * Summary: Integer power operation, O(logN).
     * Parameters: n is power.
     * Return: long result.
    ****************************************************************/
    long PowRecursive(long x, int n) const;
};