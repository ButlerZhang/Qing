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

    return 0;
}

PRIMER_NS_END