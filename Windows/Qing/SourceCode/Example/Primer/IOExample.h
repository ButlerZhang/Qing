#pragma once



class IOExample
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
