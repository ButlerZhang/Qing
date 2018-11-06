#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "EncryptDecryptDlg.h"
#include "FileRecoveryDlg.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(FileRecoveryDlg, CDialogEx)

FileRecoveryDlg::FileRecoveryDlg(CWnd* pParent /*=NULL*/)
    : BaseDialog(IDD_DIALOG_RECOVERY, pParent)
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
    ON_BN_CLICKED(IDOK, &FileRecoveryDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileRecoveryDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// FileRecoveryDlg message handlers
void FileRecoveryDlg::OnBnClickedOk()
{
    if (theApp.Validate(m_EditSourcePath))
    {
        CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
        ParentDlg->SetOperationType(OT_RECOVERY);
        ParentDlg->CreateWorkThread();
        CDialogEx::OnOK();
    }
}

void FileRecoveryDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->ResetOperationType();
    CDialogEx::OnCancel();
}

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
