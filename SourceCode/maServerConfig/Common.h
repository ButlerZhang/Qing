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
    CT_STATIC_TEXT_ENABLE,                  //�ɱ༭��̬�ı�
    CT_STATIC_TEXT_DISABLE,                 //���ɱ༭��̬�ı�
    CT_COMBO_BOX_EDIT,                      //�����ɱ༭��ѡ��
    CT_COUNT
};

struct ParamNode                            //�����ڵ�
{
    int          m_ParamNameIndex;          //�������±�
    int          m_ParamValueIndex;         //����ֵ�±�

    std::wstring m_ParamName;               //��������
    std::wstring m_ParamValue;              //����ֵ

    ControlType  m_ParamValueType;          //����ֵ�Ŀؼ�����

    ParamNode(const std::wstring& ParamName, ControlType Type = CT_DEFAULT) :
        m_ParamName(ParamName),
        m_ParamNameIndex(0),
        m_ParamValueIndex(0),
        m_ParamValueType(Type) {}
};

struct LeafNode                             //Ҷ�ӽڵ�
{
    std::vector<ParamNode> m_vecParams;     //��Ҷ�ӽ���Ӧ�Ĳ����б�
};
