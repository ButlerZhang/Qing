#pragma once
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

//�̶��������ļ���
const std::string g_XMLFile("maServer.xml");

//ʹ�õ����ַָ���
const std::wstring DOT(L".");           //�Ե�ŷָ�
const std::wstring COLON(L":");         //��ð�ŷָ�
const std::wstring SEMICOLON(L";");     //�Էֺŷָ�

//�ܹ����ƣ�g:global��m:ma��
const std::wstring gm_MA(L"ma");
const std::wstring gm_Kernel(L"kernel");
const std::wstring gm_Deployment(L"deployment");
const std::wstring gm_Construction(L"construction");

//�������ƣ�g:global��t:trunk��
const std::wstring gt_Rtdb(L"rtdb");
const std::wstring gt_Services(L"services");
const std::wstring gt_Bbu(L"bpu");
const std::wstring gt_Queues(L"queues");
const std::wstring gt_Xas(L"xas");
const std::wstring gt_Nodes(L"nodes");

//Ҷ�����ƣ�g:global��l:leaf��
const std::wstring gl_RuntimeTable(L"runtimetable");
const std::wstring gl_Service(L"service");
const std::wstring gl_MsgQueue(L"msgqueue");
const std::wstring gl_Xa(L"xa");
const std::wstring gl_Node(L"node");

//���õĹ̶��Ĳ������ƣ�g:global��p:param��
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

//�ؼ�����
enum ControlType
{
    CT_DEFAULT,                                     //�����ڳ�ʼ��
    CT_STATIC_TEXT_ENABLE,                          //�ɱ༭��̬�ı�
    CT_STATIC_TEXT_DISABLE,                         //���ɱ༭��̬�ı�
    CT_COMBO_BOX_EDIT,                              //�����ɱ༭��ѡ��
    CT_COMBO_BOX_LIST,                              //�������ɱ༭�б�
    CT_CHECK_LIST_BOX,                              //������ѡ�б��
    CT_COUNT                                        //֧�ֵĿؼ�����
};

//�����ڵ�
struct ParamNode
{
    UINT                    m_ParamNameID;          //������ʹ�õĿؼ�ID
    UINT                    m_ParamValueID;         //����ֵʹ�õĿؼ�ID
    std::wstring            m_ParamValue;           //����ֵ

    const std::wstring      m_ParamName;            //��������
    const ControlType       m_ParamValueType;       //����ֵ�Ŀؼ�����

    ParamNode(const std::wstring& ParamName, ControlType Type = CT_DEFAULT) :
        m_ParamName(ParamName),
        m_ParamNameID(0),
        m_ParamValueID(0),
        m_ParamValueType(Type) {}
};

//Ҷ�ӽڵ�
struct LeafNode
{
    std::vector<ParamNode> m_vecParams;     //��Ҷ�ӽ���Ӧ�Ĳ����б�
};
