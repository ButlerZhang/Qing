
// maServerConfig.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号
#include "Common.h"
#include <map>
#include <boost/property_tree/ptree.hpp>



class CmaServerConfigApp : public CWinApp
{
public:
    CmaServerConfigApp();
    virtual BOOL InitInstance();

public:

    bool ReadXMLFile(const std::string& XMLFile);
    bool WriteXMLFile(const std::string& XMLFile);

    CString GetLeafID(const CString& Text);
    CString GetLeftType(const CString& Text);

    void ResetControl();

    std::shared_ptr<CEdit> GetEditText(CWnd* wnd, int &TargetIndex);
    std::shared_ptr<CStatic> GetStaticText(CWnd* wnd, int &TargetIndex);
    std::shared_ptr<CComboBox> GetComboBox(CWnd* wnd, int& TargetIndex);

    std::shared_ptr<CEdit> GetEditText(int index);
    std::shared_ptr<CStatic> GetStaticText(int index);
    std::shared_ptr<CComboBox> GetComboBox(int index);

    void UpdateEdit(const std::shared_ptr<CEdit>& ptrEdit, ControlType Type);
    void UpdateComboBox(const std::shared_ptr<CComboBox>& ptrComboBox, const std::wstring &LeafType, const ParamNode &Node);

    // 实现
    DECLARE_MESSAGE_MAP()

private:

    void InitLeafNode();
    void InitSelectItem();

private:

    UINT                                     m_NextControlID;       //下一个可用的控件ID
    std::vector<std::shared_ptr<CStatic>>    m_vecStaticText;       //存储参数名称
    std::vector<std::shared_ptr<CEdit>>      m_vecEditText;         //存储参数值
    std::vector<std::shared_ptr<CComboBox>>  m_vecComboBox;         //下拉单选框

public:

    boost::property_tree::wptree             g_XMLTree;              //XML根节点
    std::map<std::wstring, LeafNode>         g_mapLeaf;              //叶子结点
    std::map<std::wstring, std::vector<std::wstring>> g_mapSelect;   //用于选择框的可选项
};

extern CmaServerConfigApp theApp;
