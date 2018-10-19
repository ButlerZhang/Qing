
// WindowsTool.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <memory>


// CWindowsToolApp:
// See WindowsTool.cpp for the implementation of this class
//

class ToolLiteProfile;

class CWindowsToolApp : public CWinApp
{
public:
	CWindowsToolApp();

// Overrides
public:
	virtual BOOL InitInstance();

    std::shared_ptr<ToolLiteProfile> GetProfile() const { return m_ToolLiteProfile; }

// Implementation

	DECLARE_MESSAGE_MAP()

private:

    std::shared_ptr<ToolLiteProfile>                 m_ToolLiteProfile;
};

extern CWindowsToolApp theApp;