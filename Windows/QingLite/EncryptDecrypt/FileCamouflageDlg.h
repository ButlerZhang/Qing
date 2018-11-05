#pragma once


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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
