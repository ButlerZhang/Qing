#include "003_CommonMath.h"



long CommonMath::GreatestCommonDivisor(long m, long n) const
{
    while(n != 0)
    {
        long rem = m % n;
        m = n;
        n = rem;
    }

    return m;
}

long CommonMath::PowRecursive(long x, int n) const
{
    if(n == 0)
    {
        return 1;
    }

    if(n % 2 == 0)
    {
        return PowRecursive(x * x, n / 2);
    }
    else
    {
        return PowRecursive(x * x, n / 2) * x;
    }
}
