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


enum ControlType                            //�ؼ�����
{
    CT_DEFAULT,
    CT_STATIC_TEXT,                         //��̬�ı�
    CT_COUNT
};

struct ParamNode                            //�����ڵ�
{
    std::wstring m_ParamName;               //��������
    std::wstring m_ParamValue;              //����ֵ
    ControlType  m_ControlType;             //����ֵ�Ŀؼ�����

    ParamNode(const std::wstring& ParamName) :
        m_ParamName(ParamName),
        m_ControlType(CT_DEFAULT) {}
};

struct LeafNode                             //Ҷ�ӽڵ�
{
    std::vector<ParamNode> m_vecParams;     //��Ҷ�ӽ���Ӧ�Ĳ����б�
};
