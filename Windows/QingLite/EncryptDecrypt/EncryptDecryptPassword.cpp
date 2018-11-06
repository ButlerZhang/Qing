#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "EncryptDecryptDlg.h"
#include "FileEncryptDlg.h"
#include "EncryptDecryptPassword.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(EncryptDecryptPassword, CDialogEx)

EncryptDecryptPassword::EncryptDecryptPassword(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_PASSWORD, pParent)
{
}

EncryptDecryptPassword::~EncryptDecryptPassword()
{
}

BOOL EncryptDecryptPassword::UserDefinedShow()
{
    m_EditInputPassword.SetWindowTextW(L"");
    m_EditDefaultPassword.SetWindowTextW(L"************");

    m_CheckInputPassword.SetCheck(BST_UNCHECKED);
    m_CheckDefaultPassword.SetCheck(BST_CHECKED);

    return ShowWindow(SW_SHOW);
}

CString EncryptDecryptPassword::GetPassword() const
{
    CString Password;
    m_EditInputPassword.GetWindowTextW(Password);
    return Password;
}

void EncryptDecryptPassword::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK_INPUT_PASSWORD, m_CheckInputPassword);
    DDX_Control(pDX, IDC_CHECK_DEFAULT_PASSWORD, m_CheckDefaultPassword);
    DDX_Control(pDX, IDC_EDIT_INPUT_PASSWORD, m_EditInputPassword);
    DDX_Control(pDX, IDC_EDIT_DEFAULT_PASSWORD, m_EditDefaultPassword);
}


BEGIN_MESSAGE_MAP(EncryptDecryptPassword, CDialogEx)
    ON_BN_CLICKED(IDOK, &EncryptDecryptPassword::OnBnClickedOk)
    ON_BN_CLICKED(IDC_CHECK_DEFAULT_PASSWORD, &EncryptDecryptPassword::OnBnClickedCheckDefaultPassword)
    ON_BN_CLICKED(IDC_CHECK_INPUT_PASSWORD, &EncryptDecryptPassword::OnBnClickedCheckInputPassword)
    ON_BN_CLICKED(IDCANCEL, &EncryptDecryptPassword::OnBnClickedCancel)
END_MESSAGE_MAP()


// EncryptDecryptPassword message handlers

void EncryptDecryptPassword::OnBnClickedCheckDefaultPassword()
{
    m_CheckInputPassword.SetCheck(BST_UNCHECKED);
    m_CheckDefaultPassword.SetCheck(BST_CHECKED);
}

void EncryptDecryptPassword::OnBnClickedCheckInputPassword()
{
    m_CheckInputPassword.SetCheck(BST_CHECKED);
    m_CheckDefaultPassword.SetCheck(BST_UNCHECKED);
}

void EncryptDecryptPassword::OnBnClickedOk()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    if (ParentDlg->m_OperationType == OT_ENCRYPT && !ParentDlg->m_FileEncryptDlg->CreateTargetPath())
    {
        return;
    }

    ParentDlg->CreateWorkThread();
    CDialogEx::OnOK();
}

void EncryptDecryptPassword::OnBnClickedCancel()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->m_OperationType = ParentDlg->m_LastOperationType;
    CDialogEx::OnCancel();
}
