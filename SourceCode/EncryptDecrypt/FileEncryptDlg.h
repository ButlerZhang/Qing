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

    CEdit                                           m_EditSourcePath;               //�ļ�·��
    CEdit                                           m_EditTargetPath;               //����·��
    CEdit                                           m_EditInputPassword;            //��������
    CEdit                                           m_EditRepeatPassword;           //ȷ������
    CButton                                         m_CheckEncryptFileName;         //�����ļ���
    CButton                                         m_CheckDeleteFile;              //ɾ��Դ�ļ�
    std::mutex                                      m_ResultMutex;                  //���½�����
    std::mutex                                      m_TaskQueueMutex;               //���������
    std::wstring                                    m_InputPassword;                //���������
    std::queue<TaskNode>                            m_TaskQueue;                    //�������
    std::shared_ptr<SimpleEncrypt>                  m_SimpleCrypt;                  //���̼߳ӽ��ܶ���
    std::vector<std::thread>                        m_vecThread;                    //���̶߳���
    std::vector<std::shared_ptr<SimpleEncrypt>>     m_vecSimpleEncrypt;             //���̶߳�Ӧ�ļӽ��ܶ���
};
