
// EncryptDecryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "EncryptDecryptDlg.h"
#include "afxdialogex.h"

#include "..\..\Qing\HeaderFiles\FileManager.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"
#include "SimpleCrypt.h"

#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEncryptDecryptDlg dialog



CEncryptDecryptDlg::CEncryptDecryptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ENCRYPTDECRYPT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEncryptDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_TARGET_PATH, m_EditTargetPath);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_EditPassword);
    DDX_Control(pDX, IDC_EDIT_CONFIR_PASSWORD, m_EditConfirmPassword);
    DDX_Control(pDX, IDC_CHECK_ENCRYPT_FILE_NAME, m_CheckEncryptFileName);
    DDX_Control(pDX, IDC_CHECK_DELETE_FILE, m_CheckDeleteOriginalFile);
}

BEGIN_MESSAGE_MAP(CEncryptDecryptDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_SOURCE, &CEncryptDecryptDlg::OnBnClickedButtonSelectSourcePath)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_TARGET, &CEncryptDecryptDlg::OnBnClickedButtonSelectTargetPath)
    ON_BN_CLICKED(IDC_ENCRYPT, &CEncryptDecryptDlg::OnBnClickedEncrypt)
    ON_BN_CLICKED(IDC_DECRYPT, &CEncryptDecryptDlg::OnBnClickedDecrypt)
END_MESSAGE_MAP()


// CEncryptDecryptDlg message handlers

BOOL CEncryptDecryptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    CreateResultList();
    m_EditSourcePath.SetWindowTextW(L"D:\\EncryptDecryptTest");
    //m_EditTargetPath.SetWindowTextW(L"D:\\EncryptDecryptTest\\EncryptionTool");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEncryptDecryptDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEncryptDecryptDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEncryptDecryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEncryptDecryptDlg::OnBnClickedButtonSelectSourcePath()
{
    std::wstring SelectPath = GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    DWORD Result = ::GetFileAttributes(SelectPath.c_str());
    if (INVALID_FILE_ATTRIBUTES == Result)
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());

    if (Result & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (SelectPath[SelectPath.size() - 1] == L'\\')
        {
            SelectPath += L"EncryptionTool\\";
        }
        else
        {
            SelectPath += L"\\EncryptionTool\\";
        }
    }
    else
    {
        std::wstring::size_type Index = SelectPath.find_last_of(L"\\");
        if (Index != std::wstring::npos)
        {
            SelectPath.replace(Index, SelectPath.size(), L"\\EncryptionTool\\");
        }
    }

    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}


void CEncryptDecryptDlg::OnBnClickedButtonSelectTargetPath()
{
    const std::wstring SelectPath = GetSelectPath();
    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

void CEncryptDecryptDlg::CreateResultList()
{
    CListCtrl *ClientListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_ED_RESULT);
    if (ClientListCtrl == NULL)
    {
        return;
    }

    LONG Styles = GetWindowLong(ClientListCtrl->m_hWnd, GWL_STYLE);
    SetWindowLong(ClientListCtrl->m_hWnd, GWL_STYLE, Styles | LVS_REPORT);

    DWORD StylesEx = ClientListCtrl->GetExtendedStyle();
    StylesEx |= LVS_EX_FULLROWSELECT;
    StylesEx |= LVS_EX_GRIDLINES;
    ClientListCtrl->SetExtendedStyle(StylesEx);

    ClientListCtrl->InsertColumn(0, _T("Number"), LVCFMT_LEFT, 80);
    ClientListCtrl->InsertColumn(1, _T("File Path"), LVCFMT_LEFT, 550);
    ClientListCtrl->InsertColumn(2, _T("Status"), LVCFMT_LEFT, 50);
}

std::wstring CEncryptDecryptDlg::GetSelectPath() const
{
    wchar_t ResultBuffer[MAX_PATH];
    wchar_t SelectPathBuffer[MAX_PATH];

    memset(ResultBuffer, 0, sizeof(ResultBuffer));
    memset(SelectPathBuffer, 0, sizeof(SelectPathBuffer));

    BROWSEINFO BrowseInfo;
    BrowseInfo.hwndOwner = m_hWnd;                                  //程序主窗口
    BrowseInfo.pidlRoot = NULL;                                     //引用桌面目录
    BrowseInfo.pszDisplayName = SelectPathBuffer;                   //返回选择的路径的缓冲区
    BrowseInfo.lpszTitle = _T("Current Path");                      //弹出的窗口的文字提示
    BrowseInfo.ulFlags = BIF_BROWSEINCLUDEFILES;                    //可以获取目录和文件
    BrowseInfo.lpfn = NULL;                                         //回调函数
    BrowseInfo.lParam = 0;                                          //给回调函数的参数指针
    BrowseInfo.iImage = 0;                                          //与选中目录相关的图像
    ITEMIDLIST* pidl = ::SHBrowseForFolder(&BrowseInfo);            //显示弹出窗口

    if (::SHGetPathFromIDList(pidl, ResultBuffer))                  //在ITEMIDLIST中得到目录名的整个路径
    {
        return std::wstring(ResultBuffer);
    }

    return std::wstring();
}

void CEncryptDecryptDlg::OnBnClickedEncrypt()
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    if (SourcePath.GetLength() <= 0)
    {
        MessageBox(NULL, _T("Source path is empty!"), MB_OK);
        return;
    }

    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (TargetPath.GetLength() <= 0)
    {
        //MessageBox(NULL, _T("Target path is empty!"), MB_OK);
        //return;
    }

    Qing::FileManager MyFileManager;
    if (!MyFileManager.IsDirectory(TargetPath.GetString()))
    {
        MessageBox(NULL, _T("Target path is not directory!"), MB_OK);
        return;
    }

    std::vector<std::wstring> FileNameVector;
    MyFileManager.GetFileNameNonRecursion(SourcePath.GetString(), FileNameVector);

    for (std::vector<std::wstring>::size_type Index = 0; Index < FileNameVector.size(); Index++)
    {
        SimpleCrypt MyCrypt;
        //MyCrypt.SetIsEncryptFileName(true);
        MyCrypt.SetIsDeleteOriginalFile(false);
        MyCrypt.Encrypt(FileNameVector[Index], TargetPath.GetString());
    }
}

void CEncryptDecryptDlg::OnBnClickedDecrypt()
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    if (SourcePath.GetLength() <= 0)
    {
        MessageBox(NULL, _T("Source path is empty!"), MB_OK);
        return;
    }

    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (TargetPath.GetLength() <= 0)
    {
        //MessageBox(NULL, _T("Target path is empty!"), MB_OK);
        //return;
    }

    Qing::FileManager MyFileManager;
    if (!MyFileManager.IsDirectory(TargetPath.GetString()))
    {
        MessageBox(NULL, _T("Target path is not directory!"), MB_OK);
        return;
    }

    std::vector<std::wstring> FileNameVector;
    MyFileManager.GetFileNameNonRecursion(SourcePath.GetString(), FileNameVector);

    for (std::vector<std::wstring>::size_type Index = 0; Index < FileNameVector.size(); Index++)
    {
        SimpleCrypt MyCrypt;
        MyCrypt.DeCrypt(FileNameVector[Index], TargetPath.GetString());
    }
}
