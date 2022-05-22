#include "pch.h"
#include "framework.h"
#include "maServerConfig.h"
#include "maServerConfigDlg.h"
#include "../Windows/System/SystemShare.h"
#include "../StandardLinux/src/StandardShare.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#include <fstream>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

    m_NextControlID = IDC_MY_CONTROL;
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
    {
        g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_RuntimeTable, LeafNode()));
        g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_Describe, gl_RuntimeTable, CT_EDIT_TEXT));
        g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_ID, gl_RuntimeTable, CT_STATIC_TEXT));
        g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_Name, gl_RuntimeTable, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_Clsid, gl_RuntimeTable, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_RuntimeTable].m_vecParams.push_back(ParamNode(gp_ImportFile, gl_RuntimeTable, CT_CHECK_LIST_BOX));
    }

    //Service节点
    {
        g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_Service, LeafNode()));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Describe, gl_Service, CT_EDIT_TEXT));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_ID, gl_Service, CT_STATIC_TEXT));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Name, gl_Service, CT_EDIT_TEXT));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Clsid, gl_Service, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Gid, gl_Service, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Runas, gl_Service, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_WorkThread, gl_Service, CT_EDIT_TEXT));
        g_mapLeaf[gl_Service].m_vecParams.push_back(ParamNode(gp_Use, gl_Service, CT_CHECK_LIST_BOX));
    }

    //msgqueue节点
    {
        g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_MsgQueue, LeafNode()));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Describe, gl_MsgQueue, CT_EDIT_TEXT));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_ID, gl_MsgQueue, CT_STATIC_TEXT));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Name, gl_MsgQueue, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Gid, gl_MsgQueue, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Direction, gl_MsgQueue, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Type, gl_MsgQueue, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Protocol, gl_MsgQueue, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Init, gl_MsgQueue, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_MaxSize, gl_MsgQueue, CT_EDIT_TEXT));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Timeout, gl_MsgQueue, CT_EDIT_TEXT));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Clsid, gl_MsgQueue, CT_STATIC_TEXT));
        g_mapLeaf[gl_MsgQueue].m_vecParams.push_back(ParamNode(gp_Connstr, gl_MsgQueue, CT_MIXED_BOX_QUEUE));
    }

    //xa节点
    {
        g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_Xa, LeafNode()));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_Describe, gl_Xa, CT_EDIT_TEXT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_ID, gl_Xa, CT_STATIC_TEXT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_Name, gl_Xa, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_Clsid, gl_Xa, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaClose, gl_Xa, CT_STATIC_TEXT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaOption, gl_Xa, CT_STATIC_TEXT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_DaoPath, gl_Xa, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaSerial, gl_Xa, CT_STATIC_TEXT));
        g_mapLeaf[gl_Xa].m_vecParams.push_back(ParamNode(gp_XaOpen, gl_Xa, CT_MIXED_BOX_XA));
    }

    //node节点
    {
        g_mapLeaf.insert(std::pair<std::wstring, LeafNode>(gl_Node, LeafNode()));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Describe, gl_Node, CT_EDIT_TEXT));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_ID, gl_Node, CT_STATIC_TEXT));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Name, gl_Node, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Type, gl_Node, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Gid, gl_Node, CT_COMBO_BOX_EDIT));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Ipv4, gl_Node, CT_EDIT_TEXT));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Path, gl_Node, CT_EDIT_TEXT));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_DefaultXa, gl_Node, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_BackupXa, gl_Node, CT_COMBO_BOX_LIST));
        g_mapLeaf[gl_Node].m_vecParams.push_back(ParamNode(gp_Use, gl_Node, CT_CHECK_BOX));
    }
}

