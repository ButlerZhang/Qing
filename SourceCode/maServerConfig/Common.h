#pragma once
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>


const std::string g_XMLFile("maServer.xml");

const std::wstring g_RuntimeTable(L"runtimetable");
const std::wstring g_Service(L"service");
const std::wstring g_MsgQueue(L"msgqueue");
const std::wstring g_Xa(L"xa");
const std::wstring g_Node(L"node");


enum ControlType                            //控件类型
{
    CT_DEFAULT,
    CT_STATIC_TEXT_ENABLE,                  //可编辑静态文本
    CT_STATIC_TEXT_DISABLE,                 //不可编辑静态文本
    CT_COMBO_BOX_EDIT,                      //下拉可编辑单选框
    CT_COUNT
};

struct ParamNode                            //参数节点
{
    int          m_ParamNameIndex;          //参数名下标
    int          m_ParamValueIndex;         //参数值下标

    std::wstring m_ParamName;               //参数名称
    std::wstring m_ParamValue;              //参数值

    ControlType  m_ParamValueType;          //参数值的控件类型

    ParamNode(const std::wstring& ParamName, ControlType Type = CT_DEFAULT) :
        m_ParamName(ParamName),
        m_ParamNameIndex(0),
        m_ParamValueIndex(0),
        m_ParamValueType(Type) {}
};

struct LeafNode                             //叶子节点
{
    std::vector<ParamNode> m_vecParams;     //此叶子结点对应的参数列表
};
