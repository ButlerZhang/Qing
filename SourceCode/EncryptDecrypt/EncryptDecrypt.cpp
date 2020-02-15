#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "EncryptDecryptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEncryptDecryptApp

BEGIN_MESSAGE_MAP(CEncryptDecryptApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEncryptDecryptApp construction

CEncryptDecryptApp::CEncryptDecryptApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CEncryptDecryptApp object

CEncryptDecryptApp theApp;


// CEncryptDecryptApp initialization

BOOL CEncryptDecryptApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CEncryptDecryptDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

bool CEncryptDecryptApp::Validate(CEdit &EditControl)
{
    CString SourcePath;
    EditControl.GetWindowTextW(SourcePath);
    if (SourcePath.IsEmpty())
    {
        MessageBox(m_pMainWnd->m_hWnd, _T("Source path is empty!"), _T("Error Tip"), MB_OK);
        return false;
    }

    if (!PathFileExists(SourcePath.GetString()))
    {
        MessageBox(m_pMainWnd->m_hWnd, _T("Source path is not exists!"), _T("Error Tip"), MB_OK);
        return false;
    }

    return true;
}

std::wstring CEncryptDecryptApp::GetSelectPath() const
{
    wchar_t ResultBuffer[MAX_PATH];
    wchar_t SelectPathBuffer[MAX_PATH];

    memset(ResultBuffer, 0, sizeof(ResultBuffer));
    memset(SelectPathBuffer, 0, sizeof(SelectPathBuffer));

    BROWSEINFO BrowseInfo;
    BrowseInfo.hwndOwner = m_pMainWnd->m_hWnd;                      //程序主窗口
    BrowseInfo.pidlRoot = NULL;                                     //引用桌面目录
    BrowseInfo.pszDisplayName = SelectPathBuffer;                   //返回选择的路径的缓冲区
    BrowseInfo.lpszTitle = _T("Current Path");                      //弹出的窗口的文字提示
    BrowseInfo.ulFlags = BIF_BROWSEINCLUDEFILES;                    //可以获取目录和文件
    BrowseInfo.lpfn = NULL;                                         //回调函数
    BrowseInfo.lParam = 0;                                          //给回调函数的参数指针
    BrowseInfo.iImage = 0;                                          //与选中目录相关的图像
    ITEMIDLIST* pidl = (ITEMIDLIST*)SHBrowseForFolder(&BrowseInfo); //显示弹出窗口

    if (::SHGetPathFromIDList(pidl, ResultBuffer))                  //在ITEMIDLIST中得到目录名的整个路径
    {
        return std::wstring(ResultBuffer);
    }

    return std::wstring();
}

std::wstring CEncryptDecryptApp::GetProcessString(ProcessType Type) const
{
    switch (Type)
    {
    case PT_PROCEING:           return L"正在处理";
    case PT_SUCCEEDED:          return L"执行成功";
    case PT_FAILED:             return L"执行失败";
    default:                    return L"未知错误";
    }
}

std::wstring CEncryptDecryptApp::GetOperationString(OperationType Type) const
{
    switch (Type)
    {
    case OT_ENCRYPT:            return L"文件加密";
    case OT_DECRYPT:            return L"文件解密";
    case OT_DISGUISE:           return L"文件伪装";
    case OT_RECOVERY:           return L"解除伪装";
    case OT_UNKNOW:             return L"未知错误";
    default:                    return L"未知错误";
    }
}



BOOL BaseDialog::ShowChildWindowMiddle()
{
    CEncryptDecryptDlg *ParentDlg = (CEncryptDecryptDlg *)GetParent();
    if (ParentDlg != NULL)
    {
        CRect ParentRect;
        ParentDlg->GetWindowRect(ParentRect);
        int ParentWidth = ParentRect.Width();
        int ParentHeight = ParentRect.Height();

        CRect ChildOldRect;
        this->GetWindowRect(ChildOldRect);
        int ChildWidth = ChildOldRect.Width();
        int ChildHeight = ChildOldRect.Height();

        CRect ChildNewRect;
        ChildNewRect.left = ParentRect.left + (ParentWidth - ChildWidth) / 2;
        ChildNewRect.right = ChildNewRect.left + ChildWidth;
        ChildNewRect.top = ParentRect.top + (ParentHeight - ChildHeight) / 2;
        ChildNewRect.bottom = ChildNewRect.top + ChildHeight;

        this->MoveWindow(ChildNewRect);
    }

    return ShowWindow(SW_SHOW);
}
