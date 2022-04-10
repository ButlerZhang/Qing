
// maServerConfig.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "maServerConfig.h"
#include "maServerConfigDlg.h"
#include "../Windows/System/SystemShare.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmaServerConfigApp

BEGIN_MESSAGE_MAP(CmaServerConfigApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CmaServerConfigApp 构造

CmaServerConfigApp::CmaServerConfigApp()
{
    // 支持重新启动管理器
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CmaServerConfigApp 对象

CmaServerConfigApp theApp;


// CmaServerConfigApp 初始化

BOOL CmaServerConfigApp::InitInstance()
{
    // 如果一个运行在 Windows XP 上的应用程序清单指定要
    // 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
    //则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // 将它设置为包括所有要在应用程序中使用的
    // 公共控件类。
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    m_NextControlID = 10000;
    InitLeafNode();

    AfxEnableControlContainer();

    // 创建 shell 管理器，以防对话框包含
    // 任何 shell 树视图控件或 shell 列表视图控件。
    CShellManager* pShellManager = new CShellManager;

    // 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

    // 标准初始化
    // 如果未使用这些功能并希望减小
    // 最终可执行文件的大小，则应移除下列
    // 不需要的特定初始化例程
    // 更改用于存储设置的注册表项
    // TODO: 应适当修改该字符串，
    // 例如修改为公司或组织名
    SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

    CmaServerConfigDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: 在此放置处理何时用
        //  “确定”来关闭对话框的代码
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: 在此放置处理何时用
        //  “取消”来关闭对话框的代码
    }
    else if (nResponse == -1)
    {
        TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
        TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
    }

    // 删除上面创建的 shell 管理器。
    if (pShellManager != nullptr)
    {
        delete pShellManager;
    }

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
#endif

    // 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
    //  而不是启动应用程序的消息泵。
    return FALSE;
}

void CmaServerConfigApp::InitLeafNode()
{
    //RuntimeTable节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(g_RuntimeTable, LeafNode()));
    g_mapLeaf[g_RuntimeTable].m_vecParams.push_back(ParamNode(L"id"));
    g_mapLeaf[g_RuntimeTable].m_vecParams.push_back(ParamNode(L"name"));
    g_mapLeaf[g_RuntimeTable].m_vecParams.push_back(ParamNode(L"clsid"));
    g_mapLeaf[g_RuntimeTable].m_vecParams.push_back(ParamNode(L"import_file"));

    //Service节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(g_Service, LeafNode()));
    g_mapLeaf[g_Service].m_vecParams.push_back(ParamNode(L"id"));
    g_mapLeaf[g_Service].m_vecParams.push_back(ParamNode(L"name"));
    g_mapLeaf[g_Service].m_vecParams.push_back(ParamNode(L"clsid"));
    g_mapLeaf[g_Service].m_vecParams.push_back(ParamNode(L"gid"));
    g_mapLeaf[g_Service].m_vecParams.push_back(ParamNode(L"runas"));
    g_mapLeaf[g_Service].m_vecParams.push_back(ParamNode(L"workthread"));
    g_mapLeaf[g_Service].m_vecParams.push_back(ParamNode(L"use"));

    //msgqueue节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(g_MsgQueue, LeafNode()));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"id"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"name"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"gid"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"direction"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"type"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"protocol"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"init"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"max_size"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"timeout"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"clsid"));
    g_mapLeaf[g_MsgQueue].m_vecParams.push_back(ParamNode(L"connstr"));

    //xa节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(g_Xa, LeafNode()));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"id"));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"name"));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"clsid"));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"xaclose"));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"xaoption"));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"daopath"));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"xaserial"));
    g_mapLeaf[g_Xa].m_vecParams.push_back(ParamNode(L"xaopen"));

    //node节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(g_Node, LeafNode()));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"id"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"name"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"type"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"gid"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"ipv4"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"path"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"use"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"defaultxa"));
    g_mapLeaf[g_Node].m_vecParams.push_back(ParamNode(L"backupxa"));
}

bool CmaServerConfigApp::ReadXMLFile(const std::string& XMLFile)
{
    try
    {
        std::locale::global(std::locale(""));                       //不设置时读取中文会乱码
        boost::property_tree::read_xml(XMLFile,                     //要读取的XML文件
            theApp.g_XMLTree,                                       //读取出来的XML树
            boost::property_tree::xml_parser::trim_whitespace);     //去除空行
        return true;
    }
    catch (const std::exception& e)
    {
        const std::wstring& ErrorMsg = L"读XML文件出错：" + StringToWString(e.what());
        MessageBox(NULL, ErrorMsg.c_str(), L"提示", 0);
        return false;
    }
}

bool CmaServerConfigApp::WriteXMLFile(const std::string& XMLFile)
{
    try
    {
        //设置写XML文件的格式，不设置时XML是乱的，没有层次
        boost::property_tree::xml_writer_settings<std::wstring> settings;
        settings = boost::property_tree::xml_writer_make_settings<std::wstring>(L'\t', 1);

        boost::property_tree::write_xml(
            XMLFile.c_str(),            //写到哪个文件
            theApp.g_XMLTree,           //使用读取时的XML树
            std::locale(),              //本地化
            settings);                  //XML配置格式

        return true;
    }
    catch (const std::exception& e)
    {
        const std::wstring& ErrorMsg = L"写XML文件出错：" + StringToWString(e.what());
        MessageBox(NULL, ErrorMsg.c_str(), L"提示", 0);
        return false;
    }
}

CString CmaServerConfigApp::GetLeafID(const CString& Text)
{
    if (Text.IsEmpty())
    {
        return CString();
    }

    int StartPos = Text.Find(L"_");
    if (StartPos <= 0 || StartPos > Text.GetLength())
    {
        return CString();
    }

    int StopPos = Text.Find(L"_", StartPos + 1);
    const CString& LeafID = Text.Mid(StartPos + 1, StopPos - StartPos - 1).GetString();
    return LeafID;
}

CString CmaServerConfigApp::GetLeftType(const CString& Text)
{
    if (Text.IsEmpty())
    {
        return CString();
    }

    int StartPos = Text.Find(L"_");
    if (StartPos <= 0 || StartPos > Text.GetLength())
    {
        return CString();
    }

    const CString& CurrentType = Text.Left(StartPos);
    return CurrentType;
}

void CmaServerConfigApp::ResetControl()
{
    for (std::vector<CEdit>::size_type index = 0; index != m_vecEditText.size(); index++)
    {
        m_vecEditText[index]->MoveWindow(0, 0, 0, 0);
        m_vecEditText[index]->ShowWindow(SW_HIDE);
    }

    for (std::vector<CStatic>::size_type index = 0; index != m_vecStaticText.size(); index++)
    {
        m_vecStaticText[index]->MoveWindow(0, 0, 0, 0);
        m_vecStaticText[index]->ShowWindow(SW_HIDE);
    }
}

std::shared_ptr<CEdit> CmaServerConfigApp::GetEditText(CWnd* wnd, int& TargetIndex)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CEdit>>::size_type index = 0; index < m_vecEditText.size(); index++)
    {
        m_vecEditText[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetIndex = index;
            return m_vecEditText[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecEditText.push_back(std::make_shared<CEdit>());
    TargetIndex = m_vecEditText.size() - 1;

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | SS_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN;
    m_vecEditText[TargetIndex]->Create(Style, Rect, wnd, m_NextControlID++);

    return m_vecEditText[TargetIndex];
}

std::shared_ptr<CEdit> CmaServerConfigApp::GetEditText(int index)
{
    if (index >= 0 && index < m_vecEditText.size())
    {
        return m_vecEditText[index];
    }

    return std::shared_ptr<CEdit>();
}

std::shared_ptr<CStatic> CmaServerConfigApp::GetStaticText(CWnd* wnd, int& TargetIndex)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CStatic>>::size_type index = 0; index < m_vecStaticText.size(); index++)
    {
        m_vecStaticText[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetIndex = index;
            return m_vecStaticText[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecStaticText.push_back(std::make_shared<CStatic>());
    TargetIndex = m_vecStaticText.size() - 1;

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE;
    m_vecStaticText[TargetIndex]->Create(NULL, Style, Rect, wnd, m_NextControlID++);

    return m_vecStaticText[TargetIndex];
}

std::shared_ptr<CStatic> CmaServerConfigApp::GetStaticText(int index)
{
    if (index >= 0 && index < m_vecStaticText.size())
    {
        return m_vecStaticText[index];
    }

    return std::shared_ptr<CStatic>();
}

void CmaServerConfigApp::AdjustEditTextHeight(const std::shared_ptr<CEdit>& ptrEdit)
{
    //获取文本信息
    TEXTMETRIC tmText;
    CClientDC dc(ptrEdit.get());
    dc.GetTextMetrics(&tmText);

    //计算文本的高度
    int TextTotalHeight = tmText.tmHeight + tmText.tmExternalLeading;

    //计算文本距离编辑框顶部的距离
    CRect rect;
    ptrEdit->GetClientRect(rect);
    int OffsetY = (rect.Height() - TextTotalHeight) / 2;

    //将工作区域往下偏移
    rect.OffsetRect(0, OffsetY);

    //设置工作区域
    ptrEdit->SetRectNP(&rect);
}
