#include "pch.h"
#include "framework.h"
#include "maServerConfig.h"
#include "maServerConfigDlg.h"
#include "afxdialogex.h"
#include "../ProjectLinux/Share/Boost/BoostFileSystem.h"
#include "../Windows/System/SystemShare.h"
#include "../StandardLinux/src/StandardShare.h"

#include <list>
#include <map>
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
    DDX_Control(pDX, IDC_COMBO1, m_ComboBoxConfig);
    DDX_Control(pDX, IDC_TREE1, m_TreeConfig);
    DDX_Control(pDX, IDC_LIST1, m_CheckListBox);
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
    ON_BN_CLICKED(ID_BUTTON_GENERATE, &CmaServerConfigDlg::OnButtonClickedGenerateConfig)
    ON_COMMAND_RANGE(IDC_BUTTON_START, IDC_BUTTON_STOP, &CmaServerConfigDlg::OnBnClicked)
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
    m_ComboBoxConfig.AddString((gm_Construction + COLON + L"构件").c_str());
    m_ComboBoxConfig.AddString((gm_Deployment + COLON + L"部署").c_str());
    m_ComboBoxConfig.AddString((gm_Kernel + COLON + L"内核").c_str());
    m_ComboBoxConfig.AddString((gm_MA + COLON + L"架构").c_str());
    m_ComboBoxConfig.SetCurSel(3);

    if (LoadConfigFile(g_XMLFile))
    {
        UpdateTreeConfig();
    }

    //m_ListBox.ModifyStyle(0, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_EXTENDEDSEL);
    m_CheckListBox.ShowWindow(SW_HIDE);
    InitControlSize();
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
    //还未使用

    // TODO: 在此添加控件通知处理程序代码
    CDialogEx::OnOK();
}

//按钮点击事件，可以弹出CheckListBox
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
        if (vecParams[index].m_ParamValueID == uID)
        {
            theApp.UpdateCheckListBox(ClickButton, m_CheckListBox, LeafType.GetBuffer(), vecParams[index]);
            break;
        }
    }
}

//配置项选择事件，比如选择ma架构还是kernel
void CmaServerConfigDlg::OnComboBoxConfigChange()
{
    m_TreeConfig.DeleteAllItems();
    UpdateTreeConfig();
}

//ComBox List选择响应事件，预定10个
void CmaServerConfigDlg::OnComboBoxListSelectChange(UINT uID)
{
    //获取当前文本框里的节点名称
    std::shared_ptr<CComboBox> ComboxList = theApp.GetComboBoxList(uID);
    if(ComboxList == nullptr)
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
        const std::wstring &NODES = gm_MA + DOT + gm_Deployment + DOT + gt_Nodes;
        if(m_LastSearchNode.Compare(NODES.c_str()) == 0)
        {
            CString LeafType = theApp.GetLeftType(m_LastLeafNode);
            if (theApp.g_mapLeaf.find(LeafType.GetString()) != theApp.g_mapLeaf.end())
            {
                const std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType.GetBuffer()].m_vecParams;
                for(std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
                {
                    if(vecParams[index].m_ParamName == gp_Use)
                    {
                        const std::shared_ptr<CButton>& Button = theApp.GetButton(vecParams[index].m_ParamValueID);
                        if (CurrentText.Find(gt_Bbu.c_str()) >= 0)
                        {
                            Button->SetWindowTextW(L"n"); //给一个默认值
                            Button->EnableWindow(TRUE);
                        }
                        else
                        {
                            Button->SetWindowTextW(L"");
                            Button->EnableWindow(FALSE);
                        }

                        break;
                    }
                }
            }
        }
    }
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
            theApp.ResetControl();
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

