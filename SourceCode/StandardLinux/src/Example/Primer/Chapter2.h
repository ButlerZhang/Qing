#pragma once
#include "../../../StandardNamespace.h"

PRIMER_NS_BEGIN

//在函数外既使用extern又赋初值，有些编译器
//会给出警告，而且extern的作用被取消了。即
//由声明变成了定义。
extern double pi1 = 3.14;

int chapter2_main() {

    extern int i;               //声明
    int j = 0;                  //定义

    //在函数内部不能extern和初值同时使用
    //extern double pi2 = 3.14;

    //只有单个下划线的变量
    int _ = 100;

    //引用
    {
        int ival = 1024;
        int& refVal = ival;
        //int& refVal2;             必须初始化
        //int& refVal3 = 10;        不能引用字面值
        //double& refVal4 = ival;   类型需要匹配
    }

    //指针
    {
        int ival = 42;
        int* p = &ival;

        double dval;
        double* pd = &dval;
        double* pd2 = pd;

        //int* pi = pd;             类型不匹配
        //int* pi = &dval;          类型不匹配

        int* ptr = nullptr;

        int zero = 0;
        //int* p1 = zero;           不能直接将整型值赋值给指针，哪怕是0值

        void* pv = &zero;
        pv = pd;                    //void*可以存放任意类型的指针

        int** p1 = &p;              //指向指针的指针
        int*& r = p;                //指向指针的引用
    }

    return 0;
}

PRIMER_NS_END