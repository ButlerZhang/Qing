#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>



class IOExample
{
public:

    IOExample() {}
    ~IOExample() {}

    //////////////////////////////////////////////////////////////////////////////
    //formatted IO
    //////////////////////////////////////////////////////////////////////////////
    void TestBoolapha()
    {
        std::cout << "default bool values: " << true << "," << false << std::endl;

        std::cout << std::boolalpha;
        std::cout << "alapha bool values: " << true << "," << false << std::endl;

        std::cout << std::noboolalpha;
    }

    void TestIntDecimal()
    {
        std::cout << "default: " << 20 << "," << 1024 << std::endl;
        std::cout << "octal: " << std::oct << 20 << "," << 1024 << std::endl;
        std::cout << "hex: " << std::hex << 20 << "," << 1024 << std::endl;
        std::cout << "decimal: " << std::dec << 20 << "," << 1024 << std::endl;
    }

    void TestShowbase()
    {
        std::cout << std::showbase;
        std::cout << "default: " << 20 << "," << 1024 << std::endl;
        std::cout << "in octal: " << std::oct << 20 << "," << 1024 << std::endl;
        std::cout << "in hex: " << std::hex << 20 << "," << 1024 << std::endl;
        std::cout << "in decimal: " << std::dec << 20 << "," << 1024 << std::endl;
        std::cout << std::noshowbase;
    }

    void TestUppercase()
    {
        std::cout << std::uppercase << std::showbase << std::hex;
        std::cout << "in hexadecimal: " << 20 << "," << 1024 << std::endl;
        std::cout << std::nouppercase << std::noshowbase << std::dec;
    }

    void TestPrecision()
    {
        std::cout << "Precision: " << std::cout.precision()
            << ", Value: " << std::sqrt(2.0) << std::endl;

        std::cout.precision(12);
        std::cout << "Precision: " << std::cout.precision()
            << ", Value: " << std::sqrt(2.0) << std::endl;

        std::cout << std::setprecision(3); //iomanip
        std::cout << "Precision: " << std::cout.precision()
            << ", Value: " << std::sqrt(2.0) << std::endl;
    }

    void TestDoubleCounting()
    {
        std::cout << "default format: " << 100 * std::sqrt(2.0) << '\n'
            << "scientific: " << std::scientific << 100 * std::sqrt(2.0) << '\n'
            << "fixed decimal: " << std::fixed << 100 * std::sqrt(2.0) << '\n'
            << "hexadecimal: " << std::hexfloat << 100 * std::sqrt(2.0) << '\n'
            << "use defaults: " << std::defaultfloat << 100 * std::sqrt(2.0) << '\n';
    }

    void TestShowpoint()
    {
        std::cout << 10.0 << std::endl;
        std::cout << std::showpoint << 10.0 << std::noshowpoint << std::endl;
    }

    void TestSpace()
    {
        int i = -16;
        double d = 3.14159;

        std::cout << "i: " << std::setw(12) << i << "next col" << '\n'
            << "d: " << std::setw(12) << d << "next col" << '\n';

        std::cout << std::left
            << "i: " << std::setw(12) << i << "next col" << '\n'
            << "d: " << std::setw(12) << d << "next col" << '\n'
            << std::right;

        std::cout << std::right
            << "i: " << std::setw(12) << i << "next col" << '\n'
            << "d: " << std::setw(12) << d << "next col" << '\n';

        std::cout << std::internal
            << "i: " << std::setw(12) << i << "next col" << '\n'
            << "d: " << std::setw(12) << d << "next col" << '\n';

        std::cout << std::setfill('#')
            << "i: " << std::setw(12) << i << "next col" << '\n'
            << "d: " << std::setw(12) << d << "next col" << '\n'
            << std::setfill(' ');
    }

    void TestNoSkipws()
    {
        char ch;
        std::cin >> std::noskipws;
        while (std::cin >> ch)
        {
            std::cout << ch;
        }
        std::cin >> std::skipws;
    }

    //////////////////////////////////////////////////////////////////////////////
    //unformatted IO
    //////////////////////////////////////////////////////////////////////////////
    void TestGetPut()
    {
        char ch;
        while (std::cin.get(ch))
        {
            std::cout.put(ch);
        }
    }

    void TestEOF()
    {
        int ch;
        while ((ch = std::cin.get()) != EOF)
        {
            std::cout.put(ch);
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    //seek and tell
    //////////////////////////////////////////////////////////////////////////////
    int TestSeekTell()
    {
        //以读写方式打开文件，并定位到文件尾
        std::fstream inOut("copyOut",
            std::fstream::ate | std::fstream::in | std::fstream::out);
        if (!inOut)
        {
            std::cerr << "Unable to open file!" << std::endl;
            return EXIT_FAILURE;
        }

        //inOut以ate模式打开，因此一开始就定位到其文件尾
        auto end_mark = inOut.tellg();              //记住原文件尾位置
        inOut.seekg(0, std::fstream::beg);          //重定位到文件开始
        size_t cnt = 0;                             //字节数累加器
        std::string line;                           //保存输入中的每行

                                                    //继续读取的条件：还未遇到错误且还在读取原数据
        while (inOut && inOut.tellg() != end_mark
            && getline(inOut, line))                //且还可以获取一行输入
        {
            cnt += line.size() + 1;                 //加1表示换行符
            auto mark = inOut.tellg();              //记住读取的位置
            inOut.seekp(0, std::fstream::end);      //将写标记移动到文件尾
            inOut << cnt;                           //输出累计的长度
            if (mark != end_mark) inOut << " ";     //如果不是最后一行，打印分隔符
            inOut.seekg(mark);                      //恢复读位置
        }

        inOut.seekp(0, std::fstream::end);          //定位到文件尾
        inOut << "\n";
        return 0;
    }
};
