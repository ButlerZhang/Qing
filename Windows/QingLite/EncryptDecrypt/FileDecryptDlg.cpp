// FileDecryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileDecryptDlg.h"
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
}


BEGIN_MESSAGE_MAP(FileDecryptDlg, CDialogEx)
END_MESSAGE_MAP()


// FileDecryptDlg message handlers
