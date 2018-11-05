#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "EncryptDecryptDlg.h"
#include "EncryptDecryptPassword.h"
#include "afxdialogex.h"

#include "..\..\Qing\HeaderFiles\BoostLog.h"
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

    Qing::BoostLog::DefaultInit();

    m_ProcessInfoVector.push_back(L"Processing...");
    m_ProcessInfoVector.push_back(L"Succeeded");
    m_ProcessInfoVector.push_back(L"Failed");

    m_OperationType = OT_UNKNOW;
    m_LastOperationType = OT_UNKNOW;
    m_WorkerThread = INVALID_HANDLE_VALUE;
    m_SimpleCrypt = std::make_shared<SimpleCrypt>();
}

CEncryptDecryptDlg::~CEncryptDecryptDlg()
{
    ReleaseThreadHandle();
    m_ProcessInfoVector.clear();
}

void CEncryptDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_TARGET_PATH, m_EditTargetPath);
    DDX_Control(pDX, IDC_CHECK_ENCRYPT_FILE_NAME, m_CheckEncryptFileName);
    DDX_Control(pDX, IDC_CHECK_DELETE_FILE, m_CheckDeleteOriginalFile);
    DDX_Control(pDX, IDC_LIST_ED_RESULT, m_ResultList);
    DDX_Control(pDX, IDC_ENCRYPT, m_ButtonEncrypt);
    DDX_Control(pDX, IDC_DECRYPT, m_ButtonDecrypt);
    DDX_Control(pDX, IDC_CHECK_TARGET_PATH, m_CheckTargetPath);
    DDX_Control(pDX, IDC_STOP, m_ButtonStop);
    DDX_Control(pDX, IDC_EXIT, m_ButtonExit);
}

BEGIN_MESSAGE_MAP(CEncryptDecryptDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_SOURCE, &CEncryptDecryptDlg::OnBnClickedButtonSelectSourcePath)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE_TARGET, &CEncryptDecryptDlg::OnBnClickedButtonSelectTargetPath)
    ON_BN_CLICKED(IDC_ENCRYPT, &CEncryptDecryptDlg::OnBnClickedEncrypt)
    ON_BN_CLICKED(IDC_DECRYPT, &CEncryptDecryptDlg::OnBnClickedDecrypt)
    ON_BN_CLICKED(IDC_CHECK_TARGET_PATH, &CEncryptDecryptDlg::OnBnClickedCheckTargetPath)
    ON_BN_CLICKED(IDC_STOP, &CEncryptDecryptDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDC_EXIT, &CEncryptDecryptDlg::OnBnClickedExit)
END_MESSAGE_MAP()


// CEncryptDecryptDlg message handlers

BOOL CEncryptDecryptDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    Qing::SetProgramDMPEnable(true);

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
    m_CheckEncryptFileName.SetCheck(BST_CHECKED);
    m_CheckDeleteOriginalFile.SetCheck(BST_CHECKED);

    m_PasswordDlg = new EncryptDecryptPassword(this->GetParent());
    m_PasswordDlg->Create(IDD_DIALOG_PASSWORD, this->GetParent());

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



void CEncryptDecryptDlg::OnBnClickedStop()
{
    if (m_WorkerThread != INVALID_HANDLE_VALUE)
    {
        m_ButtonStop.EnableWindow(FALSE);
        m_SimpleCrypt->SetIsForceStop(true);
    }
}

void CEncryptDecryptDlg::OnBnClickedExit()
{
    OnCancel();
}

void CEncryptDecryptDlg::OnBnClickedEncrypt()
{
    if (Validate())
    {
        m_OperationType = OT_ENCRYPT;
        m_PasswordDlg->UserDefinedShow();
    }
}

void CEncryptDecryptDlg::OnBnClickedDecrypt()
{
    if (Validate())
    {
        m_OperationType = OT_DECRYPT;
        m_PasswordDlg->UserDefinedShow();
    }
}

void CEncryptDecryptDlg::OnBnClickedCheckTargetPath()
{
    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (TargetPath.IsEmpty())
    {
        UpdateTargetPath();
    }
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
    if (m_CheckTargetPath.GetState() == BST_CHECKED)
    {
        UpdateTargetPath();
    }
}