void CmaServerConfigApp::InitSelectItem()
{
    //runtime table
    {
        //clsid
        const std::wstring& RuntimeTableClsid = gl_RuntimeTable + DOT + gp_Clsid;
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaStNode");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaStParam");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaStMsgQueue");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaStEvent");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaStService");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaStSfunc");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaRunLog");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaRtService");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaRtProcess");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaRtThread");
        g_MultiSelect[RuntimeTableClsid].push_back(L"CRttSysmaOem");

        //import file
        const std::wstring& ImportFile = gl_RuntimeTable + DOT + gp_ImportFile;
        g_MultiSelect[ImportFile].push_back(L"maServer.xml");
        g_MultiSelect[ImportFile].push_back(L"maOem.xml");
        g_MultiSelect[ImportFile].push_back(L"maSpd.xml");
        g_MultiSelect[ImportFile].push_back(L"maSpdAcct.xml");
        g_MultiSelect[ImportFile].push_back(L"maAdapterGsen.xml");
        g_MultiSelect[ImportFile].push_back(L"maAdapterKBSS.xml");
        g_MultiSelect[ImportFile].push_back(L"maAdapterKDSPB.xml");
        g_MultiSelect[ImportFile].push_back(L"maAdapterKDSTG.xml");
        g_MultiSelect[ImportFile].push_back(L"maAdapterKGOB.xml");
        g_MultiSelect[ImportFile].push_back(L"maAdapterAcct.xml");
    }

    //service
    {
        //clsid
        const std::wstring& ServiceClsid = gl_Service + DOT + gp_Clsid;
        g_MultiSelect[ServiceClsid].push_back(L"CService");

        //runas
        const std::wstring& ServiceRunas = gl_Service + DOT + gp_Runas;
        g_MultiSelect[ServiceRunas].push_back(L"process");
        g_MultiSelect[ServiceRunas].push_back(L"process2");
        g_MultiSelect[ServiceRunas].push_back(L"thread");
    }

    //queues
    {
        //type
        const std::wstring& MsgQueueType = gl_MsgQueue + DOT + gp_Type;
        g_MultiSelect[MsgQueueType].push_back(gs_QueueType_kcxp);
        g_MultiSelect[MsgQueueType].push_back(gs_QueueType_zmq);
        g_MultiSelect[MsgQueueType].push_back(L"socket");
        g_MultiSelect[MsgQueueType].push_back(L"shm");
        g_MultiSelect[MsgQueueType].push_back(L"mem");
        g_MultiSelect[MsgQueueType].push_back(L"kdh");

        //clsid
        const std::wstring& MsgClsID = gl_MsgQueue + DOT + gp_Clsid;
        g_SingleSelect[MsgClsID][gs_QueueType_kcxp] = L"CMsgQueueKcxp";
        g_SingleSelect[MsgClsID][gs_QueueType_zmq] = L"CMsgQueueZmq";
        g_SingleSelect[MsgClsID][gs_QueueType_socket] = L"CMsgQueueSocket";
        g_SingleSelect[MsgClsID][gs_QueueType_shm] = L"CMsgQueueShm";
        g_SingleSelect[MsgClsID][gs_QueueType_mem] = L"CMsgQueueStl";
        g_SingleSelect[MsgClsID][gs_QueueType_kdh] = L"CMsgQueueKdh";

        //connstr default value
        const std::wstring& MsgConnstr = gl_MsgQueue + DOT + gp_Connstr;
        g_SingleSelect[MsgConnstr][gs_QueueType_kcxp] = L"21000/TCP/192.168.0.2/5";
        g_SingleSelect[MsgConnstr][gs_QueueType_zmq] = L"30001/SUB/EPGM/@@IP;192.168.0.2/5/300000/2";
        g_SingleSelect[MsgConnstr][gs_QueueType_socket] = L"41002/SERVER/TCP/192.168.0.2/8";
        g_SingleSelect[MsgConnstr][gs_QueueType_shm] = L"";
        g_SingleSelect[MsgConnstr][gs_QueueType_mem] = L"";
        g_SingleSelect[MsgConnstr][gs_QueueType_kdh] = L"";

        //max_size
        const std::wstring& MsgMaxSize = gl_MsgQueue + DOT + gp_MaxSize;
        g_SingleSelect[MsgMaxSize][gs_QueueType_kcxp] = L"";
        g_SingleSelect[MsgMaxSize][gs_QueueType_zmq] = L"90000";
        g_SingleSelect[MsgMaxSize][gs_QueueType_socket] = L"10000";
        g_SingleSelect[MsgMaxSize][gs_QueueType_shm] = L"10000";
        g_SingleSelect[MsgMaxSize][gs_QueueType_mem] = L"10000";
        g_SingleSelect[MsgMaxSize][gs_QueueType_kdh] = L"10000";

        //socket mode
        const std::wstring& MsgqueueSocketMode = gl_MsgQueue + DOT + gs_QueueType_socket + DOT + gs_QueueTypeSocket_Mode;
        g_MultiSelect[MsgqueueSocketMode].push_back(L"SERVER");
        g_MultiSelect[MsgqueueSocketMode].push_back(L"CLIENT");

        //zmq mode
        const std::wstring& MsgqueueZmqMode = gl_MsgQueue + DOT + gs_QueueType_zmq + DOT + gs_QueueTypeZmq_Mode;
        g_MultiSelect[MsgqueueZmqMode].push_back(L"SUB");
        g_MultiSelect[MsgqueueZmqMode].push_back(L"PUB");
        g_MultiSelect[MsgqueueZmqMode].push_back(L"REQ");
        g_MultiSelect[MsgqueueZmqMode].push_back(L"REP");

        //zmq protocol
        const std::wstring& MsgqueueZmqProtocol = gl_MsgQueue + DOT + gs_QueueType_zmq + DOT + gs_QueueTypeZmq_Protocol;
        g_MultiSelect[MsgqueueZmqProtocol].push_back(L"EPGM");
        g_MultiSelect[MsgqueueZmqProtocol].push_back(L"TCP");

        //protocol
        const std::wstring& MsgQueueProtoclo = gl_MsgQueue + DOT + gp_Protocol;
        g_MultiSelect[MsgQueueProtoclo].push_back(L"kmap");
        g_MultiSelect[MsgQueueProtoclo].push_back(L"kcbp");
        g_MultiSelect[MsgQueueProtoclo].push_back(L"gsen");
        g_MultiSelect[MsgQueueProtoclo].push_back(L"tds");

        //init
        const std::wstring& MsgQueueInit = gl_MsgQueue + DOT + gp_Init;
        g_MultiSelect[MsgQueueInit].push_back(L"true");
        g_MultiSelect[MsgQueueInit].push_back(L"false");

        //direction
        const std::wstring& MsgQueueDirection = gl_MsgQueue + DOT + gp_Direction;
        g_MultiSelect[MsgQueueDirection].push_back(L"recv");
        g_MultiSelect[MsgQueueDirection].push_back(L"send");
        g_MultiSelect[MsgQueueDirection].push_back(L"both");
    }

    //xas
    {
        //clsid
        const std::wstring& XAClsid = gl_Xa + DOT + gp_Clsid;
        g_MultiSelect[XAClsid].push_back(L"CXaFastDB");
        g_MultiSelect[XAClsid].push_back(L"CXaOdbc");
        g_MultiSelect[XAClsid].push_back(L"CXaOdbcTimesTen");
        g_MultiSelect[XAClsid].push_back(L"CXaOdbcOracle");
        g_MultiSelect[XAClsid].push_back(L"CXaDbf");
        g_MultiSelect[XAClsid].push_back(L"CXaQueueKcxp");
        g_MultiSelect[XAClsid].push_back(L"CXaQueueSocket");
        g_MultiSelect[XAClsid].push_back(L"CXaRpcKcbp");
        g_MultiSelect[XAClsid].push_back(L"CXaQueueKV");
        g_MultiSelect[XAClsid].push_back(L"CXaExtremeDB");
        g_MultiSelect[XAClsid].push_back(L"CXaQueueShm");
        g_MultiSelect[XAClsid].push_back(L"CXaQueueTgwShBinary");
        g_MultiSelect[XAClsid].push_back(L"CXaQueueTgw");

        //daopath
        const std::wstring& XADaoPath = gl_Xa + DOT + gp_DaoPath;
        g_MultiSelect[XADaoPath].push_back(L"./biz/mssql");
        g_MultiSelect[XADaoPath].push_back(L"./biz/oracle");
        g_MultiSelect[XADaoPath].push_back(L"./biz/extremedb");
        g_MultiSelect[XADaoPath].push_back(L"./biz/fastdb");
        g_MultiSelect[XADaoPath].push_back(L"./biz/timesten");
        g_MultiSelect[XADaoPath].push_back(L"./kbss/mssql");

        //database
        const std::wstring& XADatabase = gl_Xa + DOT + gs_XADatabase;
        g_MultiSelect[XADatabase].push_back(L"kbss_stds");
        g_MultiSelect[XADatabase].push_back(L"kbss_base");
        g_MultiSelect[XADatabase].push_back(L"kbss_stds_hist");
        g_MultiSelect[XADatabase].push_back(L"Ashare_OIW");

        //log to file
        const std::wstring& XALogToFile = gl_Xa + DOT + gs_XALogToFile;
        g_MultiSelect[XALogToFile].push_back(L"true");
        g_MultiSelect[XALogToFile].push_back(L"false");

        //user
        const std::wstring& XAUser = gl_Xa + DOT + gs_XAUser;
        g_MultiSelect[XAUser].push_back(L"sa");
        g_MultiSelect[XAUser].push_back(L"root");

        //pwd
        const std::wstring& XAPwd = gl_Xa + DOT + gs_XAPassword;
        g_MultiSelect[XAPwd].push_back(L""); //提供一个空值

        //dbms
        const std::wstring& XADBMS = gl_Xa + DOT + gs_XADbms;
        g_MultiSelect[XADBMS].push_back(L"mssql2008");
        g_MultiSelect[XADBMS].push_back(L"mssql2014");
        g_MultiSelect[XADBMS].push_back(L"mssql2016");
    }

    //nodes
    {
        //type
        const std::wstring& Node = gl_Node + DOT + gp_Type;
        g_MultiSelect[Node].push_back(L"bpu");
        g_MultiSelect[Node].push_back(L"mmu");
        g_MultiSelect[Node].push_back(L"gtu");
        g_MultiSelect[Node].push_back(L"kxp");
        g_MultiSelect[Node].push_back(L"stg");
    }
}

