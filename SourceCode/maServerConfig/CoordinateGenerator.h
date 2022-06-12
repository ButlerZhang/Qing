#pragma once
#include <vector>
#include <atltypes.h>

class CoordinateGenerator
{
public:

    CoordinateGenerator(const CRect& AreaRect);

    bool Begin(int ParamCountPerLine, int ControlCountPerParm);
    bool SetSplitRatio(int NameRatio, int ValueRatio, int EmptyRatio);

    CRect GetNextRect();
    CRect GetCurrentRect();

public:

    static const int        GRID_INTERVAL;                          //�ؼ�֮��ļ��
    static const int        SPLIT_WIDTH;                            //�ͻ�����Ȳ�ֳɼ���
    static const int        SPLIT_HEIGHT;                           //�ͻ����߶Ȳ�ֳɼ���

    static const int        CONTROL_HEIGHT;                         //�ؼ��ĸ߶�
    static const int        OFFSET_X;                               //X��ƫ����
    static const int        OFFSET_Y;                               //Y��ƫ����

    int                     m_SplitNameRatio;                       //��������ռ����
    int                     m_SplitValueRatio;                      //����ֵ��ռ����
    int                     m_SplitEmptyRatio;                      //�հ״���ռ����

    int                     m_NameWidth;                            //�������ؼ��Ŀ��
    int                     m_ValueWidth;                           //����ֵ�ؼ��Ŀ��
    int                     m_EmptyWidth;                           //�հ״��ؼ��Ŀ��

    int                     m_LeftGrap;                             //������߽�ļ��
    int                     m_TopGrap;                              //�����ϱ߽�ļ��

    int                     m_ParamCountPerLine;                    //ÿ����ʾ��������
    int                     m_ControlCountPerParm;                  //ÿ������ռ�ü����ؼ�

    CRect                   m_AreaRect;                             //��ʼ��Χ
    std::vector<CRect>      m_vecRect;                              //�洢�����˵�����
};
