
// maServerConfigDlg.h: 头文件
//

#pragma once


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

private:

    afx_msg void OnBnClickedOk();
    afx_msg void OnCbnSelchangeCombo1_ChangeNode();
    afx_msg void OnTvnSelchangedTreeItem(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedButtonGenerate();

private:

    void InitControlSize();
    CString GetRootNodeName();

    void ResetControl();
    void SaveLastChange();
    bool LoadConfigFile(const std::string& XMLFile);

private:

    bool UpdateConfigTree();
    void UpdateParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode);
    void DisplayParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode);

private:

    CString                                  m_LastSearchNode;     //上一次搜索的节点
    CString                                  m_LastLeafNode;       //上一次显示的叶子节点
    CString                                  m_LastLeafID;         //上一次显示的叶子ID
    CComboBox                                m_maItem;             //配置项
    CTreeCtrl                                m_ConfigTree;         //每个配置项对应的配置内容
    std::vector<std::shared_ptr<CEdit>>      m_vecEditText;        //存储参数值
    std::vector<std::shared_ptr<CStatic>>    m_vecStaticText;      //存储参数名称
};
