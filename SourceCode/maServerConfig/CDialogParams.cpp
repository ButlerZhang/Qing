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

    CString LeafType = theApp.GetLeftType(((CmaServerConfigDlg*)GetParent())->GetLastLeafNode());
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
void CDialogParams::OnCheckListBoxChange()
{
    //找到按钮ID
    UINT ButtonID = 0;
    CString LeafType = theApp.GetLeftType(((CmaServerConfigDlg*)GetParent())->GetLastLeafNode());
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
void CDialogParams::OnCheckBoxClicked(UINT uID)
{
    CString LeafType = theApp.GetLeftType(((CmaServerConfigDlg*)GetParent())->GetLastLeafNode());
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
        if (LastSearchNode.Compare(Queues.c_str()) == 0)
        {
            CString LeafType = theApp.GetLeftType(LastLeafNode);
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

void CDialogParams::Hide()
{
    m_GridSubParams.ShowWindow(SW_HIDE);
    m_CheckListBox.ShowWindow(SW_HIDE);
}

void CDialogParams::UpdateParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode)
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

void CDialogParams::DisplayParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode)
{
    CRect ParamsArea;
    GetClientRect(ParamsArea);

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

void CDialogParams::UpdateQueueType(const std::wstring& LeafType, ParamNode& Node)
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

void CDialogParams::UpdateQueueConnstr(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType, const std::wstring& Type)
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
        ScreenToClient(ButtonRect);

        SubParamGridRect.left = ButtonRect.left;
        SubParamGridRect.right = ButtonRect.right - 2;
        SubParamGridRect.top = ButtonRect.bottom + CoordinateGenerator::OFFSET_Y;
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

void CDialogParams::UpdateComboBox(const std::shared_ptr<CComboBox>& ptrComboBox, const std::wstring& LeafType, const ParamNode& Node)
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

void CDialogParams::UpdateNodeUse(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType, const std::wstring& Type)
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
        ScreenToClient(ButtonRect);

        SubParamGridRect.left = ButtonRect.left;
        SubParamGridRect.right = ButtonRect.right - 2;
        SubParamGridRect.top = ButtonRect.bottom + CoordinateGenerator::OFFSET_Y;
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

void CDialogParams::UpdateCheckListBox(const std::shared_ptr<CButton>& ptrButton, const std::wstring& LeafType, const ParamNode& Node)
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
        ScreenToClient(&ButtonRect);

        CRect ListBoxRect;
        ListBoxRect.left = ButtonRect.left;
        ListBoxRect.right = ButtonRect.right - 3;
        ListBoxRect.top = ButtonRect.bottom;
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
