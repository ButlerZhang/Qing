#pragma once
#include "afxwin.h"



class FileDecryptDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileDecryptDlg)

public:

    FileDecryptDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~FileDecryptDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_DECRYPT };
#endif

    BOOL UserDefinedShow();
    void ProcessWork(void *Parent);
    std::wstring GetSourcePath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedCheckDefaultPassword();
    afx_msg void OnBnClickedCheckInputPassword();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    bool Validate();
    bool SetOption();

private:

    CEdit           m_EditSourcePath;
    CButton         m_CheckDefaultPassword;
    CButton         m_CheckInputPassword;
    CEdit           m_EditDefaultPassword;
    CEdit           m_EditInputPassword;
};
