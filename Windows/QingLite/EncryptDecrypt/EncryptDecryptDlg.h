#pragma once
#include <string>
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"



class CEncryptDecryptDlg : public CDialogEx
{
private:

    enum OperationType
    {
        OT_ENCRYPT,
        OT_DECRYPT,
        OT_UNKNOW
    };

    enum ProcessType
    {
        PT_PROCEING,
        PT_SUCCEEDED,
        PT_FAILED
    };

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

    afx_msg void OnBnClickedStop();
    afx_msg void OnBnClickedExit();
    afx_msg void OnBnClickedEncrypt();
    afx_msg void OnBnClickedDecrypt();
    afx_msg void OnBnClickedCheckTargetPath();
    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedButtonSelectTargetPath();

private:

    void CreateResultList();
    void UpdateResultList(size_t Index, std::wstring &FilePath, ProcessType Type);

    std::wstring GetSelectPath() const;
    void SetControlEnable(bool Enable);
    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

private:

    CEdit                               m_EditSourcePath;
    CEdit                               m_EditTargetPath;
    CEdit                               m_EditPassword;
    CEdit                               m_EditConfirmPassword;
    CButton                             m_CheckTargetPath;
    CButton                             m_CheckEncryptFileName;
    CButton                             m_CheckDeleteOriginalFile;
    CButton                             m_ButtonEncrypt;
    CButton                             m_ButtonDecrypt;
    CButton                             m_ButtonStop;
    CButton                             m_ButtonExit;
    CListCtrl                           m_ResultList;
    OperationType                       m_OperationType;
    std::vector<std::wstring>           m_ProcessInfoVector;
};
