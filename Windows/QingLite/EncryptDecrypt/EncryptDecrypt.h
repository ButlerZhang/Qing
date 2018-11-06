#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <string>



enum OperationType
{
    OT_ENCRYPT,
    OT_DECRYPT,
    OT_DISGUISE,
    OT_RECOVERY,
    OT_UNKNOW
};

enum ProcessType
{
    PT_PROCEING,
    PT_SUCCEEDED,
    PT_FAILED
};

class BaseDialog : public CDialogEx
{
public:

    BaseDialog(UINT nIDTemplate, CWnd *pParent = NULL) : CDialogEx(nIDTemplate, pParent) {}
    ~BaseDialog() {}

    virtual BOOL UserDefinedShow();
    virtual std::wstring GetSourcePath() const { return std::wstring(); }
    virtual std::wstring GetTargetPath() const { return std::wstring(); }
};



class CEncryptDecryptApp : public CWinApp
{
public:

    CEncryptDecryptApp();

    bool Validate(CEdit &EditControl);
    std::wstring GetSelectPath() const;
    std::wstring GetProcessString(ProcessType Type) const;
    std::wstring GetOperationString(OperationType Type) const;

public:

    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CEncryptDecryptApp theApp;