void CEncryptDecryptDlg::OnBnClickedButtonSelectTargetPath()
{
    const std::wstring SelectPath = GetSelectPath();
    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

bool CEncryptDecryptDlg::Validate()
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    if (SourcePath.IsEmpty())
    {
        MessageBox(_T("Source path is empty!"), _T("Error Tip"), MB_OK);
        return false;
    }

    if (!PathFileExists(SourcePath.GetString()))
    {
        MessageBox(_T("Source path is not exists!"), _T("Error Tip"), MB_OK);
        return false;
    }

    if (m_CheckTargetPath.GetState() == BST_CHECKED)
    {
        CString TargetPath;
        m_EditTargetPath.GetWindowTextW(TargetPath);
        if (!TargetPath.IsEmpty())
        {
            if (!PathIsDirectory(TargetPath.GetString()) && SHCreateDirectoryEx(NULL, TargetPath.GetString(), NULL) != ERROR_SUCCESS)
            {
                MessageBox(_T("Target path can not created!"), _T("Error Tip"), MB_OK);
                return false;
            }
        }
    }

    return true;
}

void CEncryptDecryptDlg::ReleaseThreadHandle()
{
    CloseHandle(m_WorkerThread);
    m_WorkerThread = INVALID_HANDLE_VALUE;
}

void CEncryptDecryptDlg::CreateResultList()
{
    LONG Styles = GetWindowLong(m_ResultList.m_hWnd, GWL_STYLE);
    SetWindowLong(m_ResultList.m_hWnd, GWL_STYLE, Styles | LVS_REPORT);

    DWORD StylesEx = m_ResultList.GetExtendedStyle();
    StylesEx |= LVS_EX_FULLROWSELECT;
    StylesEx |= LVS_EX_GRIDLINES;
    m_ResultList.SetExtendedStyle(StylesEx);

    m_ResultList.InsertColumn(0, _T("Number"), LVCFMT_LEFT,  80);
    m_ResultList.InsertColumn(1, _T("Option"), LVCFMT_LEFT,  80);
    m_ResultList.InsertColumn(2, _T("File Path"), LVCFMT_LEFT, 400);
    m_ResultList.InsertColumn(3, _T("Status"), LVCFMT_LEFT,  300);
}

void CEncryptDecryptDlg::CreateWorkThread()
{
    ReleaseThreadHandle();
    UpdateControlEnableStatus(false);
    m_SimpleCrypt->SetIsForceStop(false);

    if (m_LastOperationType != m_OperationType)
    {
        m_ResultList.DeleteAllItems();
    }

    DWORD nThreadID;
    m_WorkerThread = ::CreateThread(0, 0, CallBack_WorkerThread, this, 0, &nThreadID);
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

void CEncryptDecryptDlg::UpdateTargetPath()
{
    m_EditTargetPath.SetWindowTextW(L"");

    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);

    if (!SourcePath.IsEmpty())
    {
        if (!PathIsDirectory(SourcePath.GetString()))
        {
            PathRemoveFileSpec((LPWSTR)SourcePath.GetString());
        }

        std::wstring TargetPath = SourcePath.GetString() + std::wstring(L"\\EncryptDecrypt\\");
        m_EditTargetPath.SetWindowTextW(TargetPath.c_str());
    }
}

void CEncryptDecryptDlg::UpdateControlEnableStatus(bool Enable)
{
    m_ButtonEncrypt.EnableWindow(Enable);
    m_ButtonDecrypt.EnableWindow(Enable);
    m_CheckTargetPath.EnableWindow(Enable);
    m_CheckEncryptFileName.EnableWindow(Enable);
    m_CheckDeleteOriginalFile.EnableWindow(Enable);

    m_EditSourcePath.EnableWindow(Enable);
    m_EditTargetPath.EnableWindow(Enable);

    ((CButton *)GetDlgItem(IDC_BUTTON_BROWSE_SOURCE))->EnableWindow(Enable);
    ((CButton *)GetDlgItem(IDC_BUTTON_BROWSE_TARGET))->EnableWindow(Enable);
}

