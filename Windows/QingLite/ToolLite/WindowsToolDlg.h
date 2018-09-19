
// WindowsToolDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include <vector>
#include <memory>



// CWindowsToolDlg dialog
class CWindowsToolDlg : public CDialogEx
{
// Construction
public:
	CWindowsToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINDOWSTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    afx_msg void OnTcnSelchangeTabTool(NMHDR *pNMHDR, LRESULT *pResult);

private:

    void InitTabControl();

private:

    CTabCtrl                                        m_ToolTab;
    std::vector<std::shared_ptr<CDialogEx>>         m_ToolVector;
};