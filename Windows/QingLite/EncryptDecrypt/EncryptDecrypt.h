
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

class CEncryptDecryptApp : public CWinApp
{
public:
	CEncryptDecryptApp();

// Overrides
public:
	virtual BOOL InitInstance();

    std::wstring GetSelectPath() const;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CEncryptDecryptApp theApp;