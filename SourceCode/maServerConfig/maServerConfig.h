#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"     // 主符号
#include "Common.h"
#include <map>
#include <set>
#include <boost/property_tree/ptree.hpp>
#include "CDialogParams.h"

class CmaServerConfigApp : public CWinApp
{
public:
    CmaServerConfigApp();
    virtual BOOL InitInstance();

public:

    void WriteLog();
    void AddCRectToLog(const std::wstring& Key, const CRect& TempRect);

    bool ReadXMLFile(const std::string& XMLFile);
    bool WriteXMLFile(const std::string& XMLFile);

    CString GetLeafID(const CString& Text);
    CString GetLeftType(const CString& Text);
    UINT GetNextControlID() { return m_NextControlID++; }
    ControlType GetParamValueType(const std::wstring& Name);

    std::wstring GetSelectItemMapKey(const std::wstring& OldKey);
    void AddSelectItem(const std::wstring& Trunk, const std::wstring& Leaf, const std::wstring& ID, const std::wstring& GID, const std::wstring& Name);

    void ResetAllControl();
    void ResetSubParamsControl();

    std::shared_ptr<CEdit> GetEditText(CWnd* wnd, UINT& TargetID);
    std::shared_ptr<CStatic> GetStaticText(CWnd* wnd, UINT& TargetID);
    std::shared_ptr<CComboBox> GetComboBoxEdit(CWnd* wnd, UINT& TargetID);
    std::shared_ptr<CComboBox> GetComboBoxList(CWnd* wnd, UINT& TargetID);
    std::shared_ptr<CButton> GetButton(CWnd* wnd, UINT& TargetID);
    std::shared_ptr<CButton> GetCheckBox(CWnd* wnd, UINT& TargetID);

    std::shared_ptr<CEdit> GetEditText(UINT TargetID);
    std::shared_ptr<CStatic> GetStaticText(UINT TargetID);
    std::shared_ptr<CComboBox> GetComboBoxEdit(UINT TargetID);
    std::shared_ptr<CComboBox> GetComboBoxList(UINT TargetID);
    std::shared_ptr<CButton> GetButton(UINT TargetID);
    std::shared_ptr<CButton> GetCheckBox(UINT TargetID);

    // 实现
    DECLARE_MESSAGE_MAP()

private:

    void InitLeafNode();
    void InitSelectItem();

private:

    UINT                                                m_NextControlID;        //下一个可用的控件ID
    std::vector<std::shared_ptr<CStatic>>               m_vecStaticText;        //存储参数名称的静态文本控件
    std::vector<std::shared_ptr<CEdit>>                 m_vecEditText;          //存储参数值可编辑文本控件
    std::vector<std::shared_ptr<CComboBox>>             m_vecComboBoxEdit;      //下拉可编辑单选框
    std::vector<std::shared_ptr<CComboBox>>             m_vecComboBoxList;      //下拉不可编辑列表
    std::vector<std::shared_ptr<CButton>>               m_vecButton;            //点击显示下拉列表框
    std::vector<std::shared_ptr<CButton>>               m_vecCheckBox;          //勾选框

public:

    std::set<UINT>                                                  g_setSubParams;         //子配置项的控件ID
    std::shared_ptr<CDialogParams>                                  g_ParamsDlg;            //参数对话框

    boost::property_tree::wptree                                    g_XMLTree;              //XML根节点
    std::map<std::wstring, LeafNode>                                g_mapLeaf;              //叶子结点

    std::map<std::wstring, CRect>                                   g_mapRect;              //输出区域，用于调试
    std::map<std::wstring, std::wstring>                            g_mapKeyConvert;        //关键字转换
    std::map<std::wstring, std::vector<std::wstring>>               g_MultiSelect;          //多选
    std::map<std::wstring, std::map<std::wstring, std::wstring>>    g_SingleSelect;         //单选
};

extern CmaServerConfigApp theApp;
