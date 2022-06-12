#include "pch.h"
#include "framework.h"
#include "maServerConfig.h"
#include "maServerConfigDlg.h"
#include "CoordinateGenerator.h"
#include "afxdialogex.h"
#include "../Windows/System/SystemShare.h"
#include "../StandardLinux/src/StandardShare.h"
#include "../ProjectLinux/Share/Boost/BoostFileSystem.h"

#include <map>
#include <list>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CmaServerConfigDlg 对话框
CmaServerConfigDlg::CmaServerConfigDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MASERVERCONFIG_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmaServerConfigDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_ComboBoxMa);
    DDX_Control(pDX, IDC_TREE1, m_TreeConfig);
}

BEGIN_MESSAGE_MAP(CmaServerConfigDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_CLOSE()
    ON_WM_PAINT()
    ON_WM_MOVE()
    ON_WM_QUERYDRAGICON()
    ON_CBN_SELCHANGE(IDC_COMBO1, &CmaServerConfigDlg::OnComboBoxConfigChange)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CmaServerConfigDlg::OnTreeConfigChange)
    ON_COMMAND(ID_32771, &CmaServerConfigDlg::OnMenuOpenFile)
    ON_COMMAND(ID_32772, &CmaServerConfigDlg::OnMenuSaveFile)
    ON_COMMAND(ID_32773, &CmaServerConfigDlg::OnMenuDebugInfo)
    ON_COMMAND(ID_32774, &CmaServerConfigDlg::OnMenuAbout)
END_MESSAGE_MAP()


// CmaServerConfigDlg 消息处理程序

BOOL CmaServerConfigDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    {
        m_StatusBar.Create(this);
        m_StatusBar.SetBarStyle(m_StatusBar.GetBarStyle() | CBRS_TOOLTIPS);

        theApp.g_ParamsDlg = std::make_shared<CDialogParams>();
        theApp.g_ParamsDlg->Create(IDD_DIALOG_PARAMS, this);
        theApp.g_ParamsDlg->Hide();

        m_ComboBoxMa.AddString((gm_Construction + COLON + L"构件").c_str());
        m_ComboBoxMa.AddString((gm_Deployment + COLON + L"部署").c_str());
        m_ComboBoxMa.AddString((gm_Kernel + COLON + L"内核").c_str());
        m_ComboBoxMa.AddString((gm_MA + COLON + L"架构").c_str());
        m_ComboBoxMa.SetCurSel(3);

        if (LoadConfigFile(g_XMLFile))
        {
            UpdateTreeConfig();
        }

        InitControlSize();
    }

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CmaServerConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmaServerConfigDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

