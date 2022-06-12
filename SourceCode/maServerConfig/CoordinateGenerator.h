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

    static const int        GRID_INTERVAL;                          //控件之间的间隔
    static const int        SPLIT_WIDTH;                            //客户区宽度拆分成几份
    static const int        SPLIT_HEIGHT;                           //客户区高度拆分成几份

    static const int        CONTROL_HEIGHT;                         //控件的高度
    static const int        OFFSET_X;                               //X轴偏移量
    static const int        OFFSET_Y;                               //Y轴偏移量

    int                     m_SplitNameRatio;                       //参数名所占比例
    int                     m_SplitValueRatio;                      //参数值所占比例
    int                     m_SplitEmptyRatio;                      //空白处所占比例

    int                     m_NameWidth;                            //参数名控件的宽度
    int                     m_ValueWidth;                           //参数值控件的宽度
    int                     m_EmptyWidth;                           //空白处控件的宽度

    int                     m_LeftGrap;                             //距离左边界的间隔
    int                     m_TopGrap;                              //距离上边界的间隔

    int                     m_ParamCountPerLine;                    //每行显示几个参数
    int                     m_ControlCountPerParm;                  //每个参数占用几个控件

    CRect                   m_AreaRect;                             //初始范围
    std::vector<CRect>      m_vecRect;                              //存储产生了的区域
};
