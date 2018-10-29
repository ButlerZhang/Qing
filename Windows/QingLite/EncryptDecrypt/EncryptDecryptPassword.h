#pragma once
#include "afxwin.h"



class EncryptDecryptPassword : public CDialogEx
{
    DECLARE_DYNAMIC(EncryptDecryptPassword)

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_PASSWORD };
#endif

public:

    EncryptDecryptPassword(CWnd* pParent = NULL);   // standard constructor
    virtual ~EncryptDecryptPassword();

    BOOL UserDefinedShow();
    CString GetPassword() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnBnClickedCheckDefaultPassword();
    afx_msg void OnBnClickedCheckInputPassword();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    DECLARE_MESSAGE_MAP()

private:

    CEdit                        m_EditInputPassword;
    CEdit                        m_EditDefaultPassword;
    CButton                      m_CheckInputPassword;
    CButton                      m_CheckDefaultPassword;
};
