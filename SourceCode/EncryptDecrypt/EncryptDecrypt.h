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

struct TaskNode
{
    int                 m_FileIndex;
    ProcessType         m_ProcessType;
    std::wstring        m_FileName;
    std::wstring        m_Error;

    TaskNode() : m_FileIndex(0), m_ProcessType(PT_PROCEING) {}
    TaskNode(const std::wstring& FileName, int FileIndex) : m_FileName(FileName), m_FileIndex(FileIndex), m_ProcessType(PT_PROCEING) {}
};

class BaseDialog : public CDialogEx
{
public:

    BaseDialog(UINT nIDTemplate, CWnd* pParent = NULL) : CDialogEx(nIDTemplate, pParent) {}
    virtual ~BaseDialog() {}

    virtual BOOL ShowChildWindowMiddle();
    virtual std::wstring GetSourcePath() const { return std::wstring(); }
    virtual std::wstring GetTargetPath() const { return std::wstring(); }

    virtual void Stop() = 0;
    virtual void ProcessWork(void* Parent) = 0;

    int GetThreadCount(int FileCount) const;
};



class CEncryptDecryptApp : public CWinApp
{
public:

    CEncryptDecryptApp();

    bool Validate(CEdit& EditControl);
    std::wstring GetSelectPath() const;
    std::wstring GetProcessString(ProcessType Type) const;
    std::wstring GetOperationString(OperationType Type) const;

public:

    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CEncryptDecryptApp theApp;
