
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

    std::shared_ptr<CEdit> GetEditText(int index);
    std::shared_ptr<CStatic> GetStaticText(int index);

    // 实现
    DECLARE_MESSAGE_MAP()

private:

    void InitLeafNode();

private:

    UINT                                     m_NextControlID;       //下一个可用的控件ID
    std::vector<std::shared_ptr<CEdit>>      m_vecEditText;         //存储参数值
    std::vector<std::shared_ptr<CStatic>>    m_vecStaticText;       //存储参数名称

public:

    std::map<std::wstring, LeafNode>         g_mapLeaf;              //叶子结点
    boost::property_tree::wptree             g_XMLTree;              //XML根节点
};

extern CmaServerConfigApp theApp;