void CmaServerConfigApp::WriteLog()
{
    std::wfstream output(L"maServerConfigDebug.log", std::ios::out);
    if (!output)
    {
        return;
    }

    //multi select item
    {
        output << L"====================multi select item begin====================" << std::endl << std::endl;
        std::map<std::wstring, std::vector<std::wstring>>::const_iterator it = g_MultiSelect.begin();
        while (it != g_MultiSelect.end())
        {
            output << it->first << COLON << std::endl;
            for (std::vector<std::wstring>::size_type index = 0; index < it->second.size(); index++)
            {
                output << L"-----" << it->second[index] << std::endl;
            }

            output << std::endl;
            it++;
        }
        output << L"====================multi select item end======================" << std::endl << std::endl;
    }

    //single select item
    {
        output << L"====================single select item begin====================" << std::endl << std::endl;
        std::map<std::wstring, std::map<std::wstring, std::wstring>>::const_iterator OutIt = g_SingleSelect.begin();
        while (OutIt != g_SingleSelect.end())
        {
            output << OutIt->first << COLON << std::endl;
            std::map<std::wstring, std::wstring>::const_iterator InIt = OutIt->second.begin();
            while (InIt != OutIt->second.end())
            {
                output << L"-----" << InIt->first << L":" << InIt->second << std::endl;
                InIt++;
            }

            output << std::endl;
            OutIt++;
        }
        output << L"====================single select item end======================" << std::endl << std::endl;
    }

    //CRect
    {
        output << std::endl << L"====================coordinate begin====================" << std::endl;
        std::map<std::wstring, CRect>::const_iterator it = g_mapRect.begin();
        while (it != g_mapRect.end())
        {
            output << it->first << COLON << std::endl;
            output << L"LEFT  =" << it->second.left << std::endl;
            output << L"RIGHT =" << it->second.right << std::endl;
            output << L"TOP   =" << it->second.top << std::endl;
            output << L"BOTTOM=" << it->second.bottom << std::endl << std::endl;
            it++;
        }
        output << std::endl << L"====================coordinate end======================" << std::endl;
    }
}

