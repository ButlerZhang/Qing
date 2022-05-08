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
    DDX_Control(pDX, IDC_LIST1, m_CheckListBox);
    DDX_Control(pDX, IDC_STATIC_COMPLEX, m_GridSubParams);
}

BEGIN_MESSAGE_MAP(CmaServerConfigDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_CLOSE()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CmaServerConfigDlg::OnBnClickedOk)
    ON_CLBN_CHKCHANGE(IDC_LIST1, &CmaServerConfigDlg::OnCheckListBoxChange)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CmaServerConfigDlg::OnComboBoxConfigChange)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_1, &CmaServerConfigDlg::OnComboBoxListSelectChange1)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_2, &CmaServerConfigDlg::OnComboBoxListSelectChange2)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_3, &CmaServerConfigDlg::OnComboBoxListSelectChange3)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_4, &CmaServerConfigDlg::OnComboBoxListSelectChange4)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_5, &CmaServerConfigDlg::OnComboBoxListSelectChange5)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_6, &CmaServerConfigDlg::OnComboBoxListSelectChange6)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_7, &CmaServerConfigDlg::OnComboBoxListSelectChange7)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_8, &CmaServerConfigDlg::OnComboBoxListSelectChange8)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_9, &CmaServerConfigDlg::OnComboBoxListSelectChange9)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_10, &CmaServerConfigDlg::OnComboBoxListSelectChange10)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CmaServerConfigDlg::OnTreeConfigChange)
    ON_BN_CLICKED(ID_BUTTON_GENERATE, &CmaServerConfigDlg::OnBnGenerateConfigFile)
    ON_COMMAND_RANGE(IDC_BUTTON_START, IDC_BUTTON_STOP, &CmaServerConfigDlg::OnBnClicked)
    ON_COMMAND_RANGE(IDC_CHECK_BOX_START, IDC_CHECK_BOX_STOP, &CmaServerConfigDlg::OnCheckBoxClicked)
    ON_BN_CLICKED(ID_BUTTON_OPEN_FILE, &CmaServerConfigDlg::OnBnOpenConfigFile)
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
        theApp.g_ParamsDlg = std::make_shared<CDialogParams>();
        theApp.g_ParamsDlg->Create(IDD_DIALOG_PARAMS, this);

        m_ComboBoxMa.AddString((gm_Construction + COLON + L"构件").c_str());
        m_ComboBoxMa.AddString((gm_Deployment + COLON + L"部署").c_str());
        m_ComboBoxMa.AddString((gm_Kernel + COLON + L"内核").c_str());
        m_ComboBoxMa.AddString((gm_MA + COLON + L"架构").c_str());
        m_ComboBoxMa.SetCurSel(3);

        if (LoadConfigFile(g_XMLFile))
        {
            UpdateTreeConfig();
        }

        m_GridSubParams.ShowWindow(SW_HIDE);
        m_CheckListBox.ShowWindow(SW_HIDE);
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmaServerConfigDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

//OK按钮响应事件，暂未启用
void CmaServerConfigDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialogEx::OnOK();
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

    //非叶子节点不处理
    HTREEITEM hSelectTreeItem = m_TreeConfig.GetSelectedItem();
    if (m_TreeConfig.ItemHasChildren(hSelectTreeItem))
    {
        return;
    }

    //保存上一次界面的修改
    m_GridSubParams.ShowWindow(SW_HIDE);
    SaveLastChange();

    //获取叶子结点的类型和ID
    CString CurrentLeafNode = m_TreeConfig.GetItemText(hSelectTreeItem);
    CString CurrentLeafType = theApp.GetLeftType(CurrentLeafNode);
    CString CurrentLeafID = theApp.GetLeafID(CurrentLeafNode);

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

        //ID不会是空的
        const std::wstring& ID = v1.second.get<std::wstring>(L"<xmlattr>.id", L"");
        if (ID.empty() || CurrentLeafID.Compare(ID.c_str()) != 0)
        {
            continue;
        }

        if (theApp.g_mapLeaf.find(CurrentLeafType.GetString()) != theApp.g_mapLeaf.end())
        {
            theApp.ResetAllControl();
            m_CheckListBox.ShowWindow(SW_HIDE);
            DisplayParams(CurrentLeafType.GetString(), v1);
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

//点击Button弹出CheckListBox或由多个控件组成的子配置项
void CmaServerConfigDlg::OnBnClicked(UINT uID)
{
    const std::shared_ptr<CButton>& ClickButton = theApp.GetButton(uID);
    if (ClickButton == nullptr)
    {
        return;
    }

    if (m_CheckListBox.IsWindowVisible())
    {
        m_CheckListBox.ShowWindow(SW_HIDE);
        return;
    }

    CString LeafType = theApp.GetLeftType(m_LastLeafNode);
    const std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType.GetBuffer()].m_vecParams;
    for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
    {
        if (vecParams[index].m_ParamValueID != uID)
        {
            continue;
        }

        switch (vecParams[index].m_ParamValueType)
        {
        case CT_CHECK_LIST_BOX:
        {
            UpdateCheckListBox(ClickButton, LeafType.GetBuffer(), vecParams[index]);
            break;
        }
        case CT_CHECK_BOX:
        {
            const std::wstring& ParamValue = theApp.g_mapLeaf[LeafType.GetBuffer()].GetParamValue(gp_Type);
            UpdateNodeUse(ClickButton, LeafType.GetBuffer(), ParamValue);
            break;
        }
        case CT_MIXED_BOX_QUEUE:
        {
            const std::wstring& ParamValue = theApp.g_mapLeaf[LeafType.GetBuffer()].GetParamValue(gp_Type);
            UpdateQueueConnstr(ClickButton, LeafType.GetBuffer(), ParamValue);
            break;
        }
        case CT_MIXED_BOX_XA:
        {
            UpdateXaOpen(ClickButton, LeafType.GetBuffer());
            break;
        }
        default:
            break;
        }

        break;
    }
}

