#pragma once


// BootStartUp dialog

class BootStartUp : public CDialogEx
{
	DECLARE_DYNAMIC(BootStartUp)

public:
	BootStartUp(CWnd* pParent = NULL);   // standard constructor
	virtual ~BootStartUp();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_BOOT_START_UP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
