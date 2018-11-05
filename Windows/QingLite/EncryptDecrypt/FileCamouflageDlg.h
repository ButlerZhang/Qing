#pragma once
#include "afxwin.h"


// FileCamouflageDlg dialog

class FileCamouflageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FileCamouflageDlg)

public:
	FileCamouflageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~FileCamouflageDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CAMOUFLAGE };
#endif

    void ClickPasswordOK() { CDialogEx::OnOK(); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSelectSourcePath();
    CEdit m_EditSourcePath;
};
