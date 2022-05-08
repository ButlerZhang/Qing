#pragma once


// CDialogParams 对话框

class CDialogParams : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogParams)

public:
	CDialogParams(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogParams();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PARAMS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
