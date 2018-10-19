#include "stdafx.h"
#include "afxdialogex.h"
#include "EncryptionTool.h"
#include "..\..\WindowsTool.h"
#include "..\..\ToolLiteProfile.h"
#include "..\..\..\..\Qing\HeaderFiles\CommonFunction.h"
#include "..\..\..\..\Qing\HeaderFiles\FileManager.h"

#include <shlobj.h>



IMPLEMENT_DYNAMIC(EncryptionTool, CDialogEx)

EncryptionTool::EncryptionTool(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_ENCRYPTION, pParent)
{
}

EncryptionTool::~EncryptionTool()
{
}

BOOL EncryptionTool::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_EditSourcePath.SetWindowTextW(theApp.GetProfile()->m_EncryptSelectPath.c_str());
    m_EditTargetPath.SetWindowTextW(theApp.GetProfile()->m_EncryptTargetPath.c_str());

    return 0;
}

void EncryptionTool::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_ENCRYPTION_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_ENCRYPTION_PATH2, m_EditTargetPath);
}

BEGIN_MESSAGE_MAP(EncryptionTool, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON3, &EncryptionTool::OnBnClickedButtonSelectSourcePath)
    ON_BN_CLICKED(IDC_BUTTON4, &EncryptionTool::OnBnClickedButtonSelectTargetPath)
    ON_BN_CLICKED(IDC_ENCRYPT, &EncryptionTool::OnBnClickedEncrypt)
    ON_BN_CLICKED(IDC_DECRYPT, &EncryptionTool::OnBnClickedDecrypt)
END_MESSAGE_MAP()

void EncryptionTool::OnBnClickedButtonSelectSourcePath()
{
    std::wstring SelectPath = GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    DWORD Result = ::GetFileAttributes(SelectPath.c_str());
    if (INVALID_FILE_ATTRIBUTES == Result)
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());

    if (Result & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (SelectPath[SelectPath.size()-1] == L'\\')
        {
            SelectPath += L"EncryptionTool\\";
        }
        else
        {
            SelectPath += L"\\EncryptionTool\\";
        }
    }
    else
    {
        std::wstring::size_type Index = SelectPath.find_last_of(L"\\");
        if (Index != std::wstring::npos)
        {
            SelectPath.replace(Index, SelectPath.size(), L"\\EncryptionTool\\");
        }
    }

    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

void EncryptionTool::OnBnClickedButtonSelectTargetPath()
{
    const std::wstring SelectPath = GetSelectPath();
    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

std::wstring EncryptionTool::GetSelectPath() const
{
    wchar_t ResultBuffer[MAX_PATH];
    wchar_t SelectPathBuffer[MAX_PATH];

    memset(ResultBuffer, 0, sizeof(ResultBuffer));
    memset(SelectPathBuffer, 0, sizeof(SelectPathBuffer));

    BROWSEINFO BrowseInfo;
    BrowseInfo.hwndOwner = m_hWnd;                                  //程序主窗口
    BrowseInfo.pidlRoot = NULL;                                     //引用桌面目录
    BrowseInfo.pszDisplayName = SelectPathBuffer;                   //返回选择的路径的缓冲区
    BrowseInfo.lpszTitle = _T("Current Path");                      //弹出的窗口的文字提示
    BrowseInfo.ulFlags = BIF_BROWSEINCLUDEFILES;                    //可以获取目录和文件
    BrowseInfo.lpfn = NULL;                                         //回调函数
    BrowseInfo.lParam = 0;                                          //给回调函数的参数指针
    BrowseInfo.iImage = 0;                                          //与选中目录相关的图像
    ITEMIDLIST* pidl = ::SHBrowseForFolder(&BrowseInfo);            //显示弹出窗口

    if (::SHGetPathFromIDList(pidl, ResultBuffer))                  //在ITEMIDLIST中得到目录名的整个路径
    {
        return std::wstring(ResultBuffer);
    }

    return std::wstring();
}

void EncryptionTool::OnBnClickedEncrypt()
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    if (SourcePath.GetLength() <= 0)
    {
        MessageBox(NULL, _T("Source path is empty!"), MB_OK);
        return;
    }

    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (TargetPath.GetLength() <= 0)
    {
        MessageBox(NULL, _T("Target path is empty!"), MB_OK);
        return;
    }

    Qing::FileManager MyFileManager;
    if (!MyFileManager.IsDirectory(TargetPath.GetString()))
    {
        MessageBox(NULL, _T("Target path is not directory!"), MB_OK);
        return;
    }

    std::vector<std::wstring> FileNameVector;
    MyFileManager.GetFileNameNonRecursion(SourcePath.GetString(), FileNameVector);

    for (std::vector<std::wstring>::size_type Index = 0; Index < FileNameVector.size(); Index++)
    {

    }

    theApp.GetProfile()->m_EncryptSelectPath = SourcePath.GetString();
    theApp.GetProfile()->m_EncryptTargetPath = TargetPath.GetString();
    theApp.GetProfile()->SaveConfig();
}

void EncryptionTool::OnBnClickedDecrypt()
{
    // TODO: Add your control notification handler code here
}
