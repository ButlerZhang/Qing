﻿
// maServerConfigDlg.h: 头文件
//

#pragma once
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

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

private:  //初始化相关

    void InitControlSize();
    void InitCallbackFunction();

private:

    CString GetRootNodeName();
    CString GetLeafID(const CString& Text);
    CString GetLeftType(const CString& Text);

    void ResetControl();
    void SaveLastChange();
    bool LoadConfigFile(const std::string& XMLFile);

private:

    bool UpdateConfigTree();
    void UpdateParams(const std::wstring& LeafType, boost::property_tree::wptree::value_type& LeafNode);

    void DisplayXa(boost::property_tree::wptree::value_type& Xa);
    void DisplayNode(boost::property_tree::wptree::value_type& Node);
    void DisplayService(boost::property_tree::wptree::value_type& Service);
    void DisplayMsgqueue(boost::property_tree::wptree::value_type& MsgQueue);
    void DisplayRuntimeTable(boost::property_tree::wptree::value_type& RuntimeTable);

private:

    CString                                  m_LastSearchNode;     //上一次搜索的节点
    CString                                  m_LastLeafNode;       //上一次显示的叶子节点
    CString                                  m_LastLeafID;         //上一次显示的叶子ID
    CComboBox                                m_maItem;             //配置项
    CTreeCtrl                                m_ConfigTree;         //每个配置项对应的配置内容
    std::vector<CEdit*>                      m_vecEditText;        //存储参数值
    std::vector<CStatic*>                    m_vecStaticText;      //存储参数名称
    boost::property_tree::wptree             m_XMLTree;            //XML根节点
};
