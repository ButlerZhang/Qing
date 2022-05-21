#include "pch.h"
#include "maServerConfig.h"
#include "CDialogParams.h"
#include "afxdialogex.h"
#include "CoordinateGenerator.h"
#include "maServerConfigDlg.h"
#include "../StandardLinux/src/StandardShare.h"
#include <afxmsg_.h>

IMPLEMENT_DYNAMIC(CDialogParams, CDialogEx)

CDialogParams::CDialogParams(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_PARAMS, pParent)
{
}

CDialogParams::~CDialogParams()
{
}

void CDialogParams::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_CheckListBox);
    DDX_Control(pDX, IDC_STATIC_COMPLEX, m_GridSubParams);
    DDX_Control(pDX, IDC_STATIC_TITLE, m_GridTitle);
    DDX_Control(pDX, IDC_STATIC_NORMAL, m_GridNormal);
}

BEGIN_MESSAGE_MAP(CDialogParams, CDialogEx)
    //ON_WM_SYSCOMMAND()
    //ON_WM_CLOSE()
    //ON_WM_PAINT()
    //ON_WM_QUERYDRAGICON()
    ON_CLBN_CHKCHANGE(IDC_LIST1, &CDialogParams::OnCheckListBoxChange)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_1, &CDialogParams::OnComboBoxListSelectChange1)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_2, &CDialogParams::OnComboBoxListSelectChange2)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_3, &CDialogParams::OnComboBoxListSelectChange3)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_4, &CDialogParams::OnComboBoxListSelectChange4)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_5, &CDialogParams::OnComboBoxListSelectChange5)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_6, &CDialogParams::OnComboBoxListSelectChange6)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_7, &CDialogParams::OnComboBoxListSelectChange7)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_8, &CDialogParams::OnComboBoxListSelectChange8)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_9, &CDialogParams::OnComboBoxListSelectChange9)
    ON_CBN_SELCHANGE(IDC_COMBOX_LIST_10, &CDialogParams::OnComboBoxListSelectChange10)
    ON_COMMAND_RANGE(IDC_BUTTON_START, IDC_BUTTON_STOP, &CDialogParams::OnBnClicked)
    ON_COMMAND_RANGE(IDC_CHECK_BOX_START, IDC_CHECK_BOX_STOP, &CDialogParams::OnCheckBoxClicked)
END_MESSAGE_MAP()

//点击Button弹出CheckListBox或由多个控件组成的子配置项
void CDialogParams::OnBnClicked(UINT uID)
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

    //TODO
}

//CheckListBox选择响应事件
void CDialogParams::OnCheckListBoxChange()
{
    //是否能找到需要变更的参数
    const std::wstring& LeafType = GetLeafType();
    std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType].m_vecParams;
    std::vector<ParamNode>::size_type TargetIndex = vecParams.size();
    for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
    {
        if (vecParams[index].m_ValueType == CT_CHECK_LIST_BOX)
        {
            TargetIndex = index;
            break;
        }
    }

    if (TargetIndex == vecParams.size())
    {
        return;
    }

    //先拆分已有的文本
    std::vector<std::wstring> vecExistText;
    SplitString(vecParams[TargetIndex].m_Value, vecExistText, SEMICOLON);

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
    vecParams[TargetIndex].m_Value.clear();
    for (std::vector<std::wstring>::size_type index = 0; index < vecExistText.size(); index++)
    {
        //以分号分隔
        if (!vecParams[TargetIndex].m_Value.empty())
        {
            vecParams[TargetIndex].m_Value.append(SEMICOLON);
        }

        //按顺序添加到末尾
        vecParams[TargetIndex].m_Value.append(vecExistText[index]);
    }
}

