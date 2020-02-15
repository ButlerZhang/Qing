#pragma once
#include "afxwin.h"



class FileRecoveryDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileRecoveryDlg)

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_UN_CAMOUFLAGE };
#endif

public:

    FileRecoveryDlg(CWnd* pParent = NULL);
    virtual ~FileRecoveryDlg();

    virtual void ProcessWork(void *Parent);
    virtual std::wstring GetSourcePath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSourcePath();

private:

    CEdit                   m_EditSourcePath;
};
