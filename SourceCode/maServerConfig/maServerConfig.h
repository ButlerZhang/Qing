
// maServerConfig.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号
#include "Common.h"
#include <map>

// CmaServerConfigApp:
// 有关此类的实现，请参阅 maServerConfig.cpp
//



class CmaServerConfigApp : public CWinApp
{
public:
	CmaServerConfigApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

private:

    void InitLeafNode();

public:

    std::map<std::wstring, LeafNode> g_mapLeaf; //叶子结点
};

extern CmaServerConfigApp theApp;
