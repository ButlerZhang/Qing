#pragma once
#include "afxwin.h"
#include <memory>
#include "../Windows/System/SimpleEncrypt.h"



class FileEncryptDlg : public BaseDialog
{
    DECLARE_DYNAMIC(FileEncryptDlg)

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_ENCRYPT };
#endif

public:

    FileEncryptDlg(CWnd* pParent = NULL);
    virtual ~FileEncryptDlg();

    virtual BOOL ShowChildWindowMiddle();
    virtual std::wstring GetSourcePath() const;
    virtual std::wstring GetTargetPath() const;

    virtual void ProcessWork(void* Parent);
    virtual void Stop() { m_SimpleCrypt->SetIsForceStop(true); }

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedButtonSourcePath();
    afx_msg void OnBnClickedButtonTargetPath();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    bool Validate();
    bool SetOption();

private:

    CEdit                                 m_EditSourcePath;               //文件路径
    CEdit                                 m_EditTargetPath;               //保存路径
    CEdit                                 m_EditInputPassword;            //输入密码
    CEdit                                 m_EditRepeatPassword;           //确认密码
    CButton                               m_CheckEncryptFileName;         //加密文件名
    CButton                               m_CheckDeleteFile;              //删除源文件
    std::shared_ptr<SimpleEncrypt>        m_SimpleCrypt;                  //加解密对象
};
