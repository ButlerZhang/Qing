#pragma once
#include <map>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

//固定的配置文件名
const std::string g_XMLFile("maServer.xml");

//使用的两种分隔符
const std::wstring DOT(L".");           //以点号分隔（用于XML搜索）
const std::wstring EQUAL(L"=");         //以等号分隔（用于xaopen）
const std::wstring COLON(L":");         //以冒号分隔（用于注释）
const std::wstring COMMA(L",");         //以逗号分割（用于某些子配置项）
const std::wstring SLASH(L"/");         //以斜线分隔（用于connstr）
const std::wstring SEMICOLON(L";");     //以分号分隔（用于importfile，xaopen）

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
const std::wstring gp_Describe(L"Describe");
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
const std::wstring gp_Connstr(L"connstr");
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

//某些配置项存在子配置项（g:global，s:subparam）
const std::wstring gs_NodeUse_n(L"n");
const std::wstring gs_NodeUse_s(L"s");
const std::wstring gs_NodeUse_a(L"a");
const std::wstring gs_NodeUse_q(L"q");
const std::wstring gs_QueueType_kcxp(L"kcxp");
const std::wstring gs_QueueType_zmq(L"zmq");
const std::wstring gs_QueueType_socket(L"socket");
const std::wstring gs_QueueType_shm(L"shm");
const std::wstring gs_QueueType_mem(L"mem");
const std::wstring gs_QueueType_kdh(L"kdh");
const std::wstring gs_QueueTypeKcxp_IP(L"IP");
const std::wstring gs_QueueTypeKcxp_Port(L"Port");
const std::wstring gs_QueueTypeKcxp_Protocol(L"Protocol");
const std::wstring gs_QueueTypeKcxp_ConnectCount(L"ConnectCount");
const std::wstring gs_QueueTypeSocket_IP(L"IP");
const std::wstring gs_QueueTypeSocket_Port(L"Port");
const std::wstring gs_QueueTypeSocket_Mode(L"Mode");
const std::wstring gs_QueueTypeSocket_Protocol(L"Protocol");
const std::wstring gs_QueueTypeSocket_MsgLength(L"MsgLength");
const std::wstring gs_QueueTypeZmq_IP(L"IP");
const std::wstring gs_QueueTypeZmq_Port(L"Port");
const std::wstring gs_QueueTypeZmq_Mode(L"Mode");
const std::wstring gs_QueueTypeZmq_Protocol(L"Protocol");
const std::wstring gs_QueueTypeZmq_ThreadCount(L"ThreadCount");
const std::wstring gs_QueueTypeZmq_CacheTime(L"CacheTime");
const std::wstring gs_QueueTypeZmq_HopCount(L"HopCount");

//控件类型
enum ControlType
{
    CT_DEFAULT,                                     //仅用于初始化
    CT_EDIT_TEXT,                                   //可编辑文本
    CT_STATIC_TEXT,                                 //不可编辑文本
    CT_COMBO_BOX_EDIT,                              //下拉可编辑单选框
    CT_COMBO_BOX_LIST,                              //下拉不可编辑列表
    CT_CHECK_LIST_BOX,                              //下拉复选列表框
    CT_CHECK_BOX,                                   //勾选框
    CT_MIXED_BOX_XA,                                //混合输入框
    CT_MIXED_BOX_QUEUE,                             //混合输入框
    CT_COUNT                                        //支持的控件种类
};

//参数节点
struct ParamNode
{
    UINT                    m_ParamNameID;          //参数名使用的控件ID
    UINT                    m_ParamValueID;         //参数值使用的控件ID

    std::wstring            m_ParamName;            //参数名
    std::wstring            m_ParamValue;           //参数值

    ControlType             m_ParamValueType;       //参数值的控件类型，参数名的控件固定为不可编辑文本

    ParamNode(const std::wstring& ParamName, ControlType Type = CT_DEFAULT) :
        m_ParamName(ParamName),
        m_ParamNameID(0),
        m_ParamValueID(0),
        m_ParamValueType(Type) {}
};

//叶子节点
struct LeafNode
{
    std::vector<ParamNode> m_vecParams;                     //此叶子结点对应的参数列表
    std::map<UINT, std::vector<ParamNode>> m_subParams;     //某个参数的子配置项

    UINT GetParamValueID(const std::wstring& ParamName)
    {
        for (std::vector<ParamNode>::size_type index = 0; index < m_vecParams.size(); index++)
        {
            if (m_vecParams[index].m_ParamName == ParamName)
            {
                return m_vecParams[index].m_ParamValueID;
            }
        }

        return 0;
    }

    std::wstring GetParamValue(const std::wstring& ParamName)
    {
        for (std::vector<ParamNode>::size_type index = 0; index < m_vecParams.size(); index++)
        {
            if (m_vecParams[index].m_ParamName == ParamName)
            {
                return m_vecParams[index].m_ParamValue;
            }
        }

        return std::wstring();
    }

    std::vector<ParamNode>::size_type GetIndex(const std::wstring& ParamName)
    {
        for (std::vector<ParamNode>::size_type index = 0; index < m_vecParams.size(); index++)
        {
            if (m_vecParams[index].m_ParamName == ParamName)
            {
                return index;
            }
        }

        return m_vecParams.size();
    }
};
