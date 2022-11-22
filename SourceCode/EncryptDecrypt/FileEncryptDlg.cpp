#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileEncryptDlg.h"
#include "EncryptDecryptDlg.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(FileEncryptDlg, CDialogEx)

FileEncryptDlg::FileEncryptDlg(CWnd* pParent /*=NULL*/)
    : BaseDialog(IDD_DIALOG_ENCRYPT, pParent)
{
}

FileEncryptDlg::~FileEncryptDlg()
{
}

BOOL FileEncryptDlg::ShowChildWindowMiddle()
{
    m_CheckDeleteFile.SetCheck(BST_UNCHECKED);
    m_CheckEncryptFileName.SetCheck(BST_CHECKED);

    m_EditInputPassword.SetWindowTextW(L"");
    m_EditRepeatPassword.SetWindowTextW(L"");

    return BaseDialog::ShowChildWindowMiddle();
}

void FileEncryptDlg::ProcessWork(void *Parent)
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)Parent;
    const std::wstring &TargetPath = GetTargetPath();

    std::vector<std::wstring> FileNameVector;
    ParentDlg->GetFiles(FileNameVector);

    for (std::vector<std::wstring>::size_type Index = 0; Index < FileNameVector.size(); Index++)
    {
        if (ParentDlg->GetSimpleEncrypt()->IsForceStop())
        {
            break;
        }

        ParentDlg->UpdateResultList(Index + 1, FileNameVector[Index], PT_PROCEING);
        bool ProcessResult = ParentDlg->GetSimpleEncrypt()->Encrypt(FileNameVector[Index], TargetPath);
        ParentDlg->UpdateResultList(Index + 1, FileNameVector[Index], ProcessResult ? PT_SUCCEEDED : PT_FAILED);
    }
}

std::wstring FileEncryptDlg::GetSourcePath() const
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    return SourcePath.GetString();
}

std::wstring FileEncryptDlg::GetTargetPath() const
{
    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);

    wchar_t Path[MAX_PATH] = { 0 };
    wmemcpy(Path, TargetPath.GetString(), TargetPath.GetLength());

    PathAddBackslash(Path);
    TargetPath = Path;

    return TargetPath.GetString();
}

bool FileEncryptDlg::Validate()
{
    if (!theApp.Validate(m_EditSourcePath))
    {
        return false;
    }

    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (!TargetPath.IsEmpty())
    {
        if (!PathIsDirectory(TargetPath.GetString()) && SHCreateDirectoryEx(NULL, TargetPath.GetString(), NULL) != ERROR_SUCCESS)
        {
            MessageBox(_T("Target path can not created!"), _T("Error Tip"), MB_OK);
            return false;
        }
    }

    return true;
}

bool FileEncryptDlg::SetOption()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->GetSimpleEncrypt()->SetIsEncryptFileName(m_CheckEncryptFileName.GetState() == BST_CHECKED);
    ParentDlg->GetSimpleEncrypt()->SetIsDeleteOriginalFile(m_CheckDeleteFile.GetState() == BST_CHECKED);
    ParentDlg->GetSimpleEncrypt()->SetIsForceStop(false);

    CString InputPassword;
    m_EditInputPassword.GetWindowTextW(InputPassword);

    CString RepeatPassword;
    m_EditRepeatPassword.GetWindowText(RepeatPassword);

    if (InputPassword != RepeatPassword)
    {
        MessageBox(_T("The passwords entered twice are different!"), _T("Error Tip"), MB_OK);
        return false;
    }

    ParentDlg->GetSimpleEncrypt()->SetPassword(InputPassword.GetString());
    return true;
}

void FileEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_TARGET_PATH, m_EditTargetPath);
    DDX_Control(pDX, IDC_EDIT_INPUT_PASSWORD, m_EditInputPassword);
    DDX_Control(pDX, IDC_EDIT_REPEAT_PASSWORD, m_EditRepeatPassword);
    DDX_Control(pDX, IDC_CHECK_ENCRYPT_FILE_NAME, m_CheckEncryptFileName);
    DDX_Control(pDX, IDC_CHECK_DELETE_FILE, m_CheckDeleteFile);
}

BEGIN_MESSAGE_MAP(FileEncryptDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SOURCE_PATH, &FileEncryptDlg::OnBnClickedButtonSourcePath)
    ON_BN_CLICKED(IDC_BUTTON_TARGET_PATH, &FileEncryptDlg::OnBnClickedButtonTargetPath)
    ON_BN_CLICKED(IDOK, &FileEncryptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileEncryptDlg::OnBnClickedCancel)
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
}

void FileEncryptDlg::OnBnClickedButtonTargetPath()
{
    const std::wstring SelectPath = theApp.GetSelectPath();
    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

void FileEncryptDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->ResetOperationType();
    CDialogEx::OnCancel();
}

void FileEncryptDlg::OnBnClickedOk()
{
    if (Validate() && SetOption())
    {
        CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
        ParentDlg->SetOperationType(OT_ENCRYPT);
        ParentDlg->Start();
        CDialogEx::OnOK();
    }
}
