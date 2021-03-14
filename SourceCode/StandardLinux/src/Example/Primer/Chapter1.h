#pragma once
#include "../../../StandardNamespace.h"

PRIMER_NS_BEGIN

/*
 * #include指令和头文件的名字必须在同一行，
 * <和i之间不能有空格，但是m和>之间却可以。
 * 空格不允许出现在预处理指示中。
*/
#include <iostream>

int chapter1_main() {
    //endl不仅输出换行，更重要的是刷新缓冲区
    std::cout << "Enter two numbers:" << std::endl;

    //<<和>>都返回其左侧运算对象，所以可以连用
    int v1 = 0, v2 = 0;
    std::cin >> v1 >> v2;

    //cout可以自适应多种数据类型，例如下面的字符串和int
    std::cout << "The sum of " << v1 << " and " << v2
        << " is " << v1 + v2 << std::endl;

    return 0;
}

void chapter1_test() {
    std::cout << "/*";
    std::cout << "*/";

    //error
    //std::cout << /* "*/" */;

    //这个例子说明最后的注释跟else一样采用就近匹配
    std::cout << /* "*/" /* "/*" */;
}

PRIMER_NS_END