//CheckListBox选择响应事件
void CmaServerConfigDlg::OnCheckListBoxChange()
{
    //找到按钮ID
    UINT ButtonID = 0;
    CString LeafType = theApp.GetLeftType(m_LastLeafNode);
    const std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType.GetBuffer()].m_vecParams;
    for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
    {
        if (vecParams[index].m_ParamValueType == CT_CHECK_LIST_BOX)
        {
            ButtonID = vecParams[index].m_ParamValueID;
            break;
        }
    }

    //如果没有创建这个按钮，也不允许操作
    std::shared_ptr<CButton> CurrentButton = theApp.GetButton(ButtonID);
    if (CurrentButton == nullptr)
    {
        return;
    }

    //获取按钮的文本
    CString ButtonText;
    CurrentButton->GetWindowTextW(ButtonText);

    //先拆分已有的文本
    std::vector<std::wstring> vecExistText;
    SplitString(ButtonText.GetBuffer(), vecExistText, SEMICOLON);

    //根据选中状态更新文本
    CString SelectText;
    for (int index = 0; index < m_CheckListBox.GetCount(); index++)
    {
        m_CheckListBox.GetText(index, SelectText);
        std::vector<std::wstring>::const_iterator it = std::find(vecExistText.begin(), vecExistText.end(), SelectText.GetBuffer());

        if (m_CheckListBox.GetCheck(index) == BST_CHECKED)
        {
            if (it == vecExistText.end())
            {
                vecExistText.push_back(SelectText.GetBuffer());
            }
        }
        else
        {
            if (it != vecExistText.end())
            {
                vecExistText.erase(it);
            }
        }
    }

    //组装文本
    ButtonText.Empty();
    for (std::vector<std::wstring>::size_type index = 0; index < vecExistText.size(); index++)
    {
        //以分号分隔
        if (!ButtonText.IsEmpty())
        {
            ButtonText.Append(SEMICOLON.c_str());
        }

        //按顺序添加到末尾
        ButtonText.Append(vecExistText[index].c_str());
    }

    //显示文本
    CurrentButton->SetWindowTextW(ButtonText);
}

//勾选框点击事件
void CmaServerConfigDlg::OnCheckBoxClicked(UINT uID)
{
    CString LeafType = theApp.GetLeftType(m_LastLeafNode);
    if (theApp.g_mapLeaf.find(LeafType.GetString()) == theApp.g_mapLeaf.end())
    {
        return;
    }

    LeafNode& LNode = theApp.g_mapLeaf[LeafType.GetBuffer()];
    const std::vector<ParamNode>& vecParams = LNode.m_vecParams;
    for (std::vector<ParamNode>::size_type ParamIndex = 0; ParamIndex < vecParams.size(); ParamIndex++)
    {
        if (vecParams[ParamIndex].m_ParamName != gp_Use)
        {
            continue;
        }

        const std::shared_ptr<CButton>& ClickCheckBox = theApp.GetCheckBox(uID);
        if (ClickCheckBox == nullptr)
        {
            break;
        }

        CString ClickCheckBoxText;
        ClickCheckBox->GetWindowTextW(ClickCheckBoxText);
        UINT ParamsID = LNode.GetParamValueID(vecParams[ParamIndex].m_ParamName);
        const std::vector<ParamNode>& subParams = LNode.m_subParams[ParamsID];

        //选中了n/s/a的某一个时，另外两个要取消选中
        if (ClickCheckBox->GetCheck() == 1 && ClickCheckBoxText.Find(gs_NodeUse_q[0]) < 0)
        {
            for (std::vector<ParamNode>::size_type SubIndex = 0; SubIndex < subParams.size(); SubIndex++)
            {
                if (subParams[SubIndex].m_ParamNameID == uID)
                {
                    continue;
                }

                if (subParams[SubIndex].m_ParamName == gs_NodeUse_q)
                {
                    continue;
                }

                const std::shared_ptr<CButton>& CheckBox = theApp.GetCheckBox(subParams[SubIndex].m_ParamNameID);
                if (CheckBox == nullptr)
                {
                    continue;
                }

                CheckBox->SetCheck(0);
            }
        }

        //组装成字符串，设置到按钮文本上
        CString NewText;
        for (std::vector<ParamNode>::size_type SubIndex = 0; SubIndex < subParams.size(); SubIndex++)
        {
            const std::shared_ptr<CButton>& CheckBox = theApp.GetCheckBox(subParams[SubIndex].m_ParamNameID);
            if (CheckBox == nullptr)
            {
                continue;
            }

            if (CheckBox->GetCheck() == 1)
            {
                NewText.Append(subParams[SubIndex].m_ParamName.c_str());
            }
        }

        const std::shared_ptr<CButton>& UseButton = theApp.GetButton(vecParams[ParamIndex].m_ParamValueID);
        if (UseButton != nullptr)
        {
            UseButton->SetWindowTextW(NewText);
        }

        break;
    }
}

//ComboBoxList选择响应事件，预定10个
void CmaServerConfigDlg::OnComboBoxListSelectChange(UINT uID)
{
    //获取当前文本框里的节点名称
    std::shared_ptr<CComboBox> ComboxList = theApp.GetComboBoxList(uID);
    if (ComboxList == nullptr)
    {
        return;
    }

    CString CurrentText;
    ComboxList->GetWindowTextW(CurrentText);
    if (CurrentText.IsEmpty())
    {
        return;
    }

    //deployment.nodes节点
    {
        const std::wstring& NODES = gm_MA + DOT + gm_Deployment + DOT + gt_Nodes;
        if (m_LastSearchNode.Compare(NODES.c_str()) == 0)
        {
            CString LeafType = theApp.GetLeftType(m_LastLeafNode);
            if (theApp.g_mapLeaf.find(LeafType.GetString()) != theApp.g_mapLeaf.end())
            {
                const std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType.GetBuffer()].m_vecParams;
                for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
                {
                    if (vecParams[index].m_ParamValueID == uID && vecParams[index].m_ParamName == gp_Type)
                    {
                        UINT UseID = theApp.g_mapLeaf[LeafType.GetBuffer()].GetParamValueID(gp_Use);
                        const std::shared_ptr<CButton>& Button = theApp.GetButton(UseID);
                        UpdateNodeUse(Button, LeafType.GetBuffer(), CurrentText.GetBuffer());
                        return;
                    }
                }
            }
        }
    }

    //construction.bpu.queues节点
    {
        const std::wstring& Queues = gm_MA + DOT + gm_Construction + DOT + gt_Bbu + DOT + gt_Queues;
        if (m_LastSearchNode.Compare(Queues.c_str()) == 0)
        {
            CString LeafType = theApp.GetLeftType(m_LastLeafNode);
            if (theApp.g_mapLeaf.find(LeafType.GetString()) != theApp.g_mapLeaf.end())
            {
                std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType.GetBuffer()].m_vecParams;
                for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
                {
                    if (vecParams[index].m_ParamValueID == uID && vecParams[index].m_ParamName == gp_Type)
                    {
                        UpdateQueueType(LeafType.GetBuffer(), vecParams[index]);

                        std::vector<ParamNode>::size_type ConnstrIndex = theApp.g_mapLeaf[LeafType.GetBuffer()].GetIndex(gp_Connstr);
                        const std::shared_ptr<CButton>& ConnstrButton = theApp.GetButton(vecParams[ConnstrIndex].m_ParamValueID);
                        UpdateQueueConnstr(ConnstrButton, LeafType.GetBuffer(), vecParams[index].m_ParamValue);
                        return;
                    }
                }
            }
        }
    }
}