void CmaServerConfigDlg::OnMove(int x, int y)
{
    //主窗口移动时，也移动子窗口
    if (theApp.g_ParamsDlg != nullptr && ::IsWindow(theApp.g_ParamsDlg->GetSafeHwnd()))
    {
        CRect ParamsGridRect;
        GetDlgItem(IDC_STATIC_CONFIG_CONTEXT)->GetWindowRect(ParamsGridRect);

        CRect ParamsDlgRect;
        ParamsDlgRect.left = ParamsGridRect.left + 1;
        ParamsDlgRect.right = ParamsGridRect.right - 1;
        ParamsDlgRect.top = ParamsGridRect.top + 2 * CoordinateGenerator::GRID_INTERVAL;
        ParamsDlgRect.bottom = ParamsGridRect.bottom - CoordinateGenerator::GRID_INTERVAL;
        theApp.g_ParamsDlg->MoveWindow(&ParamsDlgRect);
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmaServerConfigDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

//配置项选择事件，比如选择ma架构还是kernel
void CmaServerConfigDlg::OnComboBoxConfigChange()
{
    m_TreeConfig.DeleteAllItems();
    UpdateTreeConfig();
}

//响应配置项构成的树的点击事件
void CmaServerConfigDlg::OnTreeConfigChange(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    //获取叶子结点的类型和ID
    HTREEITEM hSelectTreeItem = m_TreeConfig.GetSelectedItem();
    CString CurrentLeafNode = m_TreeConfig.GetItemText(hSelectTreeItem);
    CString CurrentLeafType = theApp.GetLeftType(CurrentLeafNode);
    CString CurrentLeafID = theApp.GetLeafID(CurrentLeafNode);
    bool IsRTDBTrunk = CurrentLeafNode.GetBuffer() == gt_Rtdb;

    //非叶子节点不处理
    if (m_TreeConfig.ItemHasChildren(hSelectTreeItem))
    {
        if (IsRTDBTrunk)
        {
            //rtdb比较特殊，虽然不是叶子节点，但也有配置
            CurrentLeafType = gt_Rtdb.c_str();
        }
        else
        {
            return;
        }
    }

    //保存上一次界面的修改
    theApp.g_ParamsDlg->Hide();
    SaveLastChange();

    //获取从根节点到当前节点的信息
    std::vector<CString> vecString;
    HTREEITEM hParent = NULL, hCurrent = hSelectTreeItem;
    while ((hParent = m_TreeConfig.GetParentItem(hCurrent)) && hParent != NULL)
    {
        vecString.push_back(m_TreeConfig.GetItemText(hParent));
        hCurrent = hParent;
    }

    //组装成要搜索的节点
    CString SearchNode = GetRootNodeName();
    for (std::vector<CString>::reverse_iterator it = vecString.rbegin(); it != vecString.rend(); it++)
    {
        SearchNode.Append(DOT.c_str());
        SearchNode.Append(*it);
    }

    //遍历找到对应的节点
    BOOST_FOREACH(boost::property_tree::wptree::value_type & v1, theApp.g_XMLTree.get_child(SearchNode.GetString()))
    {
        //忽略属性
        if (v1.first.find(L"<") != std::wstring::npos)
        {
            continue;
        }

        if (!IsRTDBTrunk)
        {
            //非叶子节点都有ID
            const std::wstring& ID = v1.second.get<std::wstring>(L"<xmlattr>.id", L"");
            if (ID.empty() || CurrentLeafID.Compare(ID.c_str()) != 0)
            {
                continue;
            }
        }
        else
        {
            //如果是树干，则要匹配名称
            if (CurrentLeafType.Compare(v1.first.c_str()) != 0)
            {
                continue;
            }
        }

        if (theApp.g_mapLeaf.find(CurrentLeafType.GetString()) != theApp.g_mapLeaf.end())
        {
            theApp.ResetAllControl();
            theApp.g_ParamsDlg->DisplayParams(CurrentLeafType.GetString(), v1);
        }
        else
        {
            MessageBox(SearchNode + CurrentLeafType);
        }

        m_LastSearchNode = SearchNode;
        m_LastLeafNode = CurrentLeafNode;
        m_LastLeafID = CurrentLeafID;
        break;
    }
}

void CmaServerConfigDlg::OnMenuOpenFile()
{
    // TODO: 在此添加命令处理程序代码
}

void CmaServerConfigDlg::OnMenuSaveFile()
{
    SaveLastChange();
    theApp.WriteXMLFile(g_XMLFile);
}

void CmaServerConfigDlg::OnMenuDebugInfo()
{
    theApp.WriteLog();
}

void CmaServerConfigDlg::OnMenuAbout()
{
    // TODO: 在此添加命令处理程序代码
}

void CmaServerConfigDlg::InitControlSize()
{
    //先设置对话框最大化
    {
        //方案一：直接最大化，但是会覆盖任务栏
        //ShowWindow(SW_MAXIMIZE);

        //方案二：获取工作区，虽然不会覆盖任务栏，但有空隙
        //CRect WrokRect;
        //SystemParametersInfo(SPI_GETWORKAREA, 0, &WrokRect, 0);
        //MoveWindow(&WrokRect);

        //方案三：获取屏幕分辨率
        int cx = GetSystemMetrics(SM_CXFULLSCREEN);
        int cy = GetSystemMetrics(SM_CYFULLSCREEN);

        CRect WorkRect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkRect, 0);

        cy = WorkRect.bottom;
        this->MoveWindow(0, 0, cx, cy);
    }

    //获取客户区的大小
    CRect ClientRect;
    GetClientRect(ClientRect);

    //状态栏
    CRect StatusBarRect;
    {
        UINT nID[] = { theApp.GetNextControlID() };
        m_StatusBar.SetIndicators(nID, 1);
        m_StatusBar.SetPaneInfo(0, nID[0], SBPS_NOBORDERS, ClientRect.Width());
        //m_StatusBar.SetPaneInfo(1, nID[1], SBPS_STRETCH, StatusBarRect.Width() - TimeWidth);
        m_StatusBar.GetStatusBarCtrl().SetMinHeight(25);
        m_StatusBar.ShowWindow(SW_SHOW);

        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, nID[0]);
        //RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, nID[1]);

        m_StatusBar.SetPaneText(0, L"Tips: 能看见这一行很小的文字吗？");
        //m_StatusBar.SetPaneText(1, L"Test");

        m_StatusBar.GetWindowRect(StatusBarRect);
    }

    //配置项
    CRect ConfigItemRect;
    {
        //配置项的宽度只占可用区域的一份，而且要先减去左右两边的间隔
        LONG ConfigItemWidth = (ClientRect.Width() - 2 * CoordinateGenerator::GRID_INTERVAL) / CoordinateGenerator::SPLIT_WIDTH;

        //设置配置项的区域
        ConfigItemRect.left = ClientRect.left + CoordinateGenerator::GRID_INTERVAL;         //距离左边的间隔
        ConfigItemRect.top = ClientRect.top + CoordinateGenerator::GRID_INTERVAL;           //距离顶部的间隔
        ConfigItemRect.right = ConfigItemRect.left + ConfigItemWidth;                       //配置项的宽度
        ConfigItemRect.bottom = ClientRect.bottom - StatusBarRect.Height()                  //减去状态来的高度
            - CoordinateGenerator::GRID_INTERVAL / 2;                                       //还要剔除间隔，除以2效果好一点
        GetDlgItem(IDC_STATIC_CONFIG_ITEM)->MoveWindow(&ConfigItemRect);

        //设置配置项的ComboBox
        CRect ComboBoxRect;
        GetDlgItem(IDC_COMBO1)->GetWindowRect(ComboBoxRect);
        LONG ComboBoxHeight = ComboBoxRect.Height();
        LONG ComboBoxWidth = ConfigItemRect.Width() - 2 * CoordinateGenerator::GRID_INTERVAL;

        ComboBoxRect.left = ConfigItemRect.left + CoordinateGenerator::GRID_INTERVAL;       //控件与Grid也有同样的间隔
        ComboBoxRect.right = ComboBoxRect.left + ComboBoxWidth;                             //也要考虑左右两边的间隔
        ComboBoxRect.top = ConfigItemRect.top + 2 * CoordinateGenerator::GRID_INTERVAL;     //这里要两倍的间隔，因为有文字遮挡
        ComboBoxRect.bottom = ComboBoxRect.top + ComboBoxHeight;                            //用原本的高度
        GetDlgItem(IDC_COMBO1)->MoveWindow(&ComboBoxRect);

        //设置配置项的树形控件
        CRect TreeRect;
        TreeRect.left = ComboBoxRect.left;                                                  //跟随ComboBox
        TreeRect.right = ComboBoxRect.right;                                                //跟随ComboBox
        TreeRect.top = ComboBoxRect.bottom + CoordinateGenerator::GRID_INTERVAL;            //与ComboBox的间隔
        TreeRect.bottom = ConfigItemRect.bottom - CoordinateGenerator::GRID_INTERVAL;       //跟底部的间隔
        GetDlgItem(IDC_TREE1)->MoveWindow(&TreeRect);
    }

    //配置参数
    {
        CRect ParamsGridRect;
        ParamsGridRect.left = ConfigItemRect.right + CoordinateGenerator::GRID_INTERVAL;    //配置项和配置参数的间隔
        ParamsGridRect.right = ClientRect.right;                                            //直接占用剩余的区域
        ParamsGridRect.top = ConfigItemRect.top;                                            //跟随配置项
        ParamsGridRect.bottom = ConfigItemRect.bottom;                                      //跟随配置项
        GetDlgItem(IDC_STATIC_CONFIG_CONTEXT)->MoveWindow(&ParamsGridRect);

        CRect ParamsDlgRect;
        ParamsDlgRect.left = ParamsGridRect.left + 1;                                      //预留一个像素避免遮挡线条
        ParamsDlgRect.right = ParamsGridRect.right - 1;                                    //预留一个像素避免遮挡线条
        ParamsDlgRect.top = ParamsGridRect.top + 2 * CoordinateGenerator::GRID_INTERVAL;   //这里要两倍的间隔，因为有文字遮挡
        ParamsDlgRect.bottom = ParamsGridRect.bottom - CoordinateGenerator::GRID_INTERVAL; //底部预留一个间隔
        ClientToScreen(&ParamsDlgRect);                                                    //因为是子对话框，因此要转换坐标

        theApp.g_ParamsDlg->MoveWindow(&ParamsDlgRect);
        theApp.g_ParamsDlg->ShowWindow(SW_SHOW);
    }
}

