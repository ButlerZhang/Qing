#pragma once
#include "../../../StandardNamespace.h"

PRIMER_NS_BEGIN

/*
 * #includeָ���ͷ�ļ������ֱ�����ͬһ�У�
 * <��i֮�䲻���пո񣬵���m��>֮��ȴ���ԡ�
 * �ո����������Ԥ����ָʾ�С�
*/
#include <iostream>

int chapter1_main() {
    //endl����������У�����Ҫ����ˢ�»�����
    std::cout << "Enter two numbers:" << std::endl;

    //<<��>>���������������������Կ�������
    int v1 = 0, v2 = 0;
    std::cin >> v1 >> v2;

    //cout��������Ӧ�����������ͣ�����������ַ�����int
    std::cout << "The sum of " << v1 << " and " << v2
        << " is " << v1 + v2 << std::endl;

    return 0;
}

void chapter1_test() {
    std::cout << "/*";
    std::cout << "*/";

    //error
    //std::cout << /* "*/" */;

    //�������˵������ע�͸�elseһ�����þͽ�ƥ��
    std::cout << /* "*/" /* "/*" */;
}

PRIMER_NS_END