#pragma once
#include "afxwin.h"




class FileUNCamouflageDlg : public CDialogEx
{
    DECLARE_DYNAMIC(FileUNCamouflageDlg)

public:
    FileUNCamouflageDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~FileUNCamouflageDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_UN_CAMOUFLAGE };
#endif

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSourcePath();
    CEdit m_EditSourcePath;
};
