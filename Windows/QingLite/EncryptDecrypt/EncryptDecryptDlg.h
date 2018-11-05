#pragma once
#include <string>
#include <vector>
#include <memory>
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Qing\HeaderFiles\SimpleEncrypt.h"

class FileEncryptDlg;
class FileDecryptDlg;
class FileCamouflageDlg;
class EncryptDecryptPassword;



class CEncryptDecryptDlg : public CDialogEx
{
    friend EncryptDecryptPassword;

private:

    enum OperationType
    {
        OT_ENCRYPT,
        OT_DECRYPT,
        OT_UNKNOW
    };

    enum ProcessType
    {
        PT_PROCEING,
        PT_SUCCEEDED,
        PT_FAILED
    };

// Construction
public:
	CEncryptDecryptDlg(CWnd* pParent = NULL);	// standard constructor
    ~CEncryptDecryptDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENCRYPTDECRYPT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:

    afx_msg void OnBnClickedStop();
    afx_msg void OnBnClickedExit();
    afx_msg void OnBnClickedEncrypt();
    afx_msg void OnBnClickedDecrypt();

    afx_msg void OnBnClickedButtonFileEncrypt();
    afx_msg void OnBnClickedButtonFileDecrypt();
    afx_msg void OnBnClickedButtonFileCamouflage();

private:

    void UpdateResultList(size_t Index, std::wstring &FilePath, ProcessType Type);

    bool Validate();
    void CreateResultList();
    void CreateWorkThread();
    void ReleaseThreadHandle();
    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

private:

    CButton                                 m_ButtonStop;
    CButton                                 m_ButtonExit;
    CListCtrl                               m_ResultList;
    OperationType                           m_OperationType;
    OperationType                           m_LastOperationType;
    HANDLE                                  m_WorkerThread;
    std::vector<std::wstring>               m_ProcessInfoVector;

    FileEncryptDlg                         *m_FileEncryptDlg;
    FileDecryptDlg                         *m_FileDecryptDlg;
    FileCamouflageDlg                      *m_FileCamouflageDlg;
    EncryptDecryptPassword                 *m_PasswordDlg;
    std::shared_ptr<Qing::SimpleEncrypt>    m_SimpleCrypt;
};
