#pragma once
#include <string>
#include "afxwin.h"



class CEncryptDecryptDlg : public CDialogEx
{
// Construction
public:
	CEncryptDecryptDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENCRYPTDECRYPT_DIALOG };
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

private:

    afx_msg void OnBnClickedEncrypt();
    afx_msg void OnBnClickedDecrypt();
    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedButtonSelectTargetPath();

private:

    void CreateResultList();
    std::wstring GetSelectPath() const;

private:

    CEdit               m_EditSourcePath;
    CEdit               m_EditTargetPath;
    CEdit               m_EditPassword;
    CEdit               m_EditConfirmPassword;
    CButton             m_CheckEncryptFileName;
    CButton             m_CheckDeleteOriginalFile;
};
