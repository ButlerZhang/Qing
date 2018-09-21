#pragma once
#include "..\HeaderFiles\QingBase.h"

QING_EXAMPLE_BEGIN



class QING_DLL IOExample
{
public:

    IOExample();
    ~IOExample();

    //formatted IO
    void TestBoolapha();
    void TestIntDecimal();
    void TestShowbase();
    void TestUppercase();
    void TestPrecision();
    void TestDoubleCounting();
    void TestShowpoint();
    void TestSpace();
    void TestNoSkipws();

    //unformatted IO
    void TestGetPut();
    void TestEOF();

    //seek and tell
    int TestSeekTell();
};

QING_EXAMPLE_END