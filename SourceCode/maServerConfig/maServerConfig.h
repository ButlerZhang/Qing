﻿#pragma once

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

    void AddSelectItem(const std::wstring& Key, const std::wstring& Value);

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

    boost::property_tree::wptree                        g_XMLTree;              //XML根节点
    std::set<UINT>                                      g_setSubParams;         //子配置项的控件ID
    std::map<std::wstring, LeafNode>                    g_mapLeaf;              //叶子结点
    std::map<std::wstring, std::wstring>                g_mapQueueClsid;        //queue节点的type对应的clsid
    std::map<std::wstring, std::wstring>                g_mapQueueConnstr;      //queue节点的type对应的connstr
    std::map<std::wstring, std::wstring>                g_mapQueueMaxSize;      //queue节点的type对应的maxsize
    std::map<std::wstring, CRect>                       g_mapRect;              //输出区域，用于调试
    std::map<std::wstring, std::vector<std::wstring>>   g_mapSelect;            //用于选择框的可选项
    std::shared_ptr<CDialogParams>                      g_ParamsDlg;            //参数对话框
};

extern CmaServerConfigApp theApp;
