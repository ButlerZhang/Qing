#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileDecryptDlg.h"
#include "EncryptDecryptDlg.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(FileDecryptDlg, CDialogEx)

FileDecryptDlg::FileDecryptDlg(CWnd* pParent /*=NULL*/)
    : BaseDialog(IDD_DIALOG_DECRYPT, pParent)
{
}

FileDecryptDlg::~FileDecryptDlg()
{
}

BOOL FileDecryptDlg::ShowChildWindowMiddle()
{
    m_EditInputPassword.SetWindowTextW(L"");
    return BaseDialog::ShowChildWindowMiddle();
}

void FileDecryptDlg::ProcessWork(void *Parent)
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
        bool ProcessResult = ParentDlg->GetSimpleEncrypt()->Decrypt(FileNameVector[Index], TargetPath);
        ParentDlg->UpdateResultList(Index + 1, FileNameVector[Index], ProcessResult ? PT_SUCCEEDED : PT_FAILED);
    }
}

std::wstring FileDecryptDlg::GetSourcePath() const
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    return SourcePath.GetString();
}

bool FileDecryptDlg::Validate()
{
    return theApp.Validate(m_EditSourcePath);
}

bool FileDecryptDlg::SetOption()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->GetSimpleEncrypt()->SetIsForceStop(false);
    ParentDlg->GetSimpleEncrypt()->SetIsDeleteOriginalFile(true);

    CString InputPassword;
    m_EditInputPassword.GetWindowTextW(InputPassword);
    if (!InputPassword.IsEmpty())
    {
        ParentDlg->GetSimpleEncrypt()->SetPassword(InputPassword.GetString());
    }

    return true;
}

void FileDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_INPUT_PASSWORD, m_EditInputPassword);
}

BEGIN_MESSAGE_MAP(FileDecryptDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &FileDecryptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileDecryptDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON1, &FileDecryptDlg::OnBnClickedButtonSelectSourcePath)
END_MESSAGE_MAP()


// FileDecryptDlg message handlers
void FileDecryptDlg::OnBnClickedButtonSelectSourcePath()
{
    const std::wstring &SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
}

void FileDecryptDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->ResetOperationType();
    CDialogEx::OnCancel();
}

void FileDecryptDlg::OnBnClickedOk()
{
    if (Validate() && SetOption())
    {
        CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
        ParentDlg->SetOperationType(OT_DECRYPT);
        ParentDlg->CreateWorkThread();
        CDialogEx::OnOK();
    }
}