//生成配置文件事件
void CmaServerConfigDlg::OnBnGenerateConfigFile()
{
    SaveLastChange();
    theApp.WriteXMLFile(g_XMLFile);
}

void CmaServerConfigDlg::OnBnOpenConfigFile()
{
    theApp.WriteSelectItemToLog();
}

void CmaServerConfigDlg::InitControlSize()
{
    //先设置对话框最大化
    ShowWindow(SW_MAXIMIZE);

    //各个控件根据这些常量来划分位置
    const int GridInterval = 10;    //各个控件的间隔
    const int SPLIT_WIDTH = 1 + 3;  //宽度拆分成SPLIT_WIDTH份，配置项占1，配置参数占3
    const int SPLIT_HEIGT = 15 + 1; //高度拆分成SPLIT_HEIGT份，配置项占15，按钮占1

    //获取客户区的大小
    CRect ClientRect;
    GetClientRect(ClientRect);

    //配置项
    CRect ConfigItemRect;
    {
        //设置配置项的区域
        ConfigItemRect.left = ClientRect.left + GridInterval;
        ConfigItemRect.top = ClientRect.top + GridInterval;
        ConfigItemRect.right = ConfigItemRect.left + (ClientRect.Width() - 2 * GridInterval) / SPLIT_WIDTH;
        ConfigItemRect.bottom = ConfigItemRect.top + (ClientRect.Height() - 2 * GridInterval) / SPLIT_HEIGT * (SPLIT_HEIGT - 1);
        GetDlgItem(IDC_STATIC_CONFIG_ITEM)->MoveWindow(&ConfigItemRect);

        //设置配置项的ComboBox
        CRect ComboBoxRect;
        GetDlgItem(IDC_COMBO1)->GetWindowRect(ComboBoxRect);
        long ComboBoxHeight = ComboBoxRect.Height();

        ComboBoxRect.left = ConfigItemRect.left + GridInterval;
        ComboBoxRect.right = ComboBoxRect.left + (ConfigItemRect.Width() - 2 * GridInterval);
        ComboBoxRect.top = ConfigItemRect.top + GridInterval * 2;
        ComboBoxRect.bottom = ComboBoxRect.top + ComboBoxHeight;
        GetDlgItem(IDC_COMBO1)->MoveWindow(&ComboBoxRect);

        //设置配置项的树形控件
        CRect TreeRect;
        TreeRect.left = ComboBoxRect.left;
        TreeRect.right = ComboBoxRect.right;
        TreeRect.top = ComboBoxRect.bottom + GridInterval / 2;
        TreeRect.bottom = ConfigItemRect.bottom - GridInterval;
        GetDlgItem(IDC_TREE1)->MoveWindow(&TreeRect);
    }

    //配置参数
    {
        CRect ParamsGridRect;
        ParamsGridRect.left = ConfigItemRect.right + GridInterval;
        ParamsGridRect.right = ClientRect.right;
        ParamsGridRect.top = ClientRect.top + GridInterval;
        ParamsGridRect.bottom = ConfigItemRect.bottom;
        GetDlgItem(IDC_STATIC_CONFIG_CONTEXT)->MoveWindow(&ParamsGridRect);

        CRect ParamsDlgRect;
        ParamsDlgRect.left = ParamsGridRect.left + 1;
        ParamsDlgRect.right = ParamsGridRect.right - 1;
        ParamsDlgRect.top = ParamsGridRect.top + GridInterval * 2;
        ParamsDlgRect.bottom = ParamsGridRect.bottom - GridInterval;
        ClientToScreen(&ParamsDlgRect);

        theApp.g_ParamsDlg->MoveWindow(&ParamsDlgRect);
        theApp.g_ParamsDlg->ShowWindow(SW_SHOW);
    }

    //按钮区域
    {
        //设置按钮区域
        CRect ButtonAreaRect;
        ButtonAreaRect.left = ConfigItemRect.left;
        ButtonAreaRect.right = ClientRect.right - GridInterval;
        ButtonAreaRect.top = ConfigItemRect.bottom + GridInterval / 2;
        ButtonAreaRect.bottom = ClientRect.bottom - GridInterval / 2;
        GetDlgItem(IDC_STATIC_BUTTON_AREA)->MoveWindow(&ButtonAreaRect);

        //设置按钮的布局
        {
            //需要实现的按钮
            std::vector<int> vecID = { ID_BUTTON_LOAD, ID_BUTTON_GENERATE, ID_BUTTON_OPEN_FILE };
            const int BUTTON_COUNT = static_cast<int>(vecID.size());

            //计算按钮的高度
            const int BUTTON_MIN_HEIGHT = 50;
            LONG HeightUsable = ButtonAreaRect.Height() - GridInterval; //上下空开一些像素
            int ButtonHeight = HeightUsable > BUTTON_MIN_HEIGHT ? BUTTON_MIN_HEIGHT : HeightUsable;
            int ButtonY = ButtonAreaRect.top + (ButtonAreaRect.Height() - ButtonHeight) / 2;

            //计算按钮的宽度
            const int BUTTON_MIN_WIDTH = 200;
            int ButtonWidth = 0, LastButtonX = 0, ButtonInterval = 0;
            if (BUTTON_COUNT * BUTTON_MIN_WIDTH < ButtonAreaRect.Width())
            {
                ButtonWidth = BUTTON_MIN_WIDTH;
                ButtonInterval = (ButtonAreaRect.Width() - BUTTON_COUNT * ButtonWidth) / BUTTON_COUNT;
                LastButtonX = ButtonAreaRect.left + ButtonInterval / 2; //几个按钮几个空格，其中一个空格分成两半，作为左右两边与Grid的间隔
            }
            else
            {
                ButtonInterval = 1;
                ButtonWidth = (ButtonAreaRect.Width() - BUTTON_COUNT * ButtonInterval) / BUTTON_COUNT; //按钮的宽度
                LastButtonX = ButtonAreaRect.left + ButtonInterval;
            }

            //设置按钮区域
            CRect ButtonRect;
            ButtonRect.top = ButtonY + GridInterval / 2;
            ButtonRect.bottom = ButtonRect.top + ButtonHeight;
            for (std::vector<int>::size_type index = 0; index < vecID.size(); index++)
            {
                ButtonRect.left = LastButtonX;
                ButtonRect.right = ButtonRect.left + ButtonWidth;
                LastButtonX = ButtonRect.right + ButtonInterval;
                GetDlgItem(vecID[index])->MoveWindow(&ButtonRect);
            }
        }
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
    if (!m_LastSearchNode.IsEmpty() && !m_LastLeafNode.IsEmpty() && !m_LastLeafID.IsEmpty())
    {
        //遍历找到对应的节点
        BOOST_FOREACH(boost::property_tree::wptree::value_type & v1, theApp.g_XMLTree.get_child(m_LastSearchNode.GetString()))
        {
            //忽略属性
            if (v1.first.find(L"<") != std::wstring::npos)
            {
                continue;
            }

            //ID不会是空的
            const std::wstring& ID = v1.second.get<std::wstring>(L"<xmlattr>.id", L"");
            if (ID.empty() || m_LastLeafID.Compare(ID.c_str()))
            {
                continue;
            }

            CString LeafType = theApp.GetLeftType(m_LastLeafNode);
            if (theApp.g_mapLeaf.find(LeafType.GetString()) != theApp.g_mapLeaf.end())
            {
                UpdateParams(LeafType.GetString(), v1);
            }

            break;
        }
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

            //可以作为树形里的根节点
            if (Key.empty() || TreeMap.find(Key) == TreeMap.end())
            {
                TreeMap[v1.first] = m_TreeConfig.InsertItem(v1.first.c_str());
                std::wstring NewNode = *it + L".";
                NewNode.append(v1.first);
                NodeList.push_back(NewNode);
                continue;
            }

            //如果ID为空，意味着不是叶子节点
            const std::wstring& ID = v1.second.get<std::wstring>(L"<xmlattr>.id", L"");
            if (ID.empty())
            {
                TreeMap[v1.first] = m_TreeConfig.InsertItem(v1.first.c_str(), 0, 0, TreeMap[Key]);
                std::wstring NewNode = *it + L".";
                NewNode.append(v1.first);
                NodeList.push_back(NewNode);
            }
            else
            {
                //对于叶子节点，不需要保存上一层的指针了
                const std::wstring& Name = v1.second.get<std::wstring>(L"<xmlattr>.name", L"");
                std::wstring TreeName = v1.first + L"_" + ID + L"_" + Name;
                m_TreeConfig.InsertItem(TreeName.c_str(), 0, 0, TreeMap[Key]);
                theApp.AddSelectItem(Key, ID);
            }
        }

        //判断是否要展开这个树形层级
        int counter = static_cast<int>(std::count(it->begin(), it->end(), DOT[0]));
        if (counter <= 2 && TreeMap.find(Key) != TreeMap.end())
        {
            m_TreeConfig.Expand(TreeMap[Key], TVE_EXPAND);
        }
    }

    return true;
}

