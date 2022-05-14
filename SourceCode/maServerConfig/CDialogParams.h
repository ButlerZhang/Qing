#pragma once
#include "CoordinateGenerator.h"

class CDialogParams : public CDialogEx
{
    DECLARE_DYNAMIC(CDialogParams)

public:
    CDialogParams(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CDialogParams();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_PARAMS };
#endif

    void Hide();

    void UpdateParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode);
    void DisplayParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode);

    void UpdateEdit(const std::shared_ptr<CEdit>& ptrEdit, ControlType Type);
    void UpdateQueueType(const std::wstring& LeafType, ParamNode& Node);
    void UpdateQueueConnstr(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType, const std::wstring& Type);
    void UpdateXaOpen(const std::shared_ptr<CButton>& pButton, const std::wstring& LeafType);
    void UpdateComboBox(const std::shared_ptr<CComboBox>& ptrComboBox, const std::wstring& LeafType, const ParamNode& Node);

    void UpdateNodeUse(const ParamNode& Node, const std::wstring& LeafType, const CRect& TempRect);
    void UpdateCheckListBox(const ParamNode& Node, const std::wstring& LeafType, const CRect& TempRect);

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:

    //点击Button弹出CheckListBox或由多个控件组成的子配置项
    afx_msg void OnBnClicked(UINT uID);

    //CheckListBox选择响应事件
    afx_msg void OnCheckListBoxChange();

    //勾选框点击事件
    afx_msg void OnCheckBoxClicked(UINT uID);

    //ComboBoxList选择响应事件，预定10个
    afx_msg void OnComboBoxListSelectChange(UINT uID);
    afx_msg void OnComboBoxListSelectChange1() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_1); }
    afx_msg void OnComboBoxListSelectChange2() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_2); }
    afx_msg void OnComboBoxListSelectChange3() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_3); }
    afx_msg void OnComboBoxListSelectChange4() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_4); }
    afx_msg void OnComboBoxListSelectChange5() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_5); }
    afx_msg void OnComboBoxListSelectChange6() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_6); }
    afx_msg void OnComboBoxListSelectChange7() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_7); }
    afx_msg void OnComboBoxListSelectChange8() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_8); }
    afx_msg void OnComboBoxListSelectChange9() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_9); }
    afx_msg void OnComboBoxListSelectChange10() { OnComboBoxListSelectChange(IDC_COMBOX_LIST_10); }

private:

    void SetParam(ParamNode& Node, CoordinateGenerator& GeneratorRect, const std::wstring& LeafType);

private:

    CStatic                 m_GridTitle;          //配置的注释区域
    CStatic                 m_GridNormal;         //配置的常规区域
    CStatic                 m_GridSubParams;      //配置的子项区域
    CCheckListBox           m_CheckListBox;       //下拉多选框
};
