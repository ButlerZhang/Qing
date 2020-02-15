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
    m_CheckEncryptFileName.SetCheck(BST_CHECKED);
    m_CheckDeleteFile.SetCheck(BST_CHECKED);

    m_CheckInputPassword.SetCheck(BST_UNCHECKED);
    m_CheckDefaultPassword.SetCheck(BST_CHECKED);

    m_EditInputPassword.SetWindowTextW(L"");
    m_EditDefaultPassword.SetWindowTextW(L"************");

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
    if (m_CheckTargetPath.GetState() == BST_CHECKED)
    {
        m_EditTargetPath.GetWindowTextW(TargetPath);
    }

    return TargetPath.GetString();
}

bool FileEncryptDlg::Validate()
{
    if (!theApp.Validate(m_EditSourcePath))
    {
        return false;
    }

    if (m_CheckTargetPath.GetState() == BST_CHECKED)
    {
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
    if (!InputPassword.IsEmpty())
    {
        ParentDlg->GetSimpleEncrypt()->SetPassword(InputPassword.GetString());
    }

    return true;
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

void FileEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_TARGET_PATH, m_EditTargetPath);
    DDX_Control(pDX, IDC_CHECK_TARGET_PATH, m_CheckTargetPath);
    DDX_Control(pDX, IDC_CHECK_ENCRYPT_FILE_NAME, m_CheckEncryptFileName);
    DDX_Control(pDX, IDC_CHECK_DELETE_FILE, m_CheckDeleteFile);
    DDX_Control(pDX, IDC_CHECK_DEFAULT_PASSWORD, m_CheckDefaultPassword);
    DDX_Control(pDX, IDC_CHECK_INPUT_PASSWORD, m_CheckInputPassword);
    DDX_Control(pDX, IDC_EDIT_DEFAULT_PASSWORD, m_EditDefaultPassword);
    DDX_Control(pDX, IDC_EDIT_INPUT_PASSWORD, m_EditInputPassword);
}

BEGIN_MESSAGE_MAP(FileEncryptDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SOURCE_PATH, &FileEncryptDlg::OnBnClickedButtonSourcePath)
    ON_BN_CLICKED(IDC_BUTTON_TARGET_PATH, &FileEncryptDlg::OnBnClickedButtonTargetPath)
    ON_BN_CLICKED(IDOK, &FileEncryptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileEncryptDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_CHECK_TARGET_PATH, &FileEncryptDlg::OnBnClickedCheckTargetPath)
    ON_BN_CLICKED(IDC_CHECK_DEFAULT_PASSWORD, &FileEncryptDlg::OnBnClickedCheckDefaultPassword)
    ON_BN_CLICKED(IDC_CHECK_INPUT_PASSWORD, &FileEncryptDlg::OnBnClickedCheckInputPassword)
END_MESSAGE_MAP()

// FileEncrypt message handlers
void FileEncryptDlg::OnBnClickedCheckDefaultPassword()
{
    m_CheckInputPassword.SetCheck(BST_UNCHECKED);
    m_CheckDefaultPassword.SetCheck(BST_CHECKED);
}

void FileEncryptDlg::OnBnClickedCheckInputPassword()
{
    m_CheckInputPassword.SetCheck(BST_CHECKED);
    m_CheckDefaultPassword.SetCheck(BST_UNCHECKED);
}

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
        ParentDlg->CreateWorkThread();
        CDialogEx::OnOK();
    }
}
