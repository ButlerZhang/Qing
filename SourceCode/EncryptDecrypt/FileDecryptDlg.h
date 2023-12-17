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

    CEdit                                           m_EditSourcePath;               //�ļ�·��
    CEdit                                           m_EditInputPassword;            //��������
    std::mutex                                      m_ResultMutex;                  //���½�����
    std::mutex                                      m_TaskQueueMutex;               //���������
    std::wstring                                    m_InputPassword;                //���������
    std::queue<TaskNode>                            m_TaskQueue;                    //�������
    std::vector<std::thread>                        m_vecThread;                    //���̶߳���
    std::shared_ptr<SimpleEncrypt>                  m_SimpleCrypt;                  //�ӽ��ܶ���
    std::vector<std::shared_ptr<SimpleEncrypt>>     m_vecSimpleEncrypt;             //���̶߳�Ӧ�ļӽ��ܶ���
};
