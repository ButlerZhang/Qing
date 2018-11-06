#pragma once
#include "afxwin.h"



class FileEncryptDlg : public CDialogEx
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
    bool CreateTargetPath();
    void ClickPasswordOK() { CDialogEx::OnOK(); }

    bool IsEncryptFileName() const;
    bool IsEncryptFileData() const;
    bool IsDeleteOriginalFile() const;

    CString GetSourcePath() const;
    CString GetTargetPath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedButtonSourcePath();
    afx_msg void OnBnClickedButtonTargetPath();
    afx_msg void OnBnClickedCheckTargetPath();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    void UpdateTargetPath();

private:

    CEdit                           m_EditSourcePath;
    CEdit                           m_EditTargetPath;
    CButton                         m_CheckTargetPath;
    CButton                         m_CheckEncryptFileName;
    CButton                         m_CheckEncryptFileData;
    CButton                         m_CheckDeleteFile;
};