void CmaServerConfigDlg::UpdateParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode)
{
    CString ControlName, ControlValue, FormatString;
    const std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType].m_vecParams;
    for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
    {
        const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(vecParams[index].m_ParamNameID);
        if (StaticText == nullptr)
        {
            continue;
        }

        StaticText->GetWindowTextW(ControlName);
        int pos = ControlName.Find(COLON.c_str());
        ControlName.Delete(pos, 1);
        if (ControlName.Compare(vecParams[index].m_ParamName.c_str()) != 0)
        {
            continue;
        }

        switch (vecParams[index].m_ParamValueType)
        {
        case CT_COMBO_BOX_EDIT:
        {
            theApp.GetComboBoxEdit(vecParams[index].m_ParamValueID)->GetWindowTextW(ControlValue);
            break;
        }
        case CT_COMBO_BOX_LIST:
        {
            theApp.GetComboBoxList(vecParams[index].m_ParamValueID)->GetWindowTextW(ControlValue);
            break;
        }
        case CT_CHECK_LIST_BOX:
        case CT_CHECK_BOX:
        case CT_MIXED_BOX_XA:
        case CT_MIXED_BOX_QUEUE:
        {
            theApp.GetButton(vecParams[index].m_ParamValueID)->GetWindowTextW(ControlValue);
            break;
        }
        default:
        {
            theApp.GetEditText(vecParams[index].m_ParamValueID)->GetWindowTextW(ControlValue);
            break;
        }
        }

        if (ControlValue.Compare(vecParams[index].m_ParamName.c_str()) != 0)
        {
            FormatString.Format(L"<xmlattr>.%s", vecParams[index].m_ParamName.c_str());
            LeafNode.second.put<std::wstring>(FormatString.GetString(), ControlValue.GetString());
        }
    }
}