void CmaServerConfigApp::AddCRectToLog(const std::wstring& Key, const CRect& TempRect)
{
    //const std::wstring& RealKey = std::to_wstring(theApp.g_mapRect.size()) + Key;
    theApp.g_mapRect.insert(std::pair<std::wstring, CRect>(Key, TempRect));
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

ControlType CmaServerConfigApp::GetParamValueType(const std::wstring& Name)
{
    static std::vector<std::vector<std::wstring>> vecControlName(CT_COUNT, std::vector<std::wstring>());
    {
        if (vecControlName[CT_COMBO_BOX_EDIT].empty())
        {
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XADatabase);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XADsn);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XADbms);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAQueName);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAReqName);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAAnsName);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAQueID);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAMaxDepth);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAMsgQueueID);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAUser);
            vecControlName[CT_COMBO_BOX_EDIT].push_back(gs_XAPassword);
        }

        if (vecControlName[CT_COMBO_BOX_LIST].empty())
        {
            vecControlName[CT_COMBO_BOX_LIST].push_back(gs_XALogToFile);
            vecControlName[CT_COMBO_BOX_LIST].push_back(gs_XAServID);
        }
    }

    //先寻找完全匹配的类型
    for (std::vector<std::vector<std::wstring>>::size_type Type = 0; Type < vecControlName.size(); Type++)
    {
        for (std::vector<std::wstring>::size_type index = 0; index < vecControlName[Type].size(); index++)
        {
            if (vecControlName[Type][index] == Name)
            {
                return ControlType(Type);
            }
        }
    }

    //然后寻找模糊匹配的类型
    for (std::vector<std::vector<std::wstring>>::size_type Type = 0; Type < vecControlName.size(); Type++)
    {
        for (std::vector<std::wstring>::size_type index = 0; index < vecControlName[Type].size(); index++)
        {
            if (Name.find(vecControlName[Type][index]) != std::wstring::npos)
            {
                return ControlType(Type);
            }
        }
    }

    //最后，默认都为可编辑
    return CT_EDIT_TEXT;
}

