#include "IOExample.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>



IOExample::IOExample()
{
}

IOExample::~IOExample()
{
}

void IOExample::TestBoolapha()
{
    std::cout << "default bool values: " << true << "," << false << std::endl;

    std::cout << std::boolalpha;
    std::cout << "alapha bool values: " << true << "," << false << std::endl;

    std::cout << std::noboolalpha;
}

void IOExample::TestIntDecimal()
{
    std::cout << "default: " << 20 << "," << 1024 << std::endl;
    std::cout << "octal: " << std::oct << 20 << "," << 1024 << std::endl;
    std::cout << "hex: " << std::hex << 20 << "," << 1024 << std::endl;
    std::cout << "decimal: " << std::dec << 20 << "," << 1024 << std::endl;
}

void IOExample::TestShowbase()
{
    std::cout << std::showbase;
    std::cout << "default: " << 20 << "," << 1024 << std::endl;
    std::cout << "in octal: " << std::oct << 20 << "," << 1024 << std::endl;
    std::cout << "in hex: " << std::hex << 20 << "," << 1024 << std::endl;
    std::cout << "in decimal: " << std::dec << 20 << "," << 1024 << std::endl;
    std::cout << std::noshowbase;
}

void IOExample::TestUppercase()
{
    std::cout << std::uppercase << std::showbase << std::hex;
    std::cout << "in hexadecimal: " << 20 << "," << 1024 << std::endl;
    std::cout << std::nouppercase << std::noshowbase << std::dec;
}

void IOExample::TestPrecision()
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

void IOExample::TestDoubleCounting()
{
    std::cout << "default format: " << 100 * std::sqrt(2.0) << '\n'
        << "scientific: " << std::scientific << 100 * std::sqrt(2.0) << '\n'
        << "fixed decimal: " << std::fixed << 100 * std::sqrt(2.0) << '\n'
        << "hexadecimal: " << std::hexfloat << 100 * std::sqrt(2.0) << '\n'
        << "use defaults: " << std::defaultfloat << 100 * std::sqrt(2.0) << '\n';
}

void IOExample::TestShowpoint()
{
    std::cout << 10.0 << std::endl;
    std::cout << std::showpoint << 10.0 << std::noshowpoint << std::endl;
}

void IOExample::TestSpace()
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

void IOExample::TestNoSkipws()
{
    char ch;
    std::cin >> std::noskipws;
    while (std::cin >> ch)
    {
        std::cout << ch;
    }
    std::cin >> std::skipws;
}

void IOExample::TestGetPut()
{
    char ch;
    while (std::cin.get(ch))
    {
        std::cout.put(ch);
    }
}

void IOExample::TestEOF()
{
    int ch;
    while ((ch = std::cin.get()) != EOF)
    {
        std::cout.put(ch);
    }
}

int IOExample::TestSeekTell()
{
    //�Զ�д��ʽ���ļ�������λ���ļ�β
    std::fstream inOut("copyOut",
        std::fstream::ate | std::fstream::in | std::fstream::out);
    if (!inOut)
    {
        std::cerr << "Unable to open file!" << std::endl;
        return EXIT_FAILURE;
    }

    //inOut��ateģʽ�򿪣����һ��ʼ�Ͷ�λ�����ļ�β
    auto end_mark = inOut.tellg();              //��סԭ�ļ�βλ��
    inOut.seekg(0, std::fstream::beg);          //�ض�λ���ļ���ʼ
    size_t cnt = 0;                             //�ֽ����ۼ���
    std::string line;                           //���������е�ÿ��

    //������ȡ����������δ���������һ��ڶ�ȡԭ����
    while (inOut && inOut.tellg() != end_mark
        && getline(inOut, line))                //�һ����Ի�ȡһ������
    {
        cnt += line.size() + 1;                 //��1��ʾ���з�
        auto mark = inOut.tellg();              //��ס��ȡ��λ��
        inOut.seekp(0, std::fstream::end);      //��д����ƶ����ļ�β
        inOut << cnt;                           //����ۼƵĳ���
        if (mark != end_mark) inOut << " ";     //����������һ�У���ӡ�ָ���
        inOut.seekg(mark);                      //�ָ���λ��
    }

    inOut.seekp(0, std::fstream::end);          //��λ���ļ�β
    inOut << "\n";
    return 0;
}
