#pragma once


// FileDecryptDlg dialog

class FileDecryptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FileDecryptDlg)

public:
	FileDecryptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~FileDecryptDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DECRYPT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