void CmaServerConfigDlg::DisplayParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode)
{
    CRect ParamsArea;
    GetDlgItem(IDC_STATIC_CONFIG_CONTEXT)->GetWindowRect(ParamsArea);

    CoordinateGenerator GeneratorRect(ParamsArea);
    GeneratorRect.Begin(1, 2);

    CString FormatString;
    std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType].m_vecParams;
    for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
    {
        FormatString.Format(L"<xmlattr>.%s", vecParams[index].m_ParamName.c_str());
        const std::wstring& Text = vecParams[index].m_ParamName + COLON;
        const std::wstring& Value = LeafNode.second.get<std::wstring>(FormatString.GetString(), L"");

        const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(this, vecParams[index].m_ParamNameID);
        StaticText->MoveWindow(GeneratorRect.GetNextRect());
        StaticText->SetWindowTextW(Text.c_str());
        StaticText->ShowWindow(SW_SHOW);

        vecParams[index].m_ParamValue = Value;
        const CRect& NewRect = GeneratorRect.GetNextRect();

        switch (vecParams[index].m_ParamValueType)
        {
        case CT_COMBO_BOX_EDIT:
        {
            const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxEdit(this, vecParams[index].m_ParamValueID);
            ComboBox->MoveWindow(NewRect);
            ComboBox->ShowWindow(SW_SHOW);
            UpdateComboBox(ComboBox, LeafType, vecParams[index]);
            break;
        }
        case CT_COMBO_BOX_LIST:
        {
            const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxList(this, vecParams[index].m_ParamValueID);
            ComboBox->MoveWindow(NewRect);
            ComboBox->ShowWindow(SW_SHOW);
            UpdateComboBox(ComboBox, LeafType, vecParams[index]);
            break;
        }
        case CT_CHECK_LIST_BOX:
        {
            const std::shared_ptr<CButton>& Button = theApp.GetButton(this, vecParams[index].m_ParamValueID);
            Button->MoveWindow(NewRect);
            Button->SetWindowTextW(Value.c_str());
            Button->ShowWindow(SW_SHOW);
            Button->EnableWindow(TRUE);
            break;
        }
        case CT_CHECK_BOX:
        {
            const std::shared_ptr<CButton>& Button = theApp.GetButton(this, vecParams[index].m_ParamValueID);
            Button->MoveWindow(NewRect);
            Button->SetWindowTextW(Value.c_str());
            Button->ShowWindow(SW_SHOW);

            UINT TypeID = theApp.g_mapLeaf[LeafType].GetParamValueID(gp_Type);
            std::shared_ptr<CComboBox> ComboxList = theApp.GetComboBoxList(TypeID);

            CString TypeText;
            ComboxList->GetWindowTextW(TypeText);
            Button->EnableWindow(TypeText.Find(gt_Bbu.c_str()) >= 0);
            break;
        }
        case CT_MIXED_BOX_XA:
        case CT_MIXED_BOX_QUEUE:
        {
            const std::shared_ptr<CButton>& Button = theApp.GetButton(this, vecParams[index].m_ParamValueID);
            Button->MoveWindow(NewRect);
            Button->SetWindowTextW(Value.c_str());
            Button->ShowWindow(SW_SHOW);
            break;
        }
        default:
        {
            const std::shared_ptr<CEdit>& EditText = theApp.GetEditText(this, vecParams[index].m_ParamValueID);
            EditText->MoveWindow(NewRect);
            EditText->SetWindowTextW(Value.c_str());
            EditText->ShowWindow(SW_SHOW);
            UpdateEdit(EditText, vecParams[index].m_ParamValueType);
            break;
        }
        }
    }

    //调整node节点中use和defaultxa/backupxa的位置
    if (LeafType == gl_Node)
    {
        CRect pUseRect, pDefaultRect, pBackupRect, vUseRect, vDefaultRect, vBackupRect;
        std::shared_ptr<CStatic> pUse, pDefault, pBackup;
        std::shared_ptr<CComboBox> vDefault, vBackup;
        std::shared_ptr<CButton> vUse;
        for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
        {
            if (vecParams[index].m_ParamName == gp_Use)
            {
                pUse = theApp.GetStaticText(vecParams[index].m_ParamNameID);
                vUse = theApp.GetButton(vecParams[index].m_ParamValueID);
                pUse->GetWindowRect(pUseRect);
                vUse->GetWindowRect(vUseRect);
                continue;
            }

            if (vecParams[index].m_ParamName == gp_DefaultXa)
            {
                pDefault = theApp.GetStaticText(vecParams[index].m_ParamNameID);
                vDefault = theApp.GetComboBoxList(vecParams[index].m_ParamValueID);
                pDefault->GetWindowRect(pDefaultRect);
                vDefault->GetWindowRect(vDefaultRect);
                continue;
            }

            if (vecParams[index].m_ParamName == gp_BackupXa)
            {
                pBackup = theApp.GetStaticText(vecParams[index].m_ParamNameID);
                vBackup = theApp.GetComboBoxList(vecParams[index].m_ParamValueID);
                pBackup->GetWindowRect(pBackupRect);
                vBackup->GetWindowRect(vBackupRect);
                continue;
            }
        }

        if (pUseRect.top < pDefaultRect.top && pDefaultRect.top < pBackupRect.top)
        {
            //GetWindowRect获取的是屏幕坐标，要转换成对话框坐标
            ScreenToClient(&pUseRect);
            ScreenToClient(&vUseRect);
            pDefault->MoveWindow(&pUseRect);
            vDefault->MoveWindow(&vUseRect);

            ScreenToClient(&pDefaultRect);
            ScreenToClient(&vDefaultRect);
            pBackup->MoveWindow(&pDefaultRect);
            vBackup->MoveWindow(&vDefaultRect);

            ScreenToClient(&pBackupRect);
            ScreenToClient(&vBackupRect);
            pUse->MoveWindow(&pBackupRect);
            vUse->MoveWindow(&vBackupRect);
        }
    }
}

void CmaServerConfigDlg::UpdateEdit(const std::shared_ptr<CEdit>& ptrEdit, ControlType Type)
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
        if (Type == ControlType::CT_EDIT_TEXT)
        {
            ptrEdit->EnableWindow(TRUE);
        }
        else
        {
            ptrEdit->EnableWindow(FALSE);
        }
    }
}

void CmaServerConfigDlg::UpdateQueueType(const std::wstring& LeafType, ParamNode& Node)
{
    const std::shared_ptr<CComboBox>& TypeBox = theApp.GetComboBoxList(Node.m_ParamValueID);
    if (TypeBox == nullptr)
    {
        return;
    }

    CString TypeText;
    TypeBox->GetWindowTextW(TypeText);
    if (TypeText.IsEmpty())
    {
        return;
    }

    std::wstring Type(TypeText.GetBuffer());
    Node.m_ParamValue = Type;

    UINT MaxSizeID = theApp.g_mapLeaf[LeafType].GetParamValueID(gp_MaxSize);
    const std::shared_ptr<CEdit>& MaxSizeEdit = theApp.GetEditText(MaxSizeID);
    if (MaxSizeEdit != nullptr && theApp.g_mapQueueMaxSize.find(Type) != theApp.g_mapQueueMaxSize.end())
    {
        MaxSizeEdit->SetWindowTextW(theApp.g_mapQueueMaxSize[Type].c_str());
    }

    UINT ClsidID = theApp.g_mapLeaf[LeafType].GetParamValueID(gp_Clsid);
    const std::shared_ptr<CEdit>& ClsidStatic = theApp.GetEditText(ClsidID);
    if (ClsidStatic != nullptr && theApp.g_mapQueueClsid.find(Type) != theApp.g_mapQueueClsid.end())
    {
        ClsidStatic->SetWindowTextW(theApp.g_mapQueueClsid[Type].c_str());
    }

    UINT ConnstrID = theApp.g_mapLeaf[LeafType].GetParamValueID(gp_Connstr);
    const std::shared_ptr<CButton>& ConnstrButton = theApp.GetButton(ConnstrID);
    if (ConnstrButton != nullptr && theApp.g_mapQueueConnstr.find(Type) != theApp.g_mapQueueConnstr.end())
    {
        ConnstrButton->SetWindowTextW(theApp.g_mapQueueConnstr[Type].c_str());
        ConnstrButton->EnableWindow(!theApp.g_mapQueueConnstr[Type].empty());
    }
}