CString CmaServerConfigDlg::GetRootNodeName()
{
    //获取当前文本框里的节点名称
    CString CurrentText;
    m_ComboBoxMa.GetWindowTextW(CurrentText);
    if (CurrentText.IsEmpty())
    {
        return CString();
    }

    //返回的名称要去掉中文
    int pos = CurrentText.Find(L":");
    if (pos <= 0 || pos > CurrentText.GetLength())
    {
        return CString();
    }

    //获取英文描述
    CurrentText = CurrentText.Left(pos);

    //判断选中的是否是ma
    CString Node(L"ma");       //根节点名称


    //如果不是ma，则以ma开头
    if (CurrentText.Compare(Node) != 0)
    {
        Node.Append(DOT.c_str());
        Node.Append(CurrentText);
    }

    //返回ma/ma.kernel/ma.construction/ma.deployment
    return Node;
}

void CmaServerConfigDlg::SaveLastChange()
{
    if (m_LastSearchNode.IsEmpty() || m_LastLeafNode.IsEmpty())
    {
        return;
    }

    //遍历找到对应的节点
    BOOST_FOREACH(boost::property_tree::wptree::value_type & v1, theApp.g_XMLTree.get_child(m_LastSearchNode.GetString()))
    {
        //忽略属性
        if (v1.first.find(L"<") != std::wstring::npos)
        {
            continue;
        }

        //ID不会是空的，除非是rtdb树干
        std::wstring LeafType;
        if (!m_LastLeafID.IsEmpty())
        {
            const std::wstring& ID = v1.second.get<std::wstring>(L"<xmlattr>.id", L"");
            if (ID.empty() || m_LastLeafID.Compare(ID.c_str()))
            {
                continue;
            }

            LeafType = theApp.GetLeftType(m_LastLeafNode);
        }
        else
        {
            if (m_LastLeafNode.Compare(gt_Rtdb.c_str()) == 0)
            {
                LeafType = gt_Rtdb;
            }

            if (LeafType != v1.first)
            {
                continue;
            }
        }

        if (theApp.g_mapLeaf.find(LeafType) != theApp.g_mapLeaf.end())
        {
            theApp.g_ParamsDlg->UpdateParams(LeafType, v1);
        }

        break;
    }
}