//生成配置文件事件
void CmaServerConfigDlg::OnButtonClickedGenerateConfig()
{
    SaveLastChange();
    theApp.WriteXMLFile(g_XMLFile);
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
    for(int index =0; index < m_CheckListBox.GetCount(); index++)
    {
        m_CheckListBox.GetText(index, SelectText);
        std::vector<std::wstring>::const_iterator it = std::find(vecExistText.begin(), vecExistText.end(), SelectText.GetBuffer());

        if(m_CheckListBox.GetCheck(index) == BST_CHECKED)
        {
            if(it == vecExistText.end())
            {
                vecExistText.push_back(SelectText.GetBuffer());
            }
        }
        else
        {
            if(it != vecExistText.end())
            {
                vecExistText.erase(it);
            }
        }
    }

    //组装文本
    ButtonText.Empty();
    for(std::vector<std::wstring>::size_type index = 0; index < vecExistText.size(); index++)
    {
        //以分号分隔
        if(!ButtonText.IsEmpty())
        {
            ButtonText.Append(SEMICOLON.c_str());
        }

        //按顺序添加到末尾
        ButtonText.Append(vecExistText[index].c_str());
    }

    //显示文本
    CurrentButton->SetWindowTextW(ButtonText);
}

CString CmaServerConfigDlg::GetRootNodeName()
{
    //获取当前文本框里的节点名称
    CString CurrentText;
    m_ComboBoxConfig.GetWindowTextW(CurrentText);
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

void CmaServerConfigDlg::InitControlSize()
{
    //先设置对话框最大化
    ShowWindow(SW_MAXIMIZE);

    //各个控件根据这些常量来划分位置
    const int GridInterval = 10;    //各个Grid的间隔
    const int SPLIT_WIDTH = 4;      //宽度拆分成SPLIT_WIDTH份
    const int SPLIT_HEIGT = 16;     //高度拆分成SPLIT_HEIGT份

    //获取客户区的大小
    CRect ClientRect;
    GetClientRect(ClientRect);

    //设置配置项的区域
    GetDlgItem(IDC_STATIC_CONFIG_ITEM)->MoveWindow(
        ClientRect.left + GridInterval,                               //客户区域左边界往右移动一个Grid间隔
        ClientRect.top + GridInterval,                                //客户区域上边界往下移动一个Grid间隔
        (ClientRect.Width() - 2 * GridInterval) / SPLIT_WIDTH,                            //配置项占的宽度份额
        (ClientRect.Height() - 2 * GridInterval) / SPLIT_HEIGT * (SPLIT_HEIGT - 1));      //配置项占的高度份额

    //获取配置项的区域
    CRect ConfigItemRect;
    GetDlgItem(IDC_STATIC_CONFIG_ITEM)->GetWindowRect(ConfigItemRect);

    //设置配置项的ComBox
    CRect ComBoxRect;
    GetDlgItem(IDC_COMBO1)->GetWindowRect(ComBoxRect);
    GetDlgItem(IDC_COMBO1)->MoveWindow(
        ConfigItemRect.left + GridInterval,                //距离配置项左侧一个Grid间隔
        ConfigItemRect.top,                                //跟配置项一致
        ConfigItemRect.Width() - 2 * GridInterval,         //减去左右两边的Grid间隔
        ComBoxRect.Height());                              //跟原来保持一致；设置了也没效果

    //再次获取ComBox的区域
    GetDlgItem(IDC_COMBO1)->GetWindowRect(ComBoxRect);

    //设置配置项的树形控件
    GetDlgItem(IDC_TREE1)->MoveWindow(
        ConfigItemRect.left + GridInterval,                //距离配置项左侧一个Grid间隔
        ComBoxRect.top,                                    //跟ComBox一致
        ConfigItemRect.Width() - 2 * GridInterval,         //减去左右两边的Grid间隔
        ConfigItemRect.Height() - ComBoxRect.top);         //配置项区域的高度减去ComBox的top

    //设置配置内容的区域
    {
        GetDlgItem(IDC_STATIC_CONFIG_CONTEXT)->MoveWindow(
            ConfigItemRect.right + GridInterval,                            //配置项区域往右移动一个Grid间隔
            ClientRect.top + GridInterval,                                  //跟配置项一样的Y轴
            ClientRect.Width() - ConfigItemRect.right - 2 * GridInterval,   //减去配置项的宽度，也减去与配置项的间隔，再减去最右边的间隔
            ConfigItemRect.Height());                                       //跟配置项一样的高度
    }

    //设置按钮区域
    GetDlgItem(IDC_STATIC_BUTTON_AREA)->MoveWindow(
        ConfigItemRect.left,                                                    //跟配置项一样的X轴
        ClientRect.top + ConfigItemRect.Height() + GridInterval,                //配置项区域往下移动一个Grid间隔
        ClientRect.Width() - GridInterval * 2,                                  //只需要减去两边的Grid间隔
        (ClientRect.Height() - ConfigItemRect.Height() - 2 * GridInterval));    //减去配置项的高度，再减去一个Grid间隔

    //设置按钮的布局
    {
        //先获取按钮的区域
        CRect ButtonAreaRect;
        GetDlgItem(IDC_STATIC_BUTTON_AREA)->GetWindowRect(ButtonAreaRect);

        //需要实现的按钮
        std::vector<int> vecID = { ID_BUTTON_LOAD, ID_BUTTON_GENERATE, ID_BUTTON_OPEN_FILE };
        const int BUTTON_COUNT = static_cast<int>(vecID.size());

        //计算按钮的高度
        const int BUTTON_MIN_HEIGHT = 50;
        int ButtonHeight = ButtonAreaRect.Height() > BUTTON_MIN_HEIGHT ? BUTTON_MIN_HEIGHT : ButtonAreaRect.Height() - 4; //上下空开两个像素
        int ButtonY = ButtonAreaRect.top - (ButtonAreaRect.Height() - ButtonHeight);

        //计算按钮的宽度
        CRect ButtonRect;
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

        for (std::vector<int>::size_type index = 0; index < vecID.size(); index++)
        {
            GetDlgItem(vecID[index])->MoveWindow(LastButtonX, ButtonY, ButtonWidth, ButtonHeight);
            GetDlgItem(vecID[index])->GetWindowRect(ButtonRect);
            LastButtonX = ButtonRect.right + ButtonInterval;
        }
    }
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

bool CmaServerConfigDlg::UpdateTreeConfig()
{
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

        FormatString.Format(L"<xmlattr>.%s", vecParams[index].m_ParamName.c_str());
        LeafNode.second.put<std::wstring>(FormatString.GetString(), ControlValue.GetString());
    }
}

void CmaServerConfigDlg::DisplayParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode)
{
    CRect ParamsArea;
    GetDlgItem(IDC_STATIC_CONFIG_CONTEXT)->GetWindowRect(ParamsArea);

    const int OFFSET = 10;                          //最小偏移量
    int StartX = ParamsArea.left + 2 * OFFSET;      //控件距离Gird左边20像素
    int CurrentY = ParamsArea.top;                  //控件跟Grid的top保持一致
    int CONTROL_SPLIT = 1 + 5;                      //参数名和参数值按1:5划分

    int StaticWidth = ParamsArea.Width() / CONTROL_SPLIT;
    if (StaticWidth > 100)
    {
        StaticWidth = 100;                  //太长不好看
    }

    int ControlWidth = ParamsArea.Width() - StaticWidth - 4 * OFFSET;
    if (ControlWidth > 1000)
    {
        ControlWidth = 1000;
    }

    int ControlHeight = 30;
    int ControlIndex = 0;

    CString FormatString;
    std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType].m_vecParams;
    for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
    {
        FormatString.Format(L"<xmlattr>.%s", vecParams[index].m_ParamName.c_str());
        const std::wstring& Text = vecParams[index].m_ParamName + COLON;
        const std::wstring& Value = LeafNode.second.get<std::wstring>(FormatString.GetString(), L"");

        const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(this, vecParams[index].m_ParamNameID);
        StaticText->MoveWindow(StartX, CurrentY, StaticWidth, ControlHeight);
        StaticText->SetWindowTextW(Text.c_str());
        StaticText->ShowWindow(SW_SHOW);

        switch (vecParams[index].m_ParamValueType)
        {
        case CT_COMBO_BOX_EDIT:
        {
            const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxEdit(this, vecParams[index].m_ParamValueID);
            ComboBox->MoveWindow(StartX + OFFSET + StaticWidth, CurrentY + 2, ControlWidth, ControlHeight);
            ComboBox->ShowWindow(SW_SHOW);

            vecParams[index].m_ParamValue = Value;
            theApp.UpdateComboBox(ComboBox, LeafType, vecParams[index]);
            break;
        }
        case CT_COMBO_BOX_LIST:
        {
            const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxList(this, vecParams[index].m_ParamValueID);
            ComboBox->MoveWindow(StartX + OFFSET + StaticWidth, CurrentY + 2, ControlWidth, ControlHeight);
            ComboBox->ShowWindow(SW_SHOW);

            vecParams[index].m_ParamValue = Value;
            theApp.UpdateComboBox(ComboBox, LeafType, vecParams[index]);
            break;
        }
        case CT_CHECK_LIST_BOX:
        {
            const std::shared_ptr<CButton>& Button = theApp.GetButton(this, vecParams[index].m_ParamValueID);
            Button->MoveWindow(StartX + OFFSET + StaticWidth, CurrentY + 2, ControlWidth, ControlHeight);
            Button->SetWindowTextW(Value.c_str());
            Button->ShowWindow(SW_SHOW);
            break;
        }
        default:
        {
            const std::shared_ptr<CEdit>& EditText = theApp.GetEditText(this, vecParams[index].m_ParamValueID);
            EditText->MoveWindow(StartX + OFFSET + StaticWidth, CurrentY + 2, ControlWidth, ControlHeight);
            EditText->SetWindowTextW(Value.c_str());
            EditText->ShowWindow(SW_SHOW);
            theApp.UpdateEdit(EditText, vecParams[index].m_ParamValueType);
            break;
        }
        }

        CurrentY += ControlHeight + OFFSET;
        ++ControlIndex;
    }

    //调整node节点中use和defaultxa/backupxa的位置
    //if(LeafType == gl_Node)
    //{
    //    CRect pUseRect, pDefaultRect, pBackupRect, vUseRect, vDefaultRect, vBackupRect;
    //    std::shared_ptr<CStatic> pUse, pDefault, pBackup;
    //    std::shared_ptr<CComboBox> vDefault, vBackup;
    //    std::shared_ptr<CButton> vUse;
    //    for(std::vector<ParamNode>::size_type index =0 ; index < vecParams.size(); index++)
    //    {
    //        if(vecParams[index].m_ParamName == gp_Use)
    //        {
    //            pUse = theApp.GetStaticText(vecParams[index].m_ParamNameID);
    //            vUse = theApp.GetButton(vecParams[index].m_ParamValueID);
    //            pUse->GetWindowRect(pUseRect);
    //            vUse->GetWindowRect(vUseRect);
    //            continue;
    //        }

    //        if (vecParams[index].m_ParamName == gp_DefaultXa)
    //        {
    //            pDefault = theApp.GetStaticText(vecParams[index].m_ParamNameID);
    //            vDefault = theApp.GetComboBoxList(vecParams[index].m_ParamValueID);
    //            pDefault->GetWindowRect(pDefaultRect);
    //            vDefault->GetWindowRect(vDefaultRect);
    //            continue;
    //        }

    //        if (vecParams[index].m_ParamName == gp_BackupXa)
    //        {
    //            pBackup = theApp.GetStaticText(vecParams[index].m_ParamNameID);
    //            vBackup = theApp.GetComboBoxList(vecParams[index].m_ParamValueID);
    //            pBackup->GetWindowRect(pBackupRect);
    //            vBackup->GetWindowRect(vBackupRect);
    //            continue;
    //        }
    //    }

    //    if(pUseRect.top < pDefaultRect.top && pDefaultRect.top < pBackupRect.top)
    //    {
    //        pDefault->MoveWindow(pUseRect);
    //        vDefault->MoveWindow(vUseRect);

    //        pBackup->MoveWindow(pDefaultRect);
    //        vBackup->MoveWindow(vDefaultRect);

    //        pUse->MoveWindow(pBackupRect);
    //        vUse->MoveWindow(vBackupRect);
    //    }
    //}
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
