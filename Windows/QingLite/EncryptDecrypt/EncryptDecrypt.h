
// EncryptDecrypt.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <string>


// CEncryptDecryptApp:
// See EncryptDecrypt.cpp for the implementation of this class
//

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

class CEncryptDecryptApp : public CWinApp
{
public:

    CEncryptDecryptApp();
    // Overrides

    bool Validate(CEdit &EditControl);

    std::wstring GetSelectPath() const;
    std::wstring GetProcessString(ProcessType Type) const;
    std::wstring GetOperationString(OperationType Type) const;

public:

    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CEncryptDecryptApp theApp;