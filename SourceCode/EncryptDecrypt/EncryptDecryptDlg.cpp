#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "EncryptDecryptDlg.h"
#include "FileEncryptDlg.h"
#include "FileDecryptDlg.h"
#include "FileDisguiseDlg.h"
#include "FileRecoveryDlg.h"
#include "afxdialogex.h"

#include "../BoostLinux/src/BoostLog.h"
#include "../Windows/System/FileManager.h"
#include "../Windows/System/SystemShare.h"

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

    m_OperationType = OT_UNKNOW;
    m_LastOperationType = OT_UNKNOW;
    m_WorkerThread = INVALID_HANDLE_VALUE;
    m_SimpleCrypt = std::make_shared<SimpleEncrypt>();
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
    DDX_Control(pDX, IDC_BUTTON_ENCRYPT, m_ButtonEncrypt);
    DDX_Control(pDX, IDC_BUTTON_DECRYPT, m_ButtonDecrypt);
    DDX_Control(pDX, IDC_BUTTON_DISGUISE, m_ButtonDisguise);
    DDX_Control(pDX, IDC_BUTTON_RECOVERY, m_ButtonRecovery);
}

BEGIN_MESSAGE_MAP(CEncryptDecryptDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_STOP, &CEncryptDecryptDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDC_EXIT, &CEncryptDecryptDlg::OnBnClickedExit)
    ON_BN_CLICKED(IDC_BUTTON_ENCRYPT, &CEncryptDecryptDlg::OnBnClickedButtonFileEncrypt)
    ON_BN_CLICKED(IDC_BUTTON_DECRYPT, &CEncryptDecryptDlg::OnBnClickedButtonFileDecrypt)
    ON_BN_CLICKED(IDC_BUTTON_DISGUISE, &CEncryptDecryptDlg::OnBnClickedButtonFileDisguise)
    ON_BN_CLICKED(IDC_BUTTON_RECOVERY, &CEncryptDecryptDlg::OnBnClickedButtonFileRecovery)
END_MESSAGE_MAP()


// CEncryptDecryptDlg message handlers

BOOL CEncryptDecryptDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetProgramDMPEnable(true);

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

    m_DialogVector.push_back(new FileEncryptDlg(this->GetParent()));
    m_DialogVector[OT_ENCRYPT]->Create(IDD_DIALOG_ENCRYPT, this->GetParent());

    m_DialogVector.push_back(new FileDecryptDlg(this->GetParent()));
    m_DialogVector[OT_DECRYPT]->Create(IDD_DIALOG_DECRYPT, this->GetParent());

    m_DialogVector.push_back(new FileDisguiseDlg(this->GetParent()));
    m_DialogVector[OT_DISGUISE]->Create(IDD_DIALOG_DISGUISE, this->GetParent());

    m_DialogVector.push_back(new FileRecoveryDlg(this->GetParent()));
    m_DialogVector[OT_RECOVERY]->Create(IDD_DIALOG_RECOVERY, this->GetParent());

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
    if (m_WorkerThread != INVALID_HANDLE_VALUE)
    {
        m_ButtonExit.EnableWindow(FALSE);
        OnBnClickedStop();
    }

    OnCancel();
}

void CEncryptDecryptDlg::OnBnClickedButtonFileEncrypt()
{
    m_DialogVector[OT_ENCRYPT]->ShowChildWindowMiddle();
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::OnBnClickedButtonFileDecrypt()
{
    m_DialogVector[OT_DECRYPT]->ShowChildWindowMiddle();
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::OnBnClickedButtonFileDisguise()
{
    m_DialogVector[OT_DISGUISE]->ShowChildWindowMiddle();
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::OnBnClickedButtonFileRecovery()
{
    m_DialogVector[OT_RECOVERY]->ShowChildWindowMiddle();
    this->EnableWindow(FALSE);
}

void CEncryptDecryptDlg::ReleaseThreadHandle()
{
    CloseHandle(m_WorkerThread);
    m_WorkerThread = INVALID_HANDLE_VALUE;
}

void CEncryptDecryptDlg::SetOptionButtonEnable(bool Enable)
{
    m_ButtonEncrypt.EnableWindow(Enable);
    m_ButtonDecrypt.EnableWindow(Enable);
    m_ButtonDisguise.EnableWindow(Enable);
    m_ButtonRecovery.EnableWindow(Enable);
}

void CEncryptDecryptDlg::ResetControlAfterWorkerThreadStop()
{
    ReleaseThreadHandle();
    SetOptionButtonEnable(true);
    m_ButtonStop.EnableWindow(TRUE);
    m_LastOperationType = m_OperationType;

    if (!m_ButtonExit.IsWindowEnabled())
    {
        OnCancel();
    }
}

void CEncryptDecryptDlg::GetFiles(std::vector<std::wstring>& FileVector)
{
    const std::wstring &SourcePath = m_DialogVector[m_OperationType]->GetSourcePath();
    if (!SourcePath.empty())
    {
        if (PathIsDirectory(SourcePath.c_str()))
        {
            FileManager MyFileManager;
            MyFileManager.GetFileNameNonRecursion(SourcePath, FileVector);
        }
        else
        {
            FileVector.push_back(SourcePath);
        }
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

    m_ResultList.InsertColumn(0, _T("ÐòºÅ"), LVCFMT_CENTER,  50);
    m_ResultList.InsertColumn(1, _T("²Ù×÷"), LVCFMT_CENTER,  80);
    m_ResultList.InsertColumn(2, _T("Â·¾¶"), LVCFMT_LEFT,    400);
    m_ResultList.InsertColumn(3, _T("×´Ì¬"), LVCFMT_LEFT,    170);
}

void CEncryptDecryptDlg::CreateWorkThread()
{
    ReleaseThreadHandle();
    SetOptionButtonEnable(false);
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
        EDDlg->m_DialogVector[EDDlg->m_OperationType]->ProcessWork(EDDlg);
    }
    catch (std::exception e)
    {
        g_Log.WriteError(e.what());
    }

    EDDlg->ResetControlAfterWorkerThreadStop();
    return 0;
}
