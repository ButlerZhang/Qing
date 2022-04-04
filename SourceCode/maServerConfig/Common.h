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
    CT_COUNT
};

struct ParamNode                            //参数节点
{
    ControlType  m_ControlType;             //控件类型
    std::wstring m_ParamName;               //参数名称
    std::wstring m_ParamValue;              //参数值

    ParamNode(const std::wstring& ParamName) :
        m_ParamName(ParamName),
        m_ControlType(CT_DEFAULT) {}
};

struct LeafNode                             //叶子节点
{
    std::vector<ParamNode> m_vecParams;     //此叶子结点对应的参数列表

    //显示叶子结点配置
    std::function<void(boost::property_tree::wptree::value_type&)> m_FDisplay;

    //更新叶子节点参数
    std::function<void(const std::wstring&, boost::property_tree::wptree::value_type&)> m_FUpdate;
};
