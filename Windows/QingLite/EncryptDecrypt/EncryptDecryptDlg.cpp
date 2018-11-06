#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "EncryptDecryptDlg.h"
#include "EncryptDecryptPassword.h"
#include "FileEncryptDlg.h"
#include "FileDecryptDlg.h"
#include "FileCamouflageDlg.h"
#include "FileUNCamouflageDlg.h"
#include "afxdialogex.h"

#include "..\..\Qing\HeaderFiles\BoostLog.h"
#include "..\..\Qing\HeaderFiles\FileManager.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"

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

    m_OperationType = OT_UNKNOW;
    m_LastOperationType = OT_UNKNOW;
    m_WorkerThread = INVALID_HANDLE_VALUE;
    m_SimpleCrypt = std::make_shared<Qing::SimpleEncrypt>();
}

CEncryptDecryptDlg::~CEncryptDecryptDlg()
{
    ReleaseThreadHandle();
}

void CEncryptDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ED_RESULT, m_ResultList);
    DDX_Control(pDX, IDC_STOP, m_ButtonStop);
    DDX_Control(pDX, IDC_EXIT, m_ButtonExit);
}

BEGIN_MESSAGE_MAP(CEncryptDecryptDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_STOP, &CEncryptDecryptDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDC_EXIT, &CEncryptDecryptDlg::OnBnClickedExit)
    ON_BN_CLICKED(IDC_BUTTON_ENCRYPT, &CEncryptDecryptDlg::OnBnClickedButtonFileEncrypt)
    ON_BN_CLICKED(IDC_BUTTON_DECRYPT, &CEncryptDecryptDlg::OnBnClickedButtonFileDecrypt)
    ON_BN_CLICKED(IDC_BUTTON_CAMOUFLAGE, &CEncryptDecryptDlg::OnBnClickedButtonFileCamouflage)
    ON_BN_CLICKED(IDC_BUTTON_UN_CAMOUFLAGE, &CEncryptDecryptDlg::OnBnClickedButtonFileUnCamouflage)
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
    //m_CheckEncryptFileName.SetCheck(BST_CHECKED);
    //m_CheckDeleteOriginalFile.SetCheck(BST_CHECKED);

    m_PasswordDlg = new EncryptDecryptPassword(this->GetParent());
    m_PasswordDlg->Create(IDD_DIALOG_PASSWORD, this->GetParent());

    m_FileEncryptDlg = new FileEncryptDlg(this->GetParent());
    m_FileEncryptDlg->Create(IDD_DIALOG_ENCRYPT, this->GetParent());

    m_FileDecryptDlg = new FileDecryptDlg(this->GetParent());
    m_FileDecryptDlg->Create(IDD_DIALOG_DECRYPT, this->GetParent());

    m_FileCamouflageDlg = new FileCamouflageDlg(this->GetParent());
    m_FileCamouflageDlg->Create(IDD_DIALOG_CAMOUFLAGE, this->GetParent());

    m_FileUNCamouflageDlg = new FileUNCamouflageDlg(this->GetParent());
    m_FileUNCamouflageDlg->Create(IDD_DIALOG_UN_CAMOUFLAGE, this->GetParent());

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

