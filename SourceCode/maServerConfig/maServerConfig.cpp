
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
    InitSelectItem();

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
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_RuntimeTable, LeafNode()));
    g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_ID, CT_STATIC_TEXT_DISABLE));
    g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_Name, CT_STATIC_TEXT_ENABLE));
    g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_Clsid, CT_COMBO_BOX_EDIT));
    g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_ImportFile, CT_LIST_BOX));

    //Service节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_Service, LeafNode()));
    g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_ID, CT_STATIC_TEXT_DISABLE));
    g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Name, CT_STATIC_TEXT_ENABLE));
    g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Clsid, CT_COMBO_BOX_EDIT));
    g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Gid));
    g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Runas, CT_COMBO_BOX_LIST));
    g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_WorkThread));
    g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Use));

    //msgqueue节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_MsgQueue, LeafNode()));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_ID, CT_STATIC_TEXT_DISABLE));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Name, CT_STATIC_TEXT_ENABLE));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Gid));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Direction, CT_COMBO_BOX_LIST));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Type, CT_COMBO_BOX_LIST));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Protocol));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Init, CT_COMBO_BOX_LIST));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_MaxSize));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Timeout));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Clsid));
    g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Connstr));

    //xa节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_Xa, LeafNode()));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_ID, CT_STATIC_TEXT_DISABLE));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_Name, CT_STATIC_TEXT_ENABLE));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_Clsid, CT_COMBO_BOX_EDIT));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaClose));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaOption));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_DaoPath));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaSerial));
    g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaOpen));

    //node节点
    g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_Node, LeafNode()));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_ID, CT_STATIC_TEXT_DISABLE));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Name, CT_STATIC_TEXT_ENABLE));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Type, CT_COMBO_BOX_LIST));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Gid));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Ipv4));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Path));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Use));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_DefaultXa));
    g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_BackupXa));
}