std::wstring CmaServerConfigApp::GetSelectItemMapKey(const std::wstring& Key)
{
    if (g_mapKeyConvert.empty())
    {
        const std::wstring& XAID = gl_Xa + DOT + gp_ID;
        const std::wstring& QueueID = gl_MsgQueue + DOT + gp_ID;
        const std::wstring& QueueName = gl_MsgQueue + DOT + gp_Name;

        g_mapKeyConvert[gl_Node + DOT + gp_DefaultXa] = XAID;
        g_mapKeyConvert[gl_Node + DOT + gp_BackupXa] = XAID;
        g_mapKeyConvert[gl_MsgQueue] = QueueID;
        g_mapKeyConvert[gs_XAQueID] = QueueID;
        g_mapKeyConvert[gl_Xa + DOT + gs_XAQueID] = QueueID;
        g_mapKeyConvert[gl_Xa + DOT + gs_XAMsgQueueID] = QueueID;
        g_mapKeyConvert[gl_Xa + DOT + gs_XADsn] = gl_Xa + DOT + gs_XADatabase;
        g_mapKeyConvert[gs_XAQueName] = QueueName;
        g_mapKeyConvert[gl_Xa + DOT + gs_XAQueName] = QueueName;
        g_mapKeyConvert[gl_Xa + DOT + gs_XAReqName] = QueueName;
        g_mapKeyConvert[gl_Xa + DOT + gs_XAAnsName] = QueueName;
    }

    //完全匹配
    if (g_mapKeyConvert.find(Key) != g_mapKeyConvert.end())
    {
        return g_mapKeyConvert[Key];
    }

    //模糊匹配
    for(std::map<std::wstring, std::wstring>::const_iterator it = g_mapKeyConvert.begin(); it != g_mapKeyConvert.end(); it++)
    {
        if(Key.find(it->first) != std::wstring::npos)
        {
            return it->second;
        }
    }

    //否则返回空值
    return std::wstring();
}

