#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileDisguiseDlg.h"
#include "EncryptDecryptDlg.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(FileDisguiseDlg, CDialogEx)

FileDisguiseDlg::FileDisguiseDlg(CWnd* pParent /*=NULL*/)
    : BaseDialog(IDD_DIALOG_DISGUISE, pParent)
{
}

FileDisguiseDlg::~FileDisguiseDlg()
{
}

void FileDisguiseDlg::ProcessWork(void *Parent)
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)Parent;
    std::vector<std::wstring> FileNameVector;
    ParentDlg->GetFiles(FileNameVector);

    for (std::vector<std::wstring>::size_type Index = 0; Index < FileNameVector.size(); Index++)
    {
        if (ParentDlg->GetSimpleEncrypt()->IsForceStop())
        {
            break;
        }

        ParentDlg->UpdateResultList(Index + 1, FileNameVector[Index], PT_PROCEING);
        bool ProcessResult = ParentDlg->GetSimpleEncrypt()->Disguise(FileNameVector[Index]);
        ParentDlg->UpdateResultList(Index + 1, FileNameVector[Index], ProcessResult ? PT_SUCCEEDED : PT_FAILED);
    }
}

std::wstring FileDisguiseDlg::GetSourcePath() const
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    return SourcePath.GetString();
}

void FileDisguiseDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
}

BEGIN_MESSAGE_MAP(FileDisguiseDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &FileDisguiseDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileDisguiseDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON1, &FileDisguiseDlg::OnBnClickedButtonSelectSourcePath)
END_MESSAGE_MAP()

// FileDisguiseDlg message handlers
void FileDisguiseDlg::OnBnClickedOk()
{
    if (theApp.Validate(m_EditSourcePath))
    {
        CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
        ParentDlg->SetOperationType(OT_DISGUISE);
        ParentDlg->CreateWorkThread();
        CDialogEx::OnOK();
    }
}

void FileDisguiseDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->ResetOperationType();
    CDialogEx::OnCancel();
}

void FileDisguiseDlg::OnBnClickedButtonSelectSourcePath()
{
    const std::wstring &SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
}