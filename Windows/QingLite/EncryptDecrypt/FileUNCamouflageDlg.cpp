#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileUNCamouflageDlg.h"
#include "afxdialogex.h"


// FileUNCamouflageDlg dialog

IMPLEMENT_DYNAMIC(FileUNCamouflageDlg, CDialogEx)

FileUNCamouflageDlg::FileUNCamouflageDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_UN_CAMOUFLAGE, pParent)
{

}

FileUNCamouflageDlg::~FileUNCamouflageDlg()
{
}

void FileUNCamouflageDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
}


BEGIN_MESSAGE_MAP(FileUNCamouflageDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON1, &FileUNCamouflageDlg::OnBnClickedButtonSourcePath)
END_MESSAGE_MAP()


// FileUNCamouflageDlg message handlers


void FileUNCamouflageDlg::OnBnClickedButtonSourcePath()
{
    const std::wstring &SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
}
