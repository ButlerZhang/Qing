#pragma once
#include "../../../StandardNamespace.h"

PRIMER_NS_BEGIN

//�ں������ʹ��extern�ָ���ֵ����Щ������
//��������棬����extern�����ñ�ȡ���ˡ���
//����������˶��塣
extern double pi1 = 3.14;

int chapter2_main() {

    extern int i;               //����
    int j = 0;                  //����

    //�ں����ڲ�����extern�ͳ�ֵͬʱʹ��
    //extern double pi2 = 3.14;

    //ֻ�е����»��ߵı���
    int _ = 100;

    return 0;
}

PRIMER_NS_END