//勾选框点击事件
void CDialogParams::OnCheckBoxClicked(UINT uID)
{
    const std::wstring& LeafType = GetLeafType();
    if (theApp.g_mapLeaf.find(LeafType) == theApp.g_mapLeaf.end())
    {
        return;
    }

    LeafNode& LNode = theApp.g_mapLeaf[LeafType];
    std::vector<ParamNode>& vecParams = LNode.m_vecParams;
    for (std::vector<ParamNode>::size_type ParamIndex = 0; ParamIndex < vecParams.size(); ParamIndex++)
    {
        if (vecParams[ParamIndex].m_Name != gp_Use)
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
        UINT ParamsID = LNode.GetValueID(vecParams[ParamIndex].m_Name);
        const std::vector<ParamNode>& subParams = LNode.m_subParams;

        //选中了n/s/a的某一个时，另外两个要取消选中
        if (ClickCheckBox->GetCheck() == 1 && ClickCheckBoxText.Find(gs_NodeUse_q[0]) < 0)
        {
            for (std::vector<ParamNode>::size_type SubIndex = 0; SubIndex < subParams.size(); SubIndex++)
            {
                if (subParams[SubIndex].m_NameID == uID)
                {
                    continue;
                }

                if (subParams[SubIndex].m_Name == gs_NodeUse_q)
                {
                    continue;
                }

                const std::shared_ptr<CButton>& CheckBox = theApp.GetCheckBox(subParams[SubIndex].m_NameID);
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
            const std::shared_ptr<CButton>& CheckBox = theApp.GetCheckBox(subParams[SubIndex].m_NameID);
            if (CheckBox == nullptr)
            {
                continue;
            }

            if (CheckBox->GetCheck() == 1)
            {
                NewText.Append(subParams[SubIndex].m_Name.c_str());
            }
        }

        vecParams[ParamIndex].m_Value = NewText.GetBuffer();
        break;
    }
}

//ComboBoxList选择响应事件，预定10个
void CDialogParams::OnComboBoxListSelectChange(UINT uID)
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

    CString LastLeafNode = ((CmaServerConfigDlg*)GetParent())->GetLastLeafNode();
    CString LastSearchNode = ((CmaServerConfigDlg*)GetParent())->GetLastSearchNode();

    //deployment.nodes节点
    {
        const std::wstring& NODES = gm_MA + DOT + gm_Deployment + DOT + gt_Nodes;
        if (LastSearchNode.Compare(NODES.c_str()) == 0)
        {
            CString LeafType = theApp.GetLeftType(LastLeafNode);
            if (theApp.g_mapLeaf.find(LeafType.GetString()) != theApp.g_mapLeaf.end())
            {
                std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType.GetBuffer()].m_vecParams;
                for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
                {
                    if (vecParams[index].m_ValueID == uID && vecParams[index].m_Name == gp_Type)
                    {
                        UINT TypeID = theApp.g_mapLeaf[LeafType.GetBuffer()].GetValueID(gp_Type);
                        CString TypeText;
                        theApp.GetComboBoxList(TypeID)->GetWindowText(TypeText);
                        vecParams[index].m_Value = TypeText.GetBuffer();

                        std::vector<ParamNode>::size_type UseIndex = theApp.g_mapLeaf[LeafType.GetBuffer()].GetIndex(gp_Use);
                        UpdateNodeUse(vecParams[UseIndex], LeafType.GetBuffer());
                        return;
                    }
                }
            }
        }
    }

    //construction.bpu.queues节点
    {
        const std::wstring& Queues = gm_MA + DOT + gm_Construction + DOT + gt_Bbu + DOT + gt_Queues;
        if (LastSearchNode.Compare(Queues.c_str()) == 0)
        {
            CString LeafType = theApp.GetLeftType(LastLeafNode);
            if (theApp.g_mapLeaf.find(LeafType.GetString()) != theApp.g_mapLeaf.end())
            {
                std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType.GetBuffer()].m_vecParams;
                for (std::vector<ParamNode>::size_type index = 0; index < vecParams.size(); index++)
                {
                    if (vecParams[index].m_ValueID == uID && vecParams[index].m_Name == gp_Type)
                    {
                        UpdateQueueType(vecParams[index], LeafType.GetBuffer());
                        return;
                    }
                }
            }
        }
    }
}

std::wstring CDialogParams::GetLeafType() const
{
    CString LeafType = theApp.GetLeftType(((CmaServerConfigDlg*)GetParent())->GetLastLeafNode());
    return LeafType.GetBuffer();
}

