// CDialogParams.cpp: 实现文件
//

#include "pch.h"
#include "maServerConfig.h"
#include "CDialogParams.h"
#include "afxdialogex.h"


// CDialogParams 对话框

IMPLEMENT_DYNAMIC(CDialogParams, CDialogEx)

CDialogParams::CDialogParams(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PARAMS, pParent)
{

}

CDialogParams::~CDialogParams()
{
}

void CDialogParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogParams, CDialogEx)
END_MESSAGE_MAP()


// CDialogParams 消息处理程序