void CmaServerConfigDlg::UpdateQueueConnstr(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType, const std::wstring& Type)
{
    //如果当前可见，点击按钮就隐藏
    if (m_GridSubParams.IsWindowVisible())
    {
        m_GridSubParams.ShowWindow(SW_HIDE);
        theApp.ResetSubParamsControl();
        return;
    }

    CString ButtonText;
    pButton->GetWindowTextW(ButtonText);
    if (ButtonText.IsEmpty())
    {
        return;
    }

    std::vector<std::wstring> SubParamsValue;
    SplitString(ButtonText.GetBuffer(), SubParamsValue, SLASH);
    if (SubParamsValue.empty())
    {
        return;
    }

    //准备数据
    UINT ButtonID = pButton->GetDlgCtrlID();
    std::vector<ParamNode>& SubParamsNode = theApp.g_mapLeaf[LeafType].m_subParams[ButtonID];
    SubParamsNode.clear();

    if (Type == gs_QueueType_kcxp)
    {
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_Port, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_Protocol, CT_STATIC_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_IP, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_ConnectCount, CT_EDIT_TEXT));
    }
    else if (Type == gs_QueueType_zmq)
    {
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_Port, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_Mode, CT_COMBO_BOX_LIST));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_Protocol, CT_COMBO_BOX_LIST));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_IP, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_ThreadCount, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_CacheTime, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_HopCount, CT_EDIT_TEXT));
    }
    else if (Type == gs_QueueType_socket)
    {
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_Port, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_Mode, CT_COMBO_BOX_LIST));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_Protocol, CT_STATIC_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_IP, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_MsgLength, CT_EDIT_TEXT));
    }

    if (SubParamsValue.size() != SubParamsNode.size())
    {
        return;
    }

    //设置Group Static的区域
    CRect ButtonRect;
    CRect SubParamGridRect;
    {
        pButton->GetWindowRect(ButtonRect);
        SubParamGridRect.left = ButtonRect.left;
        SubParamGridRect.right = ButtonRect.right - 2;
        SubParamGridRect.top = ButtonRect.top + 10;
        SubParamGridRect.bottom = SubParamGridRect.top + 300;
        m_GridSubParams.MoveWindow(SubParamGridRect);
        m_GridSubParams.ShowWindow(SW_SHOW);
        m_GridSubParams.SetWindowTextW((gp_Connstr + L"子配置项").c_str());
    }

    //设置各个子配置项的位置
    {
        CoordinateGenerator GeneratorRect(SubParamGridRect);
        GeneratorRect.m_TopGrap = CoordinateGenerator::OFFSET_Y * 6;
        GeneratorRect.SetSplitRatio(2, 7, 1);
        GeneratorRect.Begin(1, 2);

        for (std::vector<ParamNode>::size_type index = 0; index < SubParamsNode.size(); index++)
        {
            const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(this, SubParamsNode[index].m_ParamNameID);
            StaticText->SetWindowTextW((SubParamsNode[index].m_ParamName + COLON).c_str());
            StaticText->MoveWindow(GeneratorRect.GetNextRect());
            StaticText->ShowWindow(SW_SHOW);

            SubParamsNode[index].m_ParamValue = SubParamsValue[index];
            const CRect& NewRect = GeneratorRect.GetNextRect();

            switch (SubParamsNode[index].m_ParamValueType)
            {
            case CT_COMBO_BOX_EDIT:
            {
                const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxEdit(this, SubParamsNode[index].m_ParamValueID);
                ComboBox->MoveWindow(NewRect);
                ComboBox->ShowWindow(SW_SHOW);
                UpdateComboBox(ComboBox, LeafType, SubParamsNode[index]);
                break;
            }
            case CT_COMBO_BOX_LIST:
            {
                const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxList(this, SubParamsNode[index].m_ParamValueID);
                ComboBox->MoveWindow(NewRect);
                ComboBox->ShowWindow(SW_SHOW);
                UpdateComboBox(ComboBox, LeafType + DOT + Type, SubParamsNode[index]);
                break;
            }
            case CT_CHECK_LIST_BOX:
            {
                const std::shared_ptr<CButton>& Button = theApp.GetButton(this, SubParamsNode[index].m_ParamValueID);
                Button->MoveWindow(NewRect);
                Button->SetWindowTextW(SubParamsValue[index].c_str());
                Button->ShowWindow(SW_SHOW);
                Button->EnableWindow(TRUE);
                break;
            }
            case CT_CHECK_BOX:
            {
                const std::shared_ptr<CButton>& Button = theApp.GetButton(this, SubParamsNode[index].m_ParamValueID);
                Button->MoveWindow(NewRect);
                Button->SetWindowTextW(SubParamsValue[index].c_str());
                Button->ShowWindow(SW_SHOW);

                UINT TypeID = theApp.g_mapLeaf[LeafType].GetParamValueID(gp_Type);
                std::shared_ptr<CComboBox> ComboxList = theApp.GetComboBoxList(TypeID);

                CString TypeText;
                ComboxList->GetWindowTextW(TypeText);
                Button->EnableWindow(TypeText.Find(gt_Bbu.c_str()) >= 0);
                break;
            }
            default:
            {
                const std::shared_ptr<CEdit>& EditText = theApp.GetEditText(this, SubParamsNode[index].m_ParamValueID);
                EditText->MoveWindow(NewRect);
                EditText->SetWindowTextW(SubParamsValue[index].c_str());
                EditText->ShowWindow(SW_SHOW);
                UpdateEdit(EditText, SubParamsNode[index].m_ParamValueType);
                break;
            }
            }

            theApp.g_setSubParams.insert(SubParamsNode[index].m_ParamNameID);
            theApp.g_setSubParams.insert(SubParamsNode[index].m_ParamValueID);
        }
    }
}