//设置具体的参数控件
void CDialogParams::SetParam(ParamNode& Node, CoordinateGenerator& GeneratorRect, const std::wstring& LeafType)
{
    const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(this, Node.m_NameID);
    StaticText->MoveWindow(GeneratorRect.GetNextRect());
    StaticText->SetWindowTextW((Node.m_Name + COLON).c_str());
    StaticText->ShowWindow(SW_SHOW);

    Node.m_ValueRect = GeneratorRect.GetNextRect();
    switch (Node.m_ValueType)
    {
    case CT_COMBO_BOX_EDIT:
    {
        const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxEdit(this, Node.m_ValueID);
        ComboBox->MoveWindow(Node.m_ValueRect);
        ComboBox->ShowWindow(SW_SHOW);
        UpdateComboBox(ComboBox, LeafType, Node);
        break;
    }
    case CT_COMBO_BOX_LIST:
    {
        const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxList(this, Node.m_ValueID);
        ComboBox->MoveWindow(Node.m_ValueRect);
        ComboBox->ShowWindow(SW_SHOW);
        UpdateComboBox(ComboBox, LeafType, Node);
        break;
    }
    case CT_CHECK_LIST_BOX:
    {
        UpdateCheckListBox(Node, LeafType);
        break;
    }
    case CT_CHECK_BOX:
    {
        UpdateNodeUse(Node, LeafType);
        break;
    }
    case CT_MIXED_BOX_XA:
    {
        break;
    }
    case CT_MIXED_BOX_QUEUE:
    {
        UpdateQueueConnstr(Node, LeafType);
        break;
    }
    default:
    {
        const std::shared_ptr<CEdit>& EditText = theApp.GetEditText(this, Node.m_ValueID);
        EditText->MoveWindow(Node.m_ValueRect);
        EditText->SetWindowTextW(Node.m_Value.c_str());
        EditText->ShowWindow(SW_SHOW);
        UpdateEdit(EditText, Node.m_ValueType);
        break;
    }
    }
}

void CDialogParams::Hide()
{
    m_GridTitle.ShowWindow(SW_HIDE);
    m_GridNormal.ShowWindow(SW_HIDE);
    m_GridSubParams.ShowWindow(SW_HIDE);
    m_CheckListBox.ShowWindow(SW_HIDE);
}

void CDialogParams::UpdateParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode)
{
    bool IsForceUpdate = false;
    CString ControlName, ControlValue, FormatString;
    const std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType].m_vecParams;
    for (std::vector<ParamNode>::size_type index = 1; index < vecParams.size(); index++)
    {
        const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(vecParams[index].m_NameID);
        if (StaticText == nullptr)
        {
            continue;
        }

        StaticText->GetWindowTextW(ControlName);
        int pos = ControlName.Find(COLON.c_str());
        ControlName.Delete(pos, 1);
        if (ControlName.Compare(vecParams[index].m_Name.c_str()) != 0)
        {
            continue;
        }

        ControlValue.Empty();
        IsForceUpdate = false;

        switch (vecParams[index].m_ValueType)
        {
        case CT_COMBO_BOX_EDIT:
        {
            theApp.GetComboBoxEdit(vecParams[index].m_ValueID)->GetWindowTextW(ControlValue);
            break;
        }
        case CT_COMBO_BOX_LIST:
        {
            theApp.GetComboBoxList(vecParams[index].m_ValueID)->GetWindowTextW(ControlValue);
            break;
        }
        case CT_CHECK_BOX:
        case CT_CHECK_LIST_BOX:
        {
            IsForceUpdate = true;
            ControlValue = vecParams[index].m_Value.c_str();
            break;
        }
        case CT_MIXED_BOX_XA:
        case CT_MIXED_BOX_QUEUE:
        {
            IsForceUpdate = true;
            ControlValue = vecParams[index].m_Value.c_str();
            break;
        }
        default:
        {
            theApp.GetEditText(vecParams[index].m_ValueID)->GetWindowTextW(ControlValue);
            break;
        }
        }

        if (IsForceUpdate || ControlValue.Compare(vecParams[index].m_Value.c_str()) != 0)
        {
            FormatString.Format(L"<xmlattr>.%s", vecParams[index].m_Name.c_str());
            LeafNode.second.put<std::wstring>(FormatString.GetString(), ControlValue.GetString());
        }
    }
}

