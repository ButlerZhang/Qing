#pragma once
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

//固定的配置文件名
const std::string g_XMLFile("maServer.xml");

//使用的两种分隔符
const std::wstring DOT(L".");           //以点号分隔
const std::wstring COLON(L":");         //以冒号分隔
const std::wstring SEMICOLON(L";");     //以分号分隔

//架构名称（g:global，m:ma）
const std::wstring gm_MA(L"ma");
const std::wstring gm_Kernel(L"kernel");
const std::wstring gm_Deployment(L"deployment");
const std::wstring gm_Construction(L"construction");

//树干名称（g:global，t:trunk）
const std::wstring gt_Rtdb(L"rtdb");
const std::wstring gt_Services(L"services");
const std::wstring gt_Bbu(L"bpu");
const std::wstring gt_Queues(L"queues");
const std::wstring gt_Xas(L"xas");
const std::wstring gt_Nodes(L"nodes");

//叶子名称（g:global，l:leaf）
const std::wstring gl_RuntimeTable(L"runtimetable");
const std::wstring gl_Service(L"service");
const std::wstring gl_MsgQueue(L"msgqueue");
const std::wstring gl_Xa(L"xa");
const std::wstring gl_Node(L"node");

//常用的固定的参数名称（g:global，p:param）
const std::wstring gp_ID(L"id");
const std::wstring gp_Name(L"name");
const std::wstring gp_Clsid(L"clsid");
const std::wstring gp_ImportFile(L"import_file");
const std::wstring gp_Runas(L"runas");
const std::wstring gp_WorkThread(L"workthread");
const std::wstring gp_Direction(L"direction");
const std::wstring gp_Protocol(L"protocol");
const std::wstring gp_Init(L"init");
const std::wstring gp_MaxSize(L"max_size");
const std::wstring gp_Timeout(L"timeout");
const std::wstring gp_Connstr(L"constr");
const std::wstring gp_XaClose(L"xaclose");
const std::wstring gp_XaOption(L"xaoption");
const std::wstring gp_DaoPath(L"daopath");
const std::wstring gp_XaSerial(L"xaserial");
const std::wstring gp_XaOpen(L"xaopen");
const std::wstring gp_Type(L"type");
const std::wstring gp_Gid(L"gid");
const std::wstring gp_Ipv4(L"ipv4");
const std::wstring gp_Path(L"path");
const std::wstring gp_Use(L"use");
const std::wstring gp_DefaultXa(L"defaultxa");
const std::wstring gp_BackupXa(L"backupxa");

//控件类型
enum ControlType
{
    CT_DEFAULT,                                     //仅用于初始化
    CT_STATIC_TEXT_ENABLE,                          //可编辑静态文本
    CT_STATIC_TEXT_DISABLE,                         //不可编辑静态文本
    CT_COMBO_BOX_EDIT,                              //下拉可编辑单选框
    CT_COMBO_BOX_LIST,                              //下拉不可编辑列表
    CT_CHECK_LIST_BOX,                              //下拉复选列表框
    CT_COUNT                                        //支持的控件种类
};

//参数节点
struct ParamNode
{
    UINT                    m_ParamNameID;          //参数名使用的控件ID
    UINT                    m_ParamValueID;         //参数值使用的控件ID
    std::wstring            m_ParamValue;           //参数值

    const std::wstring      m_ParamName;            //参数名称
    const ControlType       m_ParamValueType;       //参数值的控件类型

    ParamNode(const std::wstring& ParamName, ControlType Type = CT_DEFAULT) :
        m_ParamName(ParamName),
        m_ParamNameID(0),
        m_ParamValueID(0),
        m_ParamValueType(Type) {}
};

//叶子节点
struct LeafNode
{
    std::vector<ParamNode> m_vecParams;     //此叶子结点对应的参数列表
};
