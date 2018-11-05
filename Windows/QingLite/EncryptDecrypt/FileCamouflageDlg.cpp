// FileCamouflageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileCamouflageDlg.h"
#include "EncryptDecryptDlg.h"
#include "EncryptDecryptPassword.h"
#include "afxdialogex.h"


// FileCamouflageDlg dialog

IMPLEMENT_DYNAMIC(FileCamouflageDlg, CDialogEx)

FileCamouflageDlg::FileCamouflageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CAMOUFLAGE, pParent)
{

}

FileCamouflageDlg::~FileCamouflageDlg()
{
}

void FileCamouflageDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_EditSourcePath);
}


BEGIN_MESSAGE_MAP(FileCamouflageDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &FileCamouflageDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileCamouflageDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON1, &FileCamouflageDlg::OnBnClickedButtonSelectSourcePath)
END_MESSAGE_MAP()


// FileCamouflageDlg message handlers


void FileCamouflageDlg::OnBnClickedOk()
{
    if (theApp.Validate(m_EditSourcePath))
    {
        CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
        ParentDlg->m_PasswordDlg->UserDefinedShow();
    }
}


void FileCamouflageDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    ParentDlg->m_OperationType = ParentDlg->m_LastOperationType;
    CDialogEx::OnCancel();
}


void FileCamouflageDlg::OnBnClickedButtonSelectSourcePath()
{
    const std::wstring &SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
}
