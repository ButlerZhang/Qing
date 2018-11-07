#pragma once
#include <string>
#include <vector>
#include <memory>
#include "afxwin.h"
#include "afxcmn.h"
#include "..\..\Qing\HeaderFiles\SimpleEncrypt.h"



class CEncryptDecryptDlg : public CDialogEx
{
public:
    CEncryptDecryptDlg(CWnd* pParent = NULL);	// standard constructor
    ~CEncryptDecryptDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ENCRYPTDECRYPT_DIALOG };
#endif

    void CreateWorkThread();
    void GetFiles(std::vector<std::wstring> &FileVector);
    void UpdateResultList(size_t Index, std::wstring &FilePath, ProcessType Type);

    inline void ResetOperationType() { m_OperationType = m_LastOperationType; }
    inline void SetOperationType(OperationType Type) { m_OperationType = Type; }
    inline const std::shared_ptr<Qing::SimpleEncrypt>& GetSimpleEncrypt() const { return m_SimpleCrypt; }

protected:
    HICON m_hIcon;

    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private:

    afx_msg void OnBnClickedStop();
    afx_msg void OnBnClickedExit();
    afx_msg void OnBnClickedButtonFileEncrypt();
    afx_msg void OnBnClickedButtonFileDecrypt();
    afx_msg void OnBnClickedButtonFileDisguise();
    afx_msg void OnBnClickedButtonFileRecovery();

private:

    void CreateResultList();
    void ReleaseThreadHandle();
    void SetOptionButtonEnable(bool Enable);
    void ResetControlAfterWorkerThreadStop();
    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

private:

    CButton                                         m_ButtonStop;
    CButton                                         m_ButtonExit;
    CButton                                         m_ButtonEncrypt;
    CButton                                         m_ButtonDecrypt;
    CButton                                         m_ButtonDisguise;
    CButton                                         m_ButtonRecovery;
    CListCtrl                                       m_ResultList;
    OperationType                                   m_OperationType;
    OperationType                                   m_LastOperationType;
    HANDLE                                          m_WorkerThread;
    std::vector<BaseDialog*>                        m_DialogVector;
    std::shared_ptr<Qing::SimpleEncrypt>            m_SimpleCrypt;
};
