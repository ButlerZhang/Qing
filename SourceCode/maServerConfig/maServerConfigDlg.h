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

    CString GetRootNodeName();
    void SaveLastChange();

    void InitControlSize();
    bool UpdateTreeConfig();
    bool LoadConfigFile(const std::string& XMLFile);

    inline CString& GetLastSearchNode() { return m_LastSearchNode; }
    inline CString& GetLastLeafNode() { return m_LastLeafNode; }
    inline CString& GetLastLeafID() { return m_LastLeafID; }

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

    //配置项选择事件，比如选择ma架构还是kernel
    afx_msg void OnComboBoxConfigChange();

    //响应配置项构成的树的点击事件
    afx_msg void OnTreeConfigChange(NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg void OnMenuOpenFile();
    afx_msg void OnMenuSaveFile();
    afx_msg void OnMenuDebugInfo();
    afx_msg void OnMenuAbout();

private:

    CString                 m_LastSearchNode;     //上一次搜索的枝干节点，例如[ma.kernel.rtdb]
    CString                 m_LastLeafNode;       //上一次显示的叶子节点，例如[runtimetable_01_sysma_st_node]
    CString                 m_LastLeafID;         //上一次显示的叶子标识，例如[01]

    CComboBox               m_ComboBoxMa;         //MA架构配置项
    CTreeCtrl               m_TreeConfig;         //每个配置项对应的树形列表
};
