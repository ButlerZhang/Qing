#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileEncryptDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(FileEncryptDlg, CDialogEx)



FileEncryptDlg::FileEncryptDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_ENCRYPT, pParent)
{
}

FileEncryptDlg::~FileEncryptDlg()
{
}

CString FileEncryptDlg::GetSourcePath() const
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    return SourcePath;
}

CString FileEncryptDlg::GetTargetPath() const
{
    CString TargetPath;
    if (m_CheckTargetPath.GetState() == BST_CHECKED)
    {
        m_EditTargetPath.GetWindowTextW(TargetPath);
    }

    return TargetPath;
}

void FileEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_TARGET_PATH, m_EditTargetPath);
    DDX_Control(pDX, IDC_CHECK_TARGET_PATH, m_CheckTargetPath);
    DDX_Control(pDX, IDC_CHECK_ENCRYPT_FILE_NAME, m_CheckEncryptFileName);
    DDX_Control(pDX, IDC_CHECK_ENCRYPT_FILE_DATA, m_CheckEncryptFileData);
    DDX_Control(pDX, IDC_CHECK_DELETE_FILE, m_CheckDeleteFile);
}

BEGIN_MESSAGE_MAP(FileEncryptDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SOURCE_PATH, &FileEncryptDlg::OnBnClickedButtonSourcePath)
    ON_BN_CLICKED(IDC_BUTTON_TARGET_PATH, &FileEncryptDlg::OnBnClickedButtonTargetPath)
    ON_BN_CLICKED(IDOK, &FileEncryptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileEncryptDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_CHECK_TARGET_PATH, &FileEncryptDlg::OnBnClickedCheckTargetPath)
END_MESSAGE_MAP()

// FileEncrypt message handlers
void FileEncryptDlg::OnBnClickedButtonSourcePath()
{
    const std::wstring &SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
    if (m_CheckTargetPath.GetState() == BST_CHECKED)
    {
        UpdateTargetPath();
    }
}


void FileEncryptDlg::OnBnClickedButtonTargetPath()
{
    const std::wstring SelectPath = theApp.GetSelectPath();
    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

void FileEncryptDlg::OnBnClickedCheckTargetPath()
{
    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (TargetPath.IsEmpty())
    {
        UpdateTargetPath();
    }
}

void FileEncryptDlg::OnBnClickedOk()
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    if (SourcePath.IsEmpty())
    {
        MessageBox(_T("Source path is empty!"), _T("Error Tip"), MB_OK);
        return;
    }

    if (!PathFileExists(SourcePath.GetString()))
    {
        MessageBox(_T("Source path is not exists!"), _T("Error Tip"), MB_OK);
        return;
    }

    CDialogEx::OnOK();
}

void FileEncryptDlg::OnBnClickedCancel()
{
    CDialogEx::OnCancel();
}

void FileEncryptDlg::UpdateTargetPath()
{
    m_EditTargetPath.SetWindowTextW(L"");

    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);

    if (!SourcePath.IsEmpty())
    {
        if (!PathIsDirectory(SourcePath.GetString()))
        {
            PathRemoveFileSpec((LPWSTR)SourcePath.GetString());
        }

        std::wstring TargetPath = SourcePath.GetString() + std::wstring(L"\\EncryptDecrypt\\");
        m_EditTargetPath.SetWindowTextW(TargetPath.c_str());
    }
}