void CEncryptDecryptDlg::OnBnClickedButtonFileEncrypt()
{
    m_OperationType = OT_ENCRYPT;
    m_FileEncryptDlg->UserDefinedShow();
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::OnBnClickedButtonFileDecrypt()
{
    m_OperationType = OT_DECRYPT;
    m_FileDecryptDlg->ShowWindow(SW_SHOW);
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::OnBnClickedButtonFileCamouflage()
{
    m_OperationType = OT_CAMOUFLAGE;
    m_FileCamouflageDlg->ShowWindow(SW_SHOW);
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::OnBnClickedButtonFileUnCamouflage()
{
    m_OperationType = OT_UN_CAMOUFLAGE;
    m_FileUNCamouflageDlg->ShowWindow(SW_SHOW);
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::ReleaseThreadHandle()
{
    CloseHandle(m_WorkerThread);
    m_WorkerThread = INVALID_HANDLE_VALUE;
}

void CEncryptDecryptDlg::SetSimpleEncryptOption()
{
    m_SimpleCrypt->SetIsEncryptFileName(m_FileEncryptDlg->IsEncryptFileName());
    m_SimpleCrypt->SetIsDeleteOriginalFile(m_FileEncryptDlg->IsDeleteOriginalFile());

    const CString &PasswordCString = m_PasswordDlg->GetPassword();
    if (!PasswordCString.IsEmpty())
    {
        m_SimpleCrypt->SetPassword(PasswordCString.GetString());
    }
}

void CEncryptDecryptDlg::GetFiles(std::vector<std::wstring>& FileVector)
{
    const CString &SourcePath = m_FileEncryptDlg->GetSourcePath();
    if (PathIsDirectory(SourcePath.GetString()))
    {
        Qing::FileManager MyFileManager;
        MyFileManager.GetFileNameNonRecursion(SourcePath.GetString(), FileVector);
    }
    else
    {
        FileVector.push_back(SourcePath.GetString());
    }
}

void CEncryptDecryptDlg::CreateResultList()
{
    LONG Styles = GetWindowLong(m_ResultList.m_hWnd, GWL_STYLE);
    SetWindowLong(m_ResultList.m_hWnd, GWL_STYLE, Styles | LVS_REPORT);

    DWORD StylesEx = m_ResultList.GetExtendedStyle();
    StylesEx |= LVS_EX_FULLROWSELECT;
    StylesEx |= LVS_EX_GRIDLINES;
    m_ResultList.SetExtendedStyle(StylesEx);

    m_ResultList.InsertColumn(0, _T("ÐòºÅ"), LVCFMT_CENTER,  80);
    m_ResultList.InsertColumn(1, _T("²Ù×÷"), LVCFMT_CENTER,  80);
    m_ResultList.InsertColumn(2, _T("Â·¾¶"), LVCFMT_LEFT,    400);
    m_ResultList.InsertColumn(3, _T("×´Ì¬"), LVCFMT_LEFT,    170);
}

void CEncryptDecryptDlg::CreateWorkThread()
{
    ReleaseThreadHandle();
    m_SimpleCrypt->SetIsForceStop(false);
    m_FileEncryptDlg->ClickPasswordOK();
    m_FileDecryptDlg->ClickPasswordOK();
    m_FileCamouflageDlg->ClickPasswordOK();

    if (m_LastOperationType != m_OperationType)
    {
        m_ResultList.DeleteAllItems();
    }

    DWORD nThreadID;
    m_WorkerThread = ::CreateThread(0, 0, CallBack_WorkerThread, this, 0, &nThreadID);
}

void CEncryptDecryptDlg::UpdateResultList(size_t Index, std::wstring &FilePath, ProcessType Type)
{
    bool Found = false;
    const std::wstring &IDString = std::to_wstring(Index);
    std::wstring OptionString(theApp.GetOperationString(m_OperationType));

    for (int Index = 0; Index < m_ResultList.GetItemCount(); ++Index)
    {
        if (m_ResultList.GetItemText(Index, 0).CompareNoCase(IDString.c_str()) == 0)
        {
            std::wstring StatusString = theApp.GetProcessString(Type);
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
        m_ResultList.SetItemText(Row, 3, theApp.GetProcessString(Type).c_str());
    }
}

DWORD CEncryptDecryptDlg::CallBack_WorkerThread(LPVOID lpParam)
{
    CEncryptDecryptDlg *EDDlg = (CEncryptDecryptDlg*)lpParam;

    try
    {
        std::vector<std::wstring> FileNameVector;
        const std::wstring &TargetPath = EDDlg->m_FileEncryptDlg->GetTargetPath().GetString();

        EDDlg->GetFiles(FileNameVector);
        EDDlg->SetSimpleEncryptOption();

        bool ProcessResult = false;
        for (std::vector<std::wstring>::size_type Index = 0; Index < FileNameVector.size(); Index++)
        {
            if (EDDlg->m_SimpleCrypt->IsForceStop())
            {
                break;
            }

            size_t ResultIndex = Index + 1;
            EDDlg->UpdateResultList(ResultIndex, FileNameVector[Index], PT_PROCEING);

            switch (EDDlg->m_OperationType)
            {
            case OT_ENCRYPT:
                ProcessResult = EDDlg->m_SimpleCrypt->Encrypt(FileNameVector[Index], TargetPath);
                break;

            case OT_DECRYPT:
                ProcessResult = EDDlg->m_SimpleCrypt->DeCrypt(FileNameVector[Index], TargetPath);
                break;

            case OT_CAMOUFLAGE:
                break;

            case OT_UN_CAMOUFLAGE:
                break;

            default:
                break;
            }

            EDDlg->UpdateResultList(ResultIndex, FileNameVector[Index], ProcessResult ? PT_SUCCEEDED : PT_FAILED);
        }
    }
    catch (std::exception e)
    {
        const std::wstring &ErrorMessage = Qing::StringToWString(e.what());
        Qing::BoostLog::WriteError(ErrorMessage);
    }

    //reset
    EDDlg->m_ButtonStop.EnableWindow(TRUE);
    EDDlg->m_LastOperationType = EDDlg->m_OperationType;
    EDDlg->ReleaseThreadHandle();
    return 0;
}
