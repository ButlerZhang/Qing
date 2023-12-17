#pragma once
#include "afxwin.h"
#include <memory>
#include <queue>
#include <mutex>
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

    virtual void Stop();
    virtual void ProcessWork(void* Parent);

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnBnClickedButtonSourcePath();
    afx_msg void OnBnClickedButtonTargetPath();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();

    bool Validate();
    bool SetOption();
    void MultiThreadWork(void* Parent, int ThreadIndex, const std::wstring& TargetPath);

private:

    CEdit                                           m_EditSourcePath;               //文件路径
    CEdit                                           m_EditTargetPath;               //保存路径
    CEdit                                           m_EditInputPassword;            //输入密码
    CEdit                                           m_EditRepeatPassword;           //确认密码
    CButton                                         m_CheckEncryptFileName;         //加密文件名
    CButton                                         m_CheckDeleteFile;              //删除源文件
    std::mutex                                      m_ResultMutex;                  //更新界面锁
    std::mutex                                      m_TaskQueueMutex;               //任务队列锁
    std::wstring                                    m_InputPassword;                //输入的密码
    std::queue<TaskNode>                            m_TaskQueue;                    //任务队列
    std::shared_ptr<SimpleEncrypt>                  m_SimpleCrypt;                  //单线程加解密对象
    std::vector<std::thread>                        m_vecThread;                    //多线程对象
    std::vector<std::shared_ptr<SimpleEncrypt>>     m_vecSimpleEncrypt;             //多线程对应的加解密对象
};
