#pragma once
#include "afxwin.h"



class FileRecoveryDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileRecoveryDlg)

public:
    FileRecoveryDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~FileRecoveryDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_UN_CAMOUFLAGE };
#endif

    void ProcessWork(void *Parent);
    std::wstring GetSourcePath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSourcePath();

private:

    CEdit           m_EditSourcePath;
};