void CmaServerConfigApp::AddSelectItem(const std::wstring& Trunk, const std::wstring& Leaf, const std::wstring& ID, const std::wstring& GID, const std::wstring& Name)
{
    //ID
    {
        const std::wstring& NewIDKey = Leaf + DOT + gp_ID;
        std::vector<std::wstring>& IDItems = g_MultiSelect[NewIDKey];
        if (std::find(IDItems.begin(), IDItems.end(), ID) == IDItems.end())
        {
            g_MultiSelect[NewIDKey].push_back(ID);
        }
    }

    //GID
    {
        if(!GID.empty())
        {
            const std::wstring& GIDKey = Leaf + DOT + gp_Gid;
            std::vector<std::wstring>& GIDItems = g_MultiSelect[GIDKey];
            if (std::find(GIDItems.begin(), GIDItems.end(), GID) == GIDItems.end())
            {
                g_MultiSelect[GIDKey].push_back(GID);
            }
        }
    }

    //Name
    {
        const std::wstring& NewNameKey = Leaf + DOT + gp_Name;
        std::vector<std::wstring>& NameItems = g_MultiSelect[NewNameKey];
        if (std::find(NameItems.begin(), NameItems.end(), Name) == NameItems.end())
        {
            g_MultiSelect[NewNameKey].push_back(Name);
        }
    }
}

void CmaServerConfigApp::ResetAllControl()
{
    CRect Zero(0, 0, 0, 0);
    for (std::vector<CEdit>::size_type index = 0; index != m_vecEditText.size(); index++)
    {
        m_vecEditText[index]->MoveWindow(&Zero);
        m_vecEditText[index]->ShowWindow(SW_HIDE);
    }

    for (std::vector<CStatic>::size_type index = 0; index != m_vecStaticText.size(); index++)
    {
        m_vecStaticText[index]->MoveWindow(&Zero);
        m_vecStaticText[index]->ShowWindow(SW_HIDE);
    }

    for (std::vector<CComboBox>::size_type index = 0; index != m_vecComboBoxEdit.size(); index++)
    {
        m_vecComboBoxEdit[index]->MoveWindow(&Zero);
        m_vecComboBoxEdit[index]->ShowWindow(SW_HIDE);
        m_vecComboBoxEdit[index]->ResetContent();
    }

    for (std::vector<CComboBox>::size_type index = 0; index != m_vecComboBoxList.size(); index++)
    {
        m_vecComboBoxList[index]->MoveWindow(&Zero);
        m_vecComboBoxList[index]->ShowWindow(SW_HIDE);
        m_vecComboBoxList[index]->ResetContent();
    }

    for (std::vector<CButton>::size_type index = 0; index != m_vecButton.size(); index++)
    {
        m_vecButton[index]->MoveWindow(&Zero);
        m_vecButton[index]->ShowWindow(SW_HIDE);
        m_vecButton[index]->SetWindowTextW(L"");
    }

    for (std::vector<CButton>::size_type index = 0; index != m_vecCheckBox.size(); index++)
    {
        m_vecCheckBox[index]->MoveWindow(&Zero);
        m_vecCheckBox[index]->ShowWindow(SW_HIDE);
        m_vecCheckBox[index]->SetWindowTextW(L"");
    }
}

void CmaServerConfigApp::ResetSubParamsControl()
{
    CRect Zero(0, 0, 0, 0);
    for (std::set<UINT>::const_iterator it = g_setSubParams.begin(); it != g_setSubParams.end(); it++)
    {
        CWnd* Control = g_ParamsDlg->GetDlgItem(*it);
        if (Control != NULL)
        {
            Control->MoveWindow(&Zero);
            Control->ShowWindow(SW_HIDE);
        }
    }

    g_setSubParams.clear();
}

