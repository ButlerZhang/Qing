#pragma once
#include <iostream>
#include "ITACommon.h"

ITA_NS_BEGIN

class CommonMath {
public:

    /***************************************************************
     * Summary: greatest common divisor, Euclidean algorithm, O(logN).
     * Parameters: long.
     * Return: greatest common divisor.
    ****************************************************************/
    long GreatestCommonDivisor(long m, long n) const {
        while (n != 0) {
            long rem = m % n;
            m = n;
            n = rem;
        }

        return m;
    }

    /***************************************************************
     * Summary: Integer power operation, O(logN).
     * Parameters: n is power.
     * Return: long result.
    ****************************************************************/
    long PowRecursive(long x, int n) const {
        if (n == 0) {
            return 1;
        }

        if (n % 2 == 0) {
            return PowRecursive(x * x, n / 2);
        }
        else {
            return PowRecursive(x * x, n / 2) * x;
        }
    }

    static void testCase() {
        CommonMath test;
        std::cout << test.GreatestCommonDivisor(50, 15) << std::endl;
        std::cout << test.GreatestCommonDivisor(15, 50) << std::endl << std::endl;

        std::cout << test.PowRecursive(2, 10) << std::endl;
        std::cout << test.PowRecursive(3, 2) << std::endl;
    }
};

ITA_NS_END