void CDialogParams::DisplayParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode)
{
    const LONG GridYGrid = CoordinateGenerator::OFFSET_Y * 2;                       //两个Grid的Y轴间隔
    const LONG TopY = CoordinateGenerator::OFFSET_Y * 3;                            //控件跟Grid.top的间隔
    const LONG BottomY = CoordinateGenerator::OFFSET_Y * 2;                         //控件跟Grid.bottom的间隔
    std::vector<ParamNode>& vecParams = theApp.g_mapLeaf[LeafType].m_vecParams;     //参数信息

    //获取可用区域
    CRect ParamsTotalArea;
    {
        GetClientRect(ParamsTotalArea);
        ParamsTotalArea.left += CoordinateGenerator::OFFSET_X;                      //左边留出空白
        ParamsTotalArea.right -= CoordinateGenerator::OFFSET_X;                     //右边留出空白
        theApp.AddCRectToLog(L"0ParamsTotalArea", ParamsTotalArea);
    }

    //设置Title区域
    {
        CRect TitleTotalRect = ParamsTotalArea;
        TitleTotalRect.bottom = TitleTotalRect.top + CoordinateGenerator::CONTROL_HEIGHT + TopY + BottomY;
        m_GridTitle.MoveWindow(&TitleTotalRect);
        m_GridTitle.ShowWindow(SW_SHOW);
        theApp.AddCRectToLog(L"1TitleTotalRect", TitleTotalRect);

        CRect TitleUsableRect = TitleTotalRect;
        TitleUsableRect.top += TopY;
        TitleUsableRect.bottom -= BottomY;
        theApp.AddCRectToLog(L"2TitleUsableRect", TitleUsableRect);

        CoordinateGenerator GeneratorTitleRect(TitleUsableRect);
        GeneratorTitleRect.Begin(1, 2);
        SetParam(vecParams[0], GeneratorTitleRect, LeafType);
        theApp.AddCRectToLog(L"3" + vecParams[0].m_Name + L"Client", GeneratorTitleRect.GetCurrentRect());

        CRect RealRect;
        theApp.GetStaticText(vecParams[0].m_NameID)->GetWindowRect(&RealRect);
        theApp.AddCRectToLog(L"4" + vecParams[0].m_Name + L"Window", RealRect);

        //更新下一个Grid的起始Y轴
        ParamsTotalArea.top = TitleTotalRect.bottom + GridYGrid;
    }

    //设置Normal区域
    CRect NormalRect = ParamsTotalArea;
    ParamsTotalArea.top = NormalRect.top + TopY;

    //设置参数
    {
        CoordinateGenerator GeneratorRect(ParamsTotalArea);
        GeneratorRect.Begin(1, 2);

        CString FormatString;
        for (std::vector<ParamNode>::size_type index = 1; index < vecParams.size(); index++)
        {
            FormatString.Format(L"<xmlattr>.%s", vecParams[index].m_Name.c_str());
            vecParams[index].m_Value = LeafNode.second.get<std::wstring>(FormatString.GetString(), L"");
            SetParam(vecParams[index], GeneratorRect, LeafType);

            if (index == vecParams.size() - 2)
            {
                NormalRect.bottom = GeneratorRect.GetCurrentRect().bottom + BottomY;
                m_GridNormal.MoveWindow(&NormalRect);
                m_GridNormal.ShowWindow(SW_SHOW);

                ParamsTotalArea.top = NormalRect.bottom + GridYGrid;
                theApp.AddCRectToLog(L"5NormalRect", NormalRect);

                //设置SubParams区域
                CRect SubParamsTotalRect = ParamsTotalArea;
                m_GridSubParams.MoveWindow(&SubParamsTotalRect);
                m_GridSubParams.ShowWindow(SW_SHOW);
                theApp.AddCRectToLog(L"6SubParamsRect", SubParamsTotalRect);

                CRect SubParamsUsablRect = SubParamsTotalRect;
                SubParamsUsablRect.top = SubParamsUsablRect.top + TopY;
                CoordinateGenerator GeneratorSubParamsRect(SubParamsUsablRect);
                GeneratorSubParamsRect.Begin(1, 2);
                GeneratorRect = GeneratorSubParamsRect;
            }
        }
    }
}

void CDialogParams::UpdateEdit(const std::shared_ptr<CEdit>& ptrEdit, ControlType Type)
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

void CDialogParams::UpdateXaOpen(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType)
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
        pButton->GetWindowRect(ButtonRect);
        ScreenToClient(ButtonRect);

        SubParamGridRect.left = ButtonRect.left;
        SubParamGridRect.right = ButtonRect.right - 2;
        SubParamGridRect.top = ButtonRect.bottom + CoordinateGenerator::OFFSET_Y;
        SubParamGridRect.bottom = SubParamGridRect.top + 500;
        m_GridSubParams.MoveWindow(SubParamGridRect);
        m_GridSubParams.ShowWindow(SW_SHOW);
        m_GridSubParams.SetWindowTextW((gp_XaOpen + L"子配置项").c_str());
    }

    //设置各个子配置项的位置
    {
        //准备数据
        UINT ButtonID = pButton->GetDlgCtrlID();
        std::vector<ParamNode>& SubParamsNode = theApp.g_mapLeaf[LeafType].m_subParams;
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
            SubParamsNode[index].m_Value = SubParamsValue[ParamIndex].substr(NameStart, NameStop);

            const std::shared_ptr<CStatic>& StaticText = theApp.GetStaticText(this, SubParamsNode[index].m_NameID);
            StaticText->SetWindowTextW((SubParamsNode[index].m_Name + COLON).c_str());
            StaticText->MoveWindow(GeneratorRect.GetNextRect());
            StaticText->ShowWindow(SW_SHOW);

            const CRect& NewRect = GeneratorRect.GetNextRect();
            switch (SubParamsNode[index].m_ValueType)
            {
            case CT_COMBO_BOX_EDIT:
            {
                const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxEdit(this, SubParamsNode[index].m_ValueID);
                ComboBox->MoveWindow(NewRect);
                ComboBox->ShowWindow(SW_SHOW);
                UpdateComboBox(ComboBox, LeafType, SubParamsNode[index]);
                break;
            }
            case CT_COMBO_BOX_LIST:
            {
                const std::shared_ptr<CComboBox>& ComboBox = theApp.GetComboBoxList(this, SubParamsNode[index].m_ValueID);
                ComboBox->MoveWindow(NewRect);
                ComboBox->ShowWindow(SW_SHOW);
                UpdateComboBox(ComboBox, LeafType, SubParamsNode[index]);
                break;
            }
            case CT_CHECK_LIST_BOX:
            {
                const std::shared_ptr<CButton>& Button = theApp.GetButton(this, SubParamsNode[index].m_ValueID);
                Button->MoveWindow(NewRect);
                Button->SetWindowTextW(SubParamsNode[index].m_Value.c_str());
                Button->ShowWindow(SW_SHOW);
                Button->EnableWindow(TRUE);
                break;
            }
            case CT_CHECK_BOX:
            {
                const std::shared_ptr<CButton>& Button = theApp.GetButton(this, SubParamsNode[index].m_ValueID);
                Button->MoveWindow(NewRect);
                Button->SetWindowTextW(SubParamsNode[index].m_Value.c_str());
                Button->ShowWindow(SW_SHOW);

                UINT TypeID = theApp.g_mapLeaf[LeafType].GetValueID(gp_Type);
                std::shared_ptr<CComboBox> ComboxList = theApp.GetComboBoxList(TypeID);

                CString TypeText;
                ComboxList->GetWindowTextW(TypeText);
                Button->EnableWindow(TypeText.Find(gt_Bbu.c_str()) >= 0);
                break;
            }
            default:
            {
                const std::shared_ptr<CEdit>& EditText = theApp.GetEditText(this, SubParamsNode[index].m_ValueID);
                EditText->MoveWindow(NewRect);
                EditText->SetWindowTextW(SubParamsNode[index].m_Value.c_str());
                EditText->ShowWindow(SW_SHOW);
                UpdateEdit(EditText, SubParamsNode[index].m_ValueType);
                break;
            }
            }

            theApp.g_setSubParams.insert(SubParamsNode[index].m_NameID);
            theApp.g_setSubParams.insert(SubParamsNode[index].m_ValueID);
        }
    }
}

void CDialogParams::UpdateComboBox(const std::shared_ptr<CComboBox>& ptrComboBox, const std::wstring& LeafType, const ParamNode& Node)
{
    std::wstring Key = LeafType + L"." + Node.m_Name;
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

    if (!Node.m_Value.empty())
    {
        for (std::vector<std::wstring>::size_type index = 0; index < vecSelect.size(); index++)
        {
            if (vecSelect[index] == Node.m_Value)
            {
                ptrComboBox->SetCurSel(static_cast<int>(index));
                break;
            }
        }
    }
}



void CDialogParams::UpdateNodeUse(ParamNode& Node, const std::wstring& LeafType)
{
    //设置各个子配置项的显示区域
    std::vector<ParamNode>& vecSubParams = theApp.g_mapLeaf[LeafType].m_subParams;
    if (vecSubParams.empty())
    {
        //添加Use的子配置项信息
        vecSubParams.clear();
        vecSubParams.push_back(ParamNode(gs_NodeUse_n, CT_CHECK_BOX));
        vecSubParams.push_back(ParamNode(gs_NodeUse_s, CT_CHECK_BOX));
        vecSubParams.push_back(ParamNode(gs_NodeUse_a, CT_CHECK_BOX));
        vecSubParams.push_back(ParamNode(gs_NodeUse_q, CT_CHECK_BOX));

        //获取总的可用区域
        CRect SubParamsRect;
        m_GridSubParams.GetWindowRect(&SubParamsRect);

        //调整控件的区域范围，用于生成各个控件的区域
        CRect NewRect = Node.m_ValueRect;
        NewRect.bottom = SubParamsRect.bottom;
        CoordinateGenerator GeneratorRect(NewRect);
        GeneratorRect.Begin(1, 1);

        //遍历显示每个控件
        for (std::vector<ParamNode>::size_type index = 0; index < vecSubParams.size(); index++)
        {
            const std::shared_ptr<CButton>& Button = theApp.GetCheckBox(this, vecSubParams[index].m_NameID);
            Button->SetWindowTextW(vecSubParams[index].m_Name.c_str());
            Button->MoveWindow(GeneratorRect.GetNextRect());
            Button->ShowWindow(SW_SHOW);

            //这里保存子控件的ID，用于快速重置
            theApp.g_setSubParams.insert(vecSubParams[index].m_NameID);
        }
    }

    //只有BPU类型才需要use参数
    std::vector<ParamNode>::size_type TargetIndex = theApp.g_mapLeaf[LeafType].GetIndex(gp_Type);
    ParamNode& TypeNode = theApp.g_mapLeaf[LeafType].m_vecParams[TargetIndex];
    bool IsBPUType = TypeNode.m_Value.find(gt_Bbu) != std::wstring::npos;

    //设置参数是否可用以及当前值
    for (std::vector<ParamNode>::size_type index = 0; index < vecSubParams.size(); index++)
    {
        const std::shared_ptr<CButton>& Button = theApp.GetCheckBox(vecSubParams[index].m_NameID);
        if (Button != nullptr)
        {
            int Check = IsBPUType && Node.m_Value.find(vecSubParams[index].m_Name[0]) != std::wstring::npos;
            Button->SetCheck(Check);
            Button->EnableWindow(IsBPUType);
        }
    }
}

void CDialogParams::UpdateQueueType(ParamNode& Node, const std::wstring& LeafType)
{
    //获取Type的控件
    const std::shared_ptr<CComboBox>& TypeBox = theApp.GetComboBoxList(Node.m_ValueID);
    if (TypeBox == nullptr)
    {
        return;
    }

    //获取控件文本
    CString TypeText;
    TypeBox->GetWindowTextW(TypeText);
    if (TypeText.IsEmpty())
    {
        return;
    }

    //保存新的Type值
    std::wstring Type(TypeText.GetBuffer());
    Node.m_Value = Type;

    //更新MaxSize控件
    UINT MaxSizeID = theApp.g_mapLeaf[LeafType].GetValueID(gp_MaxSize);
    const std::shared_ptr<CEdit>& MaxSizeEdit = theApp.GetEditText(MaxSizeID);
    if (MaxSizeEdit != nullptr && theApp.g_mapQueueMaxSize.find(Type) != theApp.g_mapQueueMaxSize.end())
    {
        MaxSizeEdit->SetWindowTextW(theApp.g_mapQueueMaxSize[Type].c_str());
    }

    //更新Clsid控件
    UINT ClsidID = theApp.g_mapLeaf[LeafType].GetValueID(gp_Clsid);
    const std::shared_ptr<CEdit>& ClsidStatic = theApp.GetEditText(ClsidID);
    if (ClsidStatic != nullptr && theApp.g_mapQueueClsid.find(Type) != theApp.g_mapQueueClsid.end())
    {
        ClsidStatic->SetWindowTextW(theApp.g_mapQueueClsid[Type].c_str());
    }

    //更新Connstr控件
    std::vector<ParamNode>::size_type ConnstrIndex = theApp.g_mapLeaf[LeafType].GetIndex(gp_Connstr);
    UpdateQueueConnstr(theApp.g_mapLeaf[LeafType].m_vecParams[ConnstrIndex], LeafType);
}

