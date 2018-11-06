#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileRecoveryDlg.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(FileRecoveryDlg, CDialogEx)

FileRecoveryDlg::FileRecoveryDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_RECOVERY, pParent)
{
}

FileRecoveryDlg::~FileRecoveryDlg()
{
}

void FileRecoveryDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
}

BEGIN_MESSAGE_MAP(FileRecoveryDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON1, &FileRecoveryDlg::OnBnClickedButtonSourcePath)
END_MESSAGE_MAP()

// FileRecoveryDlg message handlers

void FileRecoveryDlg::OnBnClickedButtonSourcePath()
{
    const std::wstring &SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
}
