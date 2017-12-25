
// WindowsToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsTool.h"
#include "WindowsToolDlg.h"
#include "afxdialogex.h"
#include "ToolEnum.h"
#include "View\BootStartUp\BootStartUp.h"
#include "View\EncryptionTool\EncryptionTool.h"

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


// CWindowsToolDlg dialog



CWindowsToolDlg::CWindowsToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WINDOWSTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWindowsToolDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_ToolTab);
}

BEGIN_MESSAGE_MAP(CWindowsToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CWindowsToolDlg::OnTcnSelchangeTabTool)
END_MESSAGE_MAP()


// CWindowsToolDlg message handlers

BOOL CWindowsToolDlg::OnInitDialog()
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
    InitTabControl();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWindowsToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWindowsToolDlg::OnPaint()
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
HCURSOR CWindowsToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWindowsToolDlg::InitTabControl()
{
    for (int ItemIndex = WTE_ENCRYPTION; ItemIndex < WTE_COUNT; ItemIndex++)
    {
        m_ToolTab.InsertItem(ItemIndex, GetToolName(static_cast<WindowsToolEnum>(ItemIndex)).c_str());
    }

    m_ToolVector.push_back(std::shared_ptr<EncryptionTool>(new EncryptionTool()));
    m_ToolVector[WTE_ENCRYPTION]->Create(IDD_DIALOG_ENCRYPTION, &m_ToolTab);

    m_ToolVector.push_back(std::shared_ptr<BootStartUp>(new BootStartUp()));
    m_ToolVector[WTE_BOOT_START_UP]->Create(IDD_DIALOG_BOOT_START_UP, &m_ToolTab);

    m_ToolTab.SetCurSel(0);
    OnTcnSelchangeTabTool(NULL, NULL);
}

void CWindowsToolDlg::OnTcnSelchangeTabTool(NMHDR *pNMHDR, LRESULT *pResult)
{
    CRect TabRect;
    m_ToolTab.GetClientRect(&TabRect);
    TabRect.left += 1;
    TabRect.right -= 1;
    TabRect.top += 21;
    TabRect.bottom -= 1;

    for (std::vector<std::shared_ptr<CDialogEx>>::iterator it = m_ToolVector.begin(); it != m_ToolVector.end(); it++)
    {
        (*it)->SetWindowPos(NULL, TabRect.left, TabRect.top, TabRect.Width(), TabRect.Height(), SWP_HIDEWINDOW);
    }

    int SelectIndex = m_ToolTab.GetCurSel();
    if (SelectIndex < 0 || SelectIndex >= static_cast<int>(m_ToolVector.size()))
    {
        SelectIndex = 0;
    }

    m_ToolVector[SelectIndex]->SetWindowPos(NULL, TabRect.left, TabRect.top, TabRect.Width(), TabRect.Height(), SWP_SHOWWINDOW);
}