void CmaServerConfigDlg::UpdateXaOpen(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType)
{
    //如果当前可见，点击按钮就隐藏
    if (m_GridSubParams.IsWindowVisible())
    {
        m_GridSubParams.ShowWindow(SW_HIDE);
        theApp.ResetSubParamsControl();
        return;
    }

    CString ButtonText;
    pButton->GetWindowTextW(ButtonText);
    if (ButtonText.IsEmpty())
    {
        return;
    }

    std::vector<std::wstring> SubParamsValue;
    SplitString(ButtonText.GetBuffer(), SubParamsValue, SEMICOLON);
    if (SubParamsValue.empty())
    {
        return;
    }

    //设置Group Static的区域
    CRect ButtonRect;
    CRect SubParamGridRect;
    {
        CRect ParamsArea;
        GetDlgItem(IDC_STATIC_CONFIG_CONTEXT)->GetWindowRect(ParamsArea);

        pButton->GetWindowRect(ButtonRect);
        SubParamGridRect.left = ButtonRect.left;
        SubParamGridRect.right = ButtonRect.right - 2;
        SubParamGridRect.top = ButtonRect.top + 10;
        SubParamGridRect.bottom = ParamsArea.bottom - CoordinateGenerator::OFFSET_Y;
        m_GridSubParams.MoveWindow(SubParamGridRect);
        m_GridSubParams.ShowWindow(SW_SHOW);
        m_GridSubParams.SetWindowTextW((gp_XaOpen + L"子配置项").c_str());
    }

    //设置各个子配置项的位置
    {
        //准备数据
        UINT ButtonID = pButton->GetDlgCtrlID();
        std::vector<ParamNode>& SubParamsNode = theApp.g_mapLeaf[LeafType].m_subParams[ButtonID];
        SubParamsNode.clear();

        CoordinateGenerator GeneratorRect(SubParamGridRect);
        GeneratorRect.m_TopGrap = CoordinateGenerator::OFFSET_Y * 6;
        GeneratorRect.SetSplitRatio(2, 7, 1);
        GeneratorRect.Begin(1, 2);

        for (std::vector<std::wstring>::size_type ParamIndex = 0; ParamIndex < SubParamsValue.size(); ParamIndex++)
        {
            std::wstring::size_type EqualPos = SubParamsValue[ParamIndex].find(EQUAL);
            if (EqualPos == std::wstring::npos)
            {
                continue;
            }

            const std::wstring& ParamName = SubParamsValue[ParamIndex].substr(0, EqualPos);
            if (ParamName.empty())
            {
                continue;
            }

            SubParamsNode.push_back(ParamNode(ParamName, CT_EDIT_TEXT));
            std::vector<ParamNode>::size_type index = SubParamsNode.size() - 1;

            std::wstring::size_type NameStart = EqualPos + 1;
            std::wstring::size_type NameStop = SubParamsValue[ParamIndex].size();
            SubParamsNode[index].m_ParamValue = SubParamsValue[ParamIndex].substr(NameStart, NameStop);

            const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(this, SubParamsNode[index].m_ParamNameID);
            StaticText->SetWindowTextW((SubParamsNode[index].m_ParamName + COLON).c_str());
            StaticText->MoveWindow(GeneratorRect.GetNextRect());
            StaticText->ShowWindow(SW_SHOW);

            const CRect& NewRect = GeneratorRect.GetNextRect();
            switch (SubParamsNode[index].m_ParamValueType)
            {
            case CT_COMBO_BOX_EDIT:
            {
                const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxEdit(this, SubParamsNode[index].m_ParamValueID);
                ComboBox->MoveWindow(NewRect);
                ComboBox->ShowWindow(SW_SHOW);
                UpdateComboBox(ComboBox, LeafType, SubParamsNode[index]);
                break;
            }
            case CT_COMBO_BOX_LIST:
            {
                const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxList(this, SubParamsNode[index].m_ParamValueID);
                ComboBox->MoveWindow(NewRect);
                ComboBox->ShowWindow(SW_SHOW);
                UpdateComboBox(ComboBox, LeafType, SubParamsNode[index]);
                break;
            }
            case CT_CHECK_LIST_BOX:
            {
                const std::shared_ptr<CButton>& Button = theApp.GetButton(this, SubParamsNode[index].m_ParamValueID);
                Button->MoveWindow(NewRect);
                Button->SetWindowTextW(SubParamsNode[index].m_ParamValue.c_str());
                Button->ShowWindow(SW_SHOW);
                Button->EnableWindow(TRUE);
                break;
            }
            case CT_CHECK_BOX:
            {
                const std::shared_ptr<CButton>& Button = theApp.GetButton(this, SubParamsNode[index].m_ParamValueID);
                Button->MoveWindow(NewRect);
                Button->SetWindowTextW(SubParamsNode[index].m_ParamValue.c_str());
                Button->ShowWindow(SW_SHOW);

                UINT TypeID = theApp.g_mapLeaf[LeafType].GetParamValueID(gp_Type);
                std::shared_ptr<CComboBox> ComboxList = theApp.GetComboBoxList(TypeID);

                CString TypeText;
                ComboxList->GetWindowTextW(TypeText);
                Button->EnableWindow(TypeText.Find(gt_Bbu.c_str()) >= 0);
                break;
            }
            default:
            {
                const std::shared_ptr<CEdit>& EditText = theApp.GetEditText(this, SubParamsNode[index].m_ParamValueID);
                EditText->MoveWindow(NewRect);
                EditText->SetWindowTextW(SubParamsNode[index].m_ParamValue.c_str());
                EditText->ShowWindow(SW_SHOW);
                UpdateEdit(EditText, SubParamsNode[index].m_ParamValueType);
                break;
            }
            }

            theApp.g_setSubParams.insert(SubParamsNode[index].m_ParamNameID);
            theApp.g_setSubParams.insert(SubParamsNode[index].m_ParamValueID);
        }
    }
}

void CmaServerConfigDlg::UpdateComboBox(const std::shared_ptr<CComboBox>& ptrComboBox, const std::wstring& LeafType, const ParamNode& Node)
{
    std::wstring Key = LeafType + L"." + Node.m_ParamName;
    if (theApp.g_mapSelect.find(Key) == theApp.g_mapSelect.end())
    {
        Key = theApp.GetSelectItemMapKey(Key);
        if (Key.empty())
        {
            return;
        }
    }

    ptrComboBox->ResetContent();
    std::vector<std::wstring>& vecSelect = theApp.g_mapSelect[Key];
    for (std::vector<std::wstring>::size_type index = 0; index < vecSelect.size(); index++)
    {
        ptrComboBox->AddString(vecSelect[index].c_str());
    }

    if (!Node.m_ParamValue.empty())
    {
        for (std::vector<std::wstring>::size_type index = 0; index < vecSelect.size(); index++)
        {
            if (vecSelect[index] == Node.m_ParamValue)
            {
                ptrComboBox->SetCurSel(static_cast<int>(index));
                break;
            }
        }
    }
}