void CEncryptDecryptDlg::UpdateResultList(size_t Index, std::wstring &FilePath, ProcessType Type)
{
    bool Found = false;
    const std::wstring &IDString = std::to_wstring(Index);
    std::wstring OptionString(m_OperationType == OT_ENCRYPT ? L"Encrypt" : L"Decrypt");

    for (int Index = 0; Index < m_ResultList.GetItemCount(); ++Index)
    {
        if (m_ResultList.GetItemText(Index, 0).CompareNoCase(IDString.c_str()) == 0)
        {
            std::wstring StatusString = m_ProcessInfoVector[Type];
            if (Type == PT_FAILED && !m_SimpleCrypt->GetErrorMessage().empty())
            {
                StatusString = m_SimpleCrypt->GetErrorMessage();
            }

            m_ResultList.SetItemText(Index, 3, StatusString.c_str());
            Found = true;
            break;
        }
    }

    if (!Found)
    {
        const int Row = m_ResultList.InsertItem(0, IDString.c_str());
        m_ResultList.SetItemText(Row, 1, OptionString.c_str());
        m_ResultList.SetItemText(Row, 2, FilePath.c_str());
        m_ResultList.SetItemText(Row, 3, m_ProcessInfoVector[Type].c_str());
    }
}

DWORD CEncryptDecryptDlg::CallBack_WorkerThread(LPVOID lpParam)
{
    CEncryptDecryptDlg *EDDlg = (CEncryptDecryptDlg*)lpParam;

    try
    {
        //get path
        CString SourcePath, TargetPath;
        EDDlg->m_EditSourcePath.GetWindowTextW(SourcePath);
        if (EDDlg->m_CheckTargetPath.GetState() == BST_CHECKED)
        {
            EDDlg->m_EditTargetPath.GetWindowTextW(TargetPath);
        }

        //get files
        std::vector<std::wstring> FileNameVector;
        if (PathIsDirectory(SourcePath.GetString()))
        {
            Qing::FileManager MyFileManager;
            MyFileManager.GetFileNameNonRecursion(SourcePath.GetString(), FileNameVector);
        }
        else
        {
            FileNameVector.push_back(SourcePath.GetString());
        }

        //set option
        EDDlg->m_SimpleCrypt->SetIsEncryptFileName(EDDlg->m_CheckEncryptFileName.GetState() == BST_CHECKED);
        EDDlg->m_SimpleCrypt->SetIsDeleteOriginalFile(EDDlg->m_CheckDeleteOriginalFile.GetState() == BST_CHECKED);

        const CString &PasswordCString = EDDlg->m_PasswordDlg->GetPassword();
        if (!PasswordCString.IsEmpty())
        {
            EDDlg->m_SimpleCrypt->SetPassword(PasswordCString.GetString());
        }

        //encrypt or decrypt
        for (std::vector<std::wstring>::size_type Index = 0; Index < FileNameVector.size(); Index++)
        {
            if (EDDlg->m_SimpleCrypt->IsForceStop())
            {
                break;
            }

            bool ProcessResult = false;
            size_t ResultIndex = Index + 1;
            EDDlg->UpdateResultList(ResultIndex, FileNameVector[Index], PT_PROCEING);

            if (EDDlg->m_OperationType == OT_ENCRYPT)
            {
                ProcessResult = EDDlg->m_SimpleCrypt->Encrypt(FileNameVector[Index], TargetPath.GetString());
            }
            else
            {
                ProcessResult = EDDlg->m_SimpleCrypt->DeCrypt(FileNameVector[Index], TargetPath.GetString());
            }

            ProcessType Type = ProcessResult ? PT_SUCCEEDED : PT_FAILED;
            EDDlg->UpdateResultList(ResultIndex, FileNameVector[Index], Type);
        }
    }
    catch (std::exception e)
    {
        const std::wstring &ErrorMessage = Qing::StringToWString(e.what());
        Qing::BoostLog::WriteError(ErrorMessage);
    }

    //reset
    EDDlg->m_ButtonStop.EnableWindow(TRUE);
    EDDlg->UpdateControlEnableStatus(true);
    EDDlg->m_LastOperationType = EDDlg->m_OperationType;
    EDDlg->ReleaseThreadHandle();
    return 0;
}
