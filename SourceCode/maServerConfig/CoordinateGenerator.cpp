#include "CoordinateGenerator.h"

const int CoordinateGenerator::CONTROL_HEIGHT = 30;
const int CoordinateGenerator::OFFSET_X = 10;
const int CoordinateGenerator::OFFSET_Y = 5;


CoordinateGenerator::CoordinateGenerator(const CRect& AreaRect)
{
    //Ĭ�ϵĲ�ֱ���������֮�͵���10��
    SetSplitRatio(1, 8, 1);

    //Ĭ��һ����ʾһ�������������ؼ�
    m_ParamCountPerLine = 1;
    m_ControlCountPerParm = 2;

    //����Ĭ��ֵ
    m_NameWidth = 0;
    m_ValueWidth = 0;
    m_EmptyWidth = 0;
    m_LeftGrap = OFFSET_X;
    m_TopGrap = 0;

    m_AreaRect = AreaRect;
}

bool CoordinateGenerator::SetSplitRatio(int NameRatio, int ValueRatio, int EmptyRatio)
{
    if (NameRatio < 0 || ValueRatio < 0 || EmptyRatio < 0)
    {
        return false;
    }

    if ((NameRatio + ValueRatio + EmptyRatio) != 10)
    {
        return false;
    }

    m_SplitNameRatio = NameRatio;
    m_SplitEmptyRatio = EmptyRatio;
    m_SplitValueRatio = ValueRatio;
    return true;
}

bool CoordinateGenerator::Begin(int ParamCountPerLine, int ControlCountPerParm)
{
    //��ǰ��֧��һ����ʾһ����������������
    if (ParamCountPerLine != 1 && ParamCountPerLine != 2)
    {
        return false;
    }

    //һ������������ֻ��һ���ؼ���Ҳ���������ؼ���ͨ���������ؼ�
    if (ControlCountPerParm != 1 && ControlCountPerParm != 2)
    {
        return false;
    }

    //��������
    m_ParamCountPerLine = ParamCountPerLine;
    m_ControlCountPerParm = ControlCountPerParm;

    //���һ������ֻ��һ���ؼ�
    if (ControlCountPerParm == 1)
    {
        SetSplitRatio(10, 0, 0);
        return true;
    }

    //���һ��ֻ��ʾһ������
    if (ParamCountPerLine == 1)
    {
        int SplitRatio = m_SplitNameRatio + m_SplitValueRatio + m_SplitEmptyRatio;

        //�������ؼ��Ŀ��
        m_NameWidth = (m_AreaRect.Width() / SplitRatio) * m_SplitNameRatio;

        //�հ״��ؼ��Ŀ��
        m_EmptyWidth = (m_AreaRect.Width() / SplitRatio) * m_SplitEmptyRatio;

        //����ֵ�ؼ��Ŀ��
        m_ValueWidth = m_AreaRect.Width() - m_NameWidth - m_EmptyWidth;
        return true;
    }

    //���һ��ֻ��ʾ��������
    //TODO

    return true;
}

CRect CoordinateGenerator::GetNextRect()
{
    CRect NewRect;
    if (m_ControlCountPerParm == 2)
    {
        if (m_vecRect.empty())
        {
            NewRect.left = m_AreaRect.left + m_LeftGrap;
            NewRect.top = m_AreaRect.top + m_TopGrap;
            NewRect.right = NewRect.left + m_NameWidth;
            NewRect.bottom = NewRect.top + CONTROL_HEIGHT;
        }
        else if (m_vecRect.size() % 2 != 0)
        {
            const CRect& LeftRect = m_vecRect[m_vecRect.size() - 1];
            NewRect.left = LeftRect.right + OFFSET_X;
            NewRect.right = NewRect.left + m_ValueWidth;
            NewRect.top = LeftRect.top;
            NewRect.bottom = LeftRect.bottom;
        }
        else
        {
            const CRect& UpRect = m_vecRect[m_vecRect.size() - 2];
            NewRect.left = UpRect.left;
            NewRect.right = UpRect.right;
            NewRect.top = UpRect.top + CONTROL_HEIGHT + OFFSET_Y;
            NewRect.bottom = NewRect.top + CONTROL_HEIGHT;
        }
    }
    else
    {
        NewRect.left = m_AreaRect.left + OFFSET_X;
        NewRect.right = m_AreaRect.right - OFFSET_X;
        NewRect.top = m_AreaRect.top + m_TopGrap + ((LONG)m_vecRect.size() * CONTROL_HEIGHT);
        NewRect.bottom = NewRect.top + CONTROL_HEIGHT;
    }

    m_vecRect.push_back(NewRect);
    return NewRect;
}

CRect CoordinateGenerator::GetCurrentRect()
{
    return m_vecRect.empty() ? CRect() : m_vecRect[m_vecRect.size() - 1];
}