void CmaServerConfigDlg::UpdateNodeUse(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType, const std::wstring& Type)
{
    //只有BPU类型的use才需要配置子项，其它类型直接禁用
    if (Type.find(gt_Bbu) == std::wstring::npos)
    {
        m_GridSubParams.ShowWindow(SW_HIDE);
        pButton->EnableWindow(FALSE);
        pButton->SetWindowTextW(L"");
        theApp.ResetSubParamsControl();
        return;
    }

    //如果当前可见，点击按钮就隐藏
    if (m_GridSubParams.IsWindowVisible())
    {
        m_GridSubParams.ShowWindow(SW_HIDE);
        theApp.ResetSubParamsControl();
        return;
    }

    //按钮要激活
    pButton->EnableWindow(TRUE);

    //设置Group Static的区域
    CRect ButtonRect;
    CRect SubParamGridRect;
    {
        pButton->GetWindowRect(ButtonRect);
        SubParamGridRect.left = ButtonRect.left;
        SubParamGridRect.right = ButtonRect.right - 2;
        SubParamGridRect.top = ButtonRect.top + 10;
        SubParamGridRect.bottom = SubParamGridRect.top + 300;
        m_GridSubParams.MoveWindow(SubParamGridRect);
        m_GridSubParams.ShowWindow(SW_SHOW);
        m_GridSubParams.SetWindowTextW((gp_Use + L"子配置项").c_str());
    }

    //设置各个子配置项的显示区域
    UINT ButtonID = pButton->GetDlgCtrlID();
    std::vector<ParamNode>& vecSubParams = theApp.g_mapLeaf[LeafType].m_subParams[ButtonID];
    {
        vecSubParams.clear();
        vecSubParams.push_back(ParamNode(gs_NodeUse_n, CT_CHECK_BOX));
        vecSubParams.push_back(ParamNode(gs_NodeUse_s, CT_CHECK_BOX));
        vecSubParams.push_back(ParamNode(gs_NodeUse_a, CT_CHECK_BOX));
        vecSubParams.push_back(ParamNode(gs_NodeUse_q, CT_CHECK_BOX));

        CoordinateGenerator GeneratorRect(SubParamGridRect);
        GeneratorRect.m_TopGrap = CoordinateGenerator::OFFSET_Y * 4;
        GeneratorRect.Begin(1, 1);

        for (std::vector<ParamNode>::size_type index = 0; index < vecSubParams.size(); index++)
        {
            const std::shared_ptr<CButton>& Button = theApp.GetCheckBox(this, vecSubParams[index].m_ParamNameID);
            Button->SetWindowTextW(vecSubParams[index].m_ParamName.c_str());
            Button->MoveWindow(GeneratorRect.GetNextRect());
            Button->ShowWindow(SW_SHOW);

            theApp.g_setSubParams.insert(vecSubParams[index].m_ParamNameID);
        }
    }

    //如果没有值，就提供一个默认值
    CString ButtonText;
    pButton->GetWindowTextW(ButtonText);
    if (ButtonText.IsEmpty())
    {
        pButton->SetWindowTextW(gs_NodeUse_n.c_str());
        pButton->GetWindowTextW(ButtonText);
    }

    //设置已选中的值
    for (std::vector<ParamNode>::size_type index = 0; index < vecSubParams.size(); index++)
    {
        const std::shared_ptr<CButton>& Button = theApp.GetCheckBox(vecSubParams[index].m_ParamNameID);
        if (Button != nullptr)
        {
            Button->SetCheck(ButtonText.Find(vecSubParams[index].m_ParamName[0]) >= 0 ? 1 : 0);
        }
    }
}

void CmaServerConfigDlg::UpdateCheckListBox(const std::shared_ptr<CButton>& ptrButton, const std::wstring& LeafType, const ParamNode& Node)
{
    const std::wstring& Key = LeafType + L"." + Node.m_ParamName;
    if (theApp.g_mapSelect.find(Key) == theApp.g_mapSelect.end())
    {
        return;
    }

    //设置显示的区域
    {
        CRect ButtonRect;
        ptrButton->GetWindowRect(ButtonRect);

        CRect ListBoxRect;
        ListBoxRect.left = ButtonRect.left;
        ListBoxRect.right = ButtonRect.right - 3;
        ListBoxRect.top = ButtonRect.top + CoordinateGenerator::OFFSET_Y;
        ListBoxRect.bottom = ListBoxRect.top + 300;
        m_CheckListBox.MoveWindow(&ListBoxRect);
        m_CheckListBox.ShowWindow(SW_SHOW);
    }

    //添加可以选择的值
    {
        m_CheckListBox.ResetContent();
        std::vector<std::wstring>& vecSelect = theApp.g_mapSelect[Key];
        for (std::vector<std::wstring>::size_type index = 0; index < vecSelect.size(); index++)
        {
            m_CheckListBox.AddString(vecSelect[index].c_str());
        }
    }

    //勾选已经有的值
    {
        CString TempText;
        ptrButton->GetWindowTextW(TempText);
        if (TempText.IsEmpty())
        {
            return;
        }

        std::vector<std::wstring> SelectItem;
        SplitString(TempText.GetBuffer(), SelectItem, SEMICOLON);
        if (SelectItem.empty())
        {
            return;
        }

        for (std::vector<std::wstring>::size_type index = 0; index < SelectItem.size(); index++)
        {
            if (SelectItem[index].empty())
            {
                continue;
            }

            bool IsFind = false;
            for (int BoxIndex = 0; BoxIndex < m_CheckListBox.GetCount(); BoxIndex++)
            {
                m_CheckListBox.GetText(BoxIndex, TempText);
                if (TempText.Compare(SelectItem[index].c_str()) == 0)
                {
                    m_CheckListBox.SetCheck(BoxIndex, 1);
                    IsFind = true;
                    break;
                }
            }

            if (!IsFind)
            {
                m_CheckListBox.AddString(SelectItem[index].c_str());
            }
        }
    }
}
