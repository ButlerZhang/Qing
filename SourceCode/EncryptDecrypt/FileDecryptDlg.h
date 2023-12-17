#pragma once
#include "afxwin.h"
#include <memory>
#include "../Windows/System/SimpleEncrypt.h"



class FileDecryptDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileDecryptDlg)

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_DECRYPT };
#endif

public:

    FileDecryptDlg(CWnd* pParent = NULL);
    virtual ~FileDecryptDlg();

    virtual BOOL ShowChildWindowMiddle();
    virtual std::wstring GetSourcePath() const;

    virtual void ProcessWork(void* Parent);
    virtual void Stop() { m_SimpleCrypt->SetIsForceStop(true); }

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    bool Validate();
    bool SetOption();

private:

    CEdit                                 m_EditSourcePath;               //文件路径
    CEdit                                 m_EditInputPassword;            //解密密码
    std::shared_ptr<SimpleEncrypt>        m_SimpleCrypt;                  //加解密对象
};
