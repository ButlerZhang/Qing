#pragma once
#include "afxwin.h"



class FileEncryptDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileEncryptDlg)

public:
    FileEncryptDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~FileEncryptDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_ENCRYPT };
#endif

    BOOL UserDefinedShow();
    std::wstring GetSourcePath() const;
    std::wstring GetTargetPath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedCheckDefaultPassword();
    afx_msg void OnBnClickedCheckInputPassword();
    afx_msg void OnBnClickedButtonSourcePath();
    afx_msg void OnBnClickedButtonTargetPath();
    afx_msg void OnBnClickedCheckTargetPath();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    bool Validate();
    bool SetOption();
    void UpdateTargetPath();

private:

    CEdit                           m_EditSourcePath;
    CEdit                           m_EditTargetPath;
    CEdit                           m_EditDefaultPassword;
    CEdit                           m_EditInputPassword;
    CButton                         m_CheckTargetPath;
    CButton                         m_CheckEncryptFileName;
    CButton                         m_CheckEncryptFileData;
    CButton                         m_CheckDeleteFile;
    CButton                         m_CheckDefaultPassword;
    CButton                         m_CheckInputPassword;
};
