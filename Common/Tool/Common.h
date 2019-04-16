#pragma once



bool IsBigEndian()
{
    union test_union
    {
        short m_value;
        char  union_bytes[sizeof(short)];
    };

    test_union test;
    test.m_value = 0x0102;

    return (test.union_bytes[0] == 1 && test.union_bytes[2] == 2);
}
