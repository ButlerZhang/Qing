#pragma once
#include "afxwin.h"



class FileDecryptDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileDecryptDlg)

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_DECRYPT };
#endif

public:

    FileDecryptDlg(CWnd* pParent = NULL);
    virtual ~FileDecryptDlg();

    virtual BOOL ShowChildWindowMiddle();
    virtual void ProcessWork(void *Parent);
    virtual std::wstring GetSourcePath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedCheckDefaultPassword();
    afx_msg void OnBnClickedCheckInputPassword();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    bool Validate();
    bool SetOption();

private:

    CEdit                   m_EditSourcePath;
    CEdit                   m_EditDefaultPassword;
    CEdit                   m_EditInputPassword;
    CButton                 m_CheckDefaultPassword;
    CButton                 m_CheckInputPassword;
};