void CDialogParams::UpdateQueueConnstr(ParamNode& Node, const std::wstring& LeafType)
{
    //先获取Type的值
    std::vector<ParamNode>::size_type TypeIndex = theApp.g_mapLeaf[LeafType].GetIndex(gp_Type);
    ParamNode& TypeNode = theApp.g_mapLeaf[LeafType].m_vecParams[TypeIndex];

    //如果是切换类型，要拆分的字符串取默认值
    theApp.ResetSubParamsControl();
    std::vector<ParamNode>& SubParamsNode = theApp.g_mapLeaf[LeafType].m_subParams;
    std::wstring ParamsValue = SubParamsNode.empty() ? Node.m_Value : theApp.g_mapQueueConnstr[TypeNode.m_Value];

    //如果值为空，说明不需要配置
    if (ParamsValue.empty())
    {
        const std::shared_ptr<CEdit>& EditText = theApp.GetEditText(this, Node.m_ValueID);
        EditText->MoveWindow(Node.m_ValueRect);
        EditText->SetWindowTextW(L"无需配置");
        EditText->ShowWindow(SW_SHOW);
        EditText->EnableWindow(FALSE);
        theApp.g_setSubParams.insert(Node.m_ValueID);
        return;
    }

    //拆分成具体的子项值
    std::vector<std::wstring> SubParamsValue;
    SplitString(ParamsValue, SubParamsValue, SLASH);
    if (SubParamsValue.empty())
    {
        return;
    }

    //准备控件类型
    SubParamsNode.clear();
    if (TypeNode.m_Value == gs_QueueType_kcxp)
    {
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_Port, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_Protocol, CT_STATIC_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_IP, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeKcxp_ConnectCount, CT_EDIT_TEXT));
    }
    else if (TypeNode.m_Value == gs_QueueType_zmq)
    {
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_Port, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_Mode, CT_COMBO_BOX_LIST));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_Protocol, CT_COMBO_BOX_LIST));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_IP, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_ThreadCount, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_CacheTime, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeZmq_HopCount, CT_EDIT_TEXT));
    }
    else if (TypeNode.m_Value == gs_QueueType_socket)
    {
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_Port, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_Mode, CT_COMBO_BOX_LIST));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_Protocol, CT_STATIC_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_IP, CT_EDIT_TEXT));
        SubParamsNode.push_back(ParamNode(gs_QueueTypeSocket_MsgLength, CT_EDIT_TEXT));
    }

    //子项的值和控件个数要匹配
    if (SubParamsValue.size() != SubParamsNode.size())
    {
        return;
    }

    //设置各个子配置项的位置
    {
        //获取总的可用区域
        CRect SubParamsRect;
        m_GridSubParams.GetWindowRect(&SubParamsRect);

        //创建生成坐标的对象
        CRect NewRect = Node.m_ValueRect;
        NewRect.bottom = SubParamsRect.bottom;
        CoordinateGenerator GeneratorRect(NewRect);
        GeneratorRect.SetSplitRatio(2, 7, 1);
        GeneratorRect.Begin(1, 2);

        //遍历控件，赋值显示
        for (std::vector<ParamNode>::size_type index = 0; index < SubParamsNode.size(); index++)
        {
            SubParamsNode[index].m_Value = SubParamsValue[index];
            SetParam(SubParamsNode[index], GeneratorRect, LeafType);
            theApp.g_setSubParams.insert(SubParamsNode[index].m_NameID);
            theApp.g_setSubParams.insert(SubParamsNode[index].m_ValueID);
        }
    }
}

void CDialogParams::UpdateCheckListBox(ParamNode& Node, const std::wstring& LeafType)
{
    //要有可供选择的数据
    const std::wstring& Key = LeafType + L"." + Node.m_Name;
    if (theApp.g_mapSelect.find(Key) == theApp.g_mapSelect.end())
    {
        return;
    }

    //设置显示的区域
    {
        CRect ListBoxRect = Node.m_ValueRect;
        ListBoxRect.top += CoordinateGenerator::OFFSET_Y;
        ListBoxRect.bottom = ListBoxRect.top + 200;
        m_CheckListBox.MoveWindow(&ListBoxRect);
        m_CheckListBox.ShowWindow(SW_SHOW);
        theApp.AddCRectToLog(L"ListBoxRect", ListBoxRect);
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
        std::vector<std::wstring> SelectItem;
        SplitString(Node.m_Value, SelectItem, SEMICOLON);
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

            CString TempText;
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
