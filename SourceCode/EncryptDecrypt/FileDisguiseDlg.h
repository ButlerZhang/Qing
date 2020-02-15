#pragma once
#include "afxwin.h"



class FileDisguiseDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileDisguiseDlg)

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_CAMOUFLAGE };
#endif

public:

    FileDisguiseDlg(CWnd* pParent = NULL);
    virtual ~FileDisguiseDlg();

    virtual void ProcessWork(void *Parent);
    virtual std::wstring GetSourcePath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSelectSourcePath();

private:

    CEdit                   m_EditSourcePath;
};
