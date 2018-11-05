// FileDecryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileDecryptDlg.h"
#include "EncryptDecryptDlg.h"
#include "EncryptDecryptPassword.h"
#include "afxdialogex.h"


// FileDecryptDlg dialog

IMPLEMENT_DYNAMIC(FileDecryptDlg, CDialogEx)

FileDecryptDlg::FileDecryptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DECRYPT, pParent)
{

}

FileDecryptDlg::~FileDecryptDlg()
{
}

void FileDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_EditSourcePath);
}


BEGIN_MESSAGE_MAP(FileDecryptDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &FileDecryptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileDecryptDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON1, &FileDecryptDlg::OnBnClickedButtonSelectSourcePath)
END_MESSAGE_MAP()


// FileDecryptDlg message handlers


void FileDecryptDlg::OnBnClickedOk()
{
    if (theApp.Validate(m_EditSourcePath))
    {
        CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
        ParentDlg->m_PasswordDlg->UserDefinedShow();
    }
}


void FileDecryptDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->m_OperationType = ParentDlg->m_LastOperationType;
    CDialogEx::OnCancel();
}


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