bool CmaServerConfigDlg::LoadConfigFile(const std::string& XMLFile)
{
    CString TipMessage;

    //读取当前目录下的文件
    std::vector<std::string> FilesVector;
    const std::string& WorkDirectory = BoostFileSystem::GetWorkDirectory();
    if (!BoostFileSystem::GetDirectoryList(FilesVector, WorkDirectory))
    {
        TipMessage.Format(_T("在路径[%s]下读取文件失败。"), StringToWString(WorkDirectory).c_str());
        MessageBox(TipMessage, _T("提示"));
        return false;
    }

    //在这里找出XML文件，之所以没有在上面匹配XML，是因为不想修改上面函数的原来逻辑
    std::vector<std::string> XMLFilesVector;
    const std::string TargetFile("maServer.xml");
    for (std::vector<std::string>::size_type index = 0; index < FilesVector.size(); index++)
    {
        if (FilesVector[index] == TargetFile)
        {
            XMLFilesVector.push_back(FilesVector[index]);
        }
    }

    if (XMLFilesVector.empty())
    {
        TipMessage.Format(_T("在路径[%s]下读取maServer.xml文件失败。"), StringToWString(WorkDirectory).c_str());
        MessageBox(TipMessage, _T("提示"));
        return false;
    }

    return theApp.ReadXMLFile(XMLFile);
}

bool CmaServerConfigDlg::UpdateTreeConfig()
{
    if (theApp.g_XMLTree.empty())
    {
        return false;
    }

    //确定要搜索的节点
    std::wstring Node = GetRootNodeName().GetString();
    if (Node.empty())
    {
        return false;
    }

    //需要遍历各个子节点，所以需要保存子节点
    std::map<std::wstring, HTREEITEM> TreeMap;
    std::list<std::wstring> NodeList;
    NodeList.push_back(Node);

    //遍历各个子节点
    for (std::list<std::wstring>::iterator it = NodeList.begin(); it != NodeList.end(); it++)
    {
        std::wstring::size_type pos = it->find_last_of(DOT);
        std::wstring Key = it->substr(pos + 1, it->size() - pos + 1);
        BOOST_FOREACH(boost::property_tree::wptree::value_type & v1, theApp.g_XMLTree.get_child(*it))
        {
            //忽略属性
            if (v1.first.find(L"<") != std::wstring::npos)
            {
                continue;
            }

            //可以作为树干
            if (Key.empty() || TreeMap.find(Key) == TreeMap.end())
            {
                TreeMap[v1.first] = m_TreeConfig.InsertItem(v1.first.c_str());
                NodeList.push_back(*it + DOT + v1.first);
                continue;
            }

            //如果ID为空，意味着不是叶子节点
            const std::wstring& ID = v1.second.get<std::wstring>(L"<xmlattr>.id", L"");
            if (ID.empty())
            {
                TreeMap[v1.first] = m_TreeConfig.InsertItem(v1.first.c_str(), 0, 0, TreeMap[Key]);
                NodeList.push_back(*it + DOT + v1.first);
                continue;
            }

            //对于叶子节点，不需要保存上一层的指针了
            const std::wstring& Name = v1.second.get<std::wstring>(L"<xmlattr>.name", L"");
            const std::wstring& TreeName = v1.first + L"_" + ID + L"_" + Name;
            m_TreeConfig.InsertItem(TreeName.c_str(), 0, 0, TreeMap[Key]);

            //保存可选项
            const std::wstring& GID = v1.second.get<std::wstring>(L"<xmlattr>.gid", L"");
            theApp.AddSelectItem(Key, v1.first, ID, GID, Name);
        }
    }

    //展开部分节点
    std::vector<std::wstring> vecExpand = { gm_Kernel ,gm_Deployment ,gm_Construction ,gt_Bbu ,gt_Nodes };
    for (std::map<std::wstring, HTREEITEM>::const_iterator it = TreeMap.begin(); it != TreeMap.end(); it++)
    {
        if (std::find(vecExpand.begin(), vecExpand.end(), it->first) != vecExpand.end())
        {
            m_TreeConfig.Expand(it->second, TVE_EXPAND);
        }
    }

    return true;
}
