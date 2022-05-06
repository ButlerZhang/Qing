﻿#pragma once

// CmaServerConfigDlg 对话框
class CmaServerConfigDlg : public CDialogEx
{
    // 构造
public:
    CmaServerConfigDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MASERVERCONFIG_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private: //添加的响应函数

    //OK按钮响应事件，暂未启用
    afx_msg void OnBnClickedOk();

    //按钮点击事件，可以弹出CheckListBox
    afx_msg void OnBnClicked(UINT uID);

    //勾选框点击事件
    afx_msg void OnCheckBoxClicked(UINT uID);

    //CheckListBox选择响应事件
    afx_msg void OnCheckListBoxChange();

    //配置项选择事件，比如选择ma架构还是kernel
    afx_msg void OnComboBoxConfigChange();

    //ComBox List选择响应事件，预定10个
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

    //生成配置文件事件
    afx_msg void OnButtonClickedGenerateConfig();

    //响应配置项构成的树的点击事件
    afx_msg void OnTreeConfigChange(NMHDR* pNMHDR, LRESULT* pResult);

private:

    void InitControlSize();
    CString GetRootNodeName();

    void SaveLastChange();
    bool LoadConfigFile(const std::string& XMLFile);

private:

    bool UpdateTreeConfig();
    void UpdateParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode);
    void DisplayParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode);

private:

    CString             m_LastSearchNode;     //上一次搜索的节点，例如[ma.kernel.rtdb]
    CString             m_LastLeafNode;       //上一次显示的叶子节点，例如[runtimetable_01_sysma_st_node]
    CString             m_LastLeafID;         //上一次显示的叶子ID，例如[01]
    CStatic             m_ComplexParams;      //复合参数区域
    CComboBox           m_ComboBoxConfig;     //配置项
    CTreeCtrl           m_TreeConfig;         //每个配置项对应的配置内容
    CCheckListBox       m_CheckListBox;       //下拉多选框
};
