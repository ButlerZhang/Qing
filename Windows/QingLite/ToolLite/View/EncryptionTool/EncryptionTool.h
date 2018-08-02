#pragma once

#include "afxwin.h"
#include <string>



class EncryptionTool : public CDialogEx
{
    DECLARE_DYNAMIC(EncryptionTool)

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_ENCRYPTION };
#endif

public:

    EncryptionTool(CWnd* pParent = NULL);
    virtual ~EncryptionTool();

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedButtonSelectTargetPath();
    afx_msg void OnBnClickedButtonEncrypt();
    afx_msg void OnBnClickedButtonDecrypt();
    DECLARE_MESSAGE_MAP()

private:

    std::wstring GetSelectPath() const;

private:

    CEdit m_EditSourcePath;
    CEdit m_EditTargetPath;
};
