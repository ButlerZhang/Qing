#pragma once
#include "afxwin.h"
#include <memory>
#include "../Windows/System/SimpleEncrypt.h"



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

    virtual void ProcessWork(void* Parent);
    virtual void Stop() { m_SimpleCrypt->SetIsForceStop(true); }

    virtual std::wstring GetSourcePath() const;

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSourcePath();

private:

    CEdit                                 m_EditSourcePath;               //文件路径
    std::shared_ptr<SimpleEncrypt>        m_SimpleCrypt;                  //加解密对象
};