std::shared_ptr<CEdit> CmaServerConfigApp::GetEditText(CWnd* wnd, UINT& TargetID)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CEdit>>::size_type index = 0; index < m_vecEditText.size(); index++)
    {
        m_vecEditText[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetID = m_vecEditText[index]->GetDlgCtrlID();
            return m_vecEditText[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecEditText.push_back(std::make_shared<CEdit>());

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_VISIBLE | SS_LEFT | ES_AUTOHSCROLL | WS_BORDER;
    m_vecEditText[m_vecEditText.size() - 1]->Create(Style, Rect, wnd, m_NextControlID++);

    TargetID = m_vecEditText[m_vecEditText.size() - 1]->GetDlgCtrlID();
    return m_vecEditText[m_vecEditText.size() - 1];
}

std::shared_ptr<CEdit> CmaServerConfigApp::GetEditText(UINT TargetID)
{
    for (std::vector<CEdit>::size_type index = 0; index < m_vecEditText.size(); index++)
    {
        if (m_vecEditText[index]->GetDlgCtrlID() == TargetID)
        {
            return m_vecEditText[index];
        }
    }

    return std::shared_ptr<CEdit>();
}

std::shared_ptr<CStatic> CmaServerConfigApp::GetStaticText(CWnd* wnd, UINT& TargetID)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CStatic>>::size_type index = 0; index < m_vecStaticText.size(); index++)
    {
        m_vecStaticText[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetID = m_vecStaticText[index]->GetDlgCtrlID();
            return m_vecStaticText[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecStaticText.push_back(std::make_shared<CStatic>());

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE;
    m_vecStaticText[m_vecStaticText.size() - 1]->Create(NULL, Style, Rect, wnd, m_NextControlID++);

    TargetID = m_vecStaticText[m_vecStaticText.size() - 1]->GetDlgCtrlID();
    return m_vecStaticText[m_vecStaticText.size() - 1];
}

std::shared_ptr<CStatic> CmaServerConfigApp::GetStaticText(UINT TargetID)
{
    for (std::vector<CStatic>::size_type index = 0; index < m_vecStaticText.size(); index++)
    {
        if (m_vecStaticText[index]->GetDlgCtrlID() == TargetID)
        {
            return m_vecStaticText[index];
        }
    }

    return std::shared_ptr<CStatic>();
}

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxEdit(CWnd* wnd, UINT& TargetID)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CComboBox>>::size_type index = 0; index < m_vecComboBoxEdit.size(); index++)
    {
        m_vecComboBoxEdit[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetID = m_vecComboBoxEdit[index]->GetDlgCtrlID();
            return m_vecComboBoxEdit[index];
        }
    }

    //找不到时创建一个新的控件
    m_vecComboBoxEdit.push_back(std::make_shared<CComboBox>());

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_OEMCONVERT;
    m_vecComboBoxEdit[m_vecComboBoxEdit.size() - 1]->Create(Style, Rect, wnd, m_NextControlID++);

    TargetID = m_vecComboBoxEdit[m_vecComboBoxEdit.size() - 1]->GetDlgCtrlID();
    return m_vecComboBoxEdit[m_vecComboBoxEdit.size() - 1];
}

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxEdit(UINT TargetID)
{
    for (std::vector<CComboBox>::size_type index = 0; index < m_vecComboBoxEdit.size(); index++)
    {
        if (m_vecComboBoxEdit[index]->GetDlgCtrlID() == TargetID)
        {
            return m_vecComboBoxEdit[index];
        }
    }

    return std::shared_ptr<CComboBox>();
}

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxList(CWnd* wnd, UINT& TargetID)
{
    CRect Rect;

    //先查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CComboBox>>::size_type index = 0; index < m_vecComboBoxList.size(); index++)
    {
        m_vecComboBoxList[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetID = m_vecComboBoxList[index]->GetDlgCtrlID();
            return m_vecComboBoxList[index];
        }
    }

    UINT NewID = IDC_COMBOX_LIST_1;
    if (!m_vecComboBoxList.empty())
    {
        NewID = m_vecComboBoxList[m_vecComboBoxList.size() - 1]->GetDlgCtrlID() + 1;
    }

    //找不到时创建一个新的控件
    m_vecComboBoxList.push_back(std::make_shared<CComboBox>());

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OEMCONVERT | WS_VSCROLL;
    m_vecComboBoxList[m_vecComboBoxList.size() - 1]->Create(Style, Rect, wnd, NewID);
    TargetID = m_vecComboBoxList[m_vecComboBoxList.size() - 1]->GetDlgCtrlID();

    return m_vecComboBoxList[m_vecComboBoxList.size() - 1];
}

std::shared_ptr<CComboBox> CmaServerConfigApp::GetComboBoxList(UINT TargetID)
{
    for (std::vector<CComboBox>::size_type index = 0; index < m_vecComboBoxList.size(); index++)
    {
        if (m_vecComboBoxList[index]->GetDlgCtrlID() == TargetID)
        {
            return m_vecComboBoxList[index];
        }
    }

    return std::shared_ptr<CComboBox>();
}

std::shared_ptr<CButton> CmaServerConfigApp::GetButton(CWnd* wnd, UINT& TargetID)
{
    CRect Rect;

    //查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CButton>>::size_type index = 0; index < m_vecButton.size(); index++)
    {
        m_vecButton[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetID = m_vecButton[index]->GetDlgCtrlID();
            return m_vecButton[index];
        }
    }

    UINT NewID = IDC_BUTTON_START;
    if (!m_vecButton.empty())
    {
        NewID = m_vecButton[m_vecButton.size() - 1]->GetDlgCtrlID() + 1;
    }

    //找不到时创建一个新的控件
    m_vecButton.push_back(std::make_shared<CButton>());
    std::vector<CButton>::size_type NewIndex = m_vecButton.size() - 1;

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | BS_LEFT/* | BS_MULTILINE*/;
    m_vecButton[NewIndex]->Create(NULL, Style, Rect, wnd, NewID);
    TargetID = m_vecButton[NewIndex]->GetDlgCtrlID();

    return m_vecButton[NewIndex];
}

std::shared_ptr<CButton> CmaServerConfigApp::GetButton(UINT TargetID)
{
    for (std::vector<CButton>::size_type index = 0; index < m_vecButton.size(); index++)
    {
        if (m_vecButton[index]->GetDlgCtrlID() == TargetID)
        {
            return m_vecButton[index];
        }
    }

    return std::shared_ptr<CButton>();
}

std::shared_ptr<CButton> CmaServerConfigApp::GetCheckBox(CWnd* wnd, UINT& TargetID)
{
    CRect Rect;

    //查找已经创建但还未使用的控件
    for (std::vector<std::shared_ptr<CButton>>::size_type index = 0; index < m_vecCheckBox.size(); index++)
    {
        m_vecCheckBox[index]->GetClientRect(Rect);
        if (Rect.left == 0 && Rect.right == 0 && Rect.top == 0 && Rect.bottom == 0)
        {
            TargetID = m_vecCheckBox[index]->GetDlgCtrlID();
            return m_vecCheckBox[index];
        }
    }

    UINT NewID = IDC_CHECK_BOX_START;
    if (!m_vecCheckBox.empty())
    {
        NewID = m_vecCheckBox[m_vecCheckBox.size() - 1]->GetDlgCtrlID() + 1;
    }

    //找不到时创建一个新的控件
    m_vecCheckBox.push_back(std::make_shared<CButton>());

    //设置新控件的属性
    Rect.left = Rect.right = Rect.top = Rect.bottom = 0;
    DWORD Style = WS_CHILD | WS_VISIBLE | BS_LEFT | BS_AUTOCHECKBOX | BS_VCENTER;
    m_vecCheckBox[m_vecCheckBox.size() - 1]->Create(NULL, Style, Rect, wnd, NewID);
    TargetID = m_vecCheckBox[m_vecCheckBox.size() - 1]->GetDlgCtrlID();

    return m_vecCheckBox[m_vecCheckBox.size() - 1];
}

std::shared_ptr<CButton> CmaServerConfigApp::GetCheckBox(UINT TargetID)
{
    for (std::vector<CButton>::size_type index = 0; index < m_vecCheckBox.size(); index++)
    {
        if (m_vecCheckBox[index]->GetDlgCtrlID() == TargetID)
        {
            return m_vecCheckBox[index];
        }
    }

    return std::shared_ptr<CButton>();
}