void CmaServerConfigApp::InitSelectItem()
{
    //runtime table clsid
    const std::wstring &RuntimeTableClsid = gl_RuntimeTable + DOT + gp_Clsid;
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaStNode");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaStParam");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaStMsgQueue");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaStEvent");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaStService");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaStSfunc");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaRunLog");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaRtService");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaRtProcess");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaRtThread");
    g_mapSelect[RuntimeTableClsid].push_back(L"CRttSysmaOem");

    //runtime table import file
    const std::wstring& ImportFile = gl_RuntimeTable + DOT + gp_ImportFile;
    g_mapSelect[ImportFile].push_back(L"maServer.xml");
    g_mapSelect[ImportFile].push_back(L"maOem.xml");
    g_mapSelect[ImportFile].push_back(L"maSpd.xml");
    g_mapSelect[ImportFile].push_back(L"maSpdAcct.xml");
    g_mapSelect[ImportFile].push_back(L"maAdapterGsen.xml");
    g_mapSelect[ImportFile].push_back(L"maAdapterKBSS.xml");
    g_mapSelect[ImportFile].push_back(L"maAdapterKDSPB.xml");
    g_mapSelect[ImportFile].push_back(L"maAdapterKDSTG.xml");
    g_mapSelect[ImportFile].push_back(L"maAdapterKGOB.xml");
    g_mapSelect[ImportFile].push_back(L"maAdapterAcct.xml");

    //service clsid
    const std::wstring& ServiceClsid = gl_Service + DOT + gp_Clsid;
    g_mapSelect[ServiceClsid].push_back(L"CService");

    //service runas
    const std::wstring& ServiceRunas = gl_Service + DOT + gp_Runas;
    g_mapSelect[ServiceRunas].push_back(L"process");
    g_mapSelect[ServiceRunas].push_back(L"process2");
    g_mapSelect[ServiceRunas].push_back(L"thread");

    //msgqueue type
    const std::wstring& MsgQueueType = gl_MsgQueue + DOT + gp_Type;
    g_mapSelect[MsgQueueType].push_back(L"kcxp");
    g_mapSelect[MsgQueueType].push_back(L"zmq");
    g_mapSelect[MsgQueueType].push_back(L"socket");
    g_mapSelect[MsgQueueType].push_back(L"shm");
    g_mapSelect[MsgQueueType].push_back(L"mem");
    g_mapSelect[MsgQueueType].push_back(L"kdh");

    //msgqueue init
    const std::wstring& MsgQueueInit = gl_MsgQueue + DOT + gp_Init;
    g_mapSelect[MsgQueueInit].push_back(L"true");
    g_mapSelect[MsgQueueInit].push_back(L"false");

    //msgqueue direction
    const std::wstring& MsgQueueDirection = gl_MsgQueue + DOT + gp_Direction;
    g_mapSelect[MsgQueueDirection].push_back(L"recv");
    g_mapSelect[MsgQueueDirection].push_back(L"send");
    g_mapSelect[MsgQueueDirection].push_back(L"both");

    //XA clsid
    const std::wstring& XAClsid = gl_Xa + DOT + gp_Clsid;
    g_mapSelect[XAClsid].push_back(L"CXaFastDB");
    g_mapSelect[XAClsid].push_back(L"CXaOdbc");
    g_mapSelect[XAClsid].push_back(L"CXaOdbcTimesTen");
    g_mapSelect[XAClsid].push_back(L"CXaOdbcOracle");
    g_mapSelect[XAClsid].push_back(L"CXaDbf");
    g_mapSelect[XAClsid].push_back(L"CXaQueueKcxp");
    g_mapSelect[XAClsid].push_back(L"CXaQueueSocket");
    g_mapSelect[XAClsid].push_back(L"CXaRpcKcbp");
    g_mapSelect[XAClsid].push_back(L"CXaQueueKV");

    //node
    const std::wstring& Node = gl_Node + DOT + gp_Type;
    g_mapSelect[Node].push_back(L"bpu");
    g_mapSelect[Node].push_back(L"mmu");
    g_mapSelect[Node].push_back(L"gtu");
    g_mapSelect[Node].push_back(L"kxp");
    g_mapSelect[Node].push_back(L"stg");
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

    for (std::vector<CComboBox>::size_type index = 0; index != m_vecComboBoxEdit.size(); index++)
    {
        m_vecComboBoxEdit[index]->MoveWindow(0, 0, 0, 0);
        m_vecComboBoxEdit[index]->ShowWindow(SW_HIDE);
        m_vecComboBoxEdit[index]->ResetContent();
    }

    for (std::vector<CComboBox>::size_type index = 0; index != m_vecComboBoxList.size(); index++)
    {
        m_vecComboBoxList[index]->MoveWindow(0, 0, 0, 0);
        m_vecComboBoxList[index]->ShowWindow(SW_HIDE);
        m_vecComboBoxList[index]->ResetContent();
    }

    for (std::vector<CButton>::size_type index = 0; index != m_vecButton.size(); index++)
    {
        m_vecButton[index]->MoveWindow(0, 0, 0, 0);
        m_vecButton[index]->ShowWindow(SW_HIDE);
        m_vecButton[index]->SetWindowTextW(L"");
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
            TargetIndex = static_cast<int>(index);
            return m_vecEditText[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecEditText.push_back(std::make_shared<CEdit>());
    TargetIndex = static_cast<int>(m_vecEditText.size() - 1);

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_VISIBLE | SS_LEFT | ES_AUTOHSCROLL | WS_BORDER;
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
            TargetIndex = static_cast<int>(index);
            return m_vecStaticText[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecStaticText.push_back(std::make_shared<CStatic>());
    TargetIndex = static_cast<int>(m_vecStaticText.size() - 1);

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

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxEdit(CWnd* wnd, int& TargetIndex)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CComboBox>>::size_type index = 0; index < m_vecComboBoxEdit.size(); index++)
    {
        m_vecComboBoxEdit[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetIndex = static_cast<int>(index);
            return m_vecComboBoxEdit[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecComboBoxEdit.push_back(std::make_shared<CComboBox>());
    TargetIndex = static_cast<int>(m_vecComboBoxEdit.size() - 1);

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_OEMCONVERT;
    m_vecComboBoxEdit[TargetIndex]->Create(Style, Rect, wnd, m_NextControlID++);

    return m_vecComboBoxEdit[TargetIndex];
}

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxEdit(int index)
{
    if (index >= 0 && index < m_vecComboBoxEdit.size())
    {
        return m_vecComboBoxEdit[index];
    }

    return std::shared_ptr<CComboBox>();
}

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxList(CWnd* wnd, int& TargetIndex)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CComboBox>>::size_type index = 0; index < m_vecComboBoxList.size(); index++)
    {
        m_vecComboBoxList[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetIndex = static_cast<int>(index);
            return m_vecComboBoxList[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecComboBoxList.push_back(std::make_shared<CComboBox>());
    TargetIndex = static_cast<int>(m_vecComboBoxList.size() - 1);

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OEMCONVERT;
    m_vecComboBoxList[TargetIndex]->Create(Style, Rect, wnd, m_NextControlID++);

    return m_vecComboBoxList[TargetIndex];
}

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxList(int index)
{
    if (index >= 0 && index < m_vecComboBoxList.size())
    {
        return m_vecComboBoxList[index];
    }

    return std::shared_ptr<CComboBox>();
}

std::shared_ptr<CButton> CmaServerConfigApp::GetButton(CWnd* wnd, int& TargetIndex)
{
    CRect Rect;

    //查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CButton>>::size_type index = 0; index < m_vecButton.size(); index++)
    {
        m_vecButton[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetIndex = static_cast<int>(index);
            return m_vecButton[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecButton.push_back(std::make_shared<CButton>());
    TargetIndex = static_cast<int>(m_vecButton.size() - 1);

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | BS_LEFT;
    m_vecButton[TargetIndex]->Create(NULL, Style, Rect, wnd, m_NextControlID++);

    return m_vecButton[TargetIndex];
}

std::shared_ptr<CButton> CmaServerConfigApp::GetButton(int index)
{
    if (index >= 0 && index < m_vecButton.size())
    {
        return m_vecButton[index];
    }

    return std::shared_ptr<CButton>();
}

void CmaServerConfigApp::UpdateEdit(const std::shared_ptr<CEdit>& ptrEdit, ControlType Type)
{
    //文本垂直居中显示，但这样会覆盖掉边框的下划线
    {
        ////获取文本信息
        //TEXTMETRIC tmText;
        //CClientDC dc(ptrEdit.get());
        //dc.GetTextMetrics(&tmText);

        ////计算文本的高度
        //int TextTotalHeight = tmText.tmHeight + tmText.tmExternalLeading;

        ////计算文本距离编辑框顶部的距离
        //CRect rect;
        //ptrEdit->GetClientRect(rect);
        //int OffsetY = (rect.Height() - TextTotalHeight) / 2;
        //

        ////将工作区域往下偏移
        //rect.OffsetRect(0, OffsetY);

        ////设置工作区域
        //ptrEdit->SetRectNP(&rect);
    }

    //设置控件是否可编辑
    {
        if(Type == ControlType::CT_STATIC_TEXT_ENABLE)
        {
            ptrEdit->EnableWindow(TRUE);
        }
        else
        {
            ptrEdit->EnableWindow(FALSE);
        }
    }
}

void CmaServerConfigApp::UpdateComboBox(const std::shared_ptr<CComboBox>& ptrComboBox, const std::wstring& LeafType, const ParamNode& Node)
{
    const std::wstring& Key = LeafType + L"." + Node.m_ParamName;
    if(g_mapSelect.find(Key) == g_mapSelect.end())
    {
        return;
    }

    std::vector<std::wstring>& vecSelect = g_mapSelect[Key];
    for(std::vector<std::wstring>::size_type index = 0; index < vecSelect.size(); index++)
    {
        ptrComboBox->AddString(vecSelect[index].c_str());
    }

    for (std::vector<std::wstring>::size_type index = 0; index < vecSelect.size(); index++)
    {
        if(vecSelect[index] == Node.m_ParamValue)
        {
            ptrComboBox->SetCurSel(static_cast<int>(index));
            break;
        }
    }
}

void CmaServerConfigApp::UpdateListBox(const std::shared_ptr<CButton>& ptrButton, CCheckListBox& ListBox, const std::wstring& LeafType, const ParamNode& Node)
{
    const std::wstring& Key = LeafType + L"." + Node.m_ParamName;
    if (g_mapSelect.find(Key) == g_mapSelect.end())
    {
        return;
    }

    CRect ButtonRect;
    ptrButton->GetWindowRect(ButtonRect);

    CRect ListBoxRect;
    ListBoxRect.left = ButtonRect.left;
    ListBoxRect.right = ButtonRect.right - 4;
    ListBoxRect.top = ButtonRect.top;
    ListBoxRect.bottom = ListBoxRect.top + 300;
    ListBox.MoveWindow(&ListBoxRect);
    ListBox.ShowWindow(SW_SHOW);

    ListBox.ResetContent();
    std::vector<std::wstring>& vecSelect = g_mapSelect[Key];
    for (std::vector<std::wstring>::size_type index = 0; index < vecSelect.size(); index++)
    {
        ListBox.AddString(vecSelect[index].c_str());
    }
}
