#pragma once
#include "afxwin.h"
#include <memory>
#include "../Windows/System/SimpleEncrypt.h"
#include <thread>
#include <mutex>
#include <queue>



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

    virtual void Stop();
    virtual void ProcessWork(void* Parent);

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedButtonSelectSourcePath();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    bool Validate();
    bool SetOption();
    void MultiThreadWork(void* Parent, int ThreadIndex, const std::wstring& TargetPath);

private:

    CEdit                                           m_EditSourcePath;               //文件路径
    CEdit                                           m_EditInputPassword;            //解密密码
    std::mutex                                      m_ResultMutex;                  //更新界面锁
    std::mutex                                      m_TaskQueueMutex;               //任务队列锁
    std::wstring                                    m_InputPassword;                //输入的密码
    std::queue<TaskNode>                            m_TaskQueue;                    //任务队列
    std::vector<std::thread>                        m_vecThread;                    //多线程对象
    std::shared_ptr<SimpleEncrypt>                  m_SimpleCrypt;                  //加解密对象
    std::vector<std::shared_ptr<SimpleEncrypt>>     m_vecSimpleEncrypt;             //多线程对应的加解密对象
};
