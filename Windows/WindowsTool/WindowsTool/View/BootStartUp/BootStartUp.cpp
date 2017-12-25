// BootStartUp.cpp : implementation file
//

#include "stdafx.h"
#include "../../WindowsTool.h"
#include "BootStartUp.h"
#include "afxdialogex.h"


// BootStartUp dialog

IMPLEMENT_DYNAMIC(BootStartUp, CDialogEx)

BootStartUp::BootStartUp(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_BOOT_START_UP, pParent)
{

}

BootStartUp::~BootStartUp()
{
}

void BootStartUp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(BootStartUp, CDialogEx)
END_MESSAGE_MAP()


// BootStartUp message handlers
