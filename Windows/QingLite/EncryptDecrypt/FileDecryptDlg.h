#pragma once
#include "afxwin.h"



class FileDecryptDlg : public CDialogEx
{
    DECLARE_DYNAMIC(FileDecryptDlg)

public:

    FileDecryptDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~FileDecryptDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_DECRYPT };
#endif

    void ClickPasswordOK() { CDialogEx::OnOK(); }

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSelectSourcePath();

private:

    CEdit m_EditSourcePath;
};
