#pragma once
#include "afxwin.h"



class FileDisguiseDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileDisguiseDlg)

public:
    FileDisguiseDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~FileDisguiseDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_CAMOUFLAGE };
#endif

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSelectSourcePath();

private:

    CEdit            m_EditSourcePath;
};
