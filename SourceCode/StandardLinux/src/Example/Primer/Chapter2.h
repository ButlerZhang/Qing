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

    //����
    {
        int ival = 1024;
        int& refVal = ival;
        //int& refVal2;             �����ʼ��
        //int& refVal3 = 10;        ������������ֵ
        //double& refVal4 = ival;   ������Ҫƥ��
    }

    //ָ��
    {
        int ival = 42;
        int* p = &ival;

        double dval;
        double* pd = &dval;
        double* pd2 = pd;

        //int* pi = pd;             ���Ͳ�ƥ��
        //int* pi = &dval;          ���Ͳ�ƥ��

        int* ptr = nullptr;

        int zero = 0;
        //int* p1 = zero;           ����ֱ�ӽ�����ֵ��ֵ��ָ�룬������0ֵ

        void* pv = &zero;
        pv = pd;                    //void*���Դ���������͵�ָ��

        int** p1 = &p;              //ָ��ָ���ָ��
        int*& r = p;                //ָ��ָ�������
    }

    return 0;
}

PRIMER_NS_END