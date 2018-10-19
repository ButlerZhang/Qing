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
    virtual BOOL OnInitDialog();

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedButtonSelectTargetPath();
    afx_msg void OnBnClickedEncrypt();
    afx_msg void OnBnClickedDecrypt();
    DECLARE_MESSAGE_MAP()

private:

    std::wstring GetSelectPath() const;

private:

    CEdit m_EditSourcePath;
    CEdit m_EditTargetPath